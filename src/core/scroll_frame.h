#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

// Per-input-source frame latch for HDZGOGGLE.
//
// Each /dev/input/eventN owns its pending REL_Y and pending KEY so a
// SYN_REPORT from another fd cannot consume another device's scroll or
// button state. SYN_REPORT is the frame boundary: it consumes at most one
// action (the last event kind seen on THIS source since the previous SYN)
// and then clears ALL pending flags. An orphan SYN therefore cannot reuse
// stale key or roller state from an earlier frame.
//
// Last-kind-wins within a frame matches the historical event_type_last
// rule, but the latch is source-owned and is always cleared on SYN.

typedef enum {
    SCROLL_FRAME_KIND_NONE = 0,
    SCROLL_FRAME_KIND_REL,
    SCROLL_FRAME_KIND_KEY,
} scroll_frame_kind_t;

typedef struct {
    bool rel_y_pending;
    int roller_value;
    bool key_pending;
    int key_value; // EV_KEY value: 0=up, 1=down, 2=repeat
    scroll_frame_kind_t syn_kind;
} scroll_frame_t;

void scroll_frame_init(scroll_frame_t *f);

// Record a REL_Y on this source; marks the next matching SYN as a scroll frame.
void scroll_frame_on_rel_y(scroll_frame_t *f, int value);

// Record an EV_KEY on this source; marks the next matching SYN as a key frame.
void scroll_frame_on_key(scroll_frame_t *f, int value);

typedef enum {
    SCROLL_FRAME_SYN_NONE = 0,
    SCROLL_FRAME_SYN_REL_Y,
    SCROLL_FRAME_SYN_KEY,
} scroll_frame_syn_kind_t;

// Consume this source's pending frame. Always clears rel_y_pending,
// key_pending, and syn_kind so a later orphan SYN is a no-op.
// If this frame's last kind was REL_Y, copies roller_value to *out_rel_y
// (when non-NULL) and returns SCROLL_FRAME_SYN_REL_Y.
// If this frame's last kind was KEY, copies key_value to *out_key_value
// (when non-NULL) and returns SCROLL_FRAME_SYN_KEY.
scroll_frame_syn_kind_t scroll_frame_take_syn(scroll_frame_t *f,
                                              int *out_rel_y,
                                              int *out_key_value);

// Pure input-event dispatch over one source's frame latch.
// type/code/value are linux/input.h EV_* / SYN_* / REL_* integers.
typedef enum {
    SCROLL_DISPATCH_NONE = 0,
    SCROLL_DISPATCH_REL_Y,    // REL_Y latched; caller may also feed burst
    SCROLL_DISPATCH_KEY,      // KEY latched
    SCROLL_DISPATCH_SYN_REL,  // SYN consumed a fresh REL_Y frame
    SCROLL_DISPATCH_SYN_KEY,  // SYN consumed a fresh KEY frame
    SCROLL_DISPATCH_SYN_NONE, // SYN with no fresh frame on this source
} scroll_dispatch_result_t;

scroll_dispatch_result_t scroll_dispatch_event(scroll_frame_t *f,
                                               int type,
                                               int code,
                                               int value,
                                               int *out_rel_y,
                                               int *out_key_value);

// Physical dial-button hold/click, matching the historical SYN_REPORT
// press_time counter (long-press fires when press_time == 10 at SYN start).
#define SCROLL_BTN_LONG_PRESS_SYN 10

typedef struct {
    int press_time;
} scroll_btn_t;

void scroll_btn_init(scroll_btn_t *b);

typedef enum {
    SCROLL_BTN_NONE = 0,
    SCROLL_BTN_DOWN,
    SCROLL_BTN_LONG_PRESS,
    SCROLL_BTN_CLICK,
} scroll_btn_action_t;

// Apply one fresh KEY SYN for this source. key_value 0=release, nonzero=down/repeat.
scroll_btn_action_t scroll_btn_apply_key_syn(scroll_btn_t *b, int key_value);

#ifdef __cplusplus
}
#endif
