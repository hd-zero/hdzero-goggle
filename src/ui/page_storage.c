#include "page_storage.h"

#include <stdlib.h>
#include <unistd.h>

#include <log/log.h>
#include <minIni.h>

#include "core/common.hh"
#include "core/settings.h"
#include "ui/page_playback.h"
#include "util/file.h"
#include "util/sdcard.h"

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

typedef enum {
    FMC_SUCCESS = 0,
    FMC_FAILURE,
    FMC_ERR_SDCARD_NOT_INSERTED,
    FMC_ERR_RESULTS_NOT_EXTRACTED,
    FMC_ERR_RESULTS_NOT_ACCESSIBLE,
    FMC_ERR_RESULTS_FILE_MISSING,
    FMC_ERR_PROCESS_DID_NOT_START,
} format_codes_t;

typedef enum {
    RPC_SUCCESS_NO_CHANGES = 0,
    RPC_SUCCESS_CARD_FIXED,
    RPC_ERR_SDCARD_NOT_INSERTED,
    RPC_ERR_RESULTS_NOT_EXTRACTED,
    RPC_ERR_RESULTS_NOT_ACCESSIBLE,
    RPC_ERR_FAILED_TO_REMOUNT_CARD,
    RPC_ERR_RESULTS_FILE_MISSING,
    RPC_ERR_PROCESS_DID_NOT_START,
} repair_codes_t;

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
    bool disable_controls;
    bool is_sd_repair_active;
    bool is_auto_sd_repair_active;
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
 * The formatting routine.
 */
static format_codes_t page_storage_format_sd() {
    if (!sdcard_mounted()) {
        return FMC_ERR_SDCARD_NOT_INSERTED;
    }

    const char *shell_command = "/mnt/app/script/formatsd.sh > /tmp/formatsd.log 2>&1 &";
    const char *results_file = "/tmp/mkfs.result";
    const char *log_file = "/tmp/mkfs.log";
    format_codes_t status = FMC_SUCCESS;

    // Temporarily disable logging if needed
    const char *applogfile = NULL;
    if (log_file_opened()) {
        applogfile = g_setting.storage.selftest ? SELF_TEST_FILE
                                                : APP_LOG_FILE;
        log_file_close();
    }

    unlink(results_file);
    system(shell_command);

    int timeout_interval = 0;
    while (!file_exists(log_file) && ++timeout_interval < 5) {
        sleep(1);
    }

    if (timeout_interval > 5) {
        status = FMC_ERR_PROCESS_DID_NOT_START;
    } else {
        timeout_interval = 0;
        while (!file_exists(results_file) && ++timeout_interval < 60) {
            sleep(1);
        }
        if (timeout_interval > 60) {
            status = FMC_ERR_RESULTS_FILE_MISSING;
        } else {
            FILE *results = fopen(results_file, "r");
            if (!results) {
                status = FMC_ERR_RESULTS_NOT_ACCESSIBLE;
            } else {
                int exit_code;
                if (fscanf(results, "%d", &exit_code) != 1) {
                    status = FMC_ERR_RESULTS_NOT_EXTRACTED;
                } else {
                    if (exit_code != 0) {
                        status = FMC_FAILURE;

                    } else {
                        status = FMC_SUCCESS;
                    }
                }
                fclose(results);
            }
        }
    }

    clear_videofile_cnt();

    // Restore logging if needed
    if (applogfile) {
        log_file_open(applogfile);
    }

    return status;
}

/**
 * The repairing routine.
 */
