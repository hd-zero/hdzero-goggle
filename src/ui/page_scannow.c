#include "page_scannow.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <log/log.h>
#include <minIni.h>

#include "../core/common.hh"
#include "../core/defines.h"
#include "../core/osd.h"
#include "../driver/dm5680.h"
#include "../driver/dm6302.h"
#include "../driver/hardware.h"
#include "../driver/i2c.h"
#include "../driver/oled.h"
#include "../driver/uart.h"
#include "fbtools.h"
#include "msp_displayport.h"
#include "page_common.h"
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
} channel_status_t;

typedef struct {
    lv_obj_t *img0;
    lv_obj_t *label;
    lv_obj_t *img1;
} channel_t;

channel_t channel_tb[10];
channel_status_t channel_status_tb[10];

////////////////////////////////////////////////////////////////////////////////////////////////////
// local
static int auto_scaned_cnt = 0;
static int valid_channel_tb[11];
static int user_select_index = 0;

static lv_obj_t *progressbar;
static lv_obj_t *label;
static lv_coord_t col_dsc[] = {500, 20, 1164 - 520, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60, 60, 80, LV_GRID_TEMPLATE_LAST};

static lv_coord_t col_dsc2[] = {120, 80, 80, 180, 100, 80, 80, 180, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc2[] = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};

static void select_signal(channel_t *channel) {
    for (int i = 0; i < 10; i++) {
        if (channel_status_tb[i].is_valid) {
            lv_img_set_src(channel_tb[i].img0, &img_signal_status2);
        } else {
            lv_img_set_src(channel_tb[i].img0, &img_signal_status);
        }
    }
    lv_img_set_src(channel->img0, &img_signal_status3);
}

