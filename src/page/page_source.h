#ifndef _PAGE_SOURCE_H
#define _PAGE_SOURCE_H


#include "lvgl/lvgl.h"
#include "page_common.h"

lv_obj_t *page_source_create(lv_obj_t *parent, struct panel_arr *arr);

void source_mode_set(int sel);

void source_status_timer();

void pp_source_exit();

void switch_to_analog(bool is_bay);

extern bool in_sourcepage;

#endif
