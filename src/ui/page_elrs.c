#include "page_elrs.h"

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
#include "core/elrs.h"
#include "core/settings.h"
#include "driver/esp32.h"
#include "page_version.h"
#include "ui/ui_style.h"

static lv_coord_t col_dsc[] = {160, 220, 160, 160, 160, 160, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60, 60, 60, 60, 60, 40, 40, 60, 60, 60, LV_GRID_TEMPLATE_LAST};
static lv_obj_t *btn_wifi;
static lv_obj_t *label_wifi_status;
static lv_obj_t *btn_bind;
static lv_obj_t *label_bind_status;
static lv_obj_t *cancel_label;
static btn_group_t elrs_group;
static bool binding = false;

static lv_obj_t *page_elrs_create(lv_obj_t *parent, panel_arr_t *arr) {
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1053, 900);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 1053, 894);

    create_text(NULL, section, false, "ELRS:", LV_MENU_ITEM_BUILDER_VARIANT_2);

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
    btn_wifi = create_label_item(cont, "WiFi", 1, 1, 1);
    label_wifi_status = create_label_item(cont, "Click to start", 2, 1, 1);
    btn_bind = create_label_item(cont, "Bind", 1, 2, 1);
    label_bind_status = create_label_item(cont, "Click to start", 2, 2, 1);

    create_label_item(cont, "< Back", 1, 3, 1);

    cancel_label = lv_label_create(cont);
    lv_obj_add_flag(cancel_label, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(cancel_label, "* Press right button to cancel binding");
    lv_obj_set_style_text_font(cancel_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(cancel_label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(cancel_label, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_pad_top(cancel_label, 12, 0);
    lv_label_set_long_mode(cancel_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_grid_cell(cancel_label, LV_GRID_ALIGN_START, 1, 3, LV_GRID_ALIGN_START, 4, 2);

    return page;
}

static void page_elrs_reset() {
    lv_label_set_text(label_wifi_status, "Click to start");
    lv_label_set_text(label_bind_status, "Click to start");
}

static void page_elrs_on_roller(uint8_t key) {
    page_elrs_reset();
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

static void request_uid()
{
    msp_send_packet(MSP_GET_BP_STATUS, MSP_PACKET_COMMAND, 0, NULL);
    lv_timer_t *timer = lv_timer_create(elrs_status_timer, 250, NULL);
    lv_timer_set_repeat_count(timer, 20);
}

static void elrs_enable_timer(struct _lv_timer_t *timer) {
    if (g_setting.elrs.enable)
        enable_esp32();
    lv_timer_del(timer);
    request_uid();
}

static void page_elrs_enter() {
    lv_label_set_text(label_wifi_status, "Click to start");
    lv_label_set_text(label_bind_status, "Click to start");
    request_uid();
}

static void page_elrs_on_click(uint8_t key, int sel) {
    page_elrs_reset();
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
        lv_label_set_text(label_wifi_status, "Starting...");
        msp_send_packet(MSP_SET_MODE, MSP_PACKET_COMMAND, 1, (uint8_t *)"W");
        lv_timer_handler();
        if (msp_await_resposne(MSP_SET_MODE, 1, (uint8_t *)"P", 1000) != AWAIT_SUCCESS)
            lv_label_set_text(label_wifi_status, "#FF0000 FAILED#");
        else
            lv_label_set_text(label_wifi_status, "#00FF00 Success#");
    } else if (sel == 2) // start ESP bind
    {
        lv_label_set_text(label_bind_status, "Starting...");
        msp_send_packet(MSP_SET_MODE, MSP_PACKET_COMMAND, 1, (uint8_t *)"B");
        lv_timer_handler();
        if (msp_await_resposne(MSP_SET_MODE, 1, (uint8_t *)"P", 1000) != AWAIT_SUCCESS) {
            lv_label_set_text(label_bind_status, "#FF0000 FAILED#");
        } else {
            lv_label_set_text(label_bind_status, "Binding...");
            lv_obj_clear_flag(cancel_label, LV_OBJ_FLAG_HIDDEN);
            binding = true;
            lv_timer_handler();
            pthread_mutex_unlock(&lvgl_mutex);
            mspAwaitResposne_e response = msp_await_resposne(MSP_SET_MODE, 1, (uint8_t *)"O", 120000);
            pthread_mutex_lock(&lvgl_mutex);
            switch(response) {
                case AWAIT_SUCCESS:
                    lv_label_set_text(label_bind_status, "#00FF00 Success#");
                    request_uid();
                    break;
                case AWAIT_TIMEDOUT:
                    lv_label_set_text(label_bind_status, "#FEBE00 Timeout#");
                    break;
                case AWAIT_FAILED:
                    lv_label_set_text(label_bind_status, "#FF0000 FAILED#");
                    break;
                case AWAIT_CANCELLED:
                    lv_label_set_text(label_bind_status, "#FEBE00 Cancelled#");
                    // repower the module and re-request binding info
                    disable_esp32();
                    if (g_setting.elrs.enable) {
                        lv_timer_create(elrs_enable_timer, 100, NULL);
                    }
                    break;
            }
            lv_obj_add_flag(cancel_label, LV_OBJ_FLAG_HIDDEN);
            binding = false;
        }
    }
}

static void page_elrs_on_rbtn(bool is_short)
{
    if (binding && is_short) {
        msp_cancel_await();
    }
}

page_pack_t pp_elrs = {
    .p_arr = {
        .cur = 0,
        .max = 4,
    },
    .name = "ELRS",
    .create = page_elrs_create,
    .enter = page_elrs_enter,
    .exit = NULL,
    .on_roller = page_elrs_on_roller,
    .on_click = page_elrs_on_click,
    .on_right_button = page_elrs_on_rbtn,
};
