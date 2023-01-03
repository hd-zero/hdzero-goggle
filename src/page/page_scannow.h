#ifndef _PAGE_SCANNOW_H
#define _PAGE_SCANNOW_H

#include "lvgl/lvgl.h"
#include "page_common.h"
lv_obj_t *page_scannow_create(lv_obj_t *parent);
extern page_pack_t pp_scannow;

int scan(void);
int scan_reinit(void);

void user_select(uint8_t key);

void switch_to_video(bool is_default);
#endif
