#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <unistd.h>

#include <log/log.h>
#include <minIni.h>

#ifdef EMULATOR_BUILD
#include <SDL2/SDL.h>
#endif

#include "defines.h"
#include "input_device.h"

#include "common.hh"
#include "ht.h"
#include "osd.h"

#include "core/app_state.h"
#include "core/dvr.h"
#include "core/settings.h"
#include "driver/dm6302.h"
#include "driver/hardware.h"
#include "driver/i2c.h"
#include "driver/it66121.h"
#include "driver/oled.h"
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

#define EPOLL_FD_CNT 4

static int epfd;
static pthread_t input_device_pid;

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

    LOGI("tune_channel:%d", action);

    if (tune_state == 0) {
        channel_osd_mode = 0;
        tune_state = 1;
    }

    if (tune_state == 1) {
        if ((action == DIAL_KEY_UP) || (action == DIAL_KEY_DOWN)) {
            tune_timer = TUNER_TIMER_LEN;
            tune_state = 2;
            channel = g_setting.scan.channel;
        } else if (action == DIAL_KEY_CLICK) {
            g_showRXOSD = !g_showRXOSD;
            if (g_showRXOSD)
                channel_osd_mode = CHANNEL_SHOWTIME;
        }
    }

    if (tune_state != 2)
        return;

    switch (action) {
    case DIAL_KEY_UP: // Tune up
        if (channel == 10)
            channel = 1;
        else
            channel++;
        break;

    case DIAL_KEY_DOWN: // Tune down
        if (channel == 1)
            channel = 10;
        else
            channel--;
        break;

    case DIAL_KEY_CLICK: // confirm to tune
        if (g_setting.scan.channel != channel) {
            g_setting.scan.channel = channel;
            ini_putl("scan", "channel", g_setting.scan.channel, SETTING_INI);
            dvr_cmd(DVR_STOP);
            app_switch_to_hdzero(true);
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

static void btn_press(void) // long press left key
{
    LOGI("btn_press (%d)", g_app_state);
    if (g_scanning || !g_init_done)
        return;

    if (g_app_state == APP_STATE_USER_INPUT_DISABLED)
        return;

    pthread_mutex_lock(&lvgl_mutex);

    g_autoscan_exit = true;
    if (g_app_state == APP_STATE_MAINMENU) // Main menu -> Video
    {
        switch (g_source_info.source) {
        case SOURCE_HDZERO:
            progress_bar.start = 1;
            app_switch_to_hdzero(true);
            break;
        case SOURCE_HDMI_IN:
            app_switch_to_hdmi_in();
            break;
        case SOURCE_AV_IN:
            app_switch_to_analog(0);
            break;
        case SOURCE_EXPANSION:
            app_switch_to_analog(1);
            break;
        }
        app_state_push(APP_STATE_VIDEO);
    } else if ((g_app_state == APP_STATE_VIDEO) || (g_app_state == APP_STATE_IMS)) // video -> Main menu
        app_switch_to_menu();
    else if (g_app_state == APP_STATE_OSD_ELEMENT_PREV) {
        ui_osd_element_pos_cancel_and_hide();
        app_switch_to_menu();
    } else if (g_app_state == APP_STATE_PLAYBACK)
        pb_key(DIAL_KEY_PRESS);
    else { // Sub-menu  -> Main menu
        submenu_exit();
        app_state_push(APP_STATE_MAINMENU);
        main_menu_show(true);
    }
    pthread_mutex_unlock(&lvgl_mutex);
}

static void btn_click(void) // short press enter key
{
    LOGI("btn_click (%d)", g_app_state);
    if (!g_init_done)
        return;

    if (g_app_state == APP_STATE_USER_INPUT_DISABLED)
        return;

    if (g_app_state == APP_STATE_VIDEO) {
        pthread_mutex_lock(&lvgl_mutex);
        tune_channel(DIAL_KEY_CLICK);
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
               g_app_state == APP_STATE_WIFI ||
               g_app_state == APP_STATE_SUBMENU_ITEM_FOCUSED ||
               g_app_state == PAGE_FAN_SLIDE ||
               g_app_state == PAGE_ANGLE_SLIDE ||
               g_app_state == PAGE_POWER_SLIDE_CELL_COUNT ||
               g_app_state == PAGE_POWER_SLIDE_CELL_VOLTAGE) {
        submenu_click();
    }
    pthread_mutex_unlock(&lvgl_mutex);
}

void rbtn_click(right_button_t click_type) {
    if (g_init_done != 1)
        return;

    if (g_app_state == APP_STATE_USER_INPUT_DISABLED)
        return;

    switch (g_app_state) {
    case APP_STATE_SUBMENU:
    case APP_STATE_WIFI:
        pthread_mutex_lock(&lvgl_mutex);
        if (click_type == RIGHT_CLICK)
            submenu_right_button(true);
        else if (click_type == RIGHT_LONG_PRESS)
            submenu_right_button(false);
        pthread_mutex_unlock(&lvgl_mutex);
        break;
    case APP_STATE_VIDEO:
        if (click_type == RIGHT_CLICK) {
            dvr_cmd(DVR_TOGGLE);
        } else if (click_type == RIGHT_LONG_PRESS) {
            step_topfan();
        } else if (click_type == RIGHT_DOUBLE_CLICK) {
            ht_set_center_position();
        }
        break;
    }
}

static void roller_up(void) {
    LOGI("roller up (%d)", g_app_state);

    if (g_scanning)
        return;

    if (g_init_done == 0)
        g_init_done = -1;

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
        if (g_source_info.source == SOURCE_HDZERO)
            tune_channel(DIAL_KEY_UP);
    } else if (g_app_state == APP_STATE_IMS) {
        ims_key(DIAL_KEY_UP);
    } else if (g_app_state == APP_STATE_OSD_ELEMENT_PREV) {
        ui_osd_element_pos_handle_input(DIAL_KEY_UP);
    } else if (g_app_state == PAGE_FAN_SLIDE) {
        fans_speed_dec();
    } else if (g_app_state == PAGE_ANGLE_SLIDE) {
        ht_angle_dec();
    } else if (g_app_state == PAGE_POWER_SLIDE_CELL_COUNT) {
        power_cell_count_dec();
    } else if (g_app_state == PAGE_POWER_SLIDE_CELL_VOLTAGE) {
        power_voltage_dec();
    }
    pthread_mutex_unlock(&lvgl_mutex);
}

static void roller_down(void) {
    LOGI("roller down (%d)", g_app_state);

    if (g_scanning)
        return;

    if (g_init_done == 0)
        g_init_done = -1;

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
        if (g_source_info.source == SOURCE_HDZERO)
            tune_channel(DIAL_KEY_DOWN);
    } else if (g_app_state == APP_STATE_IMS) {
        ims_key(DIAL_KEY_DOWN);
    } else if (g_app_state == APP_STATE_OSD_ELEMENT_PREV) {
        ui_osd_element_pos_handle_input(DIAL_KEY_DOWN);
    } else if (g_app_state == PAGE_FAN_SLIDE) {
        fans_speed_inc();
    } else if (g_app_state == PAGE_ANGLE_SLIDE) {
        ht_angle_inc();
    } else if (g_app_state == PAGE_POWER_SLIDE_CELL_COUNT) {
        power_cell_count_inc();
    } else if (g_app_state == PAGE_POWER_SLIDE_CELL_VOLTAGE) {
        power_voltage_inc();
    }

    pthread_mutex_unlock(&lvgl_mutex);
}

