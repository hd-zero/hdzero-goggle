#ifndef _PAGE_AUTOSCAN_H
#define _PAGE_AUTOSCAN_H

#include <lvgl/lvgl.h>

#include "ui/ui_main_menu.h"

extern page_pack_t pp_autoscan;

lv_obj_t *page_autoscan_create(lv_obj_t *parent, panel_arr_t *arr);

#endif
