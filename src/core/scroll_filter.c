#include <string.h>

#include "scroll_filter.h"

// Tunables for this module only -- deliberately not exposed in defines.h.
// Unlike DIAL_SENSITIVITY (used directly by input_device.c's accumulator),
// these are internal implementation details of the hysteresis state machine
// and have no reason to be global.

// Consecutive opposite-direction pulses required to accept a direction
// change once a direction is already committed. 2 is the smallest value
// that distinguishes a real reversal from a single glitch pulse *when the
// filter still sees raw/throttled REL_Y*.
//
// HDZGOGGLE Phase 2 feeds this filter one burst-gated episode candidate,
// not raw glitch trains. That path overrides reversal_confirm to 1 — see
// input_device.c — because one opposite episode is a physical reverse
// detent. The compiled default stays 2 so raw-pulse unit tests retain
// glitch rejection.
//
// Deliberately no time window on this confirmation: an adversarial review
// (see git history) showed that any window short enough to reject fast
// alternating bounce is also short enough to suppress a genuine slow
// reversal indefinitely, since both can happen at the same pulse cadence.
// A window cannot distinguish intent from speed when both scenarios move at
// the same speed. Confirmation is now purely "the next surviving opposite
// pulse, with nothing else having reset state in between" -- IDLE_RESET_MS
// below is the only remaining time-based safety valve.
#define SCROLL_FILTER_REVERSAL_CONFIRM 2

// No accepted pulses for this long => treat the next pulse as a fresh,
// unconditional start in either direction, and forget any pending reversal
// candidate. Shorter than a deliberate pause-then-reverse, longer than
// inter-pulse gaps during normal turning. Internal to this module and easy
// to retune from bench/hardware testing.
#define SCROLL_FILTER_IDLE_RESET_MS 160

#ifdef SCROLL_FILTER_DEBUG
#include <log/log.h>
#define SF_LOG(fmt, ...) LOGI(fmt, ##__VA_ARGS__)
#else
#define SF_LOG(...) \
    do {            \
    } while (0)
#endif

static long timeval_diff_ms(const struct timeval *a, const struct timeval *b) {
    long sec_diff = (long)a->tv_sec - (long)b->tv_sec;
    long usec_diff = (long)a->tv_usec - (long)b->tv_usec;
    return sec_diff * 1000L + usec_diff / 1000L;
}

void scroll_filter_init(scroll_filter_t *f) {
    memset(f, 0, sizeof(*f));
}

scroll_filter_event_t scroll_filter_step(scroll_filter_t *f, const struct timeval *t, int value) {
    int new_dir = (value > 0) ? 1 : (value < 0) ? -1
                                                 : 0;
    if (new_dir == 0)
        return SCROLL_FILTER_NONE;

    if (f->have_last_event_time) {
        long idle_ms = timeval_diff_ms(t, &f->last_event_time);
        // idle_ms < 0 covers a backward clock step: don't trust stale state.
        if (idle_ms < 0 || idle_ms > SCROLL_FILTER_IDLE_RESET_MS) {
            SF_LOG("scroll_filter: idle/clock reset (%ld ms)", idle_ms);
            f->dir = 0;
            f->pending_count = 0;
        }
    }
    f->last_event_time = *t;
    f->have_last_event_time = true;

    if (f->dir == 0) {
        // Idle: first pulse after a pause or at startup commits immediately,
        // in either direction.
        f->dir = new_dir;
        f->pending_count = 0;
        return (new_dir == 1) ? SCROLL_FILTER_UP : SCROLL_FILTER_DOWN;
    }

    if (new_dir == f->dir) {
        // Continuing the committed direction: no added latency, and any
        // pending opposite-direction candidate is cancelled (it was noise).
        f->pending_count = 0;
        return (new_dir == 1) ? SCROLL_FILTER_UP : SCROLL_FILTER_DOWN;
    }

    // new_dir opposes the committed direction: reversal candidate. There is
    // only one possible opposite of a committed +1/-1 direction, so a
    // pending count alone (no separate pending-direction field) is enough.
    int confirm = (f->reversal_confirm > 0) ? f->reversal_confirm
                                           : SCROLL_FILTER_REVERSAL_CONFIRM;
    f->pending_count++;
    if (f->pending_count >= confirm) {
        f->dir = new_dir;
        f->pending_count = 0;
        SF_LOG("scroll_filter: reversal confirmed dir=%d", new_dir);
        return (new_dir == 1) ? SCROLL_FILTER_UP : SCROLL_FILTER_DOWN;
    }

    SF_LOG("scroll_filter: reversal candidate dir=%d", new_dir);
    return SCROLL_FILTER_NONE;
}
