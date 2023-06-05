#include "page_storage.h"

#include <stdlib.h>

#include <log/log.h>
#include <minIni.h>

#include "core/common.hh"
#include "core/settings.h"
#include "ui/page_playback.h"

/**
 * Types
 */

/**
 *  Globals
 */
static lv_coord_t col_dsc[] = {160, 160, 160, 160, 160, 160, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60, 60, 60, 60, 60, 60, 60, 40, LV_GRID_TEMPLATE_LAST};
static btn_group_t btn_group0;
static lv_obj_t *label_formatSD;
static lv_obj_t *label_repairSD;
static bool confirmFormat = false;
static bool confirmRepair = false;

/**
 * Cancel operation.
 */
static void page_storage_cancel() {
    confirmFormat = false;
    confirmRepair = false;
    lv_label_set_text(label_formatSD, "Format SD Card");
    lv_label_set_text(label_repairSD, "Repair SD Card");
}

/**
 * Main allocation routine for this page.
 */
static lv_obj_t *page_storage_create(lv_obj_t *parent, panel_arr_t *arr) {
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1053, 900);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 1053, 894);

    create_text(NULL, section, false, "Storage:", LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, 1280, 800);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

    create_select_item(arr, cont);

    create_btn_group_item(&btn_group0, cont, 2, "Logging", "On", "Off", "", "", 0);
    label_formatSD = create_label_item(cont, "Format SD Card", 1, 1, 3);
    label_repairSD = create_label_item(cont, "Repair SD Card", 1, 2, 3);
    create_label_item(cont, "< Back", 1, 3, 1);

    btn_group_set_sel(&btn_group0, g_setting.storage.logging ? 0 : 1);

    return page;
}

/**
 * Main entry routine for this page.
 */
static void page_storage_enter() {
    page_storage_cancel();
}

/**
 * Main exit routine for this page.
 */
static void page_storage_exit() {
    page_storage_cancel();
}

/**
 * Main navigation routine for this page.
 */
static void page_storage_on_roller(uint8_t key) {
    page_storage_cancel();
}

/**
 * Main input selection routine for this page.
 */
static void page_storage_on_click(uint8_t key, int sel) {
    switch (sel) {
    case 0:
        break;
    case 1:
        if (confirmFormat) {
            lv_label_set_text(label_formatSD, "Formatting...");
            lv_timer_handler();
            system("/mnt/app/script/formatsd.sh");
            clear_videofile_cnt();
            lv_label_set_text(label_formatSD, "Format SD Card");
            lv_timer_handler();
            confirmFormat = false;
        } else {
            lv_label_set_text(label_formatSD, "#FFFF00 Delete all data? Click the Enter Button to confirm...#");
            lv_timer_handler();
            confirmFormat = true;
        }
        break;
    case 2:
        if (confirmRepair) {
            lv_label_set_text(label_repairSD, "Repairing...");
            lv_timer_handler();
            system("/mnt/app/script/repairsd.sh");
            lv_label_set_text(label_repairSD, "Repair SD Card");
            lv_timer_handler();
            confirmRepair = false;
        } else {
            lv_label_set_text(label_repairSD, "#FFFF00 Potential data loss? Click the Enter Button to confirm...#");
            lv_timer_handler();
            confirmRepair = true;
        }
        break;
    default:
        page_storage_cancel();
        break;
    }
}

static void page_storage_on_right_button(bool is_short) {
}

/**
 * Main Menu page data structure, notice max is set to zero
 * in order to allow us to override default user input logic.
 */
page_pack_t pp_storage = {
    .p_arr = {
        .cur = 0,
        .max = 3,
    },
    .name = "Storage",
    .create = page_storage_create,
    .enter = page_storage_enter,
    .exit = page_storage_exit,
    .on_roller = page_storage_on_roller,
    .on_click = page_storage_on_click,
    .on_right_button = page_storage_on_right_button,
};
