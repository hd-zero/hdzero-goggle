#ifndef _PAGE_SOURCE_H
#define _PAGE_SOURCE_H


#include "lvgl/lvgl.h"
#include "page_common.h"

lv_obj_t *page_source_create(lv_obj_t *parent, struct panel_arr *arr);
extern page_pack_t pp_source;

void source_mode_set(int sel);

void source_status_timer();

void pp_source_exit();

void switch_to_analog(bool is_bay);

void switch_to_hdmiin();

extern bool in_sourcepage;

#endif
