#include "scroll_burst.h"

#include <string.h>

static void tv_add_ms(const struct timeval *in, int ms, struct timeval *out) {
    long usec = (long)in->tv_usec + (long)(ms % 1000) * 1000L;
    out->tv_sec = in->tv_sec + ms / 1000;
    if (usec >= 1000000L) {
        out->tv_sec += usec / 1000000L;
        usec %= 1000000L;
    }
    out->tv_usec = (suseconds_t)usec;
}

// Strictly greater than QUIET_MS (exactly QUIET_MS is NOT a new episode).
static bool gap_exceeds_quiet(const struct timeval *newer,
                              const struct timeval *older) {
    struct timeval gap;
    timersub(newer, older, &gap);
    if (gap.tv_sec < 0)
        return false;
    if (gap.tv_sec > 0)
        return true;
    return gap.tv_usec > (SCROLL_BURST_QUIET_MS * 1000);
}

static bool event_time_le_deadline(const struct timeval *t,
                                   const struct timeval *deadline) {
    return !timercmp(t, deadline, >);
}

static int majority_candidate(const scroll_burst_t *b) {
    if (b->window_sum > 0)
        return 1;
    if (b->window_sum < 0)
        return -1;
    return b->lead_sign;
}

static void finalize_decision(scroll_burst_t *b, scroll_burst_result_t *out) {
    out->action = SCROLL_BURST_EMIT;
    out->candidate = majority_candidate(b);
    out->emit_event_time = b->decide_deadline;
    b->decided = true;
    b->phase = SCROLL_BURST_SUPPRESS;
}

static void start_episode(scroll_burst_t *b,
                          const struct timeval *event_time,
                          int sign,
                          const struct timeval *wall_now) {
    b->phase = SCROLL_BURST_WINDOWING;
    b->episode_t0 = *event_time;
    tv_add_ms(event_time, SCROLL_MICRO_MAJORITY_MS, &b->decide_deadline);
    tv_add_ms(wall_now, SCROLL_MICRO_MAJORITY_MS, &b->wake_deadline);
    b->window_sum = sign;
    b->window_count = 1;
    b->lead_sign = sign;
    b->decided = false;
}

void scroll_burst_init(scroll_burst_t *b) {
    memset(b, 0, sizeof(*b));
    b->phase = SCROLL_BURST_IDLE;
}

void scroll_burst_on_rel_y(scroll_burst_t *b,
                           const struct timeval *event_time,
                           int value,
                           const struct timeval *wall_now,
                           scroll_burst_result_t *out) {
    out->action = SCROLL_BURST_NONE;
    out->candidate = 0;

    int sign = (value > 0) ? 1 : (value < 0) ? -1 : 0;
    if (sign == 0)
        return;

    // Late-drain ordering: an event past the event-time deadline forces
    // the pending decision before this sample is classified.
    if (b->phase == SCROLL_BURST_WINDOWING && !b->decided &&
        !event_time_le_deadline(event_time, &b->decide_deadline)) {
        finalize_decision(b, out);
    }

    bool start_new = false;
    if (!b->have_last_raw) {
        start_new = true;
    } else if (b->phase == SCROLL_BURST_SUPPRESS || b->phase == SCROLL_BURST_IDLE) {
        if (gap_exceeds_quiet(event_time, &b->last_raw))
            start_new = true;
    }

    if (start_new) {
        // If we just finalized above, out already has EMIT; starting a new
        // episode in the same call is impossible (gap would be tiny). Clear
        // only if we somehow had a stale emit — keep prior EMIT if present.
        scroll_burst_result_t prior = *out;
        start_episode(b, event_time, sign, wall_now);
        b->last_raw = *event_time;
        b->have_last_raw = true;
        if (prior.action == SCROLL_BURST_EMIT)
            *out = prior;
        return;
    }

    // Continuing activity: always extend last_raw (including former DROP10).
    b->last_raw = *event_time;
    b->have_last_raw = true;

    if (b->phase == SCROLL_BURST_WINDOWING && !b->decided) {
        if (event_time_le_deadline(event_time, &b->decide_deadline)) {
            b->window_sum += sign;
            b->window_count++;
        }
    }
}

void scroll_burst_poll_due(scroll_burst_t *b,
                           const struct timeval *wall_now,
                           scroll_burst_result_t *out) {
    out->action = SCROLL_BURST_NONE;
    out->candidate = 0;

    if (b->phase != SCROLL_BURST_WINDOWING || b->decided)
        return;

    // Emit only when wake-time has reached/passed wake_deadline.
    // Early epoll wakes must return here without emitting.
    if (timercmp(wall_now, &b->wake_deadline, <))
        return;

    finalize_decision(b, out);
}

int scroll_burst_ms_until_wake(const scroll_burst_t *b,
                               const struct timeval *wall_now) {
    if (b->phase != SCROLL_BURST_WINDOWING || b->decided)
        return -1;

    struct timeval rem;
    timersub(&b->wake_deadline, wall_now, &rem);
    if (rem.tv_sec < 0 || (rem.tv_sec == 0 && rem.tv_usec <= 0))
        return 0;

    long ms = rem.tv_sec * 1000L + (rem.tv_usec + 999L) / 1000L; // ceil
    if (ms < 1)
        return 1;
    if (ms > 1000000L)
        return 1000000;
    return (int)ms;
}
