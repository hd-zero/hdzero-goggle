#include "page_autoscan.h"

#include <minIni.h>

#include "core/settings.h"
#include "ui/ui_style.h"
#include "ui/page_source.h"

static lv_coord_t col_dsc[] = {160, 150, 180, 220, 180, 160, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};

static btn_group_t btn_group0;
static btn_group_t btn_group1;
static btn_group_t btn_group2;

static lv_obj_t *page_autoscan_create(lv_obj_t *parent, panel_arr_t *arr) {
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1053, 900);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 1053, 894);

    create_text(NULL, section, false, "Auto Scan:", LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, 960, 600);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

    create_select_item(arr, cont);
    lv_obj_set_grid_cell(pp_autoscan.p_arr.panel[1], LV_GRID_ALIGN_STRETCH, 0, 6,
                         LV_GRID_ALIGN_STRETCH, 1, 2);
    lv_obj_clear_flag(pp_autoscan.p_arr.panel[2], FLAG_SELECTABLE);

    btn_group_t btn_group;
    create_btn_group_item(&btn_group0, cont, 3, "Auto Scan", "On", "Last", "Off", "", 0);
    create_btn_group_item2(&btn_group1, cont, 5, "Default", "Last", "HDZero", "Expansion", "AV In", "HDMI In", " ", 1); // 2 rows
    create_btn_group_item(&btn_group2, cont, 2, "Toggle Mode", "Cycle", "Race", "", "", 3);
    create_label_item(cont, "< Back", 1, 4, 1);

    lv_obj_t *label2 = lv_label_create(cont);
    lv_label_set_text(label2, "*if Auto Scan is 'Last', goggles will default to show last tuned channel");
    lv_obj_set_style_text_font(label2, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(label2, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_pad_top(label2, 12, 0);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);
    lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_START, 1, 3,
                         LV_GRID_ALIGN_START, 5, 2);

    btn_group_set_sel(&btn_group0, g_setting.autoscan.status);
    btn_group_set_sel(&btn_group1, g_setting.autoscan.source);
    btn_group_set_sel(&btn_group2, g_setting.autoscan.toggle);
    return page;
}

void source_toggle() {
    if (g_setting.autoscan.toggle == SETTING_AUTOSCAN_TOGGLE_RACE) {
        source_race();
    } else {
        source_cycle();
    }
}
static void page_autoscan_on_click(uint8_t key, int sel) {
    if (sel == 0) {
        btn_group_toggle_sel(&btn_group0);
        g_setting.autoscan.status = btn_group_get_sel(&btn_group0);
        ini_putl("autoscan", "status", g_setting.autoscan.status, SETTING_INI);
    } else if (sel == 1) {
        btn_group_toggle_sel(&btn_group1);
        g_setting.autoscan.source = btn_group_get_sel(&btn_group1);
        ini_putl("autoscan", "source", g_setting.autoscan.source, SETTING_INI);
    } else if (sel == 3) {
        btn_group_toggle_sel(&btn_group2);
        g_setting.autoscan.toggle = btn_group_get_sel(&btn_group2);
        ini_putl("autoscan", "toggle", g_setting.autoscan.toggle, SETTING_INI);
    }
}

page_pack_t pp_autoscan = {
    .p_arr = {
        .cur = 0,
        .max = 5,
    },
    .name = "Auto Scan",
    .create = page_autoscan_create,
    .enter = NULL,
    .exit = NULL,
    .on_created = NULL,
    .on_update = NULL,
    .on_roller = NULL,
    .on_click = page_autoscan_on_click,
    .on_right_button = NULL,
};
