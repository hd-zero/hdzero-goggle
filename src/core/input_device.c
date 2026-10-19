#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <log/log.h>
#include <minIni.h>

#include "defines.h"
#include "input_device.h"
#include "input_device_internal.h"

#include "common.hh"
#include "ht.h"
#include "osd.h"

#include "core/app_state.h"
#include "core/dvr.h"
#include "core/elrs.h"
#include "core/settings.h"
#include "core/sleep_mode.h"
#include "driver/beep.h"
#include "driver/dm6302.h"
#include "driver/hardware.h"
#include "driver/i2c.h"
#include "driver/it66121.h"
#include "driver/rtc6715.h"
#include "driver/screen.h"
#include "driver/uart.h"
#include "ui/page_common.h"
#include "ui/page_fans.h"
#include "ui/page_headtracker.h"
#include "ui/page_imagesettings.h"
#include "ui/page_playback.h"
#include "ui/page_power.h"
#include "ui/page_scannow.h"
#include "ui/page_source.h"
#include "ui/ui_image_setting.h"
#include "ui/ui_main_menu.h"
#include "ui/ui_osd_element_pos.h"
#include "ui/ui_porting.h"

///////////////////////////////////////////////////////////////////////////////
// Tune channel on video mode
#define TUNER_TIMER_LEN 30

static uint8_t tune_state = 0; // 0=init; 1=waiting for key; 2=tuning
static uint16_t tune_timer = 0;

static pthread_t input_device_pid;

// Shared with the input backends (input_device_internal.h): right button held state.
int btn_value = 0;

// action: 1 = tune up, 2 = tune down, 3 = confirm
void exit_tune_channel() {
    tune_state = 0;
    tune_timer = 0;
    channel_osd_mode = 0;
}

void tune_channel(uint8_t action) {
    static uint8_t channel = 0;

    if (g_setting.ease.no_dial)
        return;

#if defined HDZGOGGLE
    if (g_source_info.source != SOURCE_HDZERO) {
        return;
    }

#elif defined(HDZBOXPRO) || defined(HDZGOGGLE2)
    if (g_source_info.source != SOURCE_HDZERO && g_source_info.source != SOURCE_AV_MODULE) {
        return;
    }
#endif

    LOGI("tune_channel:%d", action);

    if (tune_state == 0) {
        channel_osd_mode = 0;
        tune_state = 1;
    }

    if (tune_state == 1) {
        if ((action == DIAL_KEY_UP) || (action == DIAL_KEY_DOWN)) {
            tune_timer = TUNER_TIMER_LEN;
            tune_state = 2;

#if defined HDZGOGGLE
            channel = g_setting.scan.channel;
#elif defined(HDZBOXPRO) || defined(HDZGOGGLE2)
            if (g_source_info.source == SOURCE_HDZERO) {
                channel = g_setting.scan.channel;
            } else if (g_source_info.source == SOURCE_AV_MODULE) {
                channel = g_setting.source.analog_channel;
            } else {
                return;
            }
#endif
        }
    }

    if (tune_state != 2)
        return;

    uint8_t channel_num;
    if (g_source_info.source == SOURCE_HDZERO)
        channel_num = HDZERO_CHANNEL_NUM;
    else if (g_source_info.source == SOURCE_AV_MODULE)
        channel_num = ANALOG_CHANNEL_NUM;
    else
        return;

    switch (action) {
    case DIAL_KEY_UP: // Tune up
        if (channel == channel_num)
            channel = 1;
        else
            channel++;
        break;

    case DIAL_KEY_DOWN: // Tune down
        if (channel == 1)
            channel = channel_num;
        else
            channel--;
        break;

    case DIAL_KEY_PRESS: // confirm to tune with VTX freq send
    case DIAL_KEY_CLICK: // confirm to tune
        if (g_source_info.source == SOURCE_HDZERO) {
            if (g_setting.scan.channel != channel) {
                g_setting.scan.channel = channel;
                ini_putl("scan", "channel", g_setting.scan.channel, SETTING_INI);
                dvr_cmd(DVR_STOP);
                hdzero_switch_channel(g_setting.scan.channel - 1);
                if (action == DIAL_KEY_PRESS) {
                    msp_channel_update();
                }
            }
        } else if (g_source_info.source == SOURCE_AV_MODULE) {
            if (g_setting.source.analog_channel != channel) {
                g_setting.source.analog_channel = channel;
                ini_putl("source", "analog_channel", g_setting.source.analog_channel, SETTING_INI);
                dvr_cmd(DVR_STOP);
                rtc6715.set_ch(g_setting.source.analog_channel - 1);
                if (action == DIAL_KEY_PRESS) {
                    msp_channel_update();
                }
            }
        }
        tune_timer = 0;
        tune_state = 1;
        channel_osd_mode = CHANNEL_SHOWTIME;
        return;

    default:
        perror("TuneChannel: bad command");
        break;
    }

    channel_osd_mode = 0x80 | channel;
    tune_timer = TUNER_TIMER_LEN;
}

