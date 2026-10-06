#pragma once
// Emulator-only mock of the DM5680 receiver's serial link. Compiled ONLY into the
// emulator (this whole file is #ifdef EMULATOR_BUILD, and src/emulator/*.c is added
// to the build only when EMULATOR_BUILD) -- nothing here reaches the goggle firmware.
#ifdef EMULATOR_BUILD
#ifdef __cplusplus
extern "C" {
#endif

// Attach the mock receiver to the DM5680 UART socketpairs (uart_emu_peer_fd of the
// ports opened in uart_init). It reads the goggle's polling commands and replies
// with plausible telemetry (firmware version, valid channel, RSSI), so the app sees
// a connected receiver instead of a dead link. Safe to call once, after uart_open.
void dm5680_emu_start(void);

#ifdef __cplusplus
}
#endif
#endif // EMULATOR_BUILD
