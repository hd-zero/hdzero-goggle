#ifndef _PAGE_IMAGESETTINGS_H
#define _PAGE_IMAGESETTINGS_H

#include <lvgl/lvgl.h>

#include "ui/ui_main_menu.h"

extern page_pack_t pp_imagesettings;

lv_obj_t *page_imagesettings_create(lv_obj_t *parent, panel_arr_t *arr);

void set_slider_value();

#endif
