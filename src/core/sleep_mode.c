#include "sleep_mode.h"

#include "core/app_state.h"
#include "core/common.hh"
#include "core/dvr.h"
#include "core/settings.h"
#include "driver/beep.h"
#include "driver/dm5680.h"
#include "driver/fans.h"
#include "driver/hardware.h"
#include "driver/rtc6715.h"
#include "log/log.h"
#include "ui/page_fans.h"

static app_state_t previousState;
static int fans_auto_mode_save;
static fan_speed_t fan_speed_save;

static uint16_t beepCnt = 0;

bool isSleeping = false;

void go_sleep() {
    LOGI("Entering sleep mode");
    previousState = g_app_state;
    app_state_push(APP_STATE_SLEEP);

    // Stop DVR
    dvr_cmd(DVR_STOP);

    if (TARGET_GOGGLE == getTargetType()) {
        dvr_update_vi_conf(VR_1080P30);
    } else if (TARGET_BOXPRO == getTargetType()) {
        dvr_update_vi_conf(VR_720P60);
    }

    Screen_ON(0);

    // Turn off HDZero Receiver
    HDZero_Close();

    switch (getTargetType()) {
    case TARGET_GOGGLE:
        // Turn off Analog Receiver  -- Batch 2 goggles only
        if (getHwRevision() == HW_REV_2) {
            DM5680_Power_AnalogModule(1);
        }
        break;
    case TARGET_BOXPRO:
        // Turn off Analog Receiver
        RTC6715_Open(0, 0);
        break;
    }

    // Minimum fan
    fans_auto_mode_save = g_setting.fans.auto_mode;
    fan_speed_save.top = fan_speed.top;
    fan_speed_save.left = fan_speed.left;
    fan_speed_save.right = fan_speed.right;
    g_setting.fans.top_speed = MIN_FAN_TOP;
    g_setting.fans.left_speed = MIN_FAN_SIDE;
    g_setting.fans.right_speed = MIN_FAN_SIDE;
    g_setting.fans.auto_mode = 0;
    fans_top_setspeed(MIN_FAN_TOP);

    if (TARGET_GOGGLE == getTargetType()) {
        fans_left_setspeed(MIN_FAN_SIDE);
        fans_right_setspeed(MIN_FAN_SIDE);
    }

    isSleeping = true;
    beepCnt = 0;
}

void wake_up() {
    LOGI("Exiting sleep mode");
    isSleeping = false;
    Screen_ON(1); // Turn on display

    if (TARGET_GOGGLE == getTargetType()) {
        Analog_Module_Power(1);
        g_setting.fans.right_speed = fan_speed_save.right;
        g_setting.fans.left_speed = fan_speed_save.left;
        fans_right_setspeed(fan_speed_save.right);
        fans_left_setspeed(fan_speed_save.left);
    }

    g_setting.fans.top_speed = fan_speed_save.top;
    g_setting.fans.auto_mode = fans_auto_mode_save;
    fans_top_setspeed(fan_speed_save.top);

    app_state_push(previousState);
    if (previousState == APP_STATE_SUBMENU) {
        submenu_exit();
    } else if (previousState == APP_STATE_VIDEO) {
        app_switch_to_menu();          // Necessary to display the progress bar
        app_state_push(previousState); // Because app_switch_to_menu() pushes main menu state
        app_exit_menu();
    }
}

void sleep_reminder() {
    if (isSleeping == false) {
        return;
    }

#define BEEP_INTERVAL 4000
    if (++beepCnt == BEEP_INTERVAL) {
        beep_dur(BEEP_VERY_SHORT);
        beepCnt = 0;
    }
}