static void get_event(int fd) {
    struct input_event event;
    static int roller_value = 0;
    static int event_type_last = 0;
    static int btn_value = 0;
    static int btn_press_time = 0;

    read(fd, &event, sizeof(event));

    switch (event.type) {
    case EV_SYN:
        if (event.code == SYN_REPORT) {

            if (event_type_last == EV_REL) {
                if (roller_value == 1) {
                    roller_up();
                    g_key = DIAL_KEY_UP;
                } else {
                    roller_down();
                    g_key = DIAL_KEY_DOWN;
                }
            } else if (event_type_last == EV_KEY) {
                if (btn_value) {
                    if (btn_press_time == 10) {
                        btn_press();
                        g_key = DIAL_KEY_PRESS;
                    }
                    btn_press_time++;
                    // LOGI("btn down");
                } else {
                    if (btn_press_time < 10) {
                        btn_click();
                        g_key = DIAL_KEY_CLICK;
                    }
                    // else if(btn_press_time > 200){
                    //	btn_super_press();
                    // }
                    btn_press_time = 0;
                }
            }
            // LOGI("------------ syn report ----------");
        } else if (event.code == SYN_MT_REPORT) {
            // LOGI("----------- syn mt report ------------");
        }
        break;
    case EV_KEY:
        // LOGI("key code%d is %s!", event.code, event.value?"down":"up");
        btn_value = event.value;
        event_type_last = EV_KEY;
        break;
    case EV_ABS:
        if ((event.code == ABS_X) ||
            (event.code == ABS_MT_POSITION_X)) {
            // LOGI("abs,x = %d", event.value);
        } else if ((event.code == ABS_Y) ||
                   (event.code == ABS_MT_POSITION_Y)) {
            // LOGI("abs,y = %d", event.value);
        } else if ((event.code == ABS_PRESSURE) ||
                   (event.code == ABS_MT_PRESSURE)) {
            // LOGI("pressure value: %d", event.value);
        }
        break;
    case EV_REL:
        if (event.code == REL_X) {
            // LOGI("x = %d", event.value);
        } else if (event.code == REL_Y) {
            roller_value = event.value;
            // LOGI("y = %d", event.value);
        }
        event_type_last = EV_REL;
        break;
    default:
        // LOGI("unknown [type=%d, code=%d value=%d]", event.type, event.code, event.value);
        break;
    }
}

