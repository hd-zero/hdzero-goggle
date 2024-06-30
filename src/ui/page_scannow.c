#include "page_scannow.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <log/log.h>
#include <minIni.h>

#include "core/app_state.h"
#include "core/common.hh"
#include "core/defines.h"
#include "core/msp_displayport.h"
#include "core/osd.h"
#include "core/settings.h"
#include "driver/dm5680.h"
#include "driver/dm6302.h"
#include "driver/fbtools.h"
#include "driver/hardware.h"
#include "driver/i2c.h"
#include "driver/oled.h"
#include "driver/uart.h"
#include "ui/page_common.h"
#include "ui/ui_main_menu.h"
#include "ui/ui_porting.h"
#include "ui/ui_style.h"

LV_IMG_DECLARE(img_signal_status);
LV_IMG_DECLARE(img_signal_status2);
LV_IMG_DECLARE(img_signal_status3);
LV_IMG_DECLARE(img_signal_level);
LV_IMG_DECLARE(img_ant1);
LV_IMG_DECLARE(img_ant2);
LV_IMG_DECLARE(img_ant3);
LV_IMG_DECLARE(img_ant4);
LV_IMG_DECLARE(img_ant5);
LV_IMG_DECLARE(img_ant6);
LV_IMG_DECLARE(img_ant7);
LV_IMG_DECLARE(img_ant8);
LV_IMG_DECLARE(img_ant9);
LV_IMG_DECLARE(img_ant10);
LV_IMG_DECLARE(img_ant11);
LV_IMG_DECLARE(img_ant12);
LV_IMG_DECLARE(img_ant13);
LV_IMG_DECLARE(img_ant14);

typedef struct {
    bool is_valid;
    int gain;
    int bw;
} channel_status_t;

typedef struct {
    lv_obj_t *img0;
    lv_obj_t *label;
    lv_obj_t *img1;
} channel_t;

channel_t channel_tb[BASE_CH_NUM];
channel_status_t channel_status_tb[BASE_CH_NUM];

////////////////////////////////////////////////////////////////////////////////////////////////////
int valid_channel_tb[BASE_CH_NUM];
int user_select_index = 0;

