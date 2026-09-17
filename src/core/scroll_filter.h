#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <sys/time.h>

// Stateful direction-hysteresis filter for a worn rotary encoder that can
// emit duplicate, skipped, or short opposite-direction glitch pulses.
//
// On the original Goggle 1 (HDZGOGGLE) this is the sole direction-change
// protection: it replaces the old blind 20ms direction-change gate and sees
// every REL_Y pulse that survives HDZero's unrelated 10ms duplicate-event
// throttle in input_device.c. (An adversarial review found that stacking
// this filter *behind* the old 20ms gate let the gate silently hide the
// pulse this filter needed to reject alternating bounce correctly, and
// separately let a bounded confirmation window suppress genuine slow
// reversals indefinitely -- see git history for the traced scenarios.)
//
// A single opposite-direction pulse surrounded by same-direction pulses is
// treated as encoder noise and absorbed. A real direction reversal is
// accepted once a second *consecutive* opposite pulse confirms it -- no
// fixed time window, so a deliberately slow reversal is never suppressed.
// The filter is a pure function of (timestamp, direction) pairs -- no I/O,
// locking, or UI dependencies -- so it can be driven directly by
// deterministic tests.

typedef enum {
    SCROLL_FILTER_NONE = 0, // pulse absorbed; caller must not move the menu
    SCROLL_FILTER_UP,
    SCROLL_FILTER_DOWN,
} scroll_filter_event_t;

typedef struct {
    int dir;           // committed scroll direction: 0 (idle), +1 (up), -1 (down)
    int pending_count;  // consecutive opposite-direction pulses seen so far (0 or 1)
    struct timeval last_event_time;
    bool have_last_event_time;
    // 0 => compiled default (SCROLL_FILTER_REVERSAL_CONFIRM). Tests/replay
    // may set 1 or 2 to compare episode-level reversal policy.
    int reversal_confirm;
} scroll_filter_t;

// Resets f to the idle state. Equivalent to zero-initializing the struct.
void scroll_filter_init(scroll_filter_t *f);

// Feeds one already-debounced REL_Y direction (value > 0 => up, value < 0 =>
// down) at timestamp t through the hysteresis filter. Returns the direction
// that should actually be applied, or SCROLL_FILTER_NONE if the pulse was
// absorbed as a likely glitch.
scroll_filter_event_t scroll_filter_step(scroll_filter_t *f, const struct timeval *t, int value);

#ifdef __cplusplus
}
#endif
