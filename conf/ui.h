#pragma once

#include "core/settings.h"
#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

#if HDZGOGGLE

#define UI_MENU_ENTRY_FONT       &lv_font_montserrat_26
#define UI_MENU_LABEL_FONT       &lv_font_montserrat_16
#define UI_MENU_SIZE             lv_disp_get_hor_res(NULL) - 500, lv_disp_get_ver_res(NULL) - 96
#define UI_MENU_POSITION         250, 96
#define UI_MENU_ROOT_SIZE        250, 975
#define UI_MENU_PROG_BAR_SIZE    320, 20
#define UI_OSD_TEXT_FONT         &lv_font_montserrat_20
#define UI_OSD_LABEL_FONT        &lv_font_montserrat_18
#define UI_PLAYER_TEXT_FONT      &lv_font_montserrat_26
#define UI_SCANNOW_READY_FONT    &lv_font_montserrat_26
#define UI_SCANNOW_READY_PAD     12
#define UI_SCANNOW_NOTE_FONT     &lv_font_montserrat_26
#define UI_SCANNOW_NOTE_PAD      12
#define UI_SCANNOW_CHAN_FONT     &lv_font_montserrat_40
#define UI_SCANNOW_CHAN_PAD      12
#define UI_SCANNOW_PAGE_SIZE     1158, 900
#define UI_SCANNOW_PAGE_PAD      60
#define UI_SCANNOW_SCANNER_SIZE  1158, 250
#define UI_SCANNOW_PROG_BAR_SIZE 500, 50
#define UI_SCANNOW_FREQ_SIZE     1164, 500

static inline lv_coord_t *UI_STATUS_BAR_COLS() {
    static lv_coord_t c[] = {264, 64, 267, 64, 267, 64, 267, 64, 267, 64, 267, LV_GRID_TEMPLATE_LAST};
    return c;
}

static inline lv_coord_t *UI_STATUS_BAR_ROWS() {
    static lv_coord_t r[] = {48, 96, LV_GRID_TEMPLATE_LAST};
    return r;
}

static inline int UI_STATUS_BAR_LABEL_WIDTH() {
    return 267;
}

static inline lv_coord_t *UI_SCANNOW_SCANNER_COLS() {
    static lv_coord_t c[] = {500, 20, 1164 - 520, LV_GRID_TEMPLATE_LAST};
    return c;
}

static inline lv_coord_t *UI_SCANNOW_SCANNER_ROWS() {
    static lv_coord_t r[] = {60, 60, 80, LV_GRID_TEMPLATE_LAST};
    return r;
}

static inline lv_coord_t *UI_SCANNOW_SIGNAL_COLS() {
    static lv_coord_t c[] = {120, 80, 80, 180, 100, 80, 80, 180, LV_GRID_TEMPLATE_LAST};
    return c;
}

static inline lv_coord_t *UI_SCANNOW_SIGNAL_ROWS() {
    static lv_coord_t r[] = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};
    return r;
}

#elif HDZBOXPRO

#define UI_MENU_ENTRY_FONT       &lv_font_montserrat_18
#define UI_MENU_LABEL_FONT       &lv_font_montserrat_14
#define UI_MENU_SIZE             lv_disp_get_hor_res(NULL) - 300, lv_disp_get_ver_res(NULL) - 48
#define UI_MENU_POSITION         150, 48
#define UI_MENU_ROOT_SIZE        240, 672
#define UI_MENU_PROG_BAR_SIZE    210, 12
#define UI_OSD_TEXT_FONT         &lv_font_montserrat_14
#define UI_OSD_LABEL_FONT        &lv_font_montserrat_14
#define UI_PLAYER_TEXT_FONT      &lv_font_montserrat_14
#define UI_SCANNOW_READY_FONT    &lv_font_montserrat_18
#define UI_SCANNOW_READY_PAD     8
#define UI_SCANNOW_NOTE_FONT     &lv_font_montserrat_16
#define UI_SCANNOW_NOTE_PAD      8
#define UI_SCANNOW_CHAN_FONT     &lv_font_montserrat_26
#define UI_SCANNOW_CHAN_PAD      8
#define UI_SCANNOW_PAGE_SIZE     780, 600
#define UI_SCANNOW_PAGE_PAD      40
#define UI_SCANNOW_SCANNER_SIZE  780, 160
#define UI_SCANNOW_PROG_BAR_SIZE 320, 32
#define UI_SCANNOW_FREQ_SIZE     788, 320

static inline lv_coord_t *UI_STATUS_BAR_COLS() {
    static lv_coord_t pro[] = {176, 42, 178, 42, 178, 42, 178, 42, 178, 42, 178, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t lite[] = {176, 42, 392, 42, 392, 42, 392, LV_GRID_TEMPLATE_LAST};
    return g_setting.is_pro ? pro : lite;
}

static inline lv_coord_t *UI_STATUS_BAR_ROWS() {
    static lv_coord_t r[] = {32, 32, LV_GRID_TEMPLATE_LAST};
    return r;
}

static inline int UI_STATUS_BAR_LABEL_WIDTH() {
    return g_setting.is_pro ? 178 : 392;
}

static inline lv_coord_t *UI_SCANNOW_SCANNER_COLS() {
    static lv_coord_t c[] = {320, 12, 420, LV_GRID_TEMPLATE_LAST};
    ;
    return c;
}

static inline lv_coord_t *UI_SCANNOW_SCANNER_ROWS() {
    static lv_coord_t r[] = {40, 40, 50, LV_GRID_TEMPLATE_LAST};
    return r;
}

static inline lv_coord_t *UI_SCANNOW_SIGNAL_COLS() {
    static lv_coord_t c[] = {80, 52, 52, 120, 60, 52, 52, 120, LV_GRID_TEMPLATE_LAST};
    return c;
}

static inline lv_coord_t *UI_SCANNOW_SIGNAL_ROWS() {
    static lv_coord_t r[] = {40, 40, 40, 40, 40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST};
    return r;
}

#else

#error "Platform not defined"

#endif

#ifdef __cplusplus
}
#endif