static repair_codes_t page_storage_repair_sd() {
    if (!sdcard_mounted()) {
        return RPC_ERR_SDCARD_NOT_INSERTED;
    }

    page_storage.is_sd_repair_active = true;

    const char *shell_move_files = "mkdir /mnt/extsd/FSCK; mv /mnt/extsd/FSCK*.REC /mnt/extsd/FSCK/";
    const char *shell_command = "/mnt/app/script/chkfixsd.sh > /tmp/chkfixsd.log 2>&1 &";
    const char *results_file = "/tmp/fsck.result";
    const char *log_file = "/tmp/fsck.log";
    repair_codes_t status = RPC_SUCCESS_NO_CHANGES;

    // Temporarily disable logging if needed
    const char *app_log_file = NULL;
    if (log_file_opened()) {
        app_log_file = g_setting.storage.selftest ? SELF_TEST_FILE
                                                  : APP_LOG_FILE;
        log_file_close();
    }

    unlink(results_file);
    system(shell_command);

    int timeout_interval = 0;
    while (!file_exists(log_file) && ++timeout_interval < 5) {
        sleep(1);
    }

    if (timeout_interval > 5) {
        status = RPC_ERR_PROCESS_DID_NOT_START;
    } else {
        timeout_interval = 0;
        while (!file_exists(results_file) && ++timeout_interval < 60) {
            sleep(1);
        }
        if (timeout_interval > 60) {
            status = RPC_ERR_RESULTS_FILE_MISSING;
        } else {
            timeout_interval = 0;
            while (!sdcard_mounted() && ++timeout_interval < 10) {
                sleep(1);
            }
            if (timeout_interval > 10) {
                status = RPC_ERR_FAILED_TO_REMOUNT_CARD;
            } else {
                FILE *results = fopen(results_file, "r");
                if (!results) {
                    status = RPC_ERR_RESULTS_NOT_ACCESSIBLE;
                } else {
                    int exit_code;
                    if (fscanf(results, "%d", &exit_code) != 1) {
                        status = RPC_ERR_RESULTS_NOT_EXTRACTED;
                    } else {
                        if (exit_code == 1) {
                            status = RPC_SUCCESS_CARD_FIXED;
                        } else {
                            status = RPC_SUCCESS_NO_CHANGES;
                        }
                        system(shell_move_files);
                    }
                    fclose(results);
                }
            }
        }
    }

    // Restore logging if needed
    if (app_log_file) {
        log_file_open(app_log_file);
    }

    page_storage.is_sd_repair_active = false;

    return status;
}

/**
 * Callback invoked once `Format SD` is triggered and confirmed via the menu.
 */
static void page_storage_format_sd_timer_cb(struct _lv_timer_t *timer) {
    char text[128];

    switch (page_storage_format_sd()) {
    case FMC_SUCCESS:
        snprintf(text, sizeof(text), "%s", "Format was successful.\nPress click to exit.");
        break;
    case FMC_FAILURE:
        snprintf(text, sizeof(text), "%s", "Format has failed.\nPress click to exit.");
        break;
    case FMC_ERR_SDCARD_NOT_INSERTED:
        snprintf(text, sizeof(text), "%s", "Please insert a SD Card.\nPress click to exit.");
        break;
    case FMC_ERR_RESULTS_NOT_EXTRACTED:
        snprintf(text, sizeof(text), "%s", "Failed to extract results.\nPress click to exit.");
        break;
    case FMC_ERR_RESULTS_NOT_ACCESSIBLE:
        snprintf(text, sizeof(text), "%s", "Failed to access results.\nPress click to exit.");
        break;
    case FMC_ERR_RESULTS_FILE_MISSING:
        snprintf(text, sizeof(text), "%s", "Failed to generate results.\nPress click to exit.");
        break;
    case FMC_ERR_PROCESS_DID_NOT_START:
        snprintf(text, sizeof(text), "%s", "Failed to start format.\nPress click to exit.");
        break;
    default:
        snprintf(text, sizeof(text), "%s", "Unsupported status code.\nPress click to exit.");
        break;
    }

    page_storage_open_status_box("SD Card Format Status", text);
    page_storage_cancel();
}

/**
 * Callback invoked once `Repair SD` is triggered and confirmed via the menu.
 */