// local
static int auto_scaned_cnt = 0;
static lv_obj_t *progressbar;
static lv_obj_t *label;
static lv_coord_t col_dsc1[] = {500, 20, 1164 - 520, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc1[] = {60, 60, 80, LV_GRID_TEMPLATE_LAST};

static lv_coord_t col_dsc2[] = {120, 80, 80, 180, 100, 80, 80, 180, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc2[] = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};

static void select_signal(channel_t *channel) {
    for (int i = 0; i < BASE_CH_NUM; i++) {
        if (channel_status_tb[i].is_valid) {
            lv_img_set_src(channel_tb[i].img0, &img_signal_status2);
        } else {
            lv_img_set_src(channel_tb[i].img0, &img_signal_status);
        }
    }
    lv_img_set_src(channel->img0, &img_signal_status3);
}

static void set_signal_bar(channel_t *channel, bool is_valid, int gain) {
    if (!is_valid) {
        lv_img_set_src(channel->img0, &img_signal_status);

        if (gain < 5) {
            lv_img_set_src(channel->img1, &img_ant1);
        } else if (gain < 10) {
            lv_img_set_src(channel->img1, &img_ant3);
        } else if (gain < 15) {
            lv_img_set_src(channel->img1, &img_ant4);
        } else if (gain < 16) {
            lv_img_set_src(channel->img1, &img_ant5);
        } else if (gain < 20) {
            lv_img_set_src(channel->img1, &img_ant6);
        } else if (gain < 30) {
            lv_img_set_src(channel->img1, &img_ant7);
        } else if (gain <= 77) {
            lv_img_set_src(channel->img1, &img_ant7);
        } else {
            lv_img_set_src(channel->img1, &img_ant1);
        }

    } else {
        lv_img_set_src(channel->img0, &img_signal_status2);
        if (gain < 5) {
            lv_img_set_src(channel->img1, &img_ant8);
        } else if (gain < 10) {
            lv_img_set_src(channel->img1, &img_ant10);
        } else if (gain < 15) {
            lv_img_set_src(channel->img1, &img_ant11);
        } else if (gain < 16) {
            lv_img_set_src(channel->img1, &img_ant12);
        } else if (gain < 20) {
            lv_img_set_src(channel->img1, &img_ant13);
        } else if (gain < 30) {
            lv_img_set_src(channel->img1, &img_ant14);
        } else if (gain <= 77) {
            lv_img_set_src(channel->img1, &img_ant14);
        } else {
            lv_img_set_src(channel->img1, &img_ant8);
        }
    }
}
// gain, 0-60
static void create_channel_switch(lv_obj_t *parent, int col, int row, channel_t *channel) {

    channel->img0 = lv_img_create(parent);
    lv_img_set_src(channel->img0, &img_signal_status);
    lv_obj_set_size(channel->img0, 77, 77);
    lv_obj_set_grid_cell(channel->img0, LV_GRID_ALIGN_START, col, 1,
                         LV_GRID_ALIGN_CENTER, row, 1);

    channel->label = lv_label_create(parent);
    lv_obj_set_style_text_font(channel->label, &lv_font_montserrat_40, 0);
    lv_obj_set_style_text_align(channel->label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(channel->label, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_pad_top(channel->label, 12, 0);
    lv_label_set_long_mode(channel->label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_grid_cell(channel->label, LV_GRID_ALIGN_START, col + 1, 1,
                         LV_GRID_ALIGN_CENTER, row, 1);

    channel->img1 = lv_img_create(parent);
    lv_img_set_src(channel->img1, &img_ant1);
    lv_obj_set_size(channel->img1, 164, 78);
    lv_obj_set_grid_cell(channel->img1, LV_GRID_ALIGN_START, col + 2, 1,
                         LV_GRID_ALIGN_CENTER, row, 1);
}

void page_scannow_set_channel_label(void) {
    static const char *race_band_channel_str[] = {"R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8", "E1", "F1", "F2", "F4"};
    static const char *low_band_channel_str[] = {"L1", "L2", "L3", "L4", "L5", "L6", "L7", "L8"};
    uint8_t i;

    // set channel label
    if (g_setting.source.hdzero_band == RACE_BAND) {
        // race band
        for (i = 0; i < BASE_CH_NUM; i++) {
            lv_label_set_text(channel_tb[i].label, race_band_channel_str[i]);
        }

        for (i = 8; i < BASE_CH_NUM; i++) {
            lv_obj_clear_flag(channel_tb[i].img0, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(channel_tb[i].label, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(channel_tb[i].img1, LV_OBJ_FLAG_HIDDEN);
        }
    } else {
        // lowband
        for (i = 0; i < 8; i++) {
            lv_label_set_text(channel_tb[i].label, low_band_channel_str[i]);
        }
        for (i = 8; i < BASE_CH_NUM; i++) {
            lv_obj_add_flag(channel_tb[i].img0, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(channel_tb[i].label, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(channel_tb[i].img1, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

// 1920-500
// 1420
// 1420*0.18
// 255.6
// 1420-256
// 1164
static lv_obj_t *page_scannow_create(lv_obj_t *parent, panel_arr_t *arr) {
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1158, 900);
    lv_obj_add_style(page, &style_scan, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 60, 0);

    lv_obj_t *cont1 = lv_obj_create(page);
    lv_obj_set_size(cont1, 1158, 250);
    lv_obj_set_layout(cont1, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont1, &style_scan, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(cont1, 0, 0);
    lv_obj_set_style_grid_column_dsc_array(cont1, col_dsc1, 0);
    lv_obj_set_style_grid_row_dsc_array(cont1, row_dsc1, 0);

    progressbar = lv_bar_create(cont1);
    lv_obj_set_size(progressbar, 500, 50);
    lv_obj_center(progressbar);
    lv_bar_set_value(progressbar, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(progressbar, lv_color_make(0xff, 0xff, 0xff), LV_PART_MAIN);
    lv_obj_set_style_radius(progressbar, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(progressbar, lv_color_make(0, 0xff, 0), LV_PART_INDICATOR);
    lv_obj_set_style_radius(progressbar, 0, LV_PART_INDICATOR);

    lv_obj_set_grid_cell(progressbar, LV_GRID_ALIGN_START, 0, 1,
                         LV_GRID_ALIGN_CENTER, 1, 1);

    lv_bar_set_range(progressbar, 0, 14);

    label = lv_label_create(cont1);
    lv_label_set_text(label, "Scan Ready");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(label, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_pad_top(label, 12, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 0, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_t *label2 = lv_label_create(cont1);
    lv_label_set_text(label2, "When scanning is complete, use the\n dial to select a channel and press\n the Enter button to choose");
    lv_obj_set_style_text_font(label2, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(label2, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_pad_top(label2, 12, 0);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);
    lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_START, 2, 1,
                         LV_GRID_ALIGN_START, 0, 3);

    lv_obj_t *cont2 = lv_obj_create(page);
    lv_obj_set_size(cont2, 1164, 500);
    lv_obj_set_layout(cont2, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont2, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont2, &style_scan, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(cont2, 0, 0);
    lv_obj_set_style_grid_column_dsc_array(cont2, col_dsc2, 0);
    lv_obj_set_style_grid_row_dsc_array(cont2, row_dsc2, 0);

    // create channel
    uint8_t col_offset = 1;
    uint8_t row_offset = 0;

    for (int i = 0; i < 8; i++) {
        create_channel_switch(cont2, ((i >> 2) << 2) + col_offset, i & 0x03, &channel_tb[i]);
    }

    row_offset = 4;
    for (int i = 0; i < 4; i++) {
        create_channel_switch(cont2, ((i >> 1) << 2) + col_offset, row_offset + (i & 0x01), &channel_tb[8 + i]);
    }
    page_scannow_set_channel_label();

    return page;
}

static void user_select_signal(void) {
    if (valid_channel_tb[0] == -1)
        return;

    user_select_index = 0;
    select_signal(&channel_tb[valid_channel_tb[0] & 0x7F]);
}

static void user_clear_signal(void) {
    user_select_index = 0;
    for (int i = 0; i < BASE_CH_NUM; i++) {
        lv_img_set_src(channel_tb[i].img0, &img_signal_status);
        lv_img_set_src(channel_tb[i].img1, &img_ant1);
    }
}

uint8_t max4(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4) {
    uint8_t b1 = (a1 > a2) ? a1 : a2;
    uint8_t b2 = (a3 > a4) ? a3 : a4;
    return (b1 > b2) ? b1 : b2;
}

void scan_channel(band_t band, uint8_t channel, uint8_t *gain_ret, bool *valid) {
    uint8_t vld0, vld1;
    uint8_t gain[4];

    DM6302_SetChannel(band, channel);

    usleep(100000);
    DM5680_clear_vldflg();
    DM5680_req_vldflg();

    DM6302_get_gain(gain);
    *gain_ret = max4(gain[0], gain[1], gain[2], gain[3]);

    vld0 = rx_status[0].rx_valid;
    vld1 = rx_status[1].rx_valid;
    *valid = vld0 | vld1;

    LOGI("Scan band:%d, channel%d: valid:%d, gain:%d", band, channel, *valid, *gain_ret);
}

int8_t scan_now(void) {
    uint8_t ch, gain;
    bool valid;
    uint8_t valid_index;

    lv_label_set_text(label, "Scanning...");
    lv_bar_set_value(progressbar, 0, LV_ANIM_OFF);
    lv_timer_handler();
    lv_bar_set_value(progressbar, 2, LV_ANIM_OFF);
    lv_timer_handler();

    // clear
    for (ch = 0; ch < BASE_CH_NUM; ch++) {
        valid_channel_tb[ch] = -1;
        channel_status_tb[ch].is_valid = 0;
    }

    HDZero_open(g_setting.source.hdzero_bw);
    lv_bar_set_value(progressbar, 4, LV_ANIM_OFF);
    lv_timer_handler();

    for (ch = 0; ch < CHANNEL_NUM; ch++) {
        scan_channel(g_setting.source.hdzero_band, ch, &gain, &valid);
        if (valid) {
            channel_status_tb[ch].is_valid = 1;
            channel_status_tb[ch].gain = gain;
            set_signal_bar(&channel_tb[ch], channel_status_tb[ch].is_valid, channel_status_tb[ch].gain);
        }
        lv_bar_set_value(progressbar, ch + 5, LV_ANIM_OFF);
        lv_timer_handler();
    }
    lv_bar_set_value(progressbar, 14, LV_ANIM_OFF);

    valid_index = 0;
    for (ch = 0; ch < CHANNEL_NUM; ch++) {
        if (channel_status_tb[ch].is_valid) {
            valid_channel_tb[valid_index++] = ch;
        }

        lv_timer_handler();
    }

    user_select_signal();
    lv_label_set_text(label, "Scanning done");
    if (!valid_index)
        return -1;
    else
        return valid_index;
}

int scan_reinit(void) {
    lv_label_set_text(label, "Scanning ready");
    lv_bar_set_value(progressbar, 0, LV_ANIM_OFF);
    user_clear_signal();
    lv_timer_handler();
    return 0;
}

int scan(void) {
    g_scanning = true;
    g_source_info.source = SOURCE_HDZERO;
    int8_t ret = scan_now();
    g_scanning = false;
    return ret;
}

void autoscan_exit(void) {
    if (!g_autoscan_exit) {
        LOGI("autoscan_exit, lelve=1");
        g_autoscan_exit = true;
        if (auto_scaned_cnt > 1)
            app_state_push(APP_STATE_SUBMENU);
        else
            app_state_push(APP_STATE_MAINMENU);
    }
}

static void page_scannow_enter() {
    auto_scaned_cnt = scan();
    LOGI("scan return :%d", auto_scaned_cnt);

    if (auto_scaned_cnt == 1) {
        if (!g_autoscan_exit)
            g_autoscan_exit = true;

        app_state_push(APP_STATE_VIDEO);
        app_switch_to_hdzero(false);
    }

    if (auto_scaned_cnt == -1)
        submenu_exit();
}

static void page_scannow_exit() {
    HDZero_Close();
}

static void page_scannow_on_roller(uint8_t key) {
    if (valid_channel_tb[0] == -1)
        return;

    if (key == DIAL_KEY_UP) {
        if (valid_channel_tb[user_select_index + 1] != -1)
            user_select_index++;
    } else if (key == DIAL_KEY_DOWN) {
        if (user_select_index > 0)
            user_select_index--;
    }
    select_signal(&channel_tb[valid_channel_tb[user_select_index] & 0x07F]);
}

static void page_scannow_on_click(uint8_t key, int sel) {
    app_state_push(APP_STATE_VIDEO);
    app_switch_to_hdzero(false);
}

page_pack_t pp_scannow = {
    .name = "Scan Now",
    .create = page_scannow_create,
    .enter = page_scannow_enter,
    .exit = page_scannow_exit,
    .on_created = NULL,
    .on_update = NULL,
    .on_roller = page_scannow_on_roller,
    .on_click = page_scannow_on_click,
    .on_right_button = NULL,
};