#pragma once

#include <lvgl/lvgl.h>

#include "core/settings.h"

#ifdef __cplusplus
extern "C" {
#endif

#if HDZGOGGLE

#define UI_MENU_ENTRY_FONT                &lv_font_montserrat_26
#define UI_MENU_LABEL_FONT                &lv_font_montserrat_16
#define UI_MENU_SIZE                      lv_disp_get_hor_res(NULL) - 500, lv_disp_get_ver_res(NULL) - 96
#define UI_MENU_POSITION                  250, 96
#define UI_MENU_ROOT_SIZE                 250, 975
#define UI_MENU_PROG_BAR_SIZE             320, 20
#define UI_PAGE_VIEW_SIZE                 1200, 900
#define UI_PAGE_TEXT_SIZE                 320, 60
#define UI_PAGE_TEXT_FONT                 &lv_font_montserrat_26
#define UI_PAGE_TEXT_PAD                  12
#define UI_PAGE_LABEL_FONT                &lv_font_montserrat_16
#define UI_PAGE_LABEL_SIZE(cols)          320 * cols, 60
#define UI_PAGE_LABEL_COMPACT_SIZE(cols)  120 * cols
#define UI_PAGE_INFO_FONT                 &lv_font_montserrat_18
#define UI_PAGE_INFO_SIZE(cols)           320 * cols, 40
#define UI_PAGE_MSG_BOX_SIZE              600
#define UI_PAGE_BUTTON_SIZE               160, 60
#define UI_PAGE_BUTTON_COMPACT_SIZE       120
#define UI_PAGE_SLIDER_SIZE               320, 60
#define UI_PAGE_SLIDER_RANGE_SIZE         320, 3
#define UI_PAGE_SLIDER_GROUP_SIZE         160, 60
#define UI_PAGE_SLIDER_COMPACT_SIZE       200, 40
#define UI_PAGE_SLIDER_COMPACT_GROUP_SIZE 160, 40
#define UI_PAGE_ARROW_SIZE                200, 60
#define UI_PAGE_ARROW_GROUP_SIZE          160, 60
#define UI_PAGE_ARROW_COMPACT_SIZE        200
#define UI_PAGE_ARROW_COMPACT_GROUP_SIZE  160
#define UI_PAGE_DROPDOWN_SIZE             160, 40
#define UI_PLAYER_TEXT_FONT               &lv_font_montserrat_26
#define UI_AUTOSCAN_COLS                  160, 150, 180, 220, 180, 160, LV_GRID_TEMPLATE_LAST
#define UI_AUTOSCAN_ROWS                  60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST
#define UI_SCANNOW_SCANNER_COLS           500, 20, 1164 - 520, LV_GRID_TEMPLATE_LAST
#define UI_SCANNOW_SCANNER_ROWS           60, 60, 80, LV_GRID_TEMPLATE_LAST
#define UI_SCANNOW_SIGNAL_COLS            120, 80, 80, 180, 100, 80, 80, 180, LV_GRID_TEMPLATE_LAST
#define UI_SCANNOW_SIGNAL_ROWS            60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST
#define UI_SCANNOW_READY_FONT             &lv_font_montserrat_26
#define UI_SCANNOW_READY_PAD              12
#define UI_SCANNOW_NOTE_FONT              &lv_font_montserrat_26
#define UI_SCANNOW_NOTE_PAD               12
#define UI_SCANNOW_CHAN_FONT              &lv_font_montserrat_40
#define UI_SCANNOW_CHAN_PAD               12
#define UI_SCANNOW_PAGE_PAD               60
#define UI_SCANNOW_SCANNER_SIZE           1158, 250
#define UI_SCANNOW_PROG_BAR_SIZE          500, 50
#define UI_SCANNOW_FREQ_SIZE              1164, 500
#define UI_CLOCK_COLS                     160, 160, 160, 160, 160, 160, LV_GRID_TEMPLATE_LAST
#define UI_CLOCK_ROWS                     60, 60, 60, 60, 60, 15, 10, 60, 60, 60, LV_GRID_TEMPLATE_LAST
#define UI_CLOCK_DATE_PAD                 8
#define UI_ELRS_COLS                      160, 220, 160, 160, 160, 160, LV_GRID_TEMPLATE_LAST
#define UI_ELRS_ROWS                      60, 60, 60, 60, 60, 40, 40, 60, 60, 60, LV_GRID_TEMPLATE_LAST
#define UI_FANS_COLS                      160, 160, 160, 160, 140, 160, LV_GRID_TEMPLATE_LAST
#define UI_FANS_ROWS                      60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST
#define UI_HT_COLS                        160, 160, 160, 160, 160, 160, LV_GRID_TEMPLATE_LAST
#define UI_HT_ROWS                        55, 55, 55, 55, 55, 55, 60, 30, 40, 40, 40, LV_GRID_TEMPLATE_LAST
#define UI_HT_CALIBRATION_SIZE            500, 25
#define UI_IMAGESETTING_COLS              160, 160, 160, 160, 140, 220, LV_GRID_TEMPLATE_LAST
#define UI_IMAGESETTING_ROWS              60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST
#define UI_INPUT_COLS                     160, 200, 160, 160, 160, 120, LV_GRID_TEMPLATE_LAST
#define UI_INPUT_ROWS                     60, 60, 60, 60, 60, 60, 60, 60, 80, LV_GRID_TEMPLATE_LAST
#define UI_INPUT_OPTION_HEIGHT            300
#define UI_INPUT_DROPDOWN_WIDTH           320
#define UI_OSD_COLS                       160, 180, 160, 160, 120, 160, LV_GRID_TEMPLATE_LAST
#define UI_OSD_ROWS                       60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST
#define UI_OSD_TEXT_FONT                  &lv_font_montserrat_20
#define UI_PLAYBACK_COLS                  320, 320, 320, LV_GRID_TEMPLATE_LAST
#define UI_PLAYBACK_ROWS                  150, 30, 150, 30, 150, 30, 30, LV_GRID_TEMPLATE_LAST
#define UI_POWER_COLS                     160, 200, 160, 160, 120, 160, LV_GRID_TEMPLATE_LAST
#define UI_POWER_ROWS                     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST
#define UI_RECORD_COLS                    160, 200, 200, 160, 120, 120, LV_GRID_TEMPLATE_LAST
#define UI_RECORD_ROWS                    60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST
#define UI_SOURCE_COLS                    160, 160, 200, 160, 160, 160, LV_GRID_TEMPLATE_LAST
#define UI_SOURCE_ROWS                    60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST
#define UI_STORAGE_COLS                   160, 160, 160, 160, 160, 160, LV_GRID_TEMPLATE_LAST
#define UI_STORAGE_ROWS                   60, 60, 60, 60, 60, 60, 60, 40, LV_GRID_TEMPLATE_LAST
#define UI_VERSION_COLS                   160, 160, 160, 160, 160, 160, 160, LV_GRID_TEMPLATE_LAST
#define UI_VERSION_ROWS                   60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST
#define UI_VERSION_RELEASE_NOTES_MIN_SIZE 600
#define UI_VERSION_RELEASE_NOTES_MAX_SIZE 1280
#define UI_VERSION_FIRMWARE_MIN_SIZE      UI_VERSION_RELEASE_NOTES_MIN_SIZE
#define UI_VERSION_FIRMWARE_MAX_SIZE      825
#define UI_VERSION_FIRMWARE_HEIGHT        200
#define UI_VERSION_FIRMWARE_WIDTH_OFFSET  150
#define UI_VERSION_FIRMWARE_MSGBOX_COLS   40, 160, 160, 160, 160, 160, 160, LV_GRID_TEMPLATE_LAST
#define UI_VERSION_FIRMWARE_MSGBOX_ROWS   60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST
#define UI_VERSION_FIRMWARE_MSGBOX_SIZE   600, 40
#define UI_VERSION_FIRMWARE_ALERT_SIZE    125
#define UI_VERSION_PROGRESS_BAR_SIZE      320, 20
#define UI_WIFI_COLS                      160, 160, 160, 180, 160, 160, LV_GRID_TEMPLATE_LAST
#define UI_WIFI_ROWS                      60, 60, 60, 60, 60, 60, 60, 60, 40, LV_GRID_TEMPLATE_LAST

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

#elif HDZBOXPRO

#define UI_MENU_ENTRY_FONT                &lv_font_montserrat_18
#define UI_MENU_LABEL_FONT                &lv_font_montserrat_14
#define UI_MENU_SIZE                      lv_disp_get_hor_res(NULL) - 300, lv_disp_get_ver_res(NULL) - 48
#define UI_MENU_POSITION                  150, 48
#define UI_MENU_ROOT_SIZE                 240, 672
#define UI_MENU_PROG_BAR_SIZE             210, 12
#define UI_PAGE_VIEW_SIZE                 780, 600
#define UI_PAGE_TEXT_SIZE                 210, 40
#define UI_PAGE_TEXT_FONT                 &lv_font_montserrat_18
#define UI_PAGE_TEXT_PAD                  8
#define UI_PAGE_LABEL_FONT                &lv_font_montserrat_14
#define UI_PAGE_LABEL_SIZE(cols)          210 * cols, 40
#define UI_PAGE_LABEL_COMPACT_SIZE(cols)  80 * cols
#define UI_PAGE_INFO_FONT                 &lv_font_montserrat_18
#define UI_PAGE_INFO_SIZE(cols)           210 * cols, 26
#define UI_PAGE_MSG_BOX_SIZE              400
#define UI_PAGE_BUTTON_SIZE               100, 40
#define UI_PAGE_BUTTON_COMPACT_SIZE       80
#define UI_PAGE_SLIDER_SIZE               210, 40
#define UI_PAGE_SLIDER_RANGE_SIZE         210, 2
#define UI_PAGE_SLIDER_GROUP_SIZE         106, 40
#define UI_PAGE_SLIDER_COMPACT_SIZE       130, 26
#define UI_PAGE_SLIDER_COMPACT_GROUP_SIZE 100, 26
#define UI_PAGE_ARROW_SIZE                160, 40
#define UI_PAGE_ARROW_GROUP_SIZE          104, 40
#define UI_PAGE_ARROW_COMPACT_SIZE        160
#define UI_PAGE_ARROW_COMPACT_GROUP_SIZE  106
#define UI_PAGE_DROPDOWN_SIZE             106, 26
#define UI_PLAYER_TEXT_FONT               &lv_font_montserrat_14
#define UI_AUTOSCAN_COLS                  106, 100, 120, 146, 120, 106, LV_GRID_TEMPLATE_LAST
#define UI_AUTOSCAN_ROWS                  40, 40, 40, 40, 40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST
#define UI_SCANNOW_SCANNER_COLS           320, 12, 420, LV_GRID_TEMPLATE_LAST
#define UI_SCANNOW_SCANNER_ROWS           40, 40, 50, LV_GRID_TEMPLATE_LAST
#define UI_SCANNOW_SIGNAL_COLS            80, 52, 52, 120, 60, 52, 52, 120, LV_GRID_TEMPLATE_LAST
#define UI_SCANNOW_SIGNAL_ROWS            40, 40, 40, 40, 40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST
#define UI_SCANNOW_READY_FONT             &lv_font_montserrat_18
#define UI_SCANNOW_READY_PAD              8
#define UI_SCANNOW_NOTE_FONT              &lv_font_montserrat_16
#define UI_SCANNOW_NOTE_PAD               8
#define UI_SCANNOW_CHAN_FONT              &lv_font_montserrat_26
#define UI_SCANNOW_CHAN_PAD               8
#define UI_SCANNOW_PAGE_PAD               40
#define UI_SCANNOW_SCANNER_SIZE           780, 160
#define UI_SCANNOW_PROG_BAR_SIZE          320, 32
#define UI_SCANNOW_FREQ_SIZE              788, 320
#define UI_CLOCK_COLS                     106, 106, 106, 106, 106, 106, LV_GRID_TEMPLATE_LAST
#define UI_CLOCK_ROWS                     40, 40, 40, 40, 40, 10, 6, 40, 40, 40, LV_GRID_TEMPLATE_LAST
#define UI_CLOCK_DATE_PAD                 6
#define UI_ELRS_COLS                      106, 140, 106, 106, 106, 106, LV_GRID_TEMPLATE_LAST
#define UI_ELRS_ROWS                      40, 40, 40, 40, 40, 26, 26, 40, 40, 40, LV_GRID_TEMPLATE_LAST
#define UI_FANS_COLS                      106, 106, 106, 106, 92, 106, LV_GRID_TEMPLATE_LAST
#define UI_FANS_ROWS                      40, 40, 40, 40, 40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST
#define UI_HT_COLS                        106, 106, 106, 106, 106, 106, LV_GRID_TEMPLATE_LAST
#define UI_HT_ROWS                        40, 40, 40, 40, 40, 40, 26, 26, 26, 40, LV_GRID_TEMPLATE_LAST
#define UI_HT_CALIBRATION_SIZE            320, 16
#define UI_IMAGESETTING_COLS              106, 106, 106, 106, 92, 146, LV_GRID_TEMPLATE_LAST
#define UI_IMAGESETTING_ROWS              40, 40, 40, 40, 40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST
#define UI_INPUT_COLS                     106, 130, 106, 106, 106, 80, LV_GRID_TEMPLATE_LAST
#define UI_INPUT_ROWS                     40, 40, 40, 40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST
#define UI_INPUT_OPTION_HEIGHT            200
#define UI_INPUT_DROPDOWN_WIDTH           210
#define UI_OSD_COLS                       106, 120, 106, 106, 80, 106, LV_GRID_TEMPLATE_LAST
#define UI_OSD_ROWS                       40, 40, 40, 40, 40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST
#define UI_OSD_TEXT_FONT                  &lv_font_montserrat_14
#define UI_PLAYBACK_COLS                  210, 210, 210, LV_GRID_TEMPLATE_LAST
#define UI_PLAYBACK_ROWS                  100, 20, 100, 20, 100, 20, 20, LV_GRID_TEMPLATE_LAST
#define UI_POWER_COLS                     106, 132, 106, 106, 80, 106, LV_GRID_TEMPLATE_LAST
#define UI_POWER_ROWS                     40, 40, 40, 40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST
#define UI_RECORD_COLS                    106, 132, 132, 106, 80, 80, LV_GRID_TEMPLATE_LAST
#define UI_RECORD_ROWS                    40, 40, 40, 40, 40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST
#define UI_SOURCE_COLS                    106, 106, 130, 106, 106, 106, LV_GRID_TEMPLATE_LAST
#define UI_SOURCE_ROWS                    40, 40, 40, 40, 40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST
#define UI_STORAGE_COLS                   106, 106, 106, 106, 106, 106, LV_GRID_TEMPLATE_LAST
#define UI_STORAGE_ROWS                   40, 40, 40, 40, 40, 40, 40, 26, LV_GRID_TEMPLATE_LAST
#define UI_VERSION_COLS                   106, 106, 106, 106, 106, 106, LV_GRID_TEMPLATE_LAST
#define UI_VERSION_ROWS                   40, 40, 40, 40, 40, 40, 40, 26, LV_GRID_TEMPLATE_LAST
#define UI_VERSION_RELEASE_NOTES_MIN_SIZE 600
#define UI_VERSION_RELEASE_NOTES_MAX_SIZE 1280
#define UI_VERSION_FIRMWARE_MIN_SIZE      UI_VERSION_RELEASE_NOTES_MIN_SIZE
#define UI_VERSION_FIRMWARE_MAX_SIZE      550
#define UI_VERSION_FIRMWARE_HEIGHT        160
#define UI_VERSION_FIRMWARE_WIDTH_OFFSET  100
#define UI_VERSION_FIRMWARE_MSGBOX_COLS   26, 106, 106, 106, 106, 106, 106, LV_GRID_TEMPLATE_LAST
#define UI_VERSION_FIRMWARE_MSGBOX_ROWS   40, 40, 40, 40, 40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST
#define UI_VERSION_FIRMWARE_MSGBOX_SIZE   400, 26
#define UI_VERSION_FIRMWARE_ALERT_SIZE    82
#define UI_VERSION_PROGRESS_BAR_SIZE      210, 12
#define UI_WIFI_COLS                      106, 106, 106, 120, 106, 106, LV_GRID_TEMPLATE_LAST
#define UI_WIFI_ROWS                      40, 40, 40, 40, 40, 40, 40, 26, LV_GRID_TEMPLATE_LAST

static inline lv_coord_t *UI_STATUS_BAR_COLS() {
    static lv_coord_t pro[] = {176, 42, 178, 42, 178, 42, 178, 42, 178, 42, 178, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t lite[] = {176, 42, 392, 42, 392, 42, 392, LV_GRID_TEMPLATE_LAST};
    return g_setting.has_all_features ? pro : lite;
}

static inline lv_coord_t *UI_STATUS_BAR_ROWS() {
    static lv_coord_t r[] = {32, 32, LV_GRID_TEMPLATE_LAST};
    return r;
}

static inline int UI_STATUS_BAR_LABEL_WIDTH() {
    return g_setting.has_all_features ? 178 : 392;
}

#else

#error "Platform not defined"

#endif

#ifdef __cplusplus
}
#endif
