#include "ui_player.h"

#include <pthread.h>
#include <stdio.h>

#include <log/log.h>
#include <lvgl/lvgl.h>

#include "common.hh"
#include "player/media.h"
#include "ui/ui_style.h"

///////////////////////////////////////////////////////////////////////////////
// locals
//  UI
static player_control_t controller;
static lv_style_t style_bg;
static lv_style_t style_bar;
// media control
media_t *media;
player_cmd_t cmd;

LV_IMG_DECLARE(img_Play_0);
LV_IMG_DECLARE(img_Stop_0);

///////////////////////////////////////////////////////////////////////////////
static void time2str(uint32_t t1, uint32_t t2, char *s) {
    int m1, s1, m2, s2;
    m1 = (int)(t1 / 60000.0);
    s1 = (int)((t1 % 60000) / 1000.0);
    m2 = (int)(t2 / 60000.0);
    s2 = (int)((t2 % 60000) / 1000.0);

    sprintf(s, "%02d:%02d/%02d:%02d", m1, s1, m2, s2); // not considering for video over a hour
}

void mplayer_set_time(uint32_t now, uint32_t duration) {
    controller.value = (int32_t)now;
    controller.range = (int32_t)duration;
}

static void update_time_label(bool mediaOK) {
    char s[200];
    int percent;

    if (mediaOK) {
        uint32_t now = controller.value;
        uint32_t duration = controller.range;

        time2str(now, duration, s);
        lv_label_set_text(controller._label, s);
        percent = (int)(now * 100 / (duration * 1.0));
        lv_slider_set_value(controller._slider, percent, LV_ANIM_OFF);
    } else {
        lv_label_set_text(controller._label, "Bad file");
        lv_slider_set_value(controller._slider, 0, LV_ANIM_OFF);
    }
}

