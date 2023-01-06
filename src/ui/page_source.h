#ifndef _PAGE_SOURCE_H
#define _PAGE_SOURCE_H

#include <lvgl/lvgl.h>

#include "ui/ui_main_menu.h"

extern page_pack_t pp_source;

lv_obj_t *page_source_create(lv_obj_t *parent, panel_arr_t *arr);

void source_status_timer();
void switch_to_analog(bool is_bay);
void switch_to_hdmiin();

#endif