static void add_to_epfd(int epfd, int fd) {
    struct epoll_event event = {
        .events = EPOLLIN,
        .data = {
            .fd = fd,
        },
    };

    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    assert(ret == 0);
}

static void *thread_input_device(void *ptr) {
#ifndef EMULATOR_BUILD
    for (;;) {
        struct epoll_event events[EPOLL_FD_CNT];

        int ret = epoll_wait(epfd, events, EPOLL_FD_CNT, -1);
        if (ret < 0) {
            perror("epoll_wait");
            continue;
        }

        for (int i = 0; i < ret; i++) {
            if (events[i].events & EPOLLIN) {
                get_event(events[i].data.fd);
            }
        }

        // enable dial up/down accessible before initialization is done
        if (g_init_done != 1) {
            pthread_mutex_lock(&lvgl_mutex);
            lv_timer_handler();
            pthread_mutex_unlock(&lvgl_mutex);
        }
    }
    return NULL;
#else
    static uint32_t btn_d_start = 0;
    static uint32_t btn_a_start = 0;

    while (true) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                exit(0);

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_d:
                    if (!btn_d_start) {
                        btn_d_start = event.key.timestamp;
                    }
                    break;

                case SDLK_a:
                    if (!btn_a_start) {
                        btn_a_start = event.key.timestamp;
                    }
                    break;
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_s:
                    roller_up();
                    g_key = DIAL_KEY_UP;
                    break;

                case SDLK_w:
                    roller_down();
                    g_key = DIAL_KEY_DOWN;
                    break;

                case SDLK_d:
                    if (event.key.timestamp - btn_d_start > 500) {
                        btn_press();
                        g_key = DIAL_KEY_PRESS;
                    } else {
                        btn_click();
                        g_key = DIAL_KEY_CLICK;
                    }
                    btn_d_start = 0;
                    break;

                case SDLK_a:
                    if (event.key.timestamp - btn_a_start > 500) {
                        rbtn_click(false);
                        g_key = RIGHT_KEY_PRESS;
                    } else {
                        rbtn_click(true);
                        g_key = RIGHT_KEY_CLICK;
                    }
                    btn_a_start = 0;
                    break;
                }
                break;
            }
        }
    }
#endif
}

void input_device_init() {
#ifndef EMULATOR_BUILD
    epfd = epoll_create(EPOLL_FD_CNT);
    assert(epfd > 0);

    char buf[64];
    for (int i = 0; i < EPOLL_FD_CNT; i++) {
        snprintf(buf, 64, "/dev/input/event%d", i);

        int fd = open(buf, O_RDONLY);
        if (fd >= 0) {
            add_to_epfd(epfd, fd);
            LOGI("opened %s", buf);
        }
    }
    app_state_push(APP_STATE_MAINMENU);
#endif
    pthread_create(&input_device_pid, NULL, thread_input_device, NULL);
}