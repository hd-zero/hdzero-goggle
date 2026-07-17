#pragma once

// Internal contract between the shared input logic (src/core/input_device.c) and
// the per-target input backend that feeds it raw events:
//   - src/driver/input_evdev.c        goggle hardware (Linux evdev / epoll)
//   - src/emulator/input_device_emu.c desktop emulator (SDL keyboard)
// Exactly one backend is compiled in (CMake swaps them, like uart/rtc), so the
// shared logic and the hardware path carry no #ifdef EMULATOR_BUILD. This is not
// a public API -- only these files include it.

#include <stdbool.h>

// Semantic handlers a backend calls once it has decoded a raw event into a goggle
// action. (rbtn_click() is the third and lives in the public input_device.h.)
void roller_up(void);
void roller_down(void);
void btn_click(void);
void btn_press(void);

// Right-button "scroll simulation" state for the no_dial accessibility mode. The
// hardware backend drives these in lock-step with rbtn_click(); the emulator has a
// real wheel and leaves them untouched.
#define SCROLL_REPEAT_NONE 0
#define SCROLL_REPEAT_UP   1
#define SCROLL_REPEAT_DOWN 2
extern int btn_value;                 // right button currently held (1) or released (0)
extern bool scroll_sim_mode;          // scroll-sim mode active
extern bool scroll_sim_mode_pending;  // a mode transition is settling
extern int scroll_sim_mode_repeat;    // SCROLL_REPEAT_*

// The backend, implemented by whichever backend file is compiled in.
void input_backend_init(void);      // open the event source (evdev fds / SDL)
void *input_backend_thread(void *); // read loop, run on a background pthread
