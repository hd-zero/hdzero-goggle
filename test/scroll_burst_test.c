// Deterministic tests for HDZGOGGLE causal 10ms micro-majority + 20ms quiet.
//
// Build:
//   gcc -std=gnu11 -Wall -Wextra -I../src/core scroll_burst_test.c
//       ../src/core/scroll_burst.c -o scroll_burst_test
//   ./scroll_burst_test

#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "scroll_burst.h"

static int g_failures = 0;
static int g_checks = 0;

static void check_true(const char *name, int cond) {
    g_checks++;
    if (cond)
        printf("PASS: %s\n", name);
    else {
        printf("FAIL: %s\n", name);
        g_failures++;
    }
}

static struct timeval tv_ms(long ms) {
    struct timeval t;
    t.tv_sec = ms / 1000;
    t.tv_usec = (ms % 1000) * 1000;
    return t;
}

static struct timeval tv_us(long long us) {
    struct timeval t;
    t.tv_sec = (time_t)(us / 1000000LL);
    t.tv_usec = (suseconds_t)(us % 1000000LL);
    return t;
}

static void feed(scroll_burst_t *b, long ev_ms, int sign, long wall_ms,
                 scroll_burst_result_t *out) {
    struct timeval et = tv_ms(ev_ms);
    struct timeval wt = tv_ms(wall_ms);
    scroll_burst_on_rel_y(b, &et, sign, &wt, out);
}

static void poll_at(scroll_burst_t *b, long wall_ms, scroll_burst_result_t *out) {
    struct timeval wt = tv_ms(wall_ms);
    scroll_burst_poll_due(b, &wt, out);
}

