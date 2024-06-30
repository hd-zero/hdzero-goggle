#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ui/ui_main_menu.h"
#include <lvgl/lvgl.h>

#define CHANNEL_NUM (g_setting.source.hdzero_band == RACE_BAND ? 12 : 8)

typedef enum {
    RACE_BAND = 0,
    LOW_BAND = 1,
} band_t;

int scan(void);
int scan_reinit(void);
void autoscan_exit(void);
void page_scannow_set_channel_label(void);

extern page_pack_t pp_scannow;

#ifdef __cplusplus
}
#endif
