// Standalone, deterministic tests for src/core/scroll_filter.c.
//
// This intentionally has no dependency on LVGL, log/, minIni, threads, or
// any part of the firmware UI/app_state -- it drives the filter directly
// with synthetic (timestamp, direction) sequences. Build and run with:
//
//   gcc -std=gnu11 -Wall -Wextra -I../src/core scroll_filter_test.c
//       ../src/core/scroll_filter.c -o scroll_filter_test
//   ./scroll_filter_test
//
// Several cases here (alternating_no_false_reversal,
// slow_genuine_reverse_no_window) directly reproduce scenarios from an
// adversarial review of an earlier design that stacked this filter behind
// HDZero's old 20ms direction-change gate instead of replacing it. That
// stacked design let the old gate hide, from this filter, the exact pulse
// it needed to see to reject alternating bounce (a false reversal slipped
// through), and separately let a fixed confirmation window suppress a
// genuine slow reversal indefinitely. Both are fixed by feeding every
// surviving pulse to this filter and confirming reversals by pulse count
// alone -- see scroll_filter.h/.c for the current design.

#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "scroll_filter.h"

static int g_failures = 0;
static int g_checks = 0;

static struct timeval ms(long milliseconds) {
    struct timeval t;
    t.tv_sec = milliseconds / 1000;
    t.tv_usec = (milliseconds % 1000) * 1000;
    return t;
}

static const char *event_name(scroll_filter_event_t e) {
    switch (e) {
    case SCROLL_FILTER_UP:
        return "UP";
    case SCROLL_FILTER_DOWN:
        return "DOWN";
    default:
        return "NONE";
    }
}

// Runs `values` (each +1/-1) at `times_ms` timestamps through a fresh
// filter and checks the resulting event sequence exactly matches `expect`.
static void check_sequence(const char *name, const int *values, const long *times_ms,
                            const scroll_filter_event_t *expect, int count) {
    scroll_filter_t f;
    scroll_filter_init(&f);

    int ok = 1;
    for (int i = 0; i < count; i++) {
        struct timeval t = ms(times_ms[i]);
        scroll_filter_event_t got = scroll_filter_step(&f, &t, values[i]);
        if (got != expect[i]) {
            printf("  [%s] step %d (t=%ldms, value=%d): expected %s, got %s\n",
                   name, i, times_ms[i], values[i], event_name(expect[i]), event_name(got));
            ok = 0;
        }
    }

    g_checks++;
    if (ok) {
        printf("PASS: %s\n", name);
    } else {
        printf("FAIL: %s\n", name);
        g_failures++;
    }
}

// Runs `values`/`times_ms` through a fresh filter and fails if `forbidden`
// is ever emitted. Used as a belt-and-suspenders check alongside an exact
// check_sequence() so a future change to the NONE/confirm pattern can't
// quietly reintroduce a wrong-direction emission without a test noticing.
static void check_never_emits(const char *name, const int *values, const long *times_ms,
                               int count, scroll_filter_event_t forbidden) {
    scroll_filter_t f;
    scroll_filter_init(&f);

    int ok = 1;
    for (int i = 0; i < count; i++) {
        struct timeval t = ms(times_ms[i]);
        scroll_filter_event_t got = scroll_filter_step(&f, &t, values[i]);
        if (got == forbidden) {
            printf("  [%s] step %d (t=%ldms, value=%d): forbidden %s was emitted\n",
                   name, i, times_ms[i], values[i], event_name(forbidden));
            ok = 0;
        }
    }

    g_checks++;
    if (ok) {
        printf("PASS: %s\n", name);
    } else {
        printf("FAIL: %s\n", name);
        g_failures++;
    }
}

