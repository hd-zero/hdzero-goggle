#ifndef _PAGE_IMAGESETTINGS_H
#define _PAGE_IMAGESETTINGS_H


#include "lvgl/lvgl.h"
#include "page_common.h"
lv_obj_t *page_imagesettings_create(lv_obj_t *parent, struct panel_arr *arr);
extern page_pack_t pp_imagesettings;

void page_ims_click();
void set_slider_value();
#endif
