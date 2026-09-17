#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <sys/time.h>

// HDZGOGGLE-only causal micro-majority + raw-activity quiet burst gate.
//
// Clock domains (do not mix for membership):
//   A. event-time  — input_event.time (evdev default: realtime / gettimeofday
//                    domain; diagnostic captures show epoch-style stamps).
//                    Window membership and quiet gaps use ONLY this clock.
//   B. wake-time   — gettimeofday() at REL_Y receipt / poll. Used solely to
//                    approximate when the event-loop should wake for t0+W.
//                    Late wakes are safe: drain first, then include every
//                    queued sample with event.time <= event deadline.
//
// Majority window: [episode_t0, episode_t0 + SCROLL_MICRO_MAJORITY_MS] inclusive.
// New episode:     new_raw_event_time - last_raw_event_time > SCROLL_BURST_QUIET_MS
//                  (strict >; exactly QUIET_MS does NOT start a new episode).

#define SCROLL_MICRO_MAJORITY_MS 10
#define SCROLL_BURST_QUIET_MS    20

typedef enum {
    SCROLL_BURST_IDLE = 0,
    SCROLL_BURST_WINDOWING,
    SCROLL_BURST_SUPPRESS,
} scroll_burst_phase_t;

typedef struct {
    scroll_burst_phase_t phase;
    struct timeval episode_t0;       // event-time
    struct timeval last_raw;         // event-time of every REL_Y
    struct timeval decide_deadline;  // episode_t0 + W (event-time)
    struct timeval wake_deadline;    // userspace approx for epoll
    int window_sum;
    int window_count;
    int lead_sign;
    bool have_last_raw;
    bool decided; // already emitted this episode
} scroll_burst_t;

typedef enum {
    SCROLL_BURST_NONE = 0,
    SCROLL_BURST_EMIT,
} scroll_burst_action_t;

typedef struct {
    scroll_burst_action_t action;
    int candidate;                  // +1 or -1 when EMIT
    struct timeval emit_event_time; // event-time (= decide_deadline)
} scroll_burst_result_t;

void scroll_burst_init(scroll_burst_t *b);

// Feed one raw REL_Y. Updates last_raw always. May EMIT if this event's
// event-time is past the deadline and the decision was still pending
// (late-drain / queued post-deadline ordering).
void scroll_burst_on_rel_y(scroll_burst_t *b,
                           const struct timeval *event_time,
                           int value,
                           const struct timeval *wall_now,
                           scroll_burst_result_t *out);

// If WINDOWING and wake_deadline has been reached in userspace time, emit
// once. Safe to call repeatedly — never double-emits.
void scroll_burst_poll_due(scroll_burst_t *b,
                           const struct timeval *wall_now,
                           scroll_burst_result_t *out);

// Milliseconds until wake_deadline, or -1 if no pending decision.
// Uses ceil-ms; remaining (0,1ms] -> 1 to avoid busy-spin. Caller must
// still re-check poll_due (never emit early on a rounded-early wake).
int scroll_burst_ms_until_wake(const scroll_burst_t *b,
                               const struct timeval *wall_now);

#ifdef __cplusplus
}
#endif