static void update_mplayer() {
    if (!controller.enable) {
        lv_obj_add_flag(controller.bg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(controller.bar, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_img_set_src(controller._btn, controller.is_playing ? &img_Stop_0 : &img_Play_0);
        update_time_label(media != NULL);
    }
}

static void mplayer_create_btn(lv_obj_t *parent, int16_t x, int16_t y) {
    controller._btn = lv_img_create(parent);
    lv_obj_set_size(controller._btn, MPLAYER_BTN_WIDTH, MPLAYER_BTN_HEIGHT);
    lv_obj_set_pos(controller._btn, x, y);
}

static void mplayer_create_slider(lv_obj_t *parent, int16_t x, int16_t y) {
    char buf[25];
    controller._slider = lv_slider_create(parent);
    lv_obj_remove_style_all(controller._slider);
    lv_obj_add_style(controller._slider, &style_silder_main, LV_PART_MAIN);
    lv_obj_add_style(controller._slider, &style_silder_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(controller._slider, &style_silder_pressed_color, LV_PART_INDICATOR | LV_STATE_PRESSED);
    lv_obj_add_style(controller._slider, &style_silder_knob, LV_PART_KNOB);
    lv_obj_add_style(controller._slider, &style_silder_pressed_color, LV_PART_KNOB | LV_STATE_PRESSED);

    lv_obj_set_pos(controller._slider, x, y + 20);
    lv_obj_set_size(controller._slider, MPLAYER_SLD_WIDTH, MPLAYER_SLD_HEIGHT);
    lv_slider_set_range(controller._slider, 0, 100); // in percentage
    lv_slider_set_value(controller._slider, 0, LV_ANIM_OFF);

    controller._label = lv_label_create(parent);
    sprintf(buf, "00:00/00:00");
    lv_label_set_text(controller._label, buf);
    lv_obj_set_style_text_font(controller._label, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_align(controller._label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_top(controller._label, 12, 0);
    lv_label_set_long_mode(controller._label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_color(controller._label, lv_color_hex(0xC0C0C0), 0);
    lv_obj_set_pos(controller._label, x + MPLAYER_SLD_WIDTH + MPLAYER_BTN_GAP, y);
    lv_obj_set_size(controller._label, 160, MPLAYER_BTN_HEIGHT);
}

static void init_mplayer() {
    int16_t x, y;
    // Background
    controller.bg = lv_obj_create(lv_scr_act());
    lv_obj_clear_flag(controller.bg, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(controller.bg, MPLAYER_SCR_WIDTH, MPLAYER_SCR_HEIGHT);
    lv_obj_set_pos(controller.bg, 0, 0);

    // Controller
    controller.bar = lv_obj_create(lv_scr_act());
    lv_obj_clear_flag(controller.bar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(controller.bar, MPLAYER_CB_WIDTH, MPLAYER_CB_HEIGHT);
    lv_obj_set_pos(controller.bar, (MPLAYER_SCR_WIDTH - MPLAYER_CB_WIDTH) >> 1, MPLAYER_SCR_HEIGHT - 160);

    // Buttons
    x = MPLAYER_BTN_GAP;
    y = 0;
    mplayer_create_btn(controller.bar, x, y);

    // Slider
    x += (MPLAYER_BTN_WIDTH + MPLAYER_BTN_GAP);
    mplayer_create_slider(controller.bar, x, y);
    mplayer_set_time(1000 * 60, 1000 * 240);

    // Styles bg
    lv_style_reset(&style_bg);
    lv_style_init(&style_bg);
    lv_style_set_bg_color(&style_bg, LV_COLOR_MAKE_32(0x0, 0x0, 0x0, 0x0));
    lv_style_set_border_width(&style_bg, 0);
    lv_style_set_radius(&style_bg, 0);
    lv_obj_add_style(controller.bg, &style_bg, 0);

    // Controller bar
    lv_style_reset(&style_bar);
    lv_style_init(&style_bar);
    lv_style_set_bg_color(&style_bar, lv_color_make(0x26, 0x26, 0x26));
    lv_style_set_opa(&style_bar, 90);
    lv_style_set_radius(&style_bar, 0);
    lv_style_set_border_width(&style_bar, 0);
    lv_obj_add_style(controller.bar, &style_bar, 0);

    controller.enable = true;
    controller.is_playing = true;
    controller.value = controller.range = 0;
}

static void free_mplayer() {
    controller.enable = false;
    lv_obj_del(controller._btn);
    lv_obj_del(controller._label);
    lv_obj_del(controller._slider);
    lv_obj_del(controller.bar);
    lv_obj_del(controller.bg);
}

///////////////////////////////////////////////////////////////////////////////
// key:
//   1 = dial up
//   2 = dial down
//   3 = click
//   4 = long press
// return
//   0 = still in mplayer page
//   1 = exit from mplayer page
void media_start();
void media_stop();
void media_pause();
void media_seek(uint32_t seekto);
void mplayer_exit();

uint8_t mplayer_on_key(uint8_t key) {
    if (!controller.enable)
        return 0;

    LOGI("mplayer_on_key: %d", key);

    switch (key) {
    case DIAL_KEY_PRESS:
        mplayer_exit();
        return 1;

    case DIAL_KEY_CLICK:
        if (controller.is_playing)
            media_pause();
        else
            media_start();

        controller.is_playing = !controller.is_playing;
        break;

    case DIAL_KEY_UP:
        controller.value += 5000;
        if (controller.value > controller.range)
            controller.value = controller.range;

        media_seek(controller.value);
        break;

    case DIAL_KEY_DOWN:
        controller.value -= 5000;
        if (controller.value < 0)
            controller.value = 0;

        media_seek(controller.value);
        break;
    }

    update_mplayer();

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
static void notify_cb(media_info_t *info) {
    // LOGI("%d/%d",info->playing_time,info->duration);

    if (!media)
        return;
    if (info->playing_time == 0xFFFFFFFF)
        return;
    if (info->playing_time == 0xFFFFFFFE)
        return;

    mplayer_set_time(info->playing_time, info->duration);

    pthread_mutex_lock(&lvgl_mutex);
    update_time_label(true);
    lv_timer_handler();
    pthread_mutex_unlock(&lvgl_mutex);
}

void media_init(char *fname) {
    media = media_instantiate(fname, notify_cb);
    if (!media) {
        perror("media_instantiate failed.");
        return;
    }
}

void media_start() {
    if (!media)
        return;
    cmd.opt = PLAYER_START;
    cmd.params = 0;

    media_control(media, &cmd);
}

void media_stop() {
    if (!media)
        return;
    cmd.opt = PLAYER_STOP;
    cmd.params = 0;
    media_control(media, &cmd);
}

void media_pause() {
    if (!media)
        return;
    cmd.opt = PLAYER_PAUSE;
    cmd.params = 0;
    media_control(media, &cmd);
}

void media_seek(uint32_t seekto) {
    if (!media)
        return;

    // pause
    media_pause();

    // seek
    cmd.opt = PLAYER_SEEK;
    cmd.params = seekto;
    media_control(media, &cmd);

    // start
    if (controller.is_playing) {
        media_start();
    }

    // LOGI("media_seek %d",seekto);
}

///////////////////////////////////////////////////////////////////////////////
// interface func
void mplayer_file(char *fname) {
    LOGI("mplayer %s", fname);
    init_mplayer();
    media_init(fname);
    media_start();
    update_mplayer();
}

void mplayer_exit() {
    pthread_mutex_unlock(&lvgl_mutex);
    if (media) {
        media_exit(media);
        media = NULL;
    }
    pthread_mutex_lock(&lvgl_mutex);
    free_mplayer();
}
