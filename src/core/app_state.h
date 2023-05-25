#pragma once

#include <stdbool.h>

typedef enum {
    APP_STATE_MAINMENU = 0,
    APP_STATE_SUBMENU = 1,
    APP_STATE_PLAYBACK = 2,
    APP_STATE_VIDEO = 10,
    APP_STATE_IMS = 11,
    APP_STATE_WIFI = 12,

    APP_STATE_USER_INPUT_DISABLED = 20,

    PAGE_FAN_SLIDE = 100,
    PAGE_ANGLE_SLIDE = 101,
    PAGE_POWER_SLIDE_CELL_COUNT = 102,
    PAGE_POWER_SLIDE_CELL_VOLTAGE = 103
} app_state_t;

extern app_state_t g_app_state;

void app_state_push(app_state_t state);

void app_switch_to_menu();
void app_switch_to_analog(bool is_bay);
void app_switch_to_hdmi_in();
void app_switch_to_hdzero(bool is_default);