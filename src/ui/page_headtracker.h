#ifndef _PAGE_HEADTRACKER_H
#define _PAGE_HEADTRACKER_H


#include "lvgl/lvgl.h"
#include "page_common.h"

lv_obj_t *page_headtracker_create(lv_obj_t *parent, panel_arr_t *arr);
void headtracker_set_toggle(int sel);

#endif
