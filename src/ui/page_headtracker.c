#include "page_headtracker.h"

#include <minIni.h>

#include "ht.h"
#include "page_common.h"
#include "ui/ui_style.h"

static btn_group_t btn_group;

static lv_coord_t col_dsc[] = {160, 200, 160, 160, 160, 160, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};
lv_obj_t *label_cali;

static lv_obj_t *page_headtracker_create(lv_obj_t *parent, panel_arr_t *arr) {
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1053, 900);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 1053, 894);

    create_text(NULL, section, false, "Head Tracker:", LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, 960, 600);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

    create_select_item(arr, cont);

    create_btn_group_item(&btn_group, cont, 2, "Tracking", "On", "Off", "", "", 0);

    label_cali = create_label_item(cont, "Calibrate", 1, 1, 1);

    create_label_item(cont, "< Back", 1, 2, 1);

    btn_group_set_sel(&btn_group, !g_setting.ht.enable);

    return page;
}

static void page_headtracker_on_click(uint8_t key, int sel) {
    if (sel == 0) {
        btn_group_toggle_sel(&btn_group);
        g_setting.ht.enable = btn_group_get_sel(&btn_group) == 0 ? 1 : 0;
        ini_putl("ht", "enable", g_setting.ht.enable, SETTING_INI);
        if (g_setting.ht.enable)
            enable_ht();
        else
            disable_ht();
    } else if (sel == 1) {
        lv_label_set_text(label_cali, "Calibrating...");
        lv_timer_handler();
        calibrate_ht();
        lv_label_set_text(label_cali, "Re-calibrate");
        lv_timer_handler();
    }
}

page_pack_t pp_headtracker = {
    .p_arr = {
        .cur = 0,
        .max = 3,
    },

    .create = page_headtracker_create,
    .enter = NULL,
    .exit = NULL,
    .on_roller = NULL,
    .on_click = page_headtracker_on_click,
};