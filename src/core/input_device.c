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
#include "scroll_filter.h"
#if defined(HDZGOGGLE)
#include "input_drain.h"
#include "scroll_burst.h"
#include "scroll_frame.h"
#endif

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

#define EPOLL_FD_CNT 4

static int epfd;
static pthread_t input_device_pid;

static int btn_value = 0;

#if defined(HDZGOGGLE)
// Per opened /dev/input/eventN: source-owned frame latch, burst/µmaj, 10ms
// gate, and dial-button hold counter. event1 must never mutate event0.
typedef struct {
    int fd;
    int eventn;
    scroll_frame_t frame;
    scroll_burst_t burst;
    struct timeval next_scroll;
    scroll_btn_t btn;
} scroll_src_t;
static scroll_src_t scroll_srcs[EPOLL_FD_CNT];
static int scroll_src_count;

// Secondary 10ms safety gate on burst candidates only (NOT before burst).
// Per-source: a candidate on event0 must not close event1's gate.
// Uses input_event.time / emit_event_time. With QUIET=20ms, successive
// episode decision times are typically >= ~25ms apart, so this gate should
// not suppress valid new episodes; retained for Phase 2 hardware validation.
static const struct timeval goggle1_scroll_time_diff = {0, 10000};

// CLOCK: input_event.time is CLOCK_REALTIME (evdev default). gettimeofday()
// wake estimates share that domain. EVIOCSCLOCKID/CLOCK_MONOTONIC is a
// deferred follow-up — not required to fix the orphan-SYN button bug.
#endif

// Temporary Goggle-1 scroll diagnostics (compile with -DSCROLL_DIAG=1).
// Logging only -- no changes to throttle, filter, or accumulator decisions.
#if defined(SCROLL_DIAG)
#if !defined(HDZGOGGLE)
#error "SCROLL_DIAG is supported only for HDZGOGGLE builds"
#endif

static int scroll_diag_sign(int value) {
    return (value > 0) ? 1 : (value < 0) ? -1 : 0;
}

static void scroll_diag_fmt_tv(char *buf, size_t buflen, const struct timeval *tv) {
    snprintf(buf, buflen, "%ld.%06ld", (long)tv->tv_sec, (long)tv->tv_usec);
}

static long scroll_diag_tv_diff_ms(const struct timeval *a, const struct timeval *b) {
    long sec_diff = (long)a->tv_sec - (long)b->tv_sec;
    long usec_diff = (long)a->tv_usec - (long)b->tv_usec;
    return sec_diff * 1000L + usec_diff / 1000L;
}

static const char *scroll_diag_filter_out_name(scroll_filter_event_t e) {
    switch (e) {
    case SCROLL_FILTER_UP:
        return "UP";
    case SCROLL_FILTER_DOWN:
        return "DOWN";
    default:
        return "NONE";
    }
}
#endif

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

#if defined(HDZGOGGLE)
// Original Goggle 1 rotary encoders can wear and produce duplicate, skipped,
// or short opposite-direction glitch pulses. This additional stateful layer
// sits between the timing protection below and the accumulator above, and
// only applies to this target -- see scroll_filter.h.
static scroll_filter_t goggle1_scroll_filter;
#endif

static bool scroll_sim_mode = false;
static bool scroll_sim_mode_pending = false;

#define SCROLL_REPEAT_NONE 0
#define SCROLL_REPEAT_UP   1
#define SCROLL_REPEAT_DOWN 2
static int scroll_sim_mode_repeat = SCROLL_REPEAT_NONE;

void (*btn_click_callback)() = &osd_toggle;
void (*btn_press_callback)() = &app_switch_to_menu;

void (*rbtn_click_callback)() = &dvr_toggle;
void (*rbtn_press_callback)() = &step_topfan;
void (*rbtn_double_click_callback)() = &ht_set_center_position;

void (*roller_callback)(uint8_t key) = &tune_channel;

static void roller_up(void);
static void roller_down(void);

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