void tune_channel_confirm() {
#if defined HDZGOGGLE
    if (g_source_info.source == SOURCE_HDZERO) {
        tune_channel(DIAL_KEY_CLICK);
    }
#elif defined HDZBOXPRO
    if (g_source_info.source == SOURCE_HDZERO) {
        tune_channel(DIAL_KEY_CLICK);
    } else if (g_source_info.source == SOURCE_AV_MODULE) {
        tune_channel(DIAL_KEY_CLICK);
    }
#elif defined HDZGOGGLE2
    if (g_source_info.source == SOURCE_HDZERO) {
        tune_channel(DIAL_KEY_CLICK);
    } else if (g_source_info.source == SOURCE_AV_MODULE && g_setting.source.analog_module == SETTING_SOURCES_ANALOG_MODULE_INTERNAL) {
        tune_channel(DIAL_KEY_CLICK);
    }
#endif
}

void tune_channel_timer() {
    if (tune_state == 2) {
        if (!tune_timer)
            return;

        if (tune_timer == 1) {
            tune_state = 1;
            channel_osd_mode = CHANNEL_SHOWTIME;
        }
        tune_timer--;
        // LOGI("tune_channel_timer:%d",tune_timer);
    } else {
        if (channel_osd_mode)
            channel_osd_mode--;
    }
}
///////////////////////////////////////////////////////////////////////////////

// Shared with the input backends (input_device_internal.h): the no_dial scroll-
// simulation state, driven by the hardware backend together with rbtn_click().
bool scroll_sim_mode = false;
bool scroll_sim_mode_pending = false;
int scroll_sim_mode_repeat = SCROLL_REPEAT_NONE;

void (*btn_click_callback)() = &osd_toggle;
void (*btn_press_callback)() = &app_switch_to_menu;

void (*rbtn_click_callback)() = &dvr_toggle;
void (*rbtn_press_callback)() = &step_topfan;
void (*rbtn_double_click_callback)() = &ht_set_center_position;

void (*roller_callback)(uint8_t key) = &tune_channel;

void btn_press(void) // long press left key
{
    LOGI("btn_press (%d)", g_app_state);
    if (g_scanning || (g_init_done != 1)) // no long pree Enter before done with init
        return;

    if (g_app_state == APP_STATE_USER_INPUT_DISABLED)
        return;

    pthread_mutex_lock(&lvgl_mutex);

    g_autoscan_exit = true;
    if (g_app_state == APP_STATE_MAINMENU) // Main menu -> Video
    {
        app_exit_menu();
        app_state_push(APP_STATE_VIDEO);
    } else if ((g_app_state == APP_STATE_VIDEO) || (g_app_state == APP_STATE_IMS)) { // video -> Main menu
        if (tune_timer) {
#if defined HDZGOGGLE
            if (g_source_info.source == SOURCE_HDZERO) {
                tune_channel(DIAL_KEY_PRESS);
            } else {
                (*btn_press_callback)();
            }
#elif defined HDZBOXPRO
            if (g_source_info.source == SOURCE_HDZERO) {
                tune_channel(DIAL_KEY_PRESS);
            } else if (g_source_info.source == SOURCE_AV_MODULE) {
                tune_channel(DIAL_KEY_PRESS);
            } else {
                (*btn_press_callback)();
            }

#elif defined HDZGOGGLE2
            if (g_source_info.source == SOURCE_HDZERO) {
                tune_channel(DIAL_KEY_PRESS);
            } else if (g_source_info.source == SOURCE_AV_MODULE) {
                tune_channel(DIAL_KEY_PRESS);
            } else if (g_source_info.source == SOURCE_AV_MODULE && g_setting.source.analog_module == SETTING_SOURCES_ANALOG_MODULE_INTERNAL) {
                tune_channel(DIAL_KEY_PRESS);
            } else {
                (*btn_press_callback)();
            }
#endif
        } else {
            (*btn_press_callback)();
        }
    } else if (g_app_state == APP_STATE_OSD_ELEMENT_PREV) {
        ui_osd_element_pos_cancel_and_hide();
        app_switch_to_menu();
    } else if (g_app_state == APP_STATE_PLAYBACK) {
        pb_key(DIAL_KEY_PRESS);
    } else if (g_app_state == APP_STATE_SLEEP) {
        wake_up();
    } else { // Sub-menu  -> Main menu
        submenu_exit();
        app_state_push(APP_STATE_MAINMENU);
        main_menu_show(true);
    }
    pthread_mutex_unlock(&lvgl_mutex);
}

