#include "page_connections.h"

#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <log/log.h>
#include <minIni.h>

#include "core/common.hh"
#include "core/dvr.h"
#include "core/elrs.h"
#include "core/settings.h"
#include "driver/esp32.h"
#include "page_version.h"
#include "ui/ui_style.h"

static lv_coord_t col_dsc[] = {160, 220, 160, 160, 160, 160, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60, 60, 60, 60, 60, 40, 40, 60, 60, 60, LV_GRID_TEMPLATE_LAST};
static lv_obj_t *btn_wifi;
static lv_obj_t *btn_bind;
static lv_obj_t *label_bind_status;
static btn_group_t elrs_group;
static btn_group_t wifi_group;

static lv_obj_t *page_connections_create(lv_obj_t *parent, panel_arr_t *arr) {
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1053, 900);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 1053, 894);

    create_text(NULL, section, false, "Connection Options:", LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, 960, 600);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

    create_select_item(arr, cont);

    create_btn_group_item(&elrs_group, cont, 2, "Backpack", "On", "Off", "", "", 0);
    btn_group_set_sel(&elrs_group, !g_setting.elrs.enable);
    btn_wifi = create_label_item(cont, "Start Backpack Wifi", 1, 1, 1);
    btn_bind = create_label_item(cont, "Start Backpack Binding", 1, 2, 1);
    label_bind_status = create_label_item(cont, "Not bound", 3, 2, 1);

    create_btn_group_item(&wifi_group, cont, 2, "Wifi AP*", "On", "Off", "", "", 3);
    btn_group_set_sel(&wifi_group, !g_setting.wifi.enable);

    char str[32];
    sprintf(str, "SSID:  %s", g_setting.wifi.ssid);
    create_label_item(cont, str, 2, 4, 1);
    sprintf(str, "Passwd:  %s", g_setting.wifi.passwd);
    create_label_item(cont, str, 2, 5, 1);

    create_label_item(cont, "< Back", 1, 6, 1);

    lv_obj_t *label2 = lv_label_create(cont);
    lv_label_set_text(label2, "1.Connect to the WIFI network named as the above SSID\n2.Use VLC to open Network Stream:\n     Network URL: rtsp://192.168.2.122:8554/hdzero\n3.See manual to customize SSID and passwd.\n    *Expansion module V2 is required for WIFI AP feature.");
    lv_obj_set_style_text_font(label2, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(label2, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_pad_top(label2, 12, 0);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);
    lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_START, 1, 4,
                         LV_GRID_ALIGN_START, 7, 2);
    return page;
}

static void page_connections_reset() {
    lv_label_set_text(btn_wifi, "Start Backpack Wifi");
    lv_label_set_text(btn_bind, "Start Backpack Binding");
}

static void page_connections_on_roller(uint8_t key) {
    page_connections_reset();
}

static void elrs_status_timer(struct _lv_timer_t *timer) {
    char label[80];
    uint8_t status[7] = {0};
    uint16_t size = sizeof(status) - 1;

    if (!msp_read_resposne(MSP_GET_BP_STATUS, &size, status)) {
        msp_send_packet(MSP_GET_BP_STATUS, MSP_PACKET_COMMAND, 0, NULL);
        return;
    }
    lv_timer_del(timer);
    if (status[0] & 4) {
        sprintf(label, "UID: %d,%d,%d,%d,%d,%d", status[1], status[2], status[3], status[4], status[5], status[6]);
        lv_label_set_text(label_bind_status, label);
    }
}

static void page_connections_enter() {
    lv_label_set_text(label_bind_status, "Not bound");
    msp_send_packet(MSP_GET_BP_STATUS, MSP_PACKET_COMMAND, 0, NULL);
    lv_timer_t *timer = lv_timer_create(elrs_status_timer, 250, NULL);
    lv_timer_set_repeat_count(timer, 20);
}

static void page_connections_on_click(uint8_t key, int sel) {
    page_connections_reset();
    if (sel == 0) {
        btn_group_toggle_sel(&elrs_group);
        g_setting.elrs.enable = btn_group_get_sel(&elrs_group) == 0;
        settings_put_bool("elrs", "enable", g_setting.elrs.enable);
        if (g_setting.elrs.enable)
            enable_esp32();
        else
            disable_esp32();
    } else if (sel == 1) // start ESP Wifi
    {
        lv_label_set_text(btn_wifi, "Starting...");
        msp_send_packet(MSP_SET_MODE, MSP_PACKET_COMMAND, 1, (uint8_t *)"W");
        lv_timer_handler();
        if (!msp_await_resposne(MSP_SET_MODE, 1, (uint8_t *)"P", 1000))
            lv_label_set_text(btn_wifi, "#FF0000 FAILED#");
        else
            lv_label_set_text(btn_wifi, "#00FF00 Success#");
    } else if (sel == 2) // start ESP bind
    {
        lv_label_set_text(btn_bind, "Starting...");
        msp_send_packet(MSP_SET_MODE, MSP_PACKET_COMMAND, 1, (uint8_t *)"B");
        lv_timer_handler();
        if (!msp_await_resposne(MSP_SET_MODE, 1, (uint8_t *)"P", 1000)) {
            lv_label_set_text(btn_bind, "#FF0000 FAILED#");
        } else {
            lv_label_set_text(btn_bind, "Binding...");
            lv_timer_handler();
            if (!msp_await_resposne(MSP_SET_MODE, 1, (uint8_t *)"O", 120000)) {
                lv_label_set_text(btn_bind, "#FEBE00 Timeout#");
            } else {
                lv_label_set_text(btn_bind, "#00FF00 Success#");
            }
            page_connections_enter();
        }
    } else if (sel == 3) // start WIFI on expansion module v2
    {
        btn_group_toggle_sel(&wifi_group);
        g_setting.wifi.enable = btn_group_get_sel(&wifi_group) == 0;
        settings_put_bool("wifi", "enable", g_setting.wifi.enable);
        if (g_setting.wifi.enable) {
            dvr_update_vi_conf(VR_1080P30);
            system(WIFI_AP_ON);
        } else
            system(WIFI_AP_OFF);
    }
}

page_pack_t pp_connections = {
    .p_arr = {
        .cur = 0,
        .max = 7,
    },
    .name = "Connections",
    .create = page_connections_create,
    .enter = page_connections_enter,
    .exit = NULL,
    .on_roller = page_connections_on_roller,
    .on_click = page_connections_on_click,
    .on_right_button = NULL,
};

void update_hostpad_conf() {
    FILE *fp = fopen("/tmp/hostapd.conf", "wt");
    if (!fp) {
        system("cp -y /mnt/app/wifi/hostapd.conf /tmp/");
        return;
    }

    fprintf(fp, "interface=wlan0\n");
    fprintf(fp, "driver=nl80211\n");
    fprintf(fp, "ssid=%s\n", g_setting.wifi.ssid);
    fprintf(fp, "channel=6\n");
    fprintf(fp, "hw_mode=g\n");
    fprintf(fp, "ignore_broadcast_ssid=0\n");
    fprintf(fp, "auth_algs=1\n");
    fprintf(fp, "wpa=3\n");
    fprintf(fp, "wpa_passphrase=%s\n", g_setting.wifi.passwd);
    fprintf(fp, "wpa_key_mgmt=WPA-PSK\n");
    fprintf(fp, "wpa_pairwise=TKIP\n");
    fprintf(fp, "rsn_pairwise=CCMP\n");
    fclose(fp);
}