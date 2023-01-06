#ifndef _PAGE_HEADTRACKER_H
#define _PAGE_HEADTRACKER_H

#include <lvgl/lvgl.h>

#include "ui/ui_main_menu.h"

extern page_pack_t pp_headtracker;

lv_obj_t *page_headtracker_create(lv_obj_t *parent, panel_arr_t *arr);

#endif
