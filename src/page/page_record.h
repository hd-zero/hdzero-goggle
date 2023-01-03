#ifndef _PAGE_RECORD_H
#define _PAGE_RECORD_H


#include "lvgl/lvgl.h"
#include "page_common.h"
lv_obj_t *page_record_create(lv_obj_t *parent, struct panel_arr *arr);
extern page_pack_t pp_record;

void record_set_toggle(int sel);
void formatsd_negtive();
#endif
