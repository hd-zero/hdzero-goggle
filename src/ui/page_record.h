#ifndef _PAGE_RECORD_H
#define _PAGE_RECORD_H

#include <lvgl/lvgl.h>

#include "ui/ui_main_menu.h"

extern page_pack_t pp_record;

lv_obj_t *page_record_create(lv_obj_t *parent, panel_arr_t *arr);

#endif
