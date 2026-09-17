// Deterministic tests for per-source scroll frame latch (stale-SYN fix).
//
// Build:
//   gcc -std=gnu11 -Wall -Wextra -I../src/core scroll_frame_test.c
//       ../src/core/scroll_frame.c -o scroll_frame_test
//   ./scroll_frame_test

#include <stdio.h>
#include <string.h>

#include "scroll_frame.h"

static int g_failures = 0;
static int g_checks = 0;

static void check_true(const char *name, int cond) {
    g_checks++;
    if (cond) {
        printf("PASS: %s\n", name);
    } else {
        printf("FAIL: %s\n", name);
        g_failures++;
    }
}

int main(void) {
    // event0 REL_Y → event1 SYN = no scroll (cross-source isolation)
    {
        scroll_frame_t src[2];
        scroll_frame_init(&src[0]);
        scroll_frame_init(&src[1]);

        scroll_frame_on_rel_y(&src[0], -1);

        int out = 0;
        int took1 = scroll_frame_take_pending(&src[1], &out);
        int still0 = src[0].rel_y_pending;
        int took0 = scroll_frame_take_pending(&src[0], &out);

        check_true("cross_source_event1_syn_no_consume", !took1);
        check_true("cross_source_event0_pending_preserved", still0);
        check_true("cross_source_event0_syn_consumes_once", took0 && out == -1);
        check_true("cross_source_event0_cleared_after_consume", !src[0].rel_y_pending);
    }

    // event0 REL_Y → event0 SYN = exactly one consume
    {
        scroll_frame_t f;
        scroll_frame_init(&f);
        scroll_frame_on_rel_y(&f, 1);

        int out = 0;
        int took = scroll_frame_take_pending(&f, &out);
        check_true("matching_syn_consumes", took && out == 1);
        check_true("matching_syn_clears_pending", !f.rel_y_pending);
    }

    // second orphan SYN = no second consume
    {
        scroll_frame_t f;
        scroll_frame_init(&f);
        scroll_frame_on_rel_y(&f, -1);

        int out = 0;
        int first = scroll_frame_take_pending(&f, &out);
        int second = scroll_frame_take_pending(&f, &out);
        check_true("orphan_second_syn_first_ok", first && out == -1);
        check_true("orphan_second_syn_no_second_consume", !second);
    }

    // DROP10 frame (take clears pending) → later orphan SYN = no consume
    {
        scroll_frame_t f;
        scroll_frame_init(&f);
        scroll_frame_on_rel_y(&f, 1);

        int out = 0;
        // Caller takes pending then decides to DROP10 -- pending already gone.
        int dropped_frame = scroll_frame_take_pending(&f, &out);
        check_true("drop10_take_clears_pending", dropped_frame && !f.rel_y_pending);

        int orphan = scroll_frame_take_pending(&f, &out);
        check_true("drop10_then_orphan_syn_no_consume", !orphan);
    }

    // Last REL_Y wins within one pending frame
    {
        scroll_frame_t f;
        scroll_frame_init(&f);
        scroll_frame_on_rel_y(&f, 1);
        scroll_frame_on_rel_y(&f, -1);
        int out = 0;
        int took = scroll_frame_take_pending(&f, &out);
        check_true("last_rel_y_wins", took && out == -1);
    }

    // take_syn: KEY then orphan SYN clears key so no second consume
    {
        scroll_frame_t f;
        scroll_frame_init(&f);
        scroll_frame_on_key(&f, 1);
        int rel = 0, key = 0;
        check_true("take_syn_key_down",
                   scroll_frame_take_syn(&f, &rel, &key) == SCROLL_FRAME_SYN_KEY && key == 1);
        check_true("take_syn_orphan_after_key",
                   scroll_frame_take_syn(&f, &rel, &key) == SCROLL_FRAME_SYN_NONE);
    }

    // take_syn: REL preferred last-kind, leftover KEY does not leak
    {
        scroll_frame_t f;
        scroll_frame_init(&f);
        scroll_frame_on_key(&f, 1);
        scroll_frame_on_rel_y(&f, -1);
        int rel = 0, key = 99;
        check_true("take_syn_last_kind_rel",
                   scroll_frame_take_syn(&f, &rel, &key) == SCROLL_FRAME_SYN_REL_Y && rel == -1);
        check_true("take_syn_key_does_not_leak",
                   scroll_frame_take_syn(&f, &rel, &key) == SCROLL_FRAME_SYN_NONE);
    }

    printf("\n%d/%d checks passed\n", g_checks - g_failures, g_checks);
    return g_failures == 0 ? 0 : 1;
}
