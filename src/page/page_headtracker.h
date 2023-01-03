#ifndef _PAGE_HEADTRACKER_H
#define _PAGE_HEADTRACKER_H


#include "lvgl/lvgl.h"
#include "page_common.h"

lv_obj_t *page_headtracker_create(lv_obj_t *parent, struct panel_arr *arr);
extern page_pack_t pp_headtracker;

void headtracker_set_toggle(int sel);

#endif
