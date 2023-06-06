#include "page_storage.h"

#include <stdlib.h>
#include <unistd.h>

#include <log/log.h>
#include <minIni.h>

#include "core/common.hh"
#include "core/settings.h"
#include "ui/page_playback.h"
#include "util/file.h"

/**
 * Types
 */
typedef enum {
    ITEM_LOGGING,
    ITEM_FORMAT,
    ITEM_REPAIR,
    ITEM_BACK,

    ITEM_LIST_TOTAL
} ITEM_LIST;

typedef struct {
    btn_group_t logging;
    lv_obj_t *format_sd;
    lv_obj_t *repair_sd;
    int confirm_format;
    int confirm_repair;
    bool status_displayed;
    lv_obj_t *back;
    lv_obj_t *status;
    lv_obj_t *note;
} page_options_t;

/**
 *  Globals
 */
static lv_coord_t col_dsc[] = {160, 160, 160, 160, 160, 160, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60, 60, 60, 60, 60, 60, 60, 40, LV_GRID_TEMPLATE_LAST};
static page_options_t page_storage;
static lv_timer_t *page_storage_format_sd_timer = NULL;
static lv_timer_t *page_storage_repair_sd_timer = NULL;

/**
 * Cancel operation.
 */
static void page_storage_cancel() {
    page_storage.confirm_format = 0;
    page_storage.confirm_repair = 0;
    lv_label_set_text(page_storage.format_sd, "Format SD Card");
    lv_label_set_text(page_storage.repair_sd, "Repair SD Card");
}

/**
 * Displays the status message box.
 */
static void page_storage_open_status_box(const char *title, const char *text) {
    lv_label_set_text(lv_msgbox_get_title(page_storage.status), title);
    lv_label_set_text(lv_msgbox_get_text(page_storage.status), text);
    lv_obj_clear_flag(page_storage.status, LV_OBJ_FLAG_HIDDEN);
    page_storage.status_displayed = true;
    pp_storage.p_arr.max = 0;
}

/**
 * Hides the status message box.
 */
static void page_storage_close_status_box() {
    lv_obj_add_flag(page_storage.status, LV_OBJ_FLAG_HIDDEN);
    page_storage.status_displayed = false;
    pp_storage.p_arr.max = ITEM_LIST_TOTAL;
}

/**
 * Callback invoked once `Format SD` is triggered and confirmed via the menu.
 */
static void page_storage_format_sd_timer_cb(struct _lv_timer_t *timer) {
    char text[128];

    // Temporarily disable logging if needed
    const char *logfile = NULL;
    if (log_file_opened()) {
        logfile = g_setting.storage.selftest ? SELF_TEST_FILE
                                             : APP_LOG_FILE;
        log_file_close();
    }

    unlink("/tmp/mkfs.result");
    system("/mnt/app/script/formatsd.sh > /tmp/formatsd.log 2>&1 &");

    int timeout_seconds = 30;
    int timeout_interval = 0;
    while (!file_exists("/tmp/mkfs.result") && ++timeout_interval < timeout_seconds) {
        sleep(1);
    }

    FILE *results = fopen("/tmp/mkfs.result", "r");
    if (results) {
        int exit_code;
        if (fscanf(results, "%d", &exit_code) == 1) {
            if (exit_code == 0) {
                snprintf(text, sizeof(text), "%s", "Format was successful.\nPress click to exit.");
            } else {
                snprintf(text, sizeof(text), "%s", "Format was not successful.\nPress click to exit.");
            }
        } else {
            snprintf(text, sizeof(text), "%s", "Failed to extract results.\nPress click to exit.");
        }

        fclose(results);
    } else {
        snprintf(text, sizeof(text), "%s", "Failed to access results.\nPress click to exit.");
    }

    clear_videofile_cnt();

    // Restore logging if needed
    if (logfile) {
        log_file_open(logfile);
    }

    page_storage_open_status_box("SD Card Format Status", text);
    page_storage_cancel();
}

/**
 * Callback invoked once `Repair SD` is triggered and confirmed via the menu.
 */
