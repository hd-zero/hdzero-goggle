#include "page_sleep.h"

#include "core/common.hh"
#include "core/settings.h"
#include "driver/dm5680.h"
#include "driver/fans.h"
#include "driver/hardware.h"
#include "page_fans.h"
#include <log/log.h>
#include <minIni.h>
#include <stdio.h>

lv_obj_t *page_sleep_create(lv_obj_t *parent, panel_arr_t *arr) {
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1053, 900);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 1053, 894);

    create_text(NULL, section, false, "Go Sleep:", LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_menu_cont_create(section);
    lv_obj_t *desc_label = lv_label_create(cont);
    lv_label_set_text(desc_label, "Click the Enter Button to go sleep.\nClick any button to exit sleep mode.");
    lv_obj_set_style_text_font(desc_label, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_color(desc_label, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_pad_top(desc_label, 12, 0);
    lv_label_set_long_mode(desc_label, LV_LABEL_LONG_WRAP);

    return page;
}

int fans_auto_mode_save;
int fan_speeds_save[3];

static void page_sleep_enter() {
    LOGI("page_sleep_enter");

    // Turn off OLED
    OLED_ON(0);

    // Turn off HDZero Receiver
    HDZero_Close();

    // Turn off Analog Receiver  -- Batch 2 goggles only
    if (getHwRevision() >= HW_REV_2) {
        g_setting.power.power_ana = 1;
        ini_putl("power", "power_ana_rx", g_setting.power.power_ana, SETTING_INI);
        DM5680_Power_AnalogModule(g_setting.power.power_ana);
    }

    // Minimum fan
    fans_auto_mode_save = g_setting.fans.auto_mode;
    fan_speeds_save[0] = fan_speeds[0];
    fan_speeds_save[1] = fan_speeds[1];
    fan_speeds_save[2] = fan_speeds[2];
    g_setting.fans.auto_mode = 0;
    fans_top_setspeed(MIN_FAN_TOP);
    fans_left_setspeed(MIN_FAN_SIDE);
    fans_right_setspeed(MIN_FAN_SIDE);
}

static void page_sleep_exit() {
    LOGI("page_sleep_exit");
    OLED_ON(1); // Turn on OLED

    g_setting.fans.auto_mode = fans_auto_mode_save;
    fans_top_setspeed(fan_speeds_save[2]);
    fans_left_setspeed(fan_speeds_save[1]);
    fans_right_setspeed(fan_speeds_save[0]);
}

static void page_sleep_click(uint8_t key, int sel) {
    submenu_exit();
}

static void page_sleep_roller(uint8_t key) {
    submenu_exit();
};

static void page_sleep_right_button(bool is_short) {
    submenu_exit();
}

page_pack_t pp_sleep = {
    .create = page_sleep_create,
    .enter = page_sleep_enter,
    .exit = page_sleep_exit,
    .on_roller = page_sleep_roller,
    .on_click = page_sleep_click,
    .on_right_button = page_sleep_right_button,
    .name = "Go Sleep!",
};