static void set_signal(channel_t *channel, bool is_valid, int gain) {
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
static void draw_signal(lv_obj_t *parent, const char *name, int col, int row, channel_t *channel) {

    channel->img0 = lv_img_create(parent);
    lv_img_set_src(channel->img0, &img_signal_status);
    lv_obj_set_size(channel->img0, 77, 77);
    lv_obj_set_grid_cell(channel->img0, LV_GRID_ALIGN_START, col, 1,
                         LV_GRID_ALIGN_CENTER, row, 1);

    channel->label = lv_label_create(parent);
    lv_label_set_text(channel->label, name);
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

    lv_obj_t *cont = lv_obj_create(page);
    lv_obj_set_size(cont, 1158, 250);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &style_scan, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

    progressbar = lv_bar_create(cont);
    lv_obj_set_size(progressbar, 500, 50);
    lv_obj_center(progressbar);
    lv_bar_set_value(progressbar, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(progressbar, lv_color_make(0xff, 0xff, 0xff), LV_PART_MAIN);
    lv_obj_set_style_radius(progressbar, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(progressbar, lv_color_make(0, 0xff, 0), LV_PART_INDICATOR);
    lv_obj_set_style_radius(progressbar, 0, LV_PART_INDICATOR);

    lv_obj_set_grid_cell(progressbar, LV_GRID_ALIGN_START, 0, 1,
                         LV_GRID_ALIGN_CENTER, 1, 1);

    label = lv_label_create(cont);
    lv_label_set_text(label, "Scan Ready");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(label, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_pad_top(label, 12, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 0, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_t *label2 = lv_label_create(cont);
    lv_label_set_text(label2, "When scanning is complete,use the\n dial to select a channel and press\n the Enter button to choose");
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

    draw_signal(cont2, "R1", 1, 0, &channel_tb[0]);
    draw_signal(cont2, "R2", 1, 1, &channel_tb[1]);
    draw_signal(cont2, "R3", 1, 2, &channel_tb[2]);
    draw_signal(cont2, "R4", 1, 3, &channel_tb[3]);
    draw_signal(cont2, "R5", 1, 4, &channel_tb[4]);
    draw_signal(cont2, "R6", 5, 0, &channel_tb[5]);
    draw_signal(cont2, "R7", 5, 1, &channel_tb[6]);
    draw_signal(cont2, "R8", 5, 2, &channel_tb[7]);
    draw_signal(cont2, "F2", 5, 3, &channel_tb[8]);
    draw_signal(cont2, "F4", 5, 4, &channel_tb[9]);
    return page;
}

static void user_select_signal(void) {
    if (valid_channel_tb[0] == -1)
        return;

    user_select_index = 0;
    select_signal(&channel_tb[valid_channel_tb[0]]);
}

static void user_clear_signal(void) {
    user_select_index = 0;

    for (int i = 0; i < 10; i++) {
        lv_img_set_src(channel_tb[i].img0, &img_signal_status);
        lv_img_set_src(channel_tb[i].img1, &img_ant1);
    }
}

uint8_t max4(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4) {
    uint8_t b1 = (a1 > a2) ? a1 : a2;
    uint8_t b2 = (a3 > a4) ? a3 : a4;
    return (b1 > b2) ? b1 : b2;
}

void scan_channel(uint8_t channel, uint8_t *gain_ret, bool *valid) {
    uint8_t vld0, vld1;
    uint8_t gain[4];

    DM6302_SetChannel(channel);

    usleep(100000);
    DM5680_clear_vldflg();
    DM5680_req_vldflg();

    DM6302_get_gain(gain);
    *gain_ret = max4(gain[0], gain[1], gain[2], gain[3]);

    vld0 = rx_status[0].rx_valid;
    vld1 = rx_status[1].rx_valid;
    *valid = vld0 | vld1;

    LOGI("Scan channel%d: valid:%d, gain:%d", channel, *valid, *gain_ret);
}

int8_t scan_now(void) {
    uint8_t ch, gain;
    bool valid;
    uint8_t valid_index = 0;

    lv_label_set_text(label, "Scanning...");
    lv_bar_set_value(progressbar, 0, LV_ANIM_OFF);
    lv_timer_handler();
    // dm5680_init();

    lv_bar_set_value(progressbar, 5, LV_ANIM_OFF);
    lv_timer_handler();
    // DM6302_init(0);
    lv_bar_set_value(progressbar, 10, LV_ANIM_OFF);
    lv_timer_handler();

    HDZero_open();

    // clear
    for (int i = 0; i < 10; i++) {
        valid_channel_tb[i] = -1;
    }

    for (ch = 0; ch < FREQ_NUM; ch++) {
        scan_channel(ch, &gain, &valid);

        if (valid) {
            valid_channel_tb[valid_index++] = ch;
        }

        channel_status_tb[ch].is_valid = valid;
        channel_status_tb[ch].gain = gain;

        lv_bar_set_value(progressbar, 10 + ch * 10, LV_ANIM_OFF);
        lv_timer_handler();
        set_signal(&channel_tb[ch], valid, gain);
    }

    user_select_signal();

    lv_label_set_text(label, "Scanning done");
    if (valid_channel_tb[0] == -1)
        return -1;

    return valid_index;
}

static bool is_720p60 = false;

//////////////////////////////////////////////////////////////////////
// is_default:
//    true = load from g_settings
//    false = user selected from auto scan page
void switch_to_video(bool is_default) {
    int ch = valid_channel_tb[user_select_index];

    if (is_default) {
        ch = g_setting.scan.channel - 1;
    } else {
        g_setting.scan.channel = ch + 1;
        ini_putl("scan", "channel", g_setting.scan.channel, SETTING_INI);
    }

    LOGI("switch to ch:%d, CAM_MODE=%d 4:3=%d", g_setting.scan.channel, CAM_MODE, cam_4_3);
    DM6302_SetChannel(ch);
    DM5680_clear_vldflg();
    DM5680_req_vldflg();
    progress_bar.start = 0;

    is_720p60 = true;
    switch (CAM_MODE) {
    case VR_720P50:
    case VR_720P60:
    case VR_960x720P60:
        is_720p60 = true;
        Display_720P60_50(CAM_MODE, cam_4_3);
        break;

    case VR_540P90:
    case VR_540P90_CROP:
        is_720p60 = false;
        Display_720P90(CAM_MODE);
        break;

    default:
        perror("switch_to_video CaM_MODE error");
    }

    channel_osd_mode = CHANNEL_SHOWTIME;
    osd_show(true);

    lvgl_switch_to_720p();
    draw_osd_clear();
    lv_timer_handler();

    Display_Osd(g_setting.record.osd);
    g_setting.autoscan.last_source = SETTING_SOURCE_HDZERO;
    ini_putl("autoscan", "last_source", g_setting.autoscan.last_source, SETTING_INI);
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
            g_menu_op = OPLEVEL_SUBMENU;
        else
            g_menu_op = OPLEVEL_MAINMENU;
    }
}

static void page_scannow_enter() {
    auto_scaned_cnt = scan();
    LOGI("scan return :%d", auto_scaned_cnt);

    if (auto_scaned_cnt == 1) {
        if (!g_autoscan_exit)
            g_autoscan_exit = true;

        g_menu_op = OPLEVEL_VIDEO;
        switch_to_video(false);
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
    select_signal(&channel_tb[valid_channel_tb[user_select_index]]);
}

static void page_scannow_on_click(uint8_t key, int sel) {
    g_menu_op = OPLEVEL_VIDEO;
    switch_to_video(false);
}

page_pack_t pp_scannow = {
    .create = page_scannow_create,
    .enter = page_scannow_enter,
    .exit = page_scannow_exit,
    .on_roller = page_scannow_on_roller,
    .on_click = page_scannow_on_click,
};