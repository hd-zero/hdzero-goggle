#ifndef _PAGE_SCANNOW_H
#define _PAGE_SCANNOW_H

#include <lvgl/lvgl.h>

#include "ui/ui_main_menu.h"

#define INC_17MHZ_MODE 0 // 0 or 1
#define CHANNEL_NUM    (band == RACE_BAND ? 10 : 8)

typedef enum {
    RACE_BAND = 0,
    LOW_BAND = 1,
} band_t;

int scan(void);
int scan_reinit(void);
void autoscan_exit(void);

extern band_t band;
extern page_pack_t pp_scannow;
#endif