void btn_click(void) // short press enter key
{
    LOGI("btn_click (%d)", g_app_state);
    if (g_init_done != 1) // no short pree Enter before done with init
        return;

    if (g_app_state == APP_STATE_USER_INPUT_DISABLED)
        return;

    if (g_app_state == APP_STATE_VIDEO) {
        pthread_mutex_lock(&lvgl_mutex);
        if (tune_state == 2) {
            tune_channel_confirm();
        } else {
            (*btn_click_callback)();
        }
        pthread_mutex_unlock(&lvgl_mutex);
        return;
    } else if (g_app_state == APP_STATE_IMS) {
        pthread_mutex_lock(&lvgl_mutex);
        if (ims_key(DIAL_KEY_CLICK))
            app_switch_to_menu();
        pthread_mutex_unlock(&lvgl_mutex);
        return;
    } else if (g_app_state == APP_STATE_OSD_ELEMENT_PREV) {
        pthread_mutex_lock(&lvgl_mutex);
        if (ui_osd_element_pos_handle_input(DIAL_KEY_CLICK))
            app_switch_to_menu();
        pthread_mutex_unlock(&lvgl_mutex);
        return;
    }

    if (!main_menu_is_shown())
        return;

    if (g_scanning)
        return;

    pthread_mutex_lock(&lvgl_mutex);

    autoscan_exit();
    if (g_app_state == APP_STATE_MAINMENU) {
        LOGI("level = 1");
        app_state_push(APP_STATE_SUBMENU);
        submenu_enter();
    } else if (g_app_state == APP_STATE_SUBMENU ||
               g_app_state == APP_STATE_PLAYBACK ||
               g_app_state == APP_STATE_SUBMENU_ITEM_FOCUSED ||
               g_app_state == APP_STATE_WIFI) {
        submenu_click();
    } else if (g_app_state == APP_STATE_SLEEP) {
        wake_up();
    }
    pthread_mutex_unlock(&lvgl_mutex);
}

void rbtn_click(right_button_t click_type) {
    if (g_init_done != 1)
        return;

    if (g_app_state == APP_STATE_USER_INPUT_DISABLED)
        return;

    if (scroll_sim_mode) {
        switch (click_type) {
        case RIGHT_LONG_PRESS:
            if (btn_value) {
                scroll_sim_mode = false;
                scroll_sim_mode_pending = true;
                beep();
            }
            break;
        case RIGHT_CLICK:
            if (scroll_sim_mode_repeat == SCROLL_REPEAT_NONE) {
                roller_up();
            }
            if (btn_value)
                scroll_sim_mode_repeat = SCROLL_REPEAT_UP;
            else
                scroll_sim_mode_repeat = SCROLL_REPEAT_NONE;
            break;
        case RIGHT_DOUBLE_CLICK:
            if (scroll_sim_mode_repeat == SCROLL_REPEAT_NONE) {
                roller_down();
            }
            if (btn_value)
                scroll_sim_mode_repeat = SCROLL_REPEAT_DOWN;
            else
                scroll_sim_mode_repeat = SCROLL_REPEAT_NONE;
            break;
        default:
            break;
        }
    } else if (g_setting.ease.no_dial && btn_value) {
        scroll_sim_mode = true;
        scroll_sim_mode_pending = true;
        beep();
    } else {

        pthread_mutex_lock(&lvgl_mutex);

        switch (g_app_state) {
        case APP_STATE_SUBMENU:
        case APP_STATE_WIFI:
            if (click_type == RIGHT_CLICK)
                submenu_right_button(true);
            else if (click_type == RIGHT_LONG_PRESS)
                submenu_right_button(false);
            break;
        case APP_STATE_VIDEO:
            if (click_type == RIGHT_CLICK) {
                (*rbtn_click_callback)();
            } else if (click_type == RIGHT_LONG_PRESS) {
                (*rbtn_press_callback)();
            } else if (click_type == RIGHT_DOUBLE_CLICK) {
                (*rbtn_double_click_callback)();
            }
            break;
        case APP_STATE_SLEEP:
            wake_up();
            break;
        }

        pthread_mutex_unlock(&lvgl_mutex);
    }
}

