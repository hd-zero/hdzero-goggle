#include "page_power.h"

#include <stdio.h>

#include <log/log.h>
#include <minIni.h>

#include "core/common.hh"
#include "mcp3021.h"
#include "page_common.h"
#include "ui/ui_style.h"

static btn_group_t btn_group1;
static slider_group_t slider_group;

static lv_coord_t col_dsc[] = {160, 200, 160, 160, 120, 160, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};
lv_obj_t *label_s;

static lv_obj_t *page_power_create(lv_obj_t *parent, panel_arr_t *arr) {
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1053, 900);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 1053, 894);

    create_text(NULL, section, false, "Power:", LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, 960, 600);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

    create_select_item(arr, cont);

    char str[5];
    create_label_item(cont, "Battery", 1, 0, 1);
    label_s = create_label_item(cont, "-S", 2, 0, 1);
    sprintf(str, "%d.%d", g_setting.power.voltage / 10, g_setting.power.voltage % 10);

    create_slider_item(&slider_group, cont, "Voltage", 42, g_setting.power.voltage, 1);
    create_btn_group_item(&btn_group1, cont, 3, "Warning Type", "Beep", "Visual", "Both", "", 2);
    lv_slider_set_range(slider_group.slider, 28, 42);
    lv_label_set_text(slider_group.label, str);
    create_label_item(cont, "< Back", 1, 3, 1);

    return page;
}

void set_battery_S() {
    char str[10];
    g_battery.type = mcp_detect_type();
    sprintf(str, "%dS", g_battery.type);
    lv_label_set_text(label_s, str);
}

void set_voltage(int val) {
    lv_slider_set_value(slider_group.slider, val, LV_ANIM_OFF);
    set_battery_S();
}

void set_warning_type(int type) {
    btn_group_set_sel(&btn_group1, type);
}

void power_voltage_inc(void) {
    int32_t value = 0;

    value = lv_slider_get_value(slider_group.slider);
    if (value <= 41)
        value += 1;

    lv_slider_set_value(slider_group.slider, value, LV_ANIM_OFF);

    char buf[5];
    sprintf(buf, "%d.%d", value / 10, value % 10);
    lv_label_set_text(slider_group.label, buf);

    g_setting.power.voltage = value;
    LOGI("vol:%d", g_setting.power.voltage);
    ini_putl("power", "voltage", g_setting.power.voltage, SETTING_INI);
}
void power_voltage_dec(void) {
    int32_t value = 0;

    value = lv_slider_get_value(slider_group.slider);
    if (value > 28)
        value -= 1;

    lv_slider_set_value(slider_group.slider, value, LV_ANIM_OFF);
    char buf[5];
    sprintf(buf, "%d.%d", value / 10, value % 10);
    lv_label_set_text(slider_group.label, buf);

    g_setting.power.voltage = value;
    LOGI("vol:%d", g_setting.power.voltage);
    ini_putl("power", "voltage", g_setting.power.voltage, SETTING_INI);
}

static void page_power_on_click(uint8_t key, int sel) {
    if (sel == 1) {
        if (g_menu_op == PAGE_POWER_SLIDE) {
            g_menu_op = OPLEVEL_SUBMENU;
            lv_obj_add_style(slider_group.slider, &style_silder_main, LV_PART_MAIN);
        } else {
            g_menu_op = PAGE_POWER_SLIDE;
            lv_obj_add_style(slider_group.slider, &style_silder_select, LV_PART_MAIN);
        }
    } else if (sel == 2) {
        btn_group_toggle_sel(&btn_group1);
        g_setting.power.warning_type = btn_group_get_sel(&btn_group1);
        ini_putl("power", "warning_type", g_setting.power.warning_type, SETTING_INI);
    }
}

page_pack_t pp_power = {
    .p_arr = {
        .cur = 0,
        .max = 4,
    },

    .create = page_power_create,
    .enter = NULL,
    .exit = NULL,
    .on_roller = NULL,
    .on_click = page_power_on_click,
};