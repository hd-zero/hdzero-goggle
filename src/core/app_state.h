#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef enum {
    APP_STATE_MAINMENU = 0,
    APP_STATE_SUBMENU = 1,
    APP_STATE_PLAYBACK = 2,

    // in this state, the menu pages' on_roller is called,
    // but the selected submenu item selection (e.g. pp_osd.p_arr.cur)
    // is not automatically changed.
    APP_STATE_SUBMENU_ITEM_FOCUSED = 3,

    APP_STATE_VIDEO = 10,

    // the preview for image settings
    APP_STATE_IMS = 11,
    // the preview for osd element positioning settings
    APP_STATE_OSD_ELEMENT_PREV = 12,

    // WiFi configuration page
    APP_STATE_WIFI = 13,

    APP_STATE_USER_INPUT_DISABLED = 20,

    APP_STATE_SLEEP = 30,
} app_state_t;

extern app_state_t g_app_state;

void app_state_push(app_state_t state);

void app_switch_to_menu();
void app_exit_menu();
void app_switch_to_analog(bool is_bay);
void app_switch_to_hdmi_in();
void app_switch_to_hdzero(bool is_default);

#ifdef __cplusplus
}
#endif