static void page_storage_repair_sd_timer_cb(struct _lv_timer_t *timer) {
    char text[128];

    // Temporarily disable logging if needed
    const char *logfile = NULL;
    if (log_file_opened()) {
        logfile = g_setting.storage.selftest ? SELF_TEST_FILE
                                             : APP_LOG_FILE;
        log_file_close();
    }

    unlink("/tmp/fsck.result");
    system("/mnt/app/script/chkfixsd.sh > /tmp/chkfixsd.log 2>&1 &");

    int timeout_seconds = 30;
    int timeout_interval = 0;
    while (!file_exists("/tmp/fsck.result") && ++timeout_interval < timeout_seconds) {
        sleep(1);
    }

    FILE *results = fopen("/tmp/fsck.result", "r");
    if (results) {
        int exit_code;
        if (fscanf(results, "%d", &exit_code) == 1) {
            if (exit_code == 0) {
                snprintf(text, sizeof(text), "%s", "Repair was successful.\nPress click to exit.");
            } else {
                snprintf(text, sizeof(text), "%s", "Repair was not successful.\nPress click to exit.");
            }
        } else {
            snprintf(text, sizeof(text), "%s", "Failed to extract results.\nPress click to exit.");
        }

        fclose(results);
    } else {
        snprintf(text, sizeof(text), "%s", "Failed to access results.\nPress click to exit.");
    }

    // Restore logging if needed
    if (logfile) {
        log_file_open(logfile);
    }

    page_storage_open_status_box("SD Card Repair Status", text);
    page_storage_cancel();
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

    create_btn_group_item(&page_storage.logging, cont, 2, "Logging", "On", "Off", "", "", 0);
    btn_group_set_sel(&page_storage.logging, g_setting.storage.logging ? 0 : 1);

    page_storage.format_sd = create_label_item(cont, "Format SD Card", 1, 1, 3);
    page_storage.repair_sd = create_label_item(cont, "Repair SD Card", 1, 2, 3);
    page_storage.back = create_label_item(cont, "< Back", 1, 3, 1);

    if (g_setting.storage.selftest) {
        page_storage.note = lv_label_create(cont);
        lv_label_set_text(page_storage.note, "Self-Test is enabled, All storage options are disabled");
        lv_obj_set_style_text_font(page_storage.note, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_align(page_storage.note, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_color(page_storage.note, lv_color_make(255, 255, 255), 0);
        lv_obj_set_style_pad_top(page_storage.note, 12, 0);
        lv_label_set_long_mode(page_storage.note, LV_LABEL_LONG_WRAP);
        lv_obj_set_grid_cell(page_storage.note, LV_GRID_ALIGN_START, 1, 4, LV_GRID_ALIGN_START, 7, 2);
    }

    page_storage.status = create_msgbox_item("Status", "None");
    lv_obj_add_flag(page_storage.status, LV_OBJ_FLAG_HIDDEN);

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
    page_storage_close_status_box();
    page_storage_cancel();
}

/**
 * Main navigation routine for this page.
 */
static void page_storage_on_roller(uint8_t key) {
    // Ignore commands until timer has expired before allowing user to proceed.
    if (page_storage.confirm_format == 2 ||
        page_storage.confirm_repair == 2 ||
        page_storage.status_displayed) {
        return;
    }

    // If a click was not previous pressed to confirm, then update is canceled.
    page_storage_cancel();
}

/**
 * Main input selection routine for this page.
 */
static void page_storage_on_click(uint8_t key, int sel) {
    if (page_storage.status_displayed) {
        page_storage_close_status_box();
        return;
    }

    switch (sel) {
    case 0:
        if (!g_setting.storage.selftest) {
            btn_group_toggle_sel(&page_storage.logging);
            g_setting.storage.logging = btn_group_get_sel(&page_storage.logging) == 0;
            settings_put_bool("storage", "logging", g_setting.storage.logging);

            if (g_setting.storage.logging) {
                if (!log_file_opened()) {
                    log_file_open(APP_LOG_FILE);
                }
            } else if (log_file_opened()) {
                log_file_close();
            }
        }
        break;
    case 1:
        if (!g_setting.storage.selftest) {
            if (page_storage.confirm_format) {
                page_storage.confirm_format = 2;
                page_storage_format_sd_timer = lv_timer_create(page_storage_format_sd_timer_cb, 1000, NULL);
                lv_timer_set_repeat_count(page_storage_format_sd_timer, 1);
                lv_label_set_text(page_storage.format_sd, "Format SD Card #FF0000 Formatting...#");
            } else {
                page_storage.confirm_format = 1;
                lv_label_set_text(page_storage.format_sd, "Format SD Card #FFFF00 Click to confirm or Scroll to cancel...#");
            }
        }
        break;
    case 2:
        if (!g_setting.storage.selftest) {
            if (page_storage.confirm_repair) {
                page_storage.confirm_repair = 2;
                page_storage_repair_sd_timer = lv_timer_create(page_storage_repair_sd_timer_cb, 1000, NULL);
                lv_timer_set_repeat_count(page_storage_repair_sd_timer, 1);
                lv_label_set_text(page_storage.repair_sd, "Repair SD Card #FF0000 Repairing...#");

            } else {
                page_storage.confirm_repair = 1;
                lv_label_set_text(page_storage.repair_sd, "Repair SD Card #FFFF00 Click to confirm or Scroll to cancel...#");
            }
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
        .max = ITEM_LIST_TOTAL,
    },
    .name = "Storage",
    .create = page_storage_create,
    .enter = page_storage_enter,
    .exit = page_storage_exit,
    .on_roller = page_storage_on_roller,
    .on_click = page_storage_on_click,
    .on_right_button = page_storage_on_right_button,
};