#if defined(HDZGOGGLE)
// Apply one burst-gate candidate: secondary next_scroll, then scroll_filter, then acc.
static void goggle1_apply_scroll_candidate(unsigned src_idx,
                                           const struct timeval *emit_event_time,
                                           int candidate) {
    int fd = scroll_srcs[src_idx].fd;
    int eventn = scroll_srcs[src_idx].eventn;
#if !defined(SCROLL_DIAG)
    (void)fd;
    (void)eventn;
#endif
#if defined(SCROLL_DIAG)
    char ts_ev_buf[32];
    char ts_wall_buf[32];
    char ts_next_buf[32];
#endif

    if (g_setting.ease.no_dial)
        return;

    struct timeval *next_scroll = &scroll_srcs[src_idx].next_scroll;

    if (!timercmp(emit_event_time, next_scroll, >)) {
#if defined(SCROLL_DIAG)
        {
            struct timeval last_accept;
            struct timeval gate_remain;
            long since_accept_ms;
            long gate_remain_ms;

            timersub(next_scroll, &goggle1_scroll_time_diff, &last_accept);
            timersub(next_scroll, emit_event_time, &gate_remain);
            since_accept_ms = scroll_diag_tv_diff_ms(emit_event_time, &last_accept);
            gate_remain_ms = gate_remain.tv_sec * 1000L + gate_remain.tv_usec / 1000L;

            scroll_diag_fmt_tv(ts_ev_buf, sizeof(ts_ev_buf), emit_event_time);
            scroll_diag_fmt_tv(ts_next_buf, sizeof(ts_next_buf), next_scroll);
            LOGI("SCROLL_DROP10 eventN=%d fd=%d ts_ev=%s next_scroll=%s since_accept_ms=%ld gate_remain_ms=%ld val=%d sign=%d",
                 eventn, fd, ts_ev_buf, ts_next_buf, since_accept_ms, gate_remain_ms,
                 candidate, scroll_diag_sign(candidate));
        }
#endif
        return;
    }

    timeradd(emit_event_time, &goggle1_scroll_time_diff, next_scroll);

    int filtered_value = candidate;
#if defined(SCROLL_DIAG)
    scroll_diag_fmt_tv(ts_ev_buf, sizeof(ts_ev_buf), emit_event_time);
    LOGI("SCROLL_BURST_EMIT eventN=%d fd=%d ts_ev=%s val=%d sign=%d",
         eventn, fd, ts_ev_buf, candidate, scroll_diag_sign(candidate));

    int dir_before = goggle1_scroll_filter.dir;
    int pend_before = goggle1_scroll_filter.pending_count;
    long idle_ms = -1;
    int idle_reset = 0;
    if (goggle1_scroll_filter.have_last_event_time) {
        idle_ms = scroll_diag_tv_diff_ms(emit_event_time, &goggle1_scroll_filter.last_event_time);
        if (idle_ms < 0 || idle_ms > 160)
            idle_reset = 1;
    }
    scroll_filter_event_t filter_out;
    filter_out = scroll_filter_step(&goggle1_scroll_filter, emit_event_time, candidate);
    switch (filter_out) {
#else
    switch (scroll_filter_step(&goggle1_scroll_filter, emit_event_time, candidate)) {
#endif
    case SCROLL_FILTER_UP:
        filtered_value = 1;
        break;
    case SCROLL_FILTER_DOWN:
        filtered_value = -1;
        break;
    default:
        filtered_value = 0;
        break;
    }
#if defined(SCROLL_DIAG)
    LOGI("SCROLL_FILTER eventN=%d fd=%d in=%d out=%s dir_before=%d dir_after=%d pend_before=%d pend_after=%d idle_ms=%ld reset=%d",
         eventn, fd, scroll_diag_sign(candidate), scroll_diag_filter_out_name(filter_out),
         dir_before, goggle1_scroll_filter.dir, pend_before, goggle1_scroll_filter.pending_count,
         idle_ms, idle_reset);
#endif

    if (filtered_value == 1) {
        roller_up_acc++;
        roller_down_acc = 0;
    } else if (filtered_value == -1) {
        roller_down_acc++;
        roller_up_acc = 0;
    }

#if defined(SCROLL_DIAG)
    if (filtered_value != 0) {
        LOGI("SCROLL_ACC up=%d down=%d sens=%d",
             roller_up_acc, roller_down_acc, DIAL_SENSITIVITY);
    }
#endif

    if (roller_up_acc == DIAL_SENSITIVITY) {
#if defined(SCROLL_DIAG)
        {
            struct timeval wall_now;
            gettimeofday(&wall_now, NULL);
            scroll_diag_fmt_tv(ts_ev_buf, sizeof(ts_ev_buf), emit_event_time);
            scroll_diag_fmt_tv(ts_wall_buf, sizeof(ts_wall_buf), &wall_now);
            LOGI("SCROLL_LOGICAL dir=UP ts_ev=%s ts_wall=%s eventN=%d fd=%d app_state=%d",
                 ts_ev_buf, ts_wall_buf, eventn, fd, (int)g_app_state);
        }
#endif
        roller_up();
        g_key = DIAL_KEY_UP;
        roller_up_acc = 0;
    } else if (roller_down_acc == DIAL_SENSITIVITY) {
#if defined(SCROLL_DIAG)
        {
            struct timeval wall_now;
            gettimeofday(&wall_now, NULL);
            scroll_diag_fmt_tv(ts_ev_buf, sizeof(ts_ev_buf), emit_event_time);
            scroll_diag_fmt_tv(ts_wall_buf, sizeof(ts_wall_buf), &wall_now);
            LOGI("SCROLL_LOGICAL dir=DOWN ts_ev=%s ts_wall=%s eventN=%d fd=%d app_state=%d",
                 ts_ev_buf, ts_wall_buf, eventn, fd, (int)g_app_state);
        }
#endif
        roller_down();
        g_key = DIAL_KEY_DOWN;
        roller_down_acc = 0;
    }
}

static void goggle1_handle_burst_result(unsigned src_idx, const scroll_burst_result_t *r) {
    if (r->action != SCROLL_BURST_EMIT)
        return;
    goggle1_apply_scroll_candidate(src_idx, &r->emit_event_time, r->candidate);
}

static void goggle1_poll_src_due(unsigned src_idx) {
    struct timeval wall;
    gettimeofday(&wall, NULL);
    scroll_burst_result_t r;
    scroll_burst_poll_due(&scroll_srcs[src_idx].burst, &wall, &r);
    goggle1_handle_burst_result(src_idx, &r);
}

static void goggle1_poll_all_due(void) {
    for (int i = 0; i < scroll_src_count; i++)
        goggle1_poll_src_due((unsigned)i);
}

static int goggle1_epoll_timeout_ms(void) {
    struct timeval wall;
    gettimeofday(&wall, NULL);
    int timeout = DIAL_SENSITIVTY_TIMEOUT_MS;
    for (int i = 0; i < scroll_src_count; i++) {
        int ms = scroll_burst_ms_until_wake(&scroll_srcs[i].burst, &wall);
        if (ms < 0)
            continue;
        if (ms < timeout)
            timeout = ms;
    }
    return timeout;
}
#endif

static void get_event(
#if defined(HDZGOGGLE)
    unsigned src_idx
#else
    int fd
#endif
) {
    struct input_event event;

#if !defined(HDZGOGGLE)
    static int event_type_last = 0;
    static int btn_press_time = 0;
    static int roller_value = 0;
#endif

#if defined(HDZGOGGLE)
    int fd = scroll_srcs[src_idx].fd;
    int eventn = scroll_srcs[src_idx].eventn;
    scroll_frame_t *src_frame = &scroll_srcs[src_idx].frame;
    scroll_burst_t *src_burst = &scroll_srcs[src_idx].burst;
#if defined(SCROLL_DIAG)
    char ts_ev_buf[32];
#else
    (void)eventn;
#endif
#endif

#if !defined(HDZGOGGLE)
    // time (sec, usec) difference above which will the next scroll wheel event be accepted
    // 10000 usec = 10msec is more than short enough (100Hz)
    // scroll events
    const struct timeval scroll_time_diff = {0, 10000};
    // expected timestamp in the future beyond that will the events be accepted
    static struct timeval next_scroll = {0, 0};
    // direction change events. Goggle 1 no longer uses this: it replaced this
    // blind time-based gate with the stateful scroll_filter (see below) after
    // an adversarial review found the gate could hide, from that filter, the
    // exact pulses it needed to see to reject alternating bounce correctly.
    // Goggle 2 / BoxPro are unaffected and keep this exactly as before.
    const struct timeval rel_time_diff = {0, 20000};
    static struct timeval next_rel = {0, 0};
    static bool discard_scroll = false;
#endif

#if defined(HDZGOGGLE)
    // Non-blocking drain: fd opened O_RDONLY|O_NONBLOCK. Read until EAGAIN so
    // a due 10ms decision sees every already-queued REL_Y. Never block here.
    // Budget: yield after INPUT_DRAIN_BUDGET records so a chattering encoder
    // cannot monopolize the input thread; the fd stays ready for epoll.
    unsigned drained = 0;
    for (;;) {
        if (drained >= INPUT_DRAIN_BUDGET)
            break;
        ssize_t nread = read(fd, &event, sizeof(event));
        input_drain_status_t drain_st = input_drain_status(nread, errno, sizeof(event),
                                                           drained, INPUT_DRAIN_BUDGET);
        if (drain_st == INPUT_DRAIN_EINTR)
            continue;
        if (drain_st == INPUT_DRAIN_EAGAIN || drain_st == INPUT_DRAIN_BUDGET_HIT)
            break;
        if (drain_st != INPUT_DRAIN_PROCESS) {
            if (drain_st == INPUT_DRAIN_ERROR)
                perror("input read");
            break;
        }
        drained++;
#else
    read(fd, &event, sizeof(event));
#endif

    switch (event.type) {
    case EV_SYN:
        if (event.code == SYN_REPORT) {
#if defined(HDZGOGGLE)
            // Source-owned SYN: consume this fd's fresh REL_Y or KEY frame only.
            // Orphan SYN (no pending on THIS source) is a no-op — never uses
            // process-global event_type_last, never btn_click() on stale KEY.
            int key_v = 0;
#if defined(SCROLL_DIAG)
            int roller_value = 0;
            scroll_frame_syn_kind_t syn = scroll_frame_take_syn(src_frame, &roller_value, &key_v);
#else
            scroll_frame_syn_kind_t syn = scroll_frame_take_syn(src_frame, NULL, &key_v);
#endif
            if (syn == SCROLL_FRAME_SYN_REL_Y) {
#if defined(SCROLL_DIAG)
                scroll_diag_fmt_tv(ts_ev_buf, sizeof(ts_ev_buf), &event.time);
                LOGI("SCROLL_SYN_CLEAR eventN=%d fd=%d ts_ev=%s val=%d sign=%d",
                     eventn, fd, ts_ev_buf, roller_value, scroll_diag_sign(roller_value));
#endif
            } else if (syn == SCROLL_FRAME_SYN_KEY) {
                if (!g_setting.ease.no_dial) {
                    switch (scroll_btn_apply_key_syn(&scroll_srcs[src_idx].btn, key_v)) {
                    case SCROLL_BTN_LONG_PRESS:
                        btn_press();
                        g_key = DIAL_KEY_PRESS;
                        break;
                    case SCROLL_BTN_CLICK:
                        btn_click();
                        g_key = DIAL_KEY_CLICK;
                        break;
                    default:
                        break;
                    }
                } else {
                    int *btn_press_time = &scroll_srcs[src_idx].btn.press_time;
                    if (key_v) {
                        if (scroll_sim_mode_repeat == SCROLL_REPEAT_DOWN) {
                            roller_down();
                        } else if (scroll_sim_mode_repeat == SCROLL_REPEAT_UP) {
                            roller_up();
                        }
                        if (scroll_sim_mode_pending)
                            *btn_press_time = 0;
                        else
                            (*btn_press_time)++;
                    } else {
                        if (scroll_sim_mode_pending) {
                            scroll_sim_mode_pending = false;
                        } else if (scroll_sim_mode_repeat == SCROLL_REPEAT_NONE) {
                            if (*btn_press_time < 10) {
                                btn_click();
                                g_key = DIAL_KEY_CLICK;
                            } else if (*btn_press_time < 50) {
                                btn_press();
                                g_key = DIAL_KEY_PRESS;
                            }
                        }
                        *btn_press_time = 0;
                    }
                }
            }
#else
            if (event_type_last == EV_REL) {
                if (g_setting.ease.no_dial)
                    break;

                if (!discard_scroll && timercmp(&event.time, &next_scroll, >)) {
                    timeradd(&event.time, &scroll_time_diff, &next_scroll);

                    int filtered_value = roller_value;

                    if (filtered_value == 1) {
                        roller_up_acc++;
                        roller_down_acc = 0;
                    } else if (filtered_value == -1) {
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
#endif
            // LOGI("------------ syn report ----------");
        } else if (event.code == SYN_MT_REPORT) {
            // LOGI("----------- syn mt report ------------");
        }
        break;
    case EV_KEY:
        // LOGI("key code%d is %s!", event.code, event.value?"down":"up");
        btn_value = event.value;
#if defined(HDZGOGGLE)
        scroll_frame_on_key(src_frame, event.value);
#else
        event_type_last = EV_KEY;
#endif
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
#if defined(HDZGOGGLE)
        // Every REL_Y: frame latch (stale-SYN) + burst µmaj/quiet (before any
        // next_scroll). Decision emit feeds scroll_filter; SYN only clears latch.
        if (event.code == REL_X) {
            // LOGI("x = %d", event.value);
        } else if (event.code == REL_Y) {
            struct timeval wall_now;
            scroll_burst_result_t burst_out;

            scroll_frame_on_rel_y(src_frame, event.value);
            gettimeofday(&wall_now, NULL);
            scroll_burst_on_rel_y(src_burst, &event.time, event.value, &wall_now, &burst_out);
#if defined(SCROLL_DIAG)
            scroll_diag_fmt_tv(ts_ev_buf, sizeof(ts_ev_buf), &event.time);
            LOGI("SCROLL_RAW eventN=%d fd=%d ts_ev=%s val=%d sign=%d",
                 eventn, fd, ts_ev_buf, event.value, scroll_diag_sign(event.value));
#endif
            goggle1_handle_burst_result(src_idx, &burst_out);
        }
#else
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
#endif
        break;
    default:
        // LOGI("unknown [type=%d, code=%d value=%d]", event.type, event.code, event.value);
        break;
    }
#if defined(HDZGOGGLE)
    } // end drain loop

    // After draining this source, run any due micro-majority decision.
    goggle1_poll_src_due(src_idx);
#endif
}

static void add_to_epfd(int epfd, int fd
#if defined(HDZGOGGLE)
                        ,
                        int eventn
#endif
) {
    struct epoll_event event = {
        .events = EPOLLIN,
    };

#if defined(HDZGOGGLE)
    assert(scroll_src_count < EPOLL_FD_CNT);
    unsigned idx = (unsigned)scroll_src_count;
    scroll_srcs[idx].fd = fd;
    scroll_srcs[idx].eventn = eventn;
    scroll_frame_init(&scroll_srcs[idx].frame);
    scroll_burst_init(&scroll_srcs[idx].burst);
    scroll_btn_init(&scroll_srcs[idx].btn);
    scroll_srcs[idx].next_scroll.tv_sec = 0;
    scroll_srcs[idx].next_scroll.tv_usec = 0;
    scroll_src_count++;
    event.data.u32 = idx;
#else
    event.data.fd = fd;
#endif

    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    assert(ret == 0);
}

static void *thread_input_device(void *ptr) {
#ifndef EMULATOR_BUILD
    for (;;) {
        struct epoll_event events[EPOLL_FD_CNT];

#if defined(HDZGOGGLE)
        int timeout_ms = goggle1_epoll_timeout_ms();
#else
        int timeout_ms = DIAL_SENSITIVTY_TIMEOUT_MS;
#endif
        int ret = epoll_wait(epfd, events, EPOLL_FD_CNT, timeout_ms);
        if (ret < 0) {
            perror("epoll_wait");
            continue;
        }

        if (ret > 0) {
            for (int i = 0; i < ret; i++) {
                if (events[i].events & EPOLLIN) {
#if defined(HDZGOGGLE)
                    get_event(events[i].data.u32);
#else
                    get_event(events[i].data.fd);
#endif
                }
            }
        }
#if defined(HDZGOGGLE)
        // Always poll due decisions (timeout wake, or after input drain).
        goggle1_poll_all_due();
#endif
        if (ret == 0) {
#if defined(HDZGOGGLE)
            // Only the long idle timeout resets the dial accumulator.
            // Short µmaj decision wakes use timeout_ms < 1000 and must not.
            if (timeout_ms >= DIAL_SENSITIVTY_TIMEOUT_MS) {
#endif
            roller_up_acc = 0;
            roller_down_acc = 0;
            if (scroll_sim_mode_repeat != SCROLL_REPEAT_NONE)
                beep();
#if defined(HDZGOGGLE)
            }
#endif
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
#if defined(HDZGOGGLE)
    // Burst gate already suppresses intra-detent bipolar chatter, so each
    // filter input is one quiet-bounded episode. Replay of HDZGOGGLE-diag.log
    // showed confirm=2 swallowing 20 first-opposite episodes (confirm=1
    // emitted them). Keep compiled default 2 for raw-pulse tests.
    scroll_filter_init(&goggle1_scroll_filter);
    goggle1_scroll_filter.reversal_confirm = 1;
#endif
#ifndef EMULATOR_BUILD
    epfd = epoll_create(EPOLL_FD_CNT);
    assert(epfd > 0);

    char buf[64];
    for (int i = 0; i < EPOLL_FD_CNT; i++) {
        snprintf(buf, 64, "/dev/input/event%d", i);

#if defined(HDZGOGGLE)
        // O_NONBLOCK required for safe drain-until-EAGAIN before µmaj decide.
        int fd = open(buf, O_RDONLY | O_NONBLOCK);
#else
        int fd = open(buf, O_RDONLY);
#endif
        if (fd >= 0) {
#if defined(HDZGOGGLE)
            add_to_epfd(epfd, fd, i);
#if defined(SCROLL_DIAG)
            LOGI("SCROLL_OPEN eventN=%d fd=%d nonblock=1", i, fd);
#endif
#else
            add_to_epfd(epfd, fd);
#endif
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
