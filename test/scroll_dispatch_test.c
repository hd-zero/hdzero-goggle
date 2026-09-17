// Integration tests for HDZGOGGLE SYN dispatch: per-source REL_Y and KEY
// frame ownership. Exercises scroll_dispatch_event (the production SYN
// helper), not only the low-level rel latch.
//
// Covers the original stale-SYN-scroll bug and the Phase-2 stale-SYN-button
// (ghost click) bug. This is SYN/key dispatch equivalence — not a full
// production input-path replay (burst/filter/UI are out of scope here).
//
// Build:
//   gcc -std=gnu11 -Wall -Wextra -I../src/core scroll_dispatch_test.c
//       ../src/core/scroll_frame.c -o scroll_dispatch_test
//   ./scroll_dispatch_test

#include <linux/input.h>
#include <stdio.h>
#include <string.h>

#include "scroll_frame.h"

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

typedef struct {
    scroll_frame_t frame;
    scroll_btn_t btn;
    int clicks;
    int long_presses;
    int syn_rel;
    int syn_key;
    int syn_none;
} src_t;

static void src_init(src_t *s) {
    memset(s, 0, sizeof(*s));
    scroll_frame_init(&s->frame);
    scroll_btn_init(&s->btn);
}

static scroll_dispatch_result_t feed(src_t *s, int type, int code, int value) {
    int rel = 0, key = 0;
    scroll_dispatch_result_t d = scroll_dispatch_event(&s->frame, type, code, value, &rel, &key);
    if (d == SCROLL_DISPATCH_SYN_REL)
        s->syn_rel++;
    else if (d == SCROLL_DISPATCH_SYN_NONE)
        s->syn_none++;
    else if (d == SCROLL_DISPATCH_SYN_KEY) {
        s->syn_key++;
        switch (scroll_btn_apply_key_syn(&s->btn, key)) {
        case SCROLL_BTN_CLICK:
            s->clicks++;
            break;
        case SCROLL_BTN_LONG_PRESS:
            s->long_presses++;
            break;
        default:
            break;
        }
    }
    return d;
}

static void syn(src_t *s) {
    feed(s, EV_SYN, SYN_REPORT, 0);
}

int main(void) {
    // KEY press/release frame → exactly one click
    {
        src_t s;
        src_init(&s);
        feed(&s, EV_KEY, KEY_ENTER, 1);
        syn(&s);
        feed(&s, EV_KEY, KEY_ENTER, 0);
        syn(&s);
        check_true("key_press_release_one_click", s.clicks == 1 && s.long_presses == 0);
        check_true("key_press_release_two_key_syn", s.syn_key == 2 && s.syn_none == 0);
    }

    // KEY frame → later orphan SYN → NO second click (ghost-button blocker)
    {
        src_t s;
        src_init(&s);
        feed(&s, EV_KEY, KEY_ENTER, 1);
        syn(&s);
        feed(&s, EV_KEY, KEY_ENTER, 0);
        syn(&s);
        syn(&s);
        syn(&s);
        check_true("orphan_syn_after_key_no_second_click", s.clicks == 1);
        check_true("orphan_syn_after_key_are_none", s.syn_none == 2 && s.syn_key == 2);
    }

    // REL_Y frame → orphan SYN → NO click (stale-SYN-scroll + no ghost button)
    {
        src_t s;
        src_init(&s);
        feed(&s, EV_REL, REL_Y, -1);
        syn(&s);
        syn(&s);
        check_true("rel_y_then_orphan_syn_no_click", s.clicks == 0 && s.long_presses == 0);
        check_true("rel_y_then_orphan_syn_one_rel", s.syn_rel == 1 && s.syn_none == 1 && s.syn_key == 0);
    }

    // KEY on event1 → SYN on event0 → NO cross-source click
    {
        src_t ev0, ev1;
        src_init(&ev0);
        src_init(&ev1);
        feed(&ev1, EV_KEY, KEY_ENTER, 1);
        syn(&ev0);
        feed(&ev1, EV_KEY, KEY_ENTER, 0);
        syn(&ev1);
        check_true("key_event1_syn_event0_no_cross_click", ev0.clicks == 0 && ev0.syn_key == 0);
        check_true("key_event1_syn_event0_event1_clicks_once", ev1.clicks == 1);
        check_true("key_event1_syn_event0_event0_orphan", ev0.syn_none == 1);
    }

    // REL_Y on event0 → SYN on event1 → NO scroll/click
    {
        src_t ev0, ev1;
        src_init(&ev0);
        src_init(&ev1);
        feed(&ev0, EV_REL, REL_Y, 1);
        syn(&ev1);
        check_true("rel_event0_syn_event1_no_scroll", ev1.syn_rel == 0 && ev1.syn_key == 0);
        check_true("rel_event0_syn_event1_no_click", ev0.clicks == 0 && ev1.clicks == 0);
        check_true("rel_event0_pending_preserved", ev0.frame.rel_y_pending);
        syn(&ev0);
        check_true("rel_event0_own_syn_consumes", ev0.syn_rel == 1 && !ev0.frame.rel_y_pending);
    }

    // Alternating real key and scroll frames
    {
        src_t s;
        src_init(&s);
        feed(&s, EV_KEY, KEY_ENTER, 1);
        syn(&s);
        feed(&s, EV_REL, REL_Y, 1);
        syn(&s);
        feed(&s, EV_KEY, KEY_ENTER, 0);
        syn(&s);
        feed(&s, EV_REL, REL_Y, -1);
        syn(&s);
        check_true("alternating_one_click", s.clicks == 1);
        check_true("alternating_two_rel_syn", s.syn_rel == 2);
        check_true("alternating_two_key_syn", s.syn_key == 2);
        check_true("alternating_no_orphan", s.syn_none == 0);
    }

    // Multiple orphan SYN_REPORTs after key use → no action
    {
        src_t s;
        src_init(&s);
        feed(&s, EV_KEY, KEY_ENTER, 1);
        syn(&s);
        feed(&s, EV_KEY, KEY_ENTER, 0);
        syn(&s);
        for (int i = 0; i < 8; i++)
            syn(&s);
        check_true("many_orphan_syn_after_key_one_click", s.clicks == 1 && s.long_presses == 0);
        check_true("many_orphan_syn_after_key_eights_none", s.syn_none == 8);
    }

    // Original stale-SYN-scroll: REL consume once, second SYN no second scroll
    {
        src_t s;
        src_init(&s);
        feed(&s, EV_REL, REL_Y, -1);
        syn(&s);
        syn(&s);
        syn(&s);
        check_true("stale_syn_scroll_one_consume", s.syn_rel == 1);
        check_true("stale_syn_scroll_orphans_none", s.syn_none == 2);
        check_true("stale_syn_scroll_no_click", s.clicks == 0);
    }

    // Long press: 11 down SYNs fire long-press once; release does not click
    {
        src_t s;
        src_init(&s);
        feed(&s, EV_KEY, KEY_ENTER, 1);
        syn(&s);
        for (int i = 0; i < 10; i++) {
            feed(&s, EV_KEY, KEY_ENTER, 2);
            syn(&s);
        }
        feed(&s, EV_KEY, KEY_ENTER, 0);
        syn(&s);
        check_true("hold_long_press_once", s.long_presses == 1);
        check_true("hold_release_no_click", s.clicks == 0);
    }

    printf("\n%d/%d checks passed\n", g_checks - g_failures, g_checks);
    return g_failures == 0 ? 0 : 1;
}
