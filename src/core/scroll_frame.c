#include "scroll_frame.h"

#include <linux/input.h>
#include <string.h>

void scroll_frame_init(scroll_frame_t *f) {
    memset(f, 0, sizeof(*f));
}

void scroll_frame_on_rel_y(scroll_frame_t *f, int value) {
    f->roller_value = value;
    f->rel_y_pending = true;
    f->syn_kind = SCROLL_FRAME_KIND_REL;
}

void scroll_frame_on_key(scroll_frame_t *f, int value) {
    f->key_value = value;
    f->key_pending = true;
    f->syn_kind = SCROLL_FRAME_KIND_KEY;
}

static void scroll_frame_clear_pending(scroll_frame_t *f) {
    f->rel_y_pending = false;
    f->key_pending = false;
    f->syn_kind = SCROLL_FRAME_KIND_NONE;
}

scroll_frame_syn_kind_t scroll_frame_take_syn(scroll_frame_t *f,
                                              int *out_rel_y,
                                              int *out_key_value) {
    scroll_frame_syn_kind_t kind = SCROLL_FRAME_SYN_NONE;

    if (f->syn_kind == SCROLL_FRAME_KIND_REL && f->rel_y_pending) {
        kind = SCROLL_FRAME_SYN_REL_Y;
        if (out_rel_y)
            *out_rel_y = f->roller_value;
    } else if (f->syn_kind == SCROLL_FRAME_KIND_KEY && f->key_pending) {
        kind = SCROLL_FRAME_SYN_KEY;
        if (out_key_value)
            *out_key_value = f->key_value;
    }

    scroll_frame_clear_pending(f);
    return kind;
}

scroll_dispatch_result_t scroll_dispatch_event(scroll_frame_t *f,
                                               int type,
                                               int code,
                                               int value,
                                               int *out_rel_y,
                                               int *out_key_value) {
    switch (type) {
    case EV_KEY:
        scroll_frame_on_key(f, value);
        return SCROLL_DISPATCH_KEY;
    case EV_REL:
        if (code == REL_Y) {
            scroll_frame_on_rel_y(f, value);
            return SCROLL_DISPATCH_REL_Y;
        }
        return SCROLL_DISPATCH_NONE;
    case EV_SYN:
        if (code != SYN_REPORT)
            return SCROLL_DISPATCH_NONE;
        switch (scroll_frame_take_syn(f, out_rel_y, out_key_value)) {
        case SCROLL_FRAME_SYN_REL_Y:
            return SCROLL_DISPATCH_SYN_REL;
        case SCROLL_FRAME_SYN_KEY:
            return SCROLL_DISPATCH_SYN_KEY;
        default:
            return SCROLL_DISPATCH_SYN_NONE;
        }
    default:
        return SCROLL_DISPATCH_NONE;
    }
}

void scroll_btn_init(scroll_btn_t *b) {
    b->press_time = 0;
}

scroll_btn_action_t scroll_btn_apply_key_syn(scroll_btn_t *b, int key_value) {
    if (key_value) {
        scroll_btn_action_t a = SCROLL_BTN_DOWN;
        if (b->press_time == SCROLL_BTN_LONG_PRESS_SYN)
            a = SCROLL_BTN_LONG_PRESS;
        b->press_time++;
        return a;
    }

    int t = b->press_time;
    b->press_time = 0;
    if (t < SCROLL_BTN_LONG_PRESS_SYN)
        return SCROLL_BTN_CLICK;
    return SCROLL_BTN_NONE;
}
