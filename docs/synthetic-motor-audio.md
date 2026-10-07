# Synthetic Motor Audio (Goggle side)

Proof-of-concept: synthesize stereo propeller sound from mechanical RPM carried in the HDZero `0xFF` service packet. Pair with the matching VTX change (same packing). No UI, no hex/octo, no new RF opcodes.

## Packet (VRX view)

`rx_buf[0]` is the payload length. Legacy packets use `15`. Extended packets use `21`:

| Offset | Content |
|--------|---------|
| 0 | length (`21`) |
| 1..15 | legacy service fields (unchanged) |
| 16..21 | six packed 12-bit RPM bytes |

Unpack / dequantize (`q * 32`) lives in `src/core/motor_rpm_proto.*`. Decode runs in `parser_config()` when `length >= 21`; shorter packets leave audio silent.

Packing (must match VTX):

```
q = min((rpm + 16) >> 5, 4095)     # 32 RPM steps, max 131040
b0 = q0[7:0]
b1 = q1[3:0] << 4 | q0[11:8]
b2 = q1[11:4]
b3 = q2[7:0]
b4 = q3[3:0] << 4 | q2[11:8]
b5 = q3[11:4]
```

## Audio engine

`src/core/motor_audio.*` — platform-independent stereo PCM at **22050 Hz** (enough for blade-pass + harmonics; half the CPU of 48 kHz).

- QUADX pan: fronts louder, left/right stereo
- Blade-pass pulse, shaft asymmetry, quiet air noise, soft LP
- Stale telemetry (>500 ms) or all-zero RPM fades to silence

## Menu / settings

**Record Option → Motor Audio** (`On` / `Off`), stored as `record.motor_audio` in `setting.ini` (default `Off`).

- Independent of **Record Audio** — this is a live headphone mix, not a DVR flag.
- Keeps the selected **Audio Source** (Mic / Line In / A/V In); when On, line-out also opens the DAC path (`audio_sel.sh out_dac_on`) so synthetic PCM can sit under the analog source.
- Emulator: toggles the SDL playback device. Hardware still needs a Softwinner AO PCM feed into that DAC path (not wired yet); RPM decode and mute gate are ready.

## Host / emulator tooling

Prefer the small host tool (full emulator needs Linux/ARM deps):

```bash
cmake -S utilities/motor_audio_poc -B tmp/motor_audio_poc_build
cmake --build tmp/motor_audio_poc_build
./tmp/motor_audio_poc_build/motor_audio_poc --motor-test
./tmp/motor_audio_poc_build/motor_audio_poc --motor-synthetic --motor-wav tmp/motor_synthetic.wav
./tmp/motor_audio_poc_build/motor_audio_poc --motor-replay path.csv --motor-poles 12 --motor-wav tmp/out.wav
```

Blackbox CSV needs `time` / `time (us)` and `eRPM[0..3]` or `eRPM(/100)[0..3]`. Mechanical RPM = `eRPM * 2 / poles` (with `×100` when the column is `/100`).

Emulator binary also accepts the same `--motor-*` flags when built with `EMULATOR_BUILD`.