int main(void) {
    // + + + + -> forward only
    {
        int values[] = {1, 1, 1, 1};
        long times[] = {0, 10, 20, 30};
        scroll_filter_event_t expect[] = {SCROLL_FILTER_UP, SCROLL_FILTER_UP, SCROLL_FILTER_UP, SCROLL_FILTER_UP};
        check_sequence("forward_only", values, times, expect, 4);
    }

    // - - - - -> reverse only
    {
        int values[] = {-1, -1, -1, -1};
        long times[] = {0, 10, 20, 30};
        scroll_filter_event_t expect[] = {SCROLL_FILTER_DOWN, SCROLL_FILTER_DOWN, SCROLL_FILTER_DOWN, SCROLL_FILTER_DOWN};
        check_sequence("reverse_only", values, times, expect, 4);
    }

    // + + - + + -> isolated glitch rejected, net forward motion preserved
    {
        int values[] = {1, 1, -1, 1, 1};
        long times[] = {0, 10, 20, 30, 40};
        scroll_filter_event_t expect[] = {SCROLL_FILTER_UP, SCROLL_FILTER_UP, SCROLL_FILTER_NONE, SCROLL_FILTER_UP, SCROLL_FILTER_UP};
        check_sequence("isolated_minus_rejected", values, times, expect, 5);
    }

    // - - + - - -> isolated glitch rejected, net reverse motion preserved
    {
        int values[] = {-1, -1, 1, -1, -1};
        long times[] = {0, 10, 20, 30, 40};
        scroll_filter_event_t expect[] = {SCROLL_FILTER_DOWN, SCROLL_FILTER_DOWN, SCROLL_FILTER_NONE, SCROLL_FILTER_DOWN, SCROLL_FILTER_DOWN};
        check_sequence("isolated_plus_rejected", values, times, expect, 5);
    }

    // Fast genuine reversal: + + - - confirms on the natural 2nd opposite
    // pulse with no added window latency.
    {
        int values[] = {1, 1, -1, -1};
        long times[] = {0, 15, 30, 45};
        scroll_filter_event_t expect[] = {SCROLL_FILTER_UP, SCROLL_FILTER_UP, SCROLL_FILTER_NONE, SCROLL_FILTER_DOWN};
        check_sequence("rapid_genuine_reverse", values, times, expect, 4);
    }

    // Slow genuine reversal (~70ms cadence): the old fixed 50ms confirmation
    // window suppressed this forever (every opposite pulse re-armed the
    // candidate from scratch). With no window, it confirms on the 2nd
    // consecutive opposite pulse regardless of cadence, as long as no gap
    // exceeds the idle reset.
    {
        int values[] = {1, 1, -1, -1, -1};
        long times[] = {0, 40, 100, 170, 240};
        scroll_filter_event_t expect[] = {SCROLL_FILTER_UP, SCROLL_FILTER_UP, SCROLL_FILTER_NONE, SCROLL_FILTER_DOWN, SCROLL_FILTER_DOWN};
        check_sequence("slow_genuine_reverse_no_window", values, times, expect, 5);
    }

    // Stop, then reverse exactly one detent: the pause exceeds the idle
    // reset, so the single opposite pulse afterward commits immediately,
    // with no 2-pulse confirmation required.
    {
        int values[] = {1, -1};
        long times[] = {0, 200}; // gap > idle reset (160ms)
        scroll_filter_event_t expect[] = {SCROLL_FILTER_UP, SCROLL_FILTER_DOWN};
        check_sequence("one_detent_reverse_after_pause", values, times, expect, 2);
    }

    // Heavy alternating bounce that broke the old stacked-filter design:
    // HDZero's now-removed 20ms gate used to silently eat the same-direction
    // pulse that cancels the reversal candidate, letting two disjoint
    // opposite pulses falsely confirm a DOWN. With every pulse now visible
    // to this filter, the cancelling pulse is never hidden and no reversal
    // is ever confirmed from pure noise.
    {
        int values[] = {1, -1, 1, -1, 1, -1};
        long times[] = {0, 15, 30, 45, 60, 75};
        scroll_filter_event_t expect[] = {SCROLL_FILTER_UP, SCROLL_FILTER_NONE, SCROLL_FILTER_UP,
                                           SCROLL_FILTER_NONE, SCROLL_FILTER_UP, SCROLL_FILTER_NONE};
        check_sequence("alternating_no_false_reversal", values, times, expect, 6);
        check_never_emits("alternating_no_false_reversal_never_down", values, times, 6, SCROLL_FILTER_DOWN);
    }

    // A committed-direction pulse cancels a pending reversal candidate:
    // + (commit up), - (candidate), + (cancels candidate, continues up).
    {
        int values[] = {1, -1, 1};
        long times[] = {0, 20, 40};
        scroll_filter_event_t expect[] = {SCROLL_FILTER_UP, SCROLL_FILTER_NONE, SCROLL_FILTER_UP};
        check_sequence("candidate_cancelled_by_committed_direction", values, times, expect, 3);
    }

    // Confirmation requires two CONSECUTIVE opposite pulses: a single
    // opposite pulse never confirms on its own; the second one does.
    {
        int values[] = {1, -1, -1};
        long times[] = {0, 20, 40};
        scroll_filter_event_t expect[] = {SCROLL_FILTER_UP, SCROLL_FILTER_NONE, SCROLL_FILTER_DOWN};
        check_sequence("candidate_confirmation_requires_consecutive_opposites", values, times, expect, 3);
    }

    // Episode-level (one candidate per quiet-bounded episode, 25ms apart,
    // within 160ms idle). Burst gate already removed intra-detent chatter,
    // so these pulses are episode decisions, not raw glitch trains.
    {
        int values[] = {1, -1};
        long times[] = {0, 25};
        scroll_filter_t f2, f1;
        scroll_filter_init(&f2);
        scroll_filter_init(&f1);
        f2.reversal_confirm = 2;
        f1.reversal_confirm = 1;
        struct timeval t0 = ms(0), t1 = ms(25);
        scroll_filter_event_t e2_0 = scroll_filter_step(&f2, &t0, 1);
        scroll_filter_event_t e2_1 = scroll_filter_step(&f2, &t1, -1);
        scroll_filter_event_t e1_0 = scroll_filter_step(&f1, &t0, 1);
        scroll_filter_event_t e1_1 = scroll_filter_step(&f1, &t1, -1);
        g_checks++;
        if (e2_0 == SCROLL_FILTER_UP && e2_1 == SCROLL_FILTER_NONE) {
            printf("PASS: episode_confirm2_swallows_first_opposite\n");
        } else {
            printf("FAIL: episode_confirm2_swallows_first_opposite\n");
            g_failures++;
        }
        g_checks++;
        if (e1_0 == SCROLL_FILTER_UP && e1_1 == SCROLL_FILTER_DOWN) {
            printf("PASS: episode_confirm1_accepts_first_opposite\n");
        } else {
            printf("FAIL: episode_confirm1_accepts_first_opposite\n");
            g_failures++;
        }
        (void)values;
        (void)times;
    }

    // Two consecutive opposite episodes confirm under both policies, but
    // confirm=2 emits the reversal one episode later.
    {
        scroll_filter_t f2, f1;
        scroll_filter_init(&f2);
        scroll_filter_init(&f1);
        f2.reversal_confirm = 2;
        f1.reversal_confirm = 1;
        long ts[] = {0, 25, 50};
        int vs[] = {1, -1, -1};
        scroll_filter_event_t g2[3], g1[3];
        for (int i = 0; i < 3; i++) {
            struct timeval t = ms(ts[i]);
            g2[i] = scroll_filter_step(&f2, &t, vs[i]);
            g1[i] = scroll_filter_step(&f1, &t, vs[i]);
        }
        g_checks++;
        if (g2[0] == SCROLL_FILTER_UP && g2[1] == SCROLL_FILTER_NONE && g2[2] == SCROLL_FILTER_DOWN) {
            printf("PASS: episode_confirm2_reverses_on_second_opposite\n");
        } else {
            printf("FAIL: episode_confirm2_reverses_on_second_opposite\n");
            g_failures++;
        }
        g_checks++;
        if (g1[0] == SCROLL_FILTER_UP && g1[1] == SCROLL_FILTER_DOWN && g1[2] == SCROLL_FILTER_DOWN) {
            printf("PASS: episode_confirm1_reverses_on_first_opposite\n");
        } else {
            printf("FAIL: episode_confirm1_reverses_on_first_opposite\n");
            g_failures++;
        }
    }

    // Inactivity reset: a long pause lets the very next pulse start
    // immediately in either direction, without needing confirmation.
    {
        int values[] = {1, -1};
        long times[] = {0, 200}; // gap > idle reset window (160ms)
        scroll_filter_event_t expect[] = {SCROLL_FILTER_UP, SCROLL_FILTER_DOWN};
        check_sequence("idle_reset_allows_immediate_direction_change", values, times, expect, 2);
    }

    // No artificial delay: sustained fast same-direction spin fires every
    // detent immediately, back-to-back.
    {
        int values[] = {1, 1, 1, 1, 1, 1, 1, 1};
        long times[] = {0, 5, 10, 15, 20, 25, 30, 35};
        scroll_filter_event_t expect[8];
        for (int i = 0; i < 8; i++)
            expect[i] = SCROLL_FILTER_UP;
        check_sequence("sustained_fast_spin_no_delay", values, times, expect, 8);
    }

    printf("\n%d/%d checks passed\n", g_checks - g_failures, g_checks);
    return g_failures == 0 ? 0 : 1;
}
