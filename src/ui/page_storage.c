#include "page_storage.h"

#include <stdlib.h>
#include <unistd.h>

#include <log/log.h>
#include <minIni.h>

#include "core/common.hh"
#include "core/settings.h"
#include "lang/language.h"
#include "record/record_definitions.h"
#include "ui/page_common.h"
#include "ui/page_playback.h"
#include "util/filesystem.h"
#include "util/sdcard.h"
#include "util/system.h"

extern void (*sdcard_ready_cb)();

/**
 * Types
 */
typedef enum {
    ITEM_LOGGING,
    ITEM_FORMAT,
    ITEM_REPAIR,
    ITEM_CLEAR_DVR,
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
    lv_obj_t *clear_dvr;
    int confirm_format;
    int confirm_repair;
    int confirm_clear;
    bool status_displayed;
    lv_obj_t *back;
    lv_obj_t *status;
    lv_obj_t *note;
    bool disable_controls;
    bool is_sd_repair_active;
    bool was_sd_repair_invoked;
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

static void disable_controls() {
    page_storage.disable_controls = true;

    for (int i = 0; i < ITEM_LIST_TOTAL - 1; i++) {
        lv_obj_clear_flag(pp_storage.p_arr.panel[i], FLAG_SELECTABLE);
    }
    btn_group_enable(&page_storage.logging, !page_storage.disable_controls);
    lv_obj_add_state(page_storage.format_sd, STATE_DISABLED);
    lv_obj_add_state(page_storage.repair_sd, STATE_DISABLED);
    lv_obj_add_state(page_storage.clear_dvr, STATE_DISABLED);
}

static void enable_controls() {
    page_storage.disable_controls = false;

    for (int i = 0; i < ITEM_LIST_TOTAL - 1; i++) {
        lv_obj_add_flag(pp_storage.p_arr.panel[i], FLAG_SELECTABLE);
    }
    btn_group_enable(&page_storage.logging, !page_storage.disable_controls);
    lv_obj_clear_state(page_storage.format_sd, STATE_DISABLED);
    lv_obj_clear_state(page_storage.repair_sd, STATE_DISABLED);
    lv_obj_clear_state(page_storage.clear_dvr, STATE_DISABLED);
}

/**
 * Cancel operation.
 */
static void page_storage_cancel() {
    page_storage.confirm_format = 0;
    page_storage.confirm_repair = 0;
    page_storage.confirm_clear = 0;
    lv_label_set_text(page_storage.format_sd, _lang("Format SD Card"));
    lv_label_set_text(page_storage.repair_sd, _lang("Repair SD Card"));
    lv_label_set_text(page_storage.clear_dvr, _lang("Clear DVR Folder"));
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
    if (!sdcard_inserted()) {
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
    system_exec(shell_command);

    int timeout_interval = 0;
    while (!fs_file_exists(log_file) && ++timeout_interval < 5) {
        sleep(1);
    }

    if (timeout_interval > 5) {
        status = FMC_ERR_PROCESS_DID_NOT_START;
    } else {
        timeout_interval = 0;
        while (!fs_file_exists(results_file) && ++timeout_interval < 60) {
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
    sdcard_update_free_size();

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
    system_exec(shell_command);

    int timeout_interval = 0;
    while (!fs_file_exists(log_file) && ++timeout_interval < 5) {
        sleep(1);
    }

    if (timeout_interval > 5) {
        status = RPC_ERR_PROCESS_DID_NOT_START;
    } else {
        timeout_interval = 0;
        while (!fs_file_exists(results_file) && ++timeout_interval < 60) {
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
                        system_exec(shell_move_files);
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
    char buf[128];

    switch (page_storage_format_sd()) {
    case FMC_SUCCESS:
        snprintf(buf, sizeof(buf), "%s.\n%s.", _lang("Format was successful"), _lang("Press click to exit"));
        break;
    case FMC_FAILURE:
        snprintf(buf, sizeof(buf), "%s.\n%s.", _lang("Format has failed"), _lang("Press click to exit"));
        break;
    case FMC_ERR_SDCARD_NOT_INSERTED:
        snprintf(buf, sizeof(buf), "%s.\n%s.", _lang("Please insert a SD Card"), _lang("Press click to exit"));
        break;
    case FMC_ERR_RESULTS_NOT_EXTRACTED:
        snprintf(buf, sizeof(buf), "%s.\n%s.", _lang("Failed to extract results"), _lang("Press click to exit"));
        break;
    case FMC_ERR_RESULTS_NOT_ACCESSIBLE:
        snprintf(buf, sizeof(buf), "%s.\n%s.", _lang("Failed to access results"), _lang("Press click to exit"));
        break;
    case FMC_ERR_RESULTS_FILE_MISSING:
        snprintf(buf, sizeof(buf), "%s.\n%s.", _lang("Failed to generate results"), _lang("Press click to exit"));
        break;
    case FMC_ERR_PROCESS_DID_NOT_START:
        snprintf(buf, sizeof(buf), "%s.\n%s.", _lang("Failed to start format"), _lang("Press click to exit"));
        break;
    default:
        snprintf(buf, sizeof(buf), "%s.\n%s.", _lang("Unsupported status code"), _lang("Press click to exit"));
        break;
    }

    page_storage_open_status_box(_lang("SD Card Format Status"), buf);
    page_storage_cancel();
}

/**
 * Callback invoked once `Repair SD` is triggered and confirmed via the menu.
 */
static void page_storage_repair_sd_timer_cb(struct _lv_timer_t *timer) {
    char buf[128];
    switch (page_storage_repair_sd()) {
    case RPC_SUCCESS_NO_CHANGES:
        snprintf(buf, sizeof(buf), "%s.\n%s.", _lang("Filesystem is OK"), _lang("Press click to exit."));
        break;
    case RPC_SUCCESS_CARD_FIXED:
        snprintf(buf, sizeof(buf), "%s.\n%s.", _lang("Filesystem was modified and fixed"), _lang("Press click to exit."));
        break;
    case RPC_ERR_SDCARD_NOT_INSERTED:
        snprintf(buf, sizeof(buf), "%s.\n%s.", _lang("Please insert a SD Card"), _lang("Press click to exit."));
        break;
    case RPC_ERR_RESULTS_NOT_EXTRACTED:
        snprintf(buf, sizeof(buf), "%s.\n%s.", _lang("Failed to extract results"), _lang("Press click to exit."));
        break;
    case RPC_ERR_RESULTS_NOT_ACCESSIBLE:
        snprintf(buf, sizeof(buf), "%s.\n%s.", _lang("Failed to access results"), _lang("Press click to exit."));
        break;
    case RPC_ERR_FAILED_TO_REMOUNT_CARD:
        snprintf(buf, sizeof(buf), "%s.\n%s.", _lang("Failed to remount SD Card"), _lang("Press click to exit."));
        break;
    case RPC_ERR_RESULTS_FILE_MISSING:
        snprintf(buf, sizeof(buf), "%s.\n%s.", _lang("Failed to generate results"), _lang("Press click to exit."));
        break;
    case RPC_ERR_PROCESS_DID_NOT_START:
        snprintf(buf, sizeof(buf), "%s.\n%s.", _lang("Failed to start repair"), _lang("Press click to exit."));
        break;
    default:
        snprintf(buf, sizeof(buf), "%s.\n%s.", _lang("Unsupported status code"), _lang("Press click to exit."));
        break;
    }

    page_storage_open_status_box(_lang("SD Card Repair Status"), buf);
    page_storage_cancel();
}

/**
 * Main allocation routine for this page.
 */
static lv_obj_t *page_storage_create(lv_obj_t *parent, panel_arr_t *arr) {
    char buf[256];
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1053, 900);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 1053, 894);

    snprintf(buf, sizeof(buf), "%s:", _lang("Storage"));
    create_text(NULL, section, false, buf, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, 1280, 800);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

    create_select_item(arr, cont);

    create_btn_group_item(&page_storage.logging, cont, 2, _lang("Logging"), _lang("On"), _lang("Off"), "", "", 0);
    btn_group_set_sel(&page_storage.logging, g_setting.storage.logging ? 0 : 1);

    page_storage.format_sd = create_label_item(cont, _lang("Format SD Card"), 1, 1, 3);
    page_storage.repair_sd = create_label_item(cont, _lang("Repair SD Card"), 1, 2, 3);
    page_storage.clear_dvr = create_label_item(cont, _lang("Clear DVR Folder"), 1, 3, 3);
    snprintf(buf, sizeof(buf), "< %s", _lang("Back"));
    page_storage.back = create_label_item(cont, buf, 1, 4, 1);

    page_storage.note = lv_label_create(cont);
    lv_label_set_text(page_storage.note, "");
    lv_obj_set_style_text_font(page_storage.note, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(page_storage.note, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(page_storage.note, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_pad_top(page_storage.note, 12, 0);
    lv_label_set_long_mode(page_storage.note, LV_LABEL_LONG_WRAP);
    lv_obj_set_grid_cell(page_storage.note, LV_GRID_ALIGN_START, 1, 4, LV_GRID_ALIGN_START, 5, 2);

    if (g_setting.storage.selftest) {
        snprintf(buf, sizeof(buf), "%s,%s.", _lang("Self-Test is enabled"), _lang("All storage options are disabled"));
        lv_label_set_text(page_storage.note, buf);
        disable_controls();
    } else {
        if (fs_file_exists(DEVELOP_SCRIPT) || fs_file_exists(APP_BIN_FILE)) {
            snprintf(buf, sizeof(buf), "%s, %s.\n%s:\n%s\n%s",
                     _lang("Detected files being accessed by SD Card"),
                     _lang("All storage options are disabled"),
                     _lang("Remove the following files from the SD Card and try again"),
                     DEVELOP_SCRIPT,
                     APP_BIN_FILE);
            lv_label_set_text(page_storage.note, buf);
            disable_controls();
        }
    }

    page_storage.status = create_msgbox_item(_lang("Status"), _lang("None"));
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
        page_storage.confirm_clear == 2 ||
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
    char buf[128];
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
                snprintf(buf, sizeof(buf), "%s #FF0000 %s...#", _lang("Format SD Card"), _lang("Formatting"));
                lv_label_set_text(page_storage.format_sd, buf);
            } else {
                page_storage.confirm_format = 1;
                snprintf(buf, sizeof(buf), "%s #FFFF00 %s...#", _lang("Format SD Card"), _lang("Click to confirm or Scroll to cancel"));
                lv_label_set_text(page_storage.format_sd, buf);
            }
        }
        break;
    case ITEM_REPAIR:
        if (!page_storage.disable_controls) {
            if (page_storage.confirm_repair) {
                page_storage.confirm_repair = 2;
                page_storage_repair_sd_timer = lv_timer_create(page_storage_repair_sd_timer_cb, 1000, NULL);
                lv_timer_set_repeat_count(page_storage_repair_sd_timer, 1);
                snprintf(buf, sizeof(buf), "%s #FF0000 %s...#", _lang("Repair SD Card"), _lang("Repairing"));
                lv_label_set_text(page_storage.repair_sd, buf);

            } else {
                page_storage.confirm_repair = 1;
                snprintf(buf, sizeof(buf), "%s #FFFF00 %s...#", _lang("Repair SD Card"), _lang("Click to confirm or Scroll to cancel"));
                lv_label_set_text(page_storage.repair_sd, buf);
            }
        }
        break;
    case ITEM_CLEAR_DVR:
        if (!page_storage.disable_controls) {
            if (page_storage.confirm_clear) {
                page_storage.confirm_clear = 2;
                snprintf(buf, sizeof(buf), "%s #FF0000 %s...#", _lang("Clear DVR Folder"), _lang("Removing"));
                lv_label_set_text(page_storage.clear_dvr, buf);
                lv_timer_handler();
                LOGI("Clear dvr folder");
                char buf[256];
                snprintf(buf, sizeof(buf), "rm -rf %s%s/*", REC_diskPATH, REC_packPATH);
                system_exec(buf);
                snprintf(buf, sizeof(buf), "%s #FFFF00 %s#", _lang("Clear DVR Folder"), _lang("Done"));
                lv_label_set_text(page_storage.clear_dvr, buf);
                LOGI("Clear done");
                page_storage.confirm_clear = 3;
                g_sdcard_det_req = 1;
            } else {
                page_storage.confirm_clear = 1;
                snprintf(buf, sizeof(buf), "%s #FF0000 %s...#", _lang("Clear DVR Folder"), _lang("Click to confirm or Scroll to cancel"));
                lv_label_set_text(page_storage.clear_dvr, buf);
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

static void page_storage_post_bootup_action(void (*complete_callback)()) {
    page_storage_init_auto_sd_repair();
    sdcard_ready_cb = complete_callback;
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
    .on_created = NULL,
    .on_update = NULL,
    .on_roller = page_storage_on_roller,
    .on_click = page_storage_on_click,
    .on_right_button = page_storage_on_right_button,
    .post_bootup_run_priority = 50,
    .post_bootup_run_function = page_storage_post_bootup_action,
};

/**
 * Worker thread for repairing SD Card.
 */
static void *page_storage_repair_thread(void *arg) {
    char buf[128];

    if (!page_storage.disable_controls) {
        page_storage.was_sd_repair_invoked = true;
        page_storage.is_auto_sd_repair_active = true;
        pthread_mutex_lock(&lvgl_mutex);
        disable_controls();
        snprintf(buf, sizeof(buf), "%s, %s.", _lang("SD Card integrity check is active"), _lang("controls are disabled until process has completed"));
        lv_label_set_text(page_storage.note, buf);
        pthread_mutex_unlock(&lvgl_mutex);

        page_storage_repair_sd();

        pthread_mutex_lock(&lvgl_mutex);
        enable_controls();
        lv_label_set_text(page_storage.note, "");
        pthread_mutex_unlock(&lvgl_mutex);

        page_storage.is_auto_sd_repair_active = false;
    }

    pthread_exit(NULL);
}

/**
 * Returns true if a repair was ever activated.
 */
bool page_storage_was_sd_repair_invoked() {
    return page_storage.was_sd_repair_invoked;
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
