#ifndef _PAGE_SCANNOW_H
#define _PAGE_SCANNOW_H

#include <lvgl/lvgl.h>

#include "ui/ui_main_menu.h"

extern page_pack_t pp_scannow;

int scan(void);
int scan_reinit(void);

void switch_to_video(bool is_default);
void autoscan_exit(void);

#endif