static void page_storage_repair_sd_timer_cb(struct _lv_timer_t *timer) {
    char text[128];
    switch (page_storage_repair_sd()) {
    case RPC_SUCCESS_NO_CHANGES:
        snprintf(text, sizeof(text), "%s", "Filesystem is OK.\nPress click to exit.");
        break;
    case RPC_SUCCESS_CARD_FIXED:
        snprintf(text, sizeof(text), "%s", "Filesystem was modified and fixed.\nPress click to exit.");
        break;
    case RPC_ERR_SDCARD_NOT_INSERTED:
        snprintf(text, sizeof(text), "%s", "Please insert a SD Card.\nPress click to exit.");
        break;
    case RPC_ERR_RESULTS_NOT_EXTRACTED:
        snprintf(text, sizeof(text), "%s", "Failed to extract results.\nPress click to exit.");
        break;
    case RPC_ERR_RESULTS_NOT_ACCESSIBLE:
        snprintf(text, sizeof(text), "%s", "Failed to access results.\nPress click to exit.");
        break;
    case RPC_ERR_FAILED_TO_REMOUNT_CARD:
        snprintf(text, sizeof(text), "%s", "Failed to remount SD Card.\nPress click to exit.");
        break;
    case RPC_ERR_RESULTS_FILE_MISSING:
        snprintf(text, sizeof(text), "%s", "Failed to generate results.\nPress click to exit.");
        break;
    case RPC_ERR_PROCESS_DID_NOT_START:
        snprintf(text, sizeof(text), "%s", "Failed to start repair.\nPress click to exit.");
        break;
    default:
        snprintf(text, sizeof(text), "%s", "Unsupported status code.\nPress click to exit.");
        break;
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

    page_storage.note = lv_label_create(cont);
    lv_label_set_text(page_storage.note, "");
    lv_obj_set_style_text_font(page_storage.note, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(page_storage.note, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(page_storage.note, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_pad_top(page_storage.note, 12, 0);
    lv_label_set_long_mode(page_storage.note, LV_LABEL_LONG_WRAP);
    lv_obj_set_grid_cell(page_storage.note, LV_GRID_ALIGN_START, 1, 4, LV_GRID_ALIGN_START, 5, 2);

    if (g_setting.storage.selftest) {
        lv_label_set_text(page_storage.note, "Self-Test is enabled, All storage options are disabled.");
        page_storage.disable_controls = true;
    } else {
        if (file_exists(DEVELOP_SCRIPT) || file_exists(APP_BIN_FILE)) {
            char text[256];
            snprintf(text, sizeof(text), "Detected files being accessed by SD Card, All storage options are disabled.\n"
                                         "Remove the following files from the SD Card and try again:\n" DEVELOP_SCRIPT "\n" APP_BIN_FILE);
            lv_label_set_text(page_storage.note, text);
            page_storage.disable_controls = true;
        }
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
    case ITEM_LOGGING:
        if (!page_storage.disable_controls) {
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
    case ITEM_FORMAT:
        if (!page_storage.disable_controls) {
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
    case ITEM_REPAIR:
        if (!page_storage.disable_controls) {
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

/**
 * Worker thread for repairing SD Card.
 */
static void *page_storage_repair_thread(void *arg) {
    if (!page_storage.disable_controls) {
        page_storage.is_auto_sd_repair_active = true;
        page_storage.disable_controls = true;
        lv_label_set_text(page_storage.note, "SD Card integrity check is active, controls are disabled until process has completed.");
        page_storage_repair_sd();
        page_storage.disable_controls = false;
        lv_label_set_text(page_storage.note, "");
        page_storage.is_auto_sd_repair_active = false;
    }
    pthread_exit(NULL);
}

/**
 * Returns true if repairing is active.
 */
bool page_storage_is_sd_repair_active() {
    return page_storage.is_sd_repair_active;
}

/**
 * Once initialized detach until completed.
 */
void page_storage_init_auto_sd_repair() {
    if (!page_storage.is_auto_sd_repair_active) {
        pthread_t tid;
        if (!pthread_create(&tid, NULL, page_storage_repair_thread, NULL)) {
            pthread_detach(tid);
        }
    }
}