int main(void) {
    // singleton REL_Y → one decision at +10 ms
    {
        scroll_burst_t b;
        scroll_burst_init(&b);
        scroll_burst_result_t r;

        feed(&b, 0, 1, 0, &r);
        check_true("singleton_no_emit_at_t0", r.action == SCROLL_BURST_NONE);

        poll_at(&b, 9, &r);
        check_true("singleton_no_emit_early", r.action == SCROLL_BURST_NONE);

        poll_at(&b, 10, &r);
        check_true("singleton_emit_at_10ms",
                   r.action == SCROLL_BURST_EMIT && r.candidate == 1);

        poll_at(&b, 11, &r);
        check_true("singleton_no_duplicate_poll", r.action == SCROLL_BURST_NONE);
    }

    // clean same-direction episode
    {
        scroll_burst_t b;
        scroll_burst_init(&b);
        scroll_burst_result_t r;
        feed(&b, 0, 1, 0, &r);
        feed(&b, 1, 1, 1, &r);
        feed(&b, 2, 1, 2, &r);
        poll_at(&b, 10, &r);
        check_true("clean_same_dir_emit_plus",
                   r.action == SCROLL_BURST_EMIT && r.candidate == 1);
        feed(&b, 15, 1, 15, &r);
        check_true("clean_same_dir_suppress_after", r.action == SCROLL_BURST_NONE);
    }

    // exact 10 ms boundary included
    {
        scroll_burst_t b;
        scroll_burst_init(&b);
        scroll_burst_result_t r;
        feed(&b, 0, 1, 0, &r);
        feed(&b, 10, -1, 10, &r); // at deadline: still in window → tie → lead +
        // Decision not yet via event path (event at deadline accumulates).
        check_true("boundary_10ms_no_emit_yet", r.action == SCROLL_BURST_NONE);
        poll_at(&b, 10, &r);
        check_true("boundary_10ms_included_tie_lead",
                   r.action == SCROLL_BURST_EMIT && r.candidate == 1);
        check_true("boundary_10ms_window_count", b.window_count == 2);
    }

    // event just after 10 ms excluded from majority
    {
        scroll_burst_t b;
        scroll_burst_init(&b);
        scroll_burst_result_t r;
        feed(&b, 0, 1, 0, &r);
        // +1 at 0; then many -1 after deadline should not flip majority
        feed(&b, 11, -1, 11, &r); // past deadline → finalize first with sum=+1
        check_true("after_10ms_forces_emit",
                   r.action == SCROLL_BURST_EMIT && r.candidate == 1);
        check_true("after_10ms_not_in_window", b.window_count == 1);
    }

    // tie → leading sign
    {
        scroll_burst_t b;
        scroll_burst_init(&b);
        scroll_burst_result_t r;
        feed(&b, 0, -1, 0, &r);
        feed(&b, 1, 1, 1, &r); // sum 0 → lead -
        poll_at(&b, 10, &r);
        check_true("tie_uses_leading_sign",
                   r.action == SCROLL_BURST_EMIT && r.candidate == -1);
    }

    // measured-shape bipolar chatter: lead +, majority - inside 10ms → one candidate
    {
        scroll_burst_t b;
        scroll_burst_init(&b);
        scroll_burst_result_t r;
        int emits = 0;
        int last_cand = 0;
        // Lead +, then dense - within window (matches measured worn-encoder
        // burst shape), then continue alternating chatter past 10ms and past 20ms.
        int pattern[] = {1, -1, -1, -1, -1, -1, -1, -1, -1, -1}; // 10 samples in 4.5ms
        for (int i = 0; i < 10; i++) {
            long long t_us = i * 500LL; // 0..4.5ms
            struct timeval et = tv_us(t_us);
            struct timeval wt = tv_us(t_us);
            scroll_burst_on_rel_y(&b, &et, pattern[i], &wt, &r);
            if (r.action == SCROLL_BURST_EMIT) {
                emits++;
                last_cand = r.candidate;
            }
        }
        for (int i = 0; i < 79; i++) {
            long long t_us = 10000 + i * 220LL; // continue to ~27ms+
            int s = (i & 1) ? 1 : -1;
            struct timeval et = tv_us(t_us);
            struct timeval wt = tv_us(t_us);
            scroll_burst_on_rel_y(&b, &et, s, &wt, &r);
            if (r.action == SCROLL_BURST_EMIT) {
                emits++;
                last_cand = r.candidate;
            }
        }
        struct timeval w_end = tv_us(30000);
        scroll_burst_poll_due(&b, &w_end, &r);
        if (r.action == SCROLL_BURST_EMIT) {
            emits++;
            last_cand = r.candidate;
        }
        check_true("bipolar_chatter_one_emit", emits == 1);
        check_true("bipolar_chatter_majority_minus", last_cand == -1);
    }

    // same-direction chatter spanning >10 ms → one candidate
    {
        scroll_burst_t b;
        scroll_burst_init(&b);
        scroll_burst_result_t r;
        int emits = 0;
        for (int t = 0; t <= 15; t++) {
            feed(&b, t, 1, t, &r);
            if (r.action == SCROLL_BURST_EMIT)
                emits++;
        }
        poll_at(&b, 20, &r);
        if (r.action == SCROLL_BURST_EMIT)
            emits++;
        check_true("same_dir_gt10ms_one_emit", emits == 1);
    }

    // activity continuously extending beyond 20 ms → still one candidate
    {
        scroll_burst_t b;
        scroll_burst_init(&b);
        scroll_burst_result_t r;
        int emits = 0;
        for (int t = 0; t <= 40; t++) {
            feed(&b, t, 1, t, &r);
            if (r.action == SCROLL_BURST_EMIT)
                emits++;
        }
        check_true("extend_beyond_20ms_one_emit", emits == 1);
    }

    // exactly 20 ms raw gap boundary — NOT a new episode
    {
        scroll_burst_t b;
        scroll_burst_init(&b);
        scroll_burst_result_t r;
        feed(&b, 0, 1, 0, &r);
        poll_at(&b, 10, &r);
        check_true("quiet_eq20_setup_emit", r.action == SCROLL_BURST_EMIT);
        feed(&b, 10, 1, 10, &r); // last_raw=10
        feed(&b, 30, -1, 30, &r); // gap = 20.000 ms exactly → suppress continues
        check_true("quiet_eq20_no_new_episode", r.action == SCROLL_BURST_NONE);
        check_true("quiet_eq20_still_suppress", b.phase == SCROLL_BURST_SUPPRESS);
    }

    // > 20 ms gap → new episode
    {
        scroll_burst_t b;
        scroll_burst_init(&b);
        scroll_burst_result_t r;
        feed(&b, 0, 1, 0, &r);
        poll_at(&b, 10, &r);
        feed(&b, 10, 1, 10, &r);
        feed(&b, 31, -1, 31, &r); // gap 21 ms
        check_true("quiet_gt20_starts_windowing", b.phase == SCROLL_BURST_WINDOWING);
        poll_at(&b, 41, &r);
        check_true("quiet_gt20_second_emit",
                   r.action == SCROLL_BURST_EMIT && r.candidate == -1);
    }

    // two fast valid episodes separated by just over 20 ms
    {
        scroll_burst_t b;
        scroll_burst_init(&b);
        scroll_burst_result_t r;
        feed(&b, 0, 1, 0, &r);
        poll_at(&b, 10, &r);
        check_true("two_ep_first", r.action == SCROLL_BURST_EMIT && r.candidate == 1);
        // last activity at 10 via poll doesn't update last_raw — last_raw still 0
        // Feed one more raw in suppress then gap >20 from that
        feed(&b, 5, 1, 5, &r);
        feed(&b, 26, 1, 26, &r); // 21 ms after last_raw=5
        poll_at(&b, 36, &r);
        check_true("two_ep_second", r.action == SCROLL_BURST_EMIT && r.candidate == 1);
    }

    // clean reversal between episodes
    {
        scroll_burst_t b;
        scroll_burst_init(&b);
        scroll_burst_result_t r;
        feed(&b, 0, 1, 0, &r);
        poll_at(&b, 10, &r);
        feed(&b, 5, 1, 5, &r);
        feed(&b, 30, -1, 30, &r);
        poll_at(&b, 40, &r);
        check_true("clean_reversal_second_minus",
                   r.action == SCROLL_BURST_EMIT && r.candidate == -1);
    }

    // noisy reversal
    {
        scroll_burst_t b;
        scroll_burst_init(&b);
        scroll_burst_result_t r;
        feed(&b, 0, 1, 0, &r);
        feed(&b, 1, -1, 1, &r);
        feed(&b, 2, 1, 2, &r);
        feed(&b, 3, 1, 3, &r);
        poll_at(&b, 10, &r);
        int first = r.candidate;
        feed(&b, 13, 1, 13, &r);
        feed(&b, 40, -1, 40, &r);
        feed(&b, 41, 1, 41, &r);
        feed(&b, 42, -1, 42, &r);
        feed(&b, 43, -1, 43, &r);
        poll_at(&b, 50, &r);
        check_true("noisy_reversal_two_emits", first != 0 && r.action == SCROLL_BURST_EMIT);
        check_true("noisy_reversal_second_minus", r.candidate == -1);
    }

    // source0 / source1 independent windows
    {
        scroll_burst_t s0, s1;
        scroll_burst_init(&s0);
        scroll_burst_init(&s1);
        scroll_burst_result_t r0, r1;
        feed(&s0, 0, 1, 0, &r0);
        feed(&s1, 0, -1, 0, &r1);
        poll_at(&s0, 10, &r0);
        poll_at(&s1, 10, &r1);
        check_true("multi_src_s0_plus", r0.action == SCROLL_BURST_EMIT && r0.candidate == 1);
        check_true("multi_src_s1_minus", r1.action == SCROLL_BURST_EMIT && r1.candidate == -1);
        check_true("multi_src_s0_untouched_by_s1", s0.lead_sign == 1 && s0.decided);
        check_true("multi_src_s1_untouched_by_s0", s1.lead_sign == -1 && s1.decided);
    }

    // late wake: queued pre-deadline included, post-deadline excluded
    {
        scroll_burst_t b;
        scroll_burst_init(&b);
        scroll_burst_result_t r;
        feed(&b, 0, 1, 0, &r);
        feed(&b, 2, 1, 2, &r);
        feed(&b, 4, -1, 4, &r);
        // Simulate late drain: wall already 15ms, events arrive in order including post
        feed(&b, 13, -1, 15, &r); // past event deadline → finalize with sum=+1+1-1=+1
        check_true("late_wake_includes_pre_deadline",
                   r.action == SCROLL_BURST_EMIT && r.candidate == 1);
        check_true("late_wake_window_excludes_post", b.window_count == 3);
        feed(&b, 14, -1, 15, &r);
        check_true("late_wake_no_second_emit", r.action == SCROLL_BURST_NONE);
    }

    // no duplicate decision after timeout/wakeup repeats
    {
        scroll_burst_t b;
        scroll_burst_init(&b);
        scroll_burst_result_t r;
        feed(&b, 0, -1, 0, &r);
        poll_at(&b, 10, &r);
        check_true("dup_first_emit", r.action == SCROLL_BURST_EMIT);
        poll_at(&b, 10, &r);
        poll_at(&b, 11, &r);
        poll_at(&b, 100, &r);
        check_true("dup_no_repeat", r.action == SCROLL_BURST_NONE);
    }

    // next_scroll safety: two episodes just over 20ms apart yield two candidates
    // (decision times ~26ms apart > 10ms secondary gate)
    {
        scroll_burst_t b;
        scroll_burst_init(&b);
        scroll_burst_result_t r;
        struct timeval gap_tv, d0, d1;
        feed(&b, 0, 1, 0, &r);
        poll_at(&b, 10, &r);
        d0 = r.emit_event_time;
        feed(&b, 5, 1, 5, &r);
        feed(&b, 26, 1, 26, &r);
        poll_at(&b, 36, &r);
        d1 = r.emit_event_time;
        timersub(&d1, &d0, &gap_tv);
        long gap_us = gap_tv.tv_sec * 1000000L + gap_tv.tv_usec;
        check_true("two_ep_decision_gap_gt_10ms", gap_us > 10000);
    }

    // us-precision quiet boundary
    {
        scroll_burst_t b;
        scroll_burst_init(&b);
        scroll_burst_result_t r;
        struct timeval t0 = tv_us(0);
        struct timeval w0 = tv_us(0);
        scroll_burst_on_rel_y(&b, &t0, 1, &w0, &r);
        struct timeval w5 = tv_us(5000);
        scroll_burst_poll_due(&b, &w5, &r);
        struct timeval t_last = tv_us(1000);
        struct timeval w_last = tv_us(1000);
        scroll_burst_on_rel_y(&b, &t_last, 1, &w_last, &r);
        struct timeval t_eq = tv_us(1000 + 20000); // exactly +20ms
        struct timeval w_eq = tv_us(1000 + 20000);
        scroll_burst_on_rel_y(&b, &t_eq, -1, &w_eq, &r);
        check_true("us_exact_20ms_no_new", b.phase == SCROLL_BURST_SUPPRESS);
        // Gap must be measured from updated last_raw (== t_eq).
        struct timeval t_gt = tv_us(1000 + 20000 + 20001);
        struct timeval w_gt = tv_us(1000 + 20000 + 20001);
        scroll_burst_on_rel_y(&b, &t_gt, -1, &w_gt, &r);
        check_true("us_gt_20ms_new_episode", b.phase == SCROLL_BURST_WINDOWING);
    }

    printf("\n%d checks, %d failures\n", g_checks, g_failures);
    return g_failures ? 1 : 0;
}
