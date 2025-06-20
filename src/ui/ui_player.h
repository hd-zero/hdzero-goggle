#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <lvgl/lvgl.h>

#include "player/media.h"

#define LV_COLOR_MAKE_32(a, r, g, b) \
    _LV_COLOR_MAKE_TYPE_HELPER {     \
        {                            \
            b, g, r, a               \
        }                            \
    }

#define MAX_STARS 10

typedef struct {
    lv_obj_t *_btn;
    lv_obj_t *_slider;
    lv_obj_t *_label; // text format as "mm:ss(playing)/mm:ss(total)"
    lv_obj_t *_stars[MAX_STARS];
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

#ifdef __cplusplus
}
#endif
