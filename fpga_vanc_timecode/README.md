# FPGA VANC Timecode Injection — Atomos Ninja V Trigger

## Problem

The Atomos Ninja V "HDMI Device" trigger modes (Canon EOS R, Sony ILCE-7SM3, etc.)
use **VANC (Vertical Ancillary Data)** embedded in the video blanking interval to
detect recording state, not HDMI InfoFrames.

HDfury captures (log 1590) of a real Canon EOS R confirmed:
- All InfoFrames (AVI, SPD, VSIF) are **byte-for-byte identical** between REC ON and REC OFF
- The trigger mechanism is VANC timecode: SMPTE 12M-2 / SMPTE ST 291M ancillary data packets
  injected at the pixel level during vertical blanking

The IT66121 HDMI transmitter cannot inject VANC — it only handles InfoFrames and audio.
VANC must be injected by the **FPGA** before the video reaches the IT66121.

## VANC Timecode Format

### SMPTE ST 291M Ancillary Data Packet Structure

VANC packets are embedded in the video blanking lines as pixel-level data:

```
ADF (3 words) | DID | SDID | DC | UDW[0..DC-1] | CS
```

- **ADF**: Ancillary Data Flag — `0x000 0x3FF 0x3FF` (10-bit) or `0x00 0xFF 0xFF` (8-bit)
- **DID**: Data Identifier = `0x60` (SMPTE 12M-2 timecode)
- **SDID**: Secondary Data ID = `0x60` (SMPTE 12M-2 timecode)
- **DC**: Data Count = `0x10` (16 UDW words for timecode)
- **CS**: Checksum (8-bit sum of DID through last UDW, per SMPTE ST 291M §6 for 8-bit interfaces)

### SMPTE 12M-2 Timecode UDW Layout (16 words)

| UDW |  Content                   | BCD Value               |
|-----|---------------------------|-------------------------|
| 0   | Frames units (bits 3:0)   | 0x0–0x9                 |
| 1   | Frames tens (bits 1:0) + flags | bit2=drop, bit3=CF  |
| 2   | Seconds units (bits 3:0)  | 0x0–0x9                 |
| 3   | Seconds tens (bits 2:0) + flag | bit3=field_phase    |
| 4   | Minutes units (bits 3:0)  | 0x0–0x9                 |
| 5   | Minutes tens (bits 2:0) + flag | bit3=BGF0           |
| 6   | Hours units (bits 3:0)    | 0x0–0x9                 |
| 7   | Hours tens (bits 1:0) + flags  | bit2=BGF1, bit3=BGF2|
| 8–15| Binary group data (BG1–BG8) | User bits (typically 0)|

### Timecode Example: 00:01:23:15

```
UDW[0] = 0x05  (frames units = 5)
UDW[1] = 0x01  (frames tens = 1, no flags)
UDW[2] = 0x03  (seconds units = 3)
UDW[3] = 0x02  (seconds tens = 2)
UDW[4] = 0x01  (minutes units = 1)
UDW[5] = 0x00  (minutes tens = 0)
UDW[6] = 0x00  (hours units = 0)
UDW[7] = 0x00  (hours tens = 0)
UDW[8..15] = 0x00 (binary groups, unused)
```

## Integration Notes

### FPGA Video Pipeline

The VANC inserter must be placed in the FPGA video pipeline **after** the frame
buffer / scaler and **before** the IT66121 parallel video input:

```
Camera RX → Frame Buffer → Scaler → [VANC Inserter] → IT66121 Parallel Input
```

### Blanking Line Selection

- **720p**: Lines 1–25 are vertical blanking. Use line 10–15 for VANC.
- **1080p**: Lines 1–41 are vertical blanking. Use line 10–15 for VANC.
- **1080i**: Lines 1–20 (field 1), 564–583 (field 2). Use line 10–15.

The Ninja V typically expects VANC on lines 12–15 (standard broadcast position).

### Horizontal Position

This design assumes a **DE-based parallel video interface** with separate
`hsync`, `vsync`, and `de` signals.  The VANC inserter does **not** parse
embedded BT.656 EAV/SAV codewords.

For this interface, choose a horizontal offset measured from the **start of the
horizontal blanking interval** on the selected VANC line, and place the ADF a
few pixel clocks into blanking (for example, horizontal position 12+), while
ensuring the full 23-byte packet completes before active video (`de=1`) begins.

Note: The Verilog module has a 1-cycle output register pipeline, so
`VANC_H_START` should be set to `desired_ADF_pixel - 1`.

If you adapt the design to a **BT.656 / embedded-sync** stream, the equivalent
reference point is after the EAV sequence, not from `de` timing.

### Software Interface

The ARM CPU (Allwinner V536) controls the FPGA timecode via:
1. SPI or I2C registers exposed by the FPGA
2. Registers: `TC_HOURS`, `TC_MINUTES`, `TC_SECONDS`, `TC_FRAMES`, `TC_ENABLE`
3. When `TC_ENABLE=1`, the FPGA inserts VANC packets on every frame
4. The ARM updates the timecode registers once per second

### Trigger Protocol (Record-Run)

1. **Idle**: `TC_ENABLE=0` → no VANC packets → Ninja V sees no timecode
2. **Record Start**: ARM sets timecode to 00:00:00:00, sets `TC_ENABLE=1`
3. **Recording**: ARM increments timecode every second. FPGA inserts VANC.
4. **Record Stop**: ARM sets `TC_ENABLE=0` → VANC packets stop
5. Ninja V detects timecode start/stop transitions to trigger recording

## Files

- `vanc_timecode_inserter.v` — Sample Verilog module for VANC injection
- `README.md` — This file
