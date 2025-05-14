#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ui/ui_main_menu.h"

extern page_pack_t pp_wifi;

extern void page_wifi_get_statusbar_text(char *buffer, int size);
extern bool page_wifi_is_sta_connected(void); // Nueva función

#ifdef __cplusplus
}
#endif

#ifndef PAGE_WIFI_H
#define PAGE_WIFI_H

#include <stdbool.h>

bool page_wifi_is_sta_connected(void);

#endif // PAGE_WIFI_H
