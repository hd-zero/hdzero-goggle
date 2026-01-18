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
#include <sys/time.h>
#include <unistd.h>

#include <log/log.h>
#include <minIni.h>

#ifdef EMULATOR_BUILD
#include "SDLaccess.h"
#endif

#include "defines.h"
#include "input_device.h"

#include "common.hh"
#include "ht.h"
#include "osd.h"

#include "core/app_state.h"
#include "core/dvr.h"
#include "core/elrs.h"
#include "core/settings.h"
#include "core/sleep_mode.h"
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

static int roller_up_acc = 0;
static int roller_down_acc = 0;

void (*btn_click_callback)() = &osd_toggle;
void (*btn_press_callback)() = &app_switch_to_menu;

void (*rbtn_click_callback)() = &dvr_toggle;
void (*rbtn_press_callback)() = &step_topfan;
void (*rbtn_double_click_callback)() = &ht_set_center_position;

void (*roller_callback)(uint8_t key) = &tune_channel;

static void btn_press(void) // long press left key
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

static void btn_click(void) // short press enter key
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

static void roller_up(void) {
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

static void roller_down(void) {
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

static void get_event(int fd) {
    struct input_event event;
    static int event_type_last = 0;
    static int btn_value = 0;
    static int btn_press_time = 0;

    static int roller_value = 0;

    // time (sec, usec) difference above which will the next scroll wheel event be accepted
    // 10000 usec = 10msec is more than short enough (100Hz)
    // scroll events
    const struct timeval scroll_time_diff = {0, 10000};
    // direction change events
    const struct timeval rel_time_diff = {0, 20000};
    // expected timestamp in the future beyond that will the events be accepted
    static struct timeval next_scroll = {0, 0};
    static struct timeval next_rel = {0, 0};
    static bool discard_scroll = false;

    read(fd, &event, sizeof(event));

    switch (event.type) {
    case EV_SYN:
        if (event.code == SYN_REPORT) {
            if (event_type_last == EV_REL) {
                if (!discard_scroll && timercmp(&event.time, &next_scroll, >)) {
                    timeradd(&event.time, &scroll_time_diff, &next_scroll);
                    if (roller_value == 1) {
                        roller_up_acc++;
                        roller_down_acc = 0;
                    } else if (roller_value == -1) {
                        roller_down_acc++;
                        roller_up_acc = 0;
                    }

                    if (roller_up_acc == DIAL_SENSITIVITY) {
                        roller_up();
                        g_key = DIAL_KEY_UP;
                        roller_up_acc = 0;
                    } else if (roller_down_acc == DIAL_SENSITIVITY) {
                        roller_down();
                        g_key = DIAL_KEY_DOWN;
                        roller_down_acc = 0;
                    }
                } else {
                    // LOGI("discard EV_SYN");
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
        if (timercmp(&event.time, &next_rel, >)) {
            discard_scroll = false;
            if (event.code == REL_X) {
                // LOGI("x = %d", event.value);
            } else if (event.code == REL_Y) {
                roller_value = event.value;
                // LOGI("y = %d", event.value);
            }
            event_type_last = EV_REL;
        } else {
            discard_scroll = true;
            LOGI("discard EV_REL");
        }
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

        int ret = epoll_wait(epfd, events, EPOLL_FD_CNT, DIAL_SENSITIVTY_TIMEOUT_MS);
        if (ret < 0) {
            perror("epoll_wait");
            continue;
        }

        if (ret > 0) {
            for (int i = 0; i < ret; i++) {
                if (events[i].events & EPOLLIN) {
                    get_event(events[i].data.fd);
                }
            }
        } else {
            roller_up_acc = 0;
            roller_down_acc = 0;
        }
    }
    return NULL;
#else
    static uint32_t btn_d_start = 0;
    static uint32_t btn_a_start = 0;

    while (true) {
        SDL_Event event;
        SDL_LockMutex(global_sdl_mutex);
        while (SDL_PollEvent(&event)) {
            SDL_UnlockMutex(global_sdl_mutex);
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
                        rbtn_click(RIGHT_LONG_PRESS);
                        g_key = RIGHT_KEY_PRESS;
                    } else {
                        rbtn_click(RIGHT_CLICK);
                        g_key = RIGHT_KEY_CLICK;
                    }
                    btn_a_start = 0;
                    break;
                }
                break;
            }
            SDL_LockMutex(global_sdl_mutex);
        }
        SDL_UnlockMutex(global_sdl_mutex);
        usleep(50000); // Sorry, this will break windows, but it's not like it is working now anyway :-(
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
#else
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
    }
#endif
    pthread_create(&input_device_pid, NULL, thread_input_device, NULL);
}
