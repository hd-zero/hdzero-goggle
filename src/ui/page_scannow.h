#ifndef _PAGE_SCANNOW_H
#define _PAGE_SCANNOW_H

#include <lvgl/lvgl.h>

#include "ui/ui_main_menu.h"

#define INC_17MHZ_MODE 0 // 0 or 1
typedef enum {
    RACE_BAND = 0,
    LOW_BAND = 1,
} band_t;

extern page_pack_t pp_scannow;

int scan(void);
int scan_reinit(void);

void autoscan_exit(void);

extern band_t band;
#endif
