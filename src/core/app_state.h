#pragma once

typedef enum {
    APP_STATE_MAINMENU = 0,
    APP_STATE_SUBMENU = 1,
    APP_STATE_PLAYBACK = 2,
    APP_STATE_VIDEO = 10,
    APP_STATE_IMS = 11,

    PAGE_FAN_SLIDE = 100,
    PAGE_ANGLE_SLIDE = 101,
    PAGE_POWER_SLIDE_CELL_COUNT = 102,
    PAGE_POWER_SLIDE_CELL_VOLTAGE = 103
} app_state_t;

extern app_state_t g_app_state;

void app_state_push(app_state_t state);