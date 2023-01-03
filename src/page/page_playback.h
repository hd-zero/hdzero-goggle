#ifndef _PAGE_PLAYBACK_H
#define _PAGE_PLAYBACK_H


#include "lvgl/lvgl.h"
#include "page_common.h"
lv_obj_t *page_playback_create(lv_obj_t *parent);
extern page_pack_t pp_playback;

void pb_key(uint8_t key);
#endif
