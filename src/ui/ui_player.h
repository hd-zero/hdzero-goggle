#ifndef __MPLAYER_H__
#define __MPLAYER_H__

#include <stdbool.h>
#include <stdint.h>

#include <lvgl/lvgl.h>

#include "player/media.h"

#define MPLAYER_SCR_WIDTH  1920
#define MPLAYER_SCR_HEIGHT 1080
#define MPLAYER_CB_WIDTH   700
#define MPLAYER_CB_HEIGHT  84
#define MPLAYER_BTN_WIDTH  48
#define MPLAYER_BTN_HEIGHT 48
#define MPLAYER_BTN_GAP    16
#define MPLAYER_SLD_WIDTH  400
#define MPLAYER_SLD_HEIGHT 3
#define LV_COLOR_MAKE_32(a, r, g, b) \
    _LV_COLOR_MAKE_TYPE_HELPER {     \
        { b, g, r, a }               \
    }

typedef struct {
    lv_obj_t *_btn;
    lv_obj_t *_slider;
    lv_obj_t *_label; // text format as "mm:ss(playing)/mm:ss(total)"
    bool is_playing;
    int32_t value;
    int32_t range;

    bool enable;
    lv_obj_t *bg;
    lv_obj_t *bar;
} player_control_t;

uint8_t mplayer_on_key(uint8_t key);
void mplayer_set_time(uint32_t now, uint32_t duration);
void mplayer_file(char *fname);

#endif