void roller_up(void) {
    LOGI("roller up (%d)", g_app_state);

    if (g_scanning)
        return;

    if (g_init_done == 0) // disable roller before init done
        return;

    if (g_app_state == APP_STATE_USER_INPUT_DISABLED)
        return;

    pthread_mutex_lock(&lvgl_mutex);
    autoscan_exit();
    if (g_app_state == APP_STATE_MAINMENU) // main menu
    {
        menu_nav(DIAL_KEY_UP);
    } else if (g_app_state == APP_STATE_SUBMENU ||
               g_app_state == APP_STATE_PLAYBACK ||
               g_app_state == APP_STATE_WIFI) {
        submenu_roller(DIAL_KEY_UP);
    } else if ((g_app_state == APP_STATE_SUBMENU_ITEM_FOCUSED)) {
        submenu_roller_no_selection_change(DIAL_KEY_UP);
    } else if (g_app_state == APP_STATE_VIDEO) {
        (*roller_callback)(DIAL_KEY_UP);
    } else if (g_app_state == APP_STATE_IMS) {
        ims_key(DIAL_KEY_UP);
    } else if (g_app_state == APP_STATE_OSD_ELEMENT_PREV) {
        ui_osd_element_pos_handle_input(DIAL_KEY_UP);
    } else if (g_app_state == APP_STATE_SLEEP) {
        wake_up();
    }
    pthread_mutex_unlock(&lvgl_mutex);
}

void roller_down(void) {
    LOGI("roller down (%d)", g_app_state);

    if (g_scanning)
        return;

    if (g_init_done == 0) // disable roller before init done
        return;

    if (g_app_state == APP_STATE_USER_INPUT_DISABLED)
        return;

    pthread_mutex_lock(&lvgl_mutex);
    autoscan_exit();
    if (g_app_state == APP_STATE_MAINMENU) {
        menu_nav(DIAL_KEY_DOWN);
    } else if (g_app_state == APP_STATE_SUBMENU ||
               g_app_state == APP_STATE_PLAYBACK ||
               g_app_state == APP_STATE_WIFI) {
        submenu_roller(DIAL_KEY_DOWN);
    } else if ((g_app_state == APP_STATE_SUBMENU_ITEM_FOCUSED)) {
        submenu_roller_no_selection_change(DIAL_KEY_DOWN);
    } else if (g_app_state == APP_STATE_VIDEO) {
        (*roller_callback)(DIAL_KEY_DOWN);
    } else if (g_app_state == APP_STATE_IMS) {
        ims_key(DIAL_KEY_DOWN);
    } else if (g_app_state == APP_STATE_OSD_ELEMENT_PREV) {
        ui_osd_element_pos_handle_input(DIAL_KEY_DOWN);
    } else if (g_app_state == APP_STATE_SLEEP) {
        wake_up();
    }
    pthread_mutex_unlock(&lvgl_mutex);
}

void input_device_init() {
    // Bring up whichever input backend was compiled in (evdev on the goggle,
    // SDL in the emulator) and run its read loop on a background thread.
    input_backend_init();
    pthread_create(&input_device_pid, NULL, input_backend_thread, NULL);
}
