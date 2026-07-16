// Goggle hardware input backend: reads the jog wheel + centre button from the
// Linux evdev devices (/dev/input/event*) via epoll and feeds them to the shared
// handlers in core/input_device.c. Compiled only for the real goggle -- CMake drops
// this file for EMULATOR_BUILD and swaps in src/emulator/input_device_emu.c. (The
// right-side function button arrives separately over UART, in driver/dm5680.c.)

#include <assert.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <unistd.h>

#include <log/log.h>

#include "defines.h"

#include "common.hh"

#include "core/app_state.h"
#include "core/input_device_internal.h"
#include "core/settings.h"
#include "driver/beep.h"

#define EPOLL_FD_CNT 4

static int epfd;

static int roller_up_acc = 0;
static int roller_down_acc = 0;

static void get_event(int fd) {
    struct input_event event;
    static int event_type_last = 0;
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
                if (g_setting.ease.no_dial)
                    break;

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
                    if (!g_setting.ease.no_dial) {
                        if (btn_press_time == 10) {
                            btn_press();
                            g_key = DIAL_KEY_PRESS;
                        }
                    } else {
                        if (scroll_sim_mode_repeat == SCROLL_REPEAT_DOWN) {
                            roller_down();
                        } else if (scroll_sim_mode_repeat == SCROLL_REPEAT_UP) {
                            roller_up();
                        }
                    }
                    if (scroll_sim_mode_pending)
                        btn_press_time = 0;
                    else
                        btn_press_time++;
                    // LOGI("btn down");
                } else {
                    if (scroll_sim_mode_pending) {
                        scroll_sim_mode_pending = false;
                    } else {
                        if (scroll_sim_mode_repeat == SCROLL_REPEAT_NONE) {
                            if (btn_press_time < 10) {
                                btn_click();
                                g_key = DIAL_KEY_CLICK;
                            } else if (g_setting.ease.no_dial) {
                                if (btn_press_time < 50) {
                                    btn_press();
                                    g_key = DIAL_KEY_PRESS;
                                }
                                // else if(btn_press_time > 200){
                                //	btn_super_press();
                                // }
                            }
                        }
                    }
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
                if (roller_value != event.value) {
                    timeradd(&event.time, &rel_time_diff, &next_rel);
                    roller_value = event.value;
                    // LOGI("y = %d", event.value);
                }
                event_type_last = EV_REL;
            }
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

void input_backend_init(void) {
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
}

void *input_backend_thread(void *ptr) {
    (void)ptr;
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
            if (scroll_sim_mode_repeat != SCROLL_REPEAT_NONE)
                beep();
        }
    }
    return NULL;
}
