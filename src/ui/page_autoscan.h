#ifndef _PAGE_AUTOSCAN_H
#define _PAGE_AUTOSCAN_H


#include "lvgl/lvgl.h"
#include "page_common.h"

lv_obj_t *page_autoscan_create(lv_obj_t *parent, panel_arr_t *arr);
void autoscan_toggle(int sel);

#endif
