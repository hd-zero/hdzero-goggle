#include "page_version.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <log/log.h>
#include <minIni.h>

#include "common.hh"
#include "core/app_state.h"
#include "core/elrs.h"
#include "core/esp32_flash.h"
#include "core/osd.h"
#include "core/settings.h"
#include "driver/beep.h"
#include "driver/dm5680.h"
#include "driver/esp32.h"
#include "driver/fans.h"
#include "driver/i2c.h"
#include "driver/uart.h"
#include "ui/page_common.h"
#include "ui/ui_main_menu.h"
#include "ui/ui_style.h"
#include "util/filesystem.h"
#include "util/strings.h"
#include "util/system.h"

enum {
    ROW_CUR_VERSION = 0,
    ROW_RESET_ALL_SETTINGS,
    ROW_UPDATE_VTX,
    ROW_UPDATE_GOGGLE,
    ROW_UPDATE_ESP32,
    ROW_BACK,

    ROW_COUNT
};

typedef enum {
    CONFIRMATION_UNCONFIRMED = 0,
    CONFIRMATION_CONFIRMED,
    CONFIRMATION_TIMEOUT
} ui_confirmation_t;

typedef struct {
    void (*flash)();
    panel_arr_t page;
    panel_arr_t this;
    lv_obj_t *container;
    lv_obj_t *msgbox;
    lv_obj_t *dropdown;
    lv_obj_t *update;
    lv_obj_t *back;
    char *alt_title;
    char path[256];
    char **files;
    bool dropdown_focused;
    bool visible;
    int which;
    int count;
    bool zipped;
    bool ready;
} fw_select_t;

static lv_coord_t col_dsc[] = {160, 160, 160, 160, 160, 160, 160, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};

static lv_obj_t *btn_reset_all_settings = NULL;
static lv_obj_t *bar_vtx = NULL;
static lv_obj_t *btn_vtx = NULL;
static lv_obj_t *bar_goggle = NULL;
static lv_obj_t *btn_goggle = NULL;
static lv_obj_t *bar_esp = NULL;
static lv_obj_t *btn_esp = NULL;
static lv_obj_t *label_esp = NULL;
static lv_obj_t *msgbox_update_complete = NULL;
static lv_obj_t *msgbox_settings_reset = NULL;
static lv_obj_t *msgbox_release_notes = NULL;
static lv_obj_t *label_note = NULL;
static lv_obj_t *alert_img = NULL;
static fw_select_t fw_select_goggle;
static fw_select_t fw_select_vtx;
static fw_select_t *fw_select_current = &fw_select_vtx;

#define ADDR_AL            0x65
#define ADDR_FPGA          0x64
#define I2C_Write(s, a, d) i2c_write(2, s, a, d)
#define I2C_Read(s, a)     i2c_read(2, s, a)

static bool is_need_update_progress = false;
static bool reboot_flag = false;
static lv_obj_t *cur_ver_label;
static int reset_all_settings_confirm = CONFIRMATION_UNCONFIRMED;
static atomic_bool autoscan_filesystem = ATOMIC_VAR_INIT(true);

#undef RETURN_ON_ERROR
#define RETURN_ON_ERROR(m, x)              \
    do {                                   \
        esp_loader_error_t _err_ = (x);    \
        if (_err_ != ESP_LOADER_SUCCESS) { \
            LOGE("ERR %s: %d", m, _err_);  \
            return _err_;                  \
        }                                  \
    } while (0)

/**
 * Declarations
 */
static void page_version_on_roller(uint8_t key);
static void page_version_on_click(uint8_t key, int sel);
static void page_version_on_right_button(bool is_short);
static void page_version_on_roller_fw_select(uint8_t key);
static void page_version_on_click_fw_select(uint8_t key, int sel);

static esp_loader_error_t flash_esp32_file(char *path, uint32_t offset) {
    char fpath[80];
    strcpy(fpath, "/mnt/extsd/ELRS/");
    strcat(fpath, path);

    FILE *image = fopen(fpath, "r");
    if (image == NULL) {
        LOGI("Firmware file does not exist %s", fpath);
        return ESP_LOADER_SUCCESS;
    }

    lv_label_set_text(btn_esp, path);
    lv_timer_handler();

    fseek(image, 0L, SEEK_END);
    size_t size = ftell(image);
    rewind(image);

    uint8_t buffer[4096];
    RETURN_ON_ERROR("start", esp_loader_flash_start(offset, size, sizeof(buffer)));
    uint32_t read, current = 0;
    while ((read = fread(buffer, 1, sizeof(buffer), image))) {
        RETURN_ON_ERROR("write", esp_loader_flash_write(buffer, read));
        current += read;
        int percent = (current * 100) / size;
        lv_bar_set_value(bar_esp, percent, LV_ANIM_OFF);
        lv_timer_handler();
        LOGD("%d %d %d", current, size, percent);
    }
    return ESP_LOADER_SUCCESS;
}

static esp_loader_error_t flash_esp32() {
    disable_esp32();

    esp_loader_connect_args_t config = ESP_LOADER_CONNECT_DEFAULT();
    RETURN_ON_ERROR("init", loader_port_init());
    RETURN_ON_ERROR("connect", esp_loader_connect(&config));
    RETURN_ON_ERROR("get_target", esp_loader_get_target() == ESP32_CHIP ? ESP_LOADER_SUCCESS : ESP_LOADER_ERROR_UNSUPPORTED_CHIP);

    lv_bar_set_value(bar_esp, 0, LV_ANIM_OFF);
    RETURN_ON_ERROR("flash", flash_esp32_file("bootloader.bin", 0x1000));
    lv_bar_set_value(bar_esp, 0, LV_ANIM_OFF);
    RETURN_ON_ERROR("flash", flash_esp32_file("partitions.bin", 0x8000));
    lv_bar_set_value(bar_esp, 0, LV_ANIM_OFF);
    RETURN_ON_ERROR("flash", flash_esp32_file("boot_app0.bin", 0xE000));
    lv_bar_set_value(bar_esp, 0, LV_ANIM_OFF);
    RETURN_ON_ERROR("flash", flash_esp32_file("firmware.bin", 0x10000));

    RETURN_ON_ERROR("finish", esp_loader_flash_finish(true));
    loader_port_close();

    if (g_setting.elrs.enable)
        enable_esp32();

    return ESP_LOADER_SUCCESS;
}

static bool flash_elrs() {
    esp_loader_error_t ret = flash_esp32();
    loader_port_close();

    if (g_setting.elrs.enable)
        enable_esp32();

    return ret;
}

static int flash_hdzero(const char *dst_path, const char *dst_file,
                        const char *src_path, const char *src_file,
                        const char *update_script, bool is_zipped) {
    uint8_t ret = 2;
    char cmd_buff[1024] = {0};

    // Flush working directory
    snprintf(cmd_buff,
             sizeof(cmd_buff),
             "rm -rf %s && mkdir -p %s",
             dst_path, dst_path);

    if (0 == system_exec(cmd_buff)) {
        // Extract or copy files to working directory
        if (is_zipped) {
            snprintf(cmd_buff, sizeof(cmd_buff),
                     "unzip %s/%s -d %s",
                     src_path, src_file, dst_path);
        } else {
            snprintf(cmd_buff, sizeof(cmd_buff),
                     "cp -a %s/%s %s/%s",
                     src_path, src_file,
                     dst_path, dst_file);
        }
        // Now begin flashing
        if (0 == system_exec(cmd_buff)) {
            snprintf(cmd_buff, sizeof(cmd_buff),
                     "%s %s/%s",
                     update_script, dst_path, dst_file);
            ret = command_monitor(cmd_buff);
        }
    }

    return ret;
}

static void flash_vtx() {
    uint8_t ret = 2;
    char cmd_buff[1024] = {0};

    lv_obj_clear_flag(bar_vtx, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(btn_vtx, "Flashing...");
    lv_timer_handler();

    is_need_update_progress = true;
    ret = flash_hdzero("/tmp/VTX",
                       "HDZERO_TX.bin",
                       fw_select_vtx.path,
                       fw_select_vtx.files[fw_select_vtx.which],
                       "/mnt/app/script/update_vtx.sh",
                       fw_select_vtx.zipped);
    is_need_update_progress = false;

    if (ret == 1) {
        if (fs_compare_files("/tmp/HDZERO_TX.bin", "/tmp/HDZERO_TX_RB.bin")) {
            lv_label_set_text(btn_vtx, "#00FF00 SUCCESS#");
        } else
            lv_label_set_text(btn_vtx, "#FF0000 Verification failed, try it again#");
    } else if (ret == 2) {
        lv_label_set_text(btn_vtx, "#FFFF00 No firmware found.#");
    } else {
        lv_label_set_text(btn_vtx, "#FF0000 Failed, check connection...#");
    }
    lv_timer_handler();

    system_exec("rm /tmp/HDZERO_TX.bin");
    system_exec("rm /tmp/HDZERO_TX_RB.bin");

    sleep(2);
    beep();
    sleep(2);

    lv_obj_add_flag(bar_vtx, LV_OBJ_FLAG_HIDDEN);
}

static void flash_goggle() {
    uint8_t ret = 0;
    char cmd_buff[1024] = {0};

    lv_obj_clear_flag(bar_goggle, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(btn_goggle, "WAIT... DO NOT POWER OFF... ");
    lv_timer_handler();

    is_need_update_progress = true;
    ret = flash_hdzero("/tmp/GOGGLE",
                       fw_select_goggle.files[fw_select_goggle.which],
                       fw_select_goggle.path,
                       fw_select_goggle.files[fw_select_goggle.which],
                       "/mnt/app/script/update_goggle.sh",
                       fw_select_goggle.zipped);
    is_need_update_progress = false;

    lv_obj_add_flag(bar_goggle, LV_OBJ_FLAG_HIDDEN);
    if (ret == 1) {
        lv_obj_clear_flag(msgbox_update_complete, LV_OBJ_FLAG_HIDDEN);
        lv_timer_handler();
        app_state_push(APP_STATE_USER_INPUT_DISABLED);
        beep();
        usleep(1000000);
        beep();
        usleep(1000000);
        beep();
        reboot_flag = true;
        lv_timer_handler();
    } else if (ret == 2) {
        lv_label_set_text(btn_goggle, "#FFFF00 No firmware found.#");
    } else if (ret == 3) {
        lv_label_set_text(btn_goggle, "#FFFF00 Multiple versions been found. Keep only one.#");
    } else {
        lv_label_set_text(btn_goggle, "#FF0000 FAILED#");
    }
    lv_obj_add_flag(bar_goggle, LV_OBJ_FLAG_HIDDEN);
}

int generate_current_version(sys_version_t *sys_ver) {
    sys_ver->va = I2C_Read(ADDR_FPGA, 0xff);
    sys_ver->rx = rx_status[0].rx_ver;
    memset(sys_ver->commit, 0, sizeof(sys_ver->commit));

    FILE *fp = fopen("/mnt/app/version", "r");
    if (!fp) {
        return -1;
    }

    // %9s to read max 9 chars and leave room for null terminator, since sys_ver->commit has length 10
    fscanf(fp, "%hhd.%hhd.%hhd-%9s",
           &sys_ver->app_major,
           &sys_ver->app_minor,
           &sys_ver->app_patch,
           sys_ver->commit);
    fclose(fp);

    if (strlen(sys_ver->commit)) {
        LOGI("app: %hhu.%hhu.%hhu-%s rx: %u va: %u",
             sys_ver->app_major,
             sys_ver->app_minor,
             sys_ver->app_patch,
             sys_ver->commit,
             sys_ver->rx, sys_ver->va);

        sprintf(sys_ver->current, "app: %hhu.%hhu.%hhu-%s rx: %u va: %u",
                sys_ver->app_major,
                sys_ver->app_minor,
                sys_ver->app_patch,
                sys_ver->commit,
                sys_ver->rx, sys_ver->va);
    } else {
        LOGI("app: %hhu.%hhu.%hhu rx: %u va: %u",
             sys_ver->app_major,
             sys_ver->app_minor,
             sys_ver->app_patch,
             sys_ver->rx, sys_ver->va);

        sprintf(sys_ver->current, "app: %hhu.%hhu.%hhu rx: %u va: %u",
                sys_ver->app_major,
                sys_ver->app_minor,
                sys_ver->app_patch,
                sys_ver->rx, sys_ver->va);
    }

    return 0;
}

static const char *page_version_find_latest_fw(const char *path) {
    char *dname = NULL;
    DIR *dir = opendir(path);
    if (dir) {
        struct dirent *entry = readdir(dir);
        while (entry != NULL) {
            if (entry->d_type == DT_DIR) {
                if (dname != NULL) {
                    if (str_compare_versions(entry->d_name, dname) > 0) {
                        dname = entry->d_name;
                    }
                } else {
                    dname = entry->d_name;
                }
            }
            entry = readdir(dir);
        }
        closedir(dir);
    }
    return dname;
}

static int page_version_get_latest_fw_path(const char *device, char *path, size_t size) {
    char tmp_path[256], sdc_path[256];
    snprintf(tmp_path, sizeof(tmp_path), "/tmp/FIRMWARE/%s", device);
    snprintf(sdc_path, sizeof(sdc_path), "/mnt/extsd/FIRMWARE/%s", device);

    const char *tmp_latest = page_version_find_latest_fw(tmp_path);
    const char *sdc_latest = page_version_find_latest_fw(sdc_path);

    if (tmp_latest && sdc_latest) {
        if (str_compare_versions(tmp_latest, sdc_latest) > 0) {
            sdc_latest = NULL;
        } else {
            tmp_latest = NULL;
        }
    }

    if (tmp_latest) {
        return snprintf(path, size, "%s/%s", tmp_path, tmp_latest);
    } else if (sdc_latest) {
        return snprintf(path, size, "%s/%s", sdc_path, sdc_latest);
    } else {
        return 0;
    }
}

static void page_version_fw_select_reset(fw_select_t *fw_select) {
    if (fw_select->count) {
        for (int i = 0; i < fw_select->count; ++i) {
            free(fw_select->files[i]);
            fw_select->files[i] = NULL;
        }
        free(fw_select->files);
        fw_select->ready = false;
        fw_select->files = NULL;
        fw_select->count = 0;
        fw_select->zipped = false;
        fw_select->alt_title = NULL;
    }
}

static int page_version_get_latest_fw_files(fw_select_t *fw_select, const char *pattern, bool has_release_notes) {
    DIR *dir = opendir(fw_select->path);
    if (dir) {
        struct dirent *entry = readdir(dir);
        while (entry != NULL) {
            if (entry->d_type == DT_REG) {
                if (strstr(entry->d_name, pattern)) {
                    bool match = false;
                    for (int j = 0; j < fw_select->count; ++j) {
                        if (0 == strcmp(fw_select->files[j], entry->d_name)) {
                            match = true;
                            break;
                        }
                    }
                    if (!match) {
                        if (fw_select->count == 0) {
                            fw_select->files = malloc(sizeof(char *) * 1);
                        } else {
                            fw_select->files = realloc(fw_select->files, sizeof(char *) * (fw_select->count + 1));
                        }
                        fw_select->files[fw_select->count++] = strdup(entry->d_name);
                        if (strstr(entry->d_name, ".zip")) {
                            fw_select->zipped = true;
                        }
                    }
                } else if (strstr(entry->d_name, "release.notes")) {
                    fw_select->ready = true;
                }
            }

            entry = readdir(dir);
        }
        closedir(dir);

        if (has_release_notes) {
            if (!fw_select->ready) {
                page_version_fw_select_reset(fw_select);
            } else if (fw_select->count > 1) {
                str_qsort(&fw_select->files[0], fw_select->count);
            }
        } else if (fw_select->count > 0) {
            fw_select->ready = true;
        }
    }

    return fw_select->count;
}

static void page_version_fw_scan_for_updates() {
    bool has_online_goggle_update = false;
    bool has_online_vtx_update = false;

    page_version_fw_select_reset(&fw_select_goggle);
    snprintf(fw_select_goggle.path, sizeof(fw_select_goggle.path), "/mnt/extsd");
    page_version_get_latest_fw_files(&fw_select_goggle, "HDZERO_GOGGLE", false);

    if (fw_select_goggle.ready) {
        fw_select_goggle.alt_title = "SD Card";
    }

    page_version_fw_select_reset(&fw_select_vtx);
    snprintf(fw_select_vtx.path, sizeof(fw_select_vtx.path), "/mnt/extsd");
    page_version_get_latest_fw_files(&fw_select_vtx, "HDZERO_TX", false);

    if (fw_select_vtx.ready) {
        fw_select_vtx.alt_title = "SD Card";
    }

    if (g_setting.wifi.enable) {
        if (!fw_select_goggle.ready) {
            has_online_goggle_update =
                0 < page_version_get_latest_fw_path("GOGGLE", fw_select_goggle.path, sizeof(fw_select_goggle.path)) &&
                0 < page_version_get_latest_fw_files(&fw_select_goggle, ".bin", true);
        }

        if (!fw_select_vtx.ready) {
            has_online_vtx_update =
                0 < page_version_get_latest_fw_path("VTX", fw_select_vtx.path, sizeof(fw_select_vtx.path)) &&
                0 < page_version_get_latest_fw_files(&fw_select_vtx, ".zip", true);
        }

        if (has_online_goggle_update || has_online_vtx_update) {
            lv_label_set_text(label_note, "To view release notes, select either Update VTX or Update Goggle\n"
                                          "then press the Func button to display or hide the release notes.");
        } else if (fw_select_goggle.alt_title || fw_select_vtx.alt_title) {
            lv_label_set_text(label_note, "Remove HDZERO_TX or HDZERO_GOGGLE binary files from the root of\n"
                                          "SD Card in order to install the latest online downloaded firmware files.");
        }
    } else {
        lv_label_set_text(label_note, "");
    }
}

static bool page_version_release_notes_active() {
    return !lv_obj_has_flag(msgbox_release_notes, LV_OBJ_FLAG_HIDDEN);
}

static void page_version_release_notes_show(fw_select_t *fw_select) {
    static char buff[512];
    static char tbuff[128];
    static char mbuff[2048];

    snprintf(buff, sizeof(buff), "%s/release.notes", fw_select->path);
    FILE *notes = fopen(buff, "r");

    if (notes) {
        int max_msg_box_width = 1280;
        int max_line_length = 1;
        size_t written = 0;
        size_t nbytes = 0;
        size_t rbytes = 0;
        char *line = NULL;

        while (-1 != (rbytes = getline(&line, &nbytes, notes)) && written < sizeof(mbuff)) {
            max_line_length = rbytes > max_line_length ? rbytes : max_line_length;
            // Read until buffer only contains 256 characters are left.
            if (sizeof(mbuff) - written > 256) {
                if (memcpy(&mbuff[written], line, rbytes)) {
                    written += rbytes;
                }
            } else {
                strcpy(&mbuff[written], "\n\nVisit https://github.com/hdzero for the complete list of changes.");
                break;
            }
        }
        free(line);
        fclose(notes);

        lv_obj_t *title = lv_msgbox_get_title(msgbox_release_notes);
        lv_obj_t *message = lv_msgbox_get_text(msgbox_release_notes);
        int msg_box_width = ((max_line_length / 10)) * 100;
        if (msg_box_width > 1280) {
            msg_box_width = 1280;
        } else if (msg_box_width < 600) {
            msg_box_width = 600;
        }

        snprintf(tbuff, sizeof(tbuff), "Release Notes: %s", fs_basename(fw_select->path));
        lv_label_set_text_static(title, tbuff);
        lv_label_set_text_static(message, mbuff);
        lv_obj_set_style_text_font(message, &lv_font_montserrat_16, 0);
        lv_obj_set_width(msgbox_release_notes, msg_box_width);
        lv_obj_clear_flag(msgbox_release_notes, LV_OBJ_FLAG_HIDDEN);

        // Mark file as read and remove alert only if there are
        // no binary files detected on the root of the sd card.
        if (!fw_select->alt_title) {
            snprintf(buff, sizeof(buff), "touch %s/release.notes.checked", fw_select->path);
            if (!fs_file_exists(buff)) {
                if (0 == system_exec(buff)) {
                    lv_obj_add_flag(alert_img, LV_OBJ_FLAG_HIDDEN);
                }
            }
        }

        // Disable scrolling
        pp_version.p_arr.max = 0;
    }
}
static void page_version_release_notes_hide() {
    // Restore scrolling
    pp_version.p_arr.max = ROW_COUNT;
    lv_obj_add_flag(msgbox_release_notes, LV_OBJ_FLAG_HIDDEN);
}

static void page_version_fw_select_toggle_panel(fw_select_t *fw_select) {
    fw_select_current = fw_select;
    fw_select_current->visible = !fw_select_current->visible;

    if (fw_select_current->visible) {
        fw_select_current->page = pp_version.p_arr;
        pp_version.p_arr = fw_select_current->this;
        pp_version.on_roller = page_version_on_roller_fw_select;
        pp_version.on_click = page_version_on_click_fw_select;
        pp_version.on_right_button = NULL;
    } else {
        for (int i = 0; i < pp_version.p_arr.max; ++i) {
            lv_obj_add_flag(pp_version.p_arr.panel[i], LV_OBJ_FLAG_HIDDEN);
        }
        lv_obj_add_style(fw_select_current->dropdown, &style_dropdown, LV_PART_MAIN);

        pp_version.p_arr.cur = 0;
        fw_select_current->this = pp_version.p_arr;
        pp_version.p_arr = fw_select_current->page;
        pp_version.on_roller = page_version_on_roller;
        pp_version.on_click = page_version_on_click;
        pp_version.on_right_button = page_version_on_right_button;
    }
}

static void page_version_fw_select_populate(fw_select_t *fw_select) {
    int max_line_length = 1;
    if (fw_select->count) {
        char buffer[1024] = {0};
        int written = 0;
        for (int i = 0; i < fw_select->count; ++i) {
            int len = strlen(fw_select->files[i]);
            max_line_length = len > max_line_length ? len : max_line_length;
            written += snprintf(&buffer[written], sizeof(buffer) - written, "%s\n", fw_select->files[i]);
        }
        buffer[written - 1] = '\0';

        lv_dropdown_set_options(fw_select->dropdown, buffer);
        lv_dropdown_set_selected(fw_select->dropdown, fw_select->which);
        lv_dropdown_set_text(fw_select->dropdown, fw_select->files[fw_select->which]);
    }

    int text_width = ((max_line_length / 10)) * 175;
    int msg_box_width = text_width + 200;
    if (msg_box_width > 825) {
        msg_box_width = 825;
    } else if (msg_box_width < 600) {
        msg_box_width = 600;
    }

    lv_obj_set_size(fw_select->container, msg_box_width - 50, 200);
    lv_obj_set_width(fw_select->msgbox, msg_box_width);
    lv_obj_set_width(fw_select->dropdown, msg_box_width - 150);
}

static void page_version_fw_select_show(const char *title, fw_select_t *fw_select) {
    static char text[1024];
    if (fw_select->ready) {
        snprintf(text, sizeof(text),
                 "%s %s", title,
                 fw_select->alt_title ? fw_select->alt_title : fs_basename(fw_select->path));
    } else {
        snprintf(text, sizeof(text), "%s %s", title, "not found");
    }

    page_version_fw_select_populate(fw_select);
    lv_label_set_text(lv_msgbox_get_title(fw_select->msgbox), text);
    lv_obj_clear_flag(fw_select->msgbox, LV_OBJ_FLAG_HIDDEN);
    page_version_fw_select_toggle_panel(fw_select);
}

static void page_version_fw_select_hide(fw_select_t *fw_select) {
    lv_obj_add_flag(fw_select->msgbox, LV_OBJ_FLAG_HIDDEN);
}

static void page_version_on_roller_fw_select(uint8_t key) {
    for (int i = 0; i < pp_version.p_arr.max; ++i) {
        lv_obj_add_flag(pp_version.p_arr.panel[i], LV_OBJ_FLAG_HIDDEN);
    }
    lv_obj_remove_style(fw_select_current->dropdown, &style_dropdown, LV_PART_MAIN);

    if (fw_select_current->dropdown_focused) {
        if (key == DIAL_KEY_UP) {
            if (++fw_select_current->which >= fw_select_current->count) {
                fw_select_current->which = fw_select_current->count - 1;
            }
            uint32_t evt = LV_KEY_DOWN;
            lv_event_send(fw_select_current->dropdown, LV_EVENT_KEY, &evt);
        } else if (key == DIAL_KEY_DOWN) {
            if (--fw_select_current->which < 0) {
                fw_select_current->which = 0;
            }
            uint32_t evt = LV_KEY_UP;
            lv_event_send(fw_select_current->dropdown, LV_EVENT_KEY, &evt);
        }
    }

    switch (pp_version.p_arr.cur) {
    case 0:
        lv_obj_add_style(fw_select_current->dropdown, &style_dropdown, LV_PART_MAIN);
        break;
    default:
        lv_obj_clear_flag(pp_version.p_arr.panel[pp_version.p_arr.cur], LV_OBJ_FLAG_HIDDEN);
        break;
    }
}

static void page_version_on_click_fw_select(uint8_t key, int sel) {
    switch (sel) {
    case 0:
        if (fw_select_current->count > 1) {
            if (!fw_select_current->dropdown_focused) {
                fw_select_current->dropdown_focused = true;
                lv_dropdown_open(fw_select_current->dropdown);
                pp_version.p_arr.max = 0;
            } else {
                fw_select_current->dropdown_focused = false;
                lv_event_send(fw_select_current->dropdown, LV_EVENT_RELEASED, NULL);
                lv_dropdown_set_text(fw_select_current->dropdown, fw_select_current->files[fw_select_current->which]);
                pp_version.p_arr.max = 3;
            }
        }
        break;
    case 1:
        if (fw_select_current->count) {
            page_version_fw_select_hide(fw_select_current);
            page_version_fw_select_toggle_panel(fw_select_current);
            fw_select_current->flash();
        }
        break;
    case 2:
        page_version_fw_select_hide(fw_select_current);
        page_version_fw_select_toggle_panel(fw_select_current);
        break;
    }
}

static void page_version_fw_select_create(const char *device, fw_select_t *fw_select, void (*flash)()) {
    static lv_coord_t msgbox_col_dsc[] = {40, 160, 160, 160, 160, 160, 160, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t mbsbox_row_dsc[] = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};

    char text[256];
    snprintf(text, sizeof(text), "Update %s", device);

    fw_select->flash = flash;
    fw_select->msgbox = create_msgbox_item(device, "Target:");
    fw_select->container = lv_obj_create(fw_select->msgbox);

    lv_obj_set_pos(fw_select->container, 0, 0);
    lv_obj_set_layout(fw_select->container, LV_LAYOUT_GRID);
    lv_obj_clear_flag(fw_select->container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(fw_select->container, &style_context, LV_PART_MAIN);
    lv_obj_set_style_bg_color(fw_select->container, lv_color_make(19, 19, 19), 0);
    lv_obj_set_style_grid_column_dsc_array(fw_select->container, msgbox_col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(fw_select->container, mbsbox_row_dsc, 0);

    fw_select->this.max = 3;
    create_select_item(&fw_select->this, fw_select->container);
    for (int i = 0; i < fw_select->this.max; ++i) {
        lv_obj_set_style_bg_color(fw_select->this.panel[i], lv_color_make(0x44, 0x44, 0x44), 0);
    }
    fw_select->page = pp_version.p_arr;
    fw_select->dropdown = create_dropdown_item(fw_select->container, "", 1, 0, 600, 40, 1, 4, LV_GRID_ALIGN_START, &lv_font_montserrat_26);
    fw_select->update = create_label_item(fw_select->container, text, 1, 1, 4);
    fw_select->back = create_label_item(fw_select->container, "< Back", 1, 2, 4);

    lv_obj_set_style_grid_column_dsc_array(fw_select->msgbox, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(fw_select->msgbox, row_dsc, 0);
    lv_obj_add_style(fw_select->dropdown, &style_dropdown, LV_PART_MAIN);
    lv_obj_add_style(fw_select->update, &style_context, LV_PART_MAIN);
    lv_obj_add_style(fw_select->back, &style_context, LV_PART_MAIN);

    lv_obj_t *list = lv_dropdown_get_list(fw_select->dropdown);
    lv_obj_add_style(list, &style_dropdown, LV_PART_MAIN);
    lv_obj_set_style_text_color(list, lv_color_make(0, 0, 0), LV_PART_SELECTED | LV_STATE_CHECKED);

    page_version_fw_select_populate(fw_select);
    page_version_fw_select_hide(fw_select);
}

static lv_obj_t *page_version_create(lv_obj_t *parent, panel_arr_t *arr) {
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1053, 900);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 1053, 894);

    create_text(NULL, section, false, "Firmware:", LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, 960, 600);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

    create_select_item(arr, cont);
    cur_ver_label = create_label_item(cont, "Current Version", 1, ROW_CUR_VERSION, 2);

    btn_reset_all_settings = create_label_item(cont, "Reset all settings", 1, ROW_RESET_ALL_SETTINGS, 2);
    btn_vtx = create_label_item(cont, "Update VTX", 1, ROW_UPDATE_VTX, 2);
    btn_goggle = create_label_item(cont, "Update Goggle", 1, ROW_UPDATE_GOGGLE, 2);
    btn_esp = create_label_item(cont, "Update ESP32", 1, ROW_UPDATE_ESP32, 2);
    label_esp = create_label_item(cont, "", 3, ROW_UPDATE_ESP32, 2);
    create_label_item(cont, "< Back", 1, ROW_BACK, 1);

    bar_vtx = lv_bar_create(cont);
    lv_obj_set_size(bar_vtx, 320, 20);
    lv_obj_set_grid_cell(bar_vtx, LV_GRID_ALIGN_CENTER, 3, 3,
                         LV_GRID_ALIGN_CENTER, ROW_UPDATE_VTX, 1);
    lv_obj_add_flag(bar_vtx, LV_OBJ_FLAG_HIDDEN);

    bar_goggle = lv_bar_create(cont);
    lv_obj_set_size(bar_goggle, 320, 20);
    lv_obj_set_grid_cell(bar_goggle, LV_GRID_ALIGN_CENTER, 3, 3,
                         LV_GRID_ALIGN_CENTER, ROW_UPDATE_GOGGLE, 1);
    lv_obj_add_flag(bar_goggle, LV_OBJ_FLAG_HIDDEN);

    bar_esp = lv_bar_create(cont);
    lv_obj_set_size(bar_esp, 320, 20);
    lv_obj_set_grid_cell(bar_esp, LV_GRID_ALIGN_CENTER, 3, 3,
                         LV_GRID_ALIGN_CENTER, ROW_UPDATE_ESP32, 1);
    lv_obj_add_flag(bar_esp, LV_OBJ_FLAG_HIDDEN);

    msgbox_update_complete = create_msgbox_item("Update complete", "Goggle update completed successfully.\nPlease repower goggle now.");
    lv_obj_add_flag(msgbox_update_complete, LV_OBJ_FLAG_HIDDEN);

    msgbox_settings_reset = create_msgbox_item("Settings reset", "All settings have been reset.\nPlease repower goggle now.");
    lv_obj_add_flag(msgbox_settings_reset, LV_OBJ_FLAG_HIDDEN);

    msgbox_release_notes = create_msgbox_item("Release Notes", "Empty");
    lv_obj_add_flag(msgbox_release_notes, LV_OBJ_FLAG_HIDDEN);

    label_note = lv_label_create(cont);
    lv_label_set_text(label_note, "");
    lv_obj_set_style_text_font(label_note, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(label_note, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(label_note, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_pad_top(label_note, 12, 0);
    lv_label_set_long_mode(label_note, LV_LABEL_LONG_WRAP);
    lv_obj_set_grid_cell(label_note, LV_GRID_ALIGN_START, 1, 4, LV_GRID_ALIGN_START, 6, 2);

    page_version_fw_scan_for_updates();
    page_version_fw_select_create("Goggle", &fw_select_goggle, flash_goggle);
    page_version_fw_select_create("VTX", &fw_select_vtx, flash_vtx);

    return page;
}

static void page_version_on_created() {
    alert_img = lv_img_create(pp_version.label);
    char filename[256];
    osd_resource_path(filename, "%s", OSD_RESOURCE_720, ALERT_IMG);
    lv_img_set_src(alert_img, filename);

    lv_obj_add_flag(alert_img, LV_OBJ_FLAG_FLOATING);
    lv_obj_clear_flag(alert_img, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(alert_img, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_pos(alert_img, 125, 0);

    /**
     * If user reads release notes, then we can remove the alert.
     */
    static char checked[1024];
    snprintf(checked, sizeof(checked), "%s/release.notes.checked", fw_select_goggle.path);
    if (fs_file_exists(checked)) {
        lv_obj_add_flag(alert_img, LV_OBJ_FLAG_HIDDEN);
    } else {
        static char notes[1024];
        snprintf(notes, sizeof(notes), "%s/release.notes", fw_select_goggle.path);
        if (fs_file_exists(notes)) {
            lv_obj_clear_flag(alert_img, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

static void page_version_on_update(uint32_t delta_ms) {
    static uint32_t elapsed_ms = 0;

    if ((elapsed_ms += delta_ms) > 5000) {
        elapsed_ms = 0;
        if (autoscan_filesystem) {
            page_version_fw_scan_for_updates();
        }
    }
}

uint8_t command_monitor(char *cmd) {
    FILE *stream;
    char buf[128];
    size_t rsize = 0;
    uint8_t ret;

    stream = popen(cmd, "r");
    if (!stream)
        return 0;

    LOGI("---%s---", cmd);
    ret = 0;
    do {
        rsize = fread(buf, sizeof(char), sizeof(buf), stream);
        LOGI("%s", buf);
        if (strstr(buf, "all done")) {
            ret = 1;
            break;
        } else if (strstr(buf, "skip")) {
            ret = 2;
            break;
        } else if (strstr(buf, "repeat")) {
            ret = 3;
            break;
        }
    } while (rsize == sizeof(buf));
    pclose(stream);
    LOGI("");
    return ret;
}

static void elrs_version_timer(struct _lv_timer_t *timer) {
    char label[80];
    uint8_t version[32] = {0};
    uint16_t size = sizeof(version) - 1;

    if (!msp_read_resposne(MSP_GET_BP_VERSION, &size, version)) {
        msp_send_packet(MSP_GET_BP_VERSION, MSP_PACKET_COMMAND, 0, NULL);
        return;
    }
    lv_timer_del(timer);
    sprintf(label, "ver: %s", version);
    lv_label_set_text(label_esp, label);
}

static void reset_all_settings_reset_label_text() {
    lv_label_set_text(btn_reset_all_settings, "Reset all settings");
}

static void page_version_enter() {
    autoscan_filesystem = false;
    version_update_title();

    lv_label_set_text(label_esp, "");
    msp_send_packet(MSP_GET_BP_VERSION, MSP_PACKET_COMMAND, 0, NULL);
    lv_timer_t *timer = lv_timer_create(elrs_version_timer, 250, NULL);
    lv_timer_set_repeat_count(timer, 20);
}

static void page_version_exit() {
    lv_obj_add_flag(msgbox_release_notes, LV_OBJ_FLAG_HIDDEN);
    page_version_fw_select_hide(&fw_select_vtx);
    page_version_fw_select_hide(&fw_select_goggle);
    autoscan_filesystem = true;
}

static void page_version_on_roller(uint8_t key) {
    version_update_title();

    if (reset_all_settings_confirm == CONFIRMATION_CONFIRMED) {
        reset_all_settings_reset_label_text();
        reset_all_settings_confirm = CONFIRMATION_UNCONFIRMED;
    }
}

static void page_version_on_click(uint8_t key, int sel) {
    if (!page_version_release_notes_active()) {
        version_update_title();
        if (sel == ROW_CUR_VERSION) {
            FILE *fp;
            char buf[80];
            int dat[16];
            fp = fopen("/tmp/wr_reg", "r");
            if (fp) {
                while (fgets(buf, 80, fp)) {
                    sscanf(buf, "%x %x %x", &dat[0], &dat[1], &dat[2]);
                    DM5680_WriteReg(dat[0], dat[1], dat[2]);
                    LOGI("DM5680 REG[%02x,%02x]<-%02x", dat[0], dat[1], dat[2]);
                    usleep(100000);
                }
                fclose(fp);
                // system_exec("rm /tmp/wr_reg");
            }

            fp = fopen("/tmp/rd_reg", "r");
            if (!fp)
                return;
            while (fgets(buf, 80, fp)) {
                sscanf(buf, "%x %x", &dat[0], &dat[1]);
                DM5680_ReadReg(dat[0], dat[1]);
                sleep(1);
                LOGI("DM5680_0 REG[%02x,%02x]-> %02x", dat[0], dat[1], rx_status[0].rx_regval);
                LOGI("DM5680_1 REG[%02x,%02x]-> %02x", dat[0], dat[1], rx_status[1].rx_regval);
            }
            fclose(fp);
            // system_exec("rm /tmp/rd_reg");
        } else if (sel == ROW_RESET_ALL_SETTINGS) {
            if (reset_all_settings_confirm) {
                settings_reset();
                reset_all_settings_reset_label_text();
                lv_obj_clear_flag(msgbox_settings_reset, LV_OBJ_FLAG_HIDDEN);
                app_state_push(APP_STATE_USER_INPUT_DISABLED);
            } else {
                lv_label_set_text(btn_reset_all_settings, "#FFFF00 click to confirm/scroll to cancel#");
                reset_all_settings_confirm = CONFIRMATION_CONFIRMED;
            }
        } else if (sel == ROW_UPDATE_VTX) {
            page_version_fw_scan_for_updates();
            page_version_fw_select_show("VTX Firmware", &fw_select_vtx);
        } else if ((sel == ROW_UPDATE_GOGGLE) && !reboot_flag) {
            page_version_fw_scan_for_updates();
            page_version_fw_select_show("Goggle Firmware", &fw_select_goggle);
        } else if (sel == ROW_UPDATE_ESP32) { // flash ESP via SD
            lv_obj_clear_flag(bar_esp, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(label_esp, LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text(btn_esp, "Flashing...");
            lv_timer_handler();
            esp_loader_error_t ret = flash_elrs();
            lv_obj_add_flag(bar_esp, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(label_esp, LV_OBJ_FLAG_HIDDEN);
            if (ret == ESP_LOADER_SUCCESS)
                lv_label_set_text(btn_esp, "#00FF00 Success#");
            else
                lv_label_set_text(btn_esp, "#FF0000 FAILED#");
            page_version_enter();
        }
    }
}

void page_version_on_right_button(bool is_short) {
    if (is_short) {
        if (!page_version_release_notes_active()) {
            switch (pp_version.p_arr.cur) {
            case ROW_UPDATE_VTX:
                page_version_release_notes_show(&fw_select_vtx);
                break;
            case ROW_UPDATE_GOGGLE:
                page_version_release_notes_show(&fw_select_goggle);
                break;
            default:
                break;
            }
        } else {
            page_version_release_notes_hide();
        }
    }
}

void process_bar_update(const int value0,
                        const int value1) {
    if (bar_vtx && bar_goggle) {
        // LOGI("v0=%d, v1=%d\n", value0, value1);
        lv_bar_set_value(bar_vtx, value0, LV_ANIM_OFF);
        lv_bar_set_value(bar_goggle, value1, LV_ANIM_OFF);
    }
}

void bar_update(int sel, int value) {
    if (bar_goggle && sel)
        lv_bar_set_value(bar_goggle, value, LV_ANIM_OFF);
    else if (bar_vtx && !sel)
        lv_bar_set_value(bar_vtx, value, LV_ANIM_OFF);
    lv_timer_handler();
}

void update_current_version() {
    char strtmp[128];
    static bool bInit = true;
    if (bInit) {
        sys_version_t sys_version;
        generate_current_version(&sys_version);
        memset(strtmp, 0, sizeof(strtmp));
        strcat(strtmp, "Current Version ");
        strcat(strtmp, sys_version.current);
        lv_label_set_text(cur_ver_label, strtmp);
        bInit = false;
    }
}

void version_update_title() {
    update_current_version();
    lv_label_set_text(btn_vtx, "Update VTX");
    if (!reboot_flag)
        lv_label_set_text(btn_goggle, "Update Goggle");
    lv_label_set_text(btn_esp, "Update ESP32");
}

///////////////////////////////////////////////////////////////////////////////
// for progress info
static int get_progress_info(int *v0, int *v1) {
    FILE *stream;
    char buf[128];
    memset(buf, '\0', sizeof(buf));
    stream = popen("/mnt/app/script/get_progress_info.sh", "r");
    fread(buf, sizeof(char), sizeof(buf), stream);
    pclose(stream);

    char *pos = strchr(buf, 0xa);
    char buf_v0[10];
    char buf_v1[10];
    memset(buf_v0, '\0', sizeof(buf_v0));
    memset(buf_v1, '\0', sizeof(buf_v1));
    memcpy(buf_v0, buf, pos - buf);
    memcpy(buf_v1, pos + 1, strlen(buf) - (pos - buf - 1));

    *v0 = atoi(buf_v0);
    *v1 = atoi(buf_v1);
    return 0;
}

void *thread_version(void *ptr) {
    int count = 0;
    int sec = 0;
    int sec_last = 0;
    int v0 = 0;
    int v1 = 0;
    bool is_step1 = false;
    bool is_step2 = false;

    int percentage = 0;
    for (;;) {
        if (is_need_update_progress) {
            get_progress_info(&v0, &v1);
            if (v1 == 0) {
                percentage = 0;
            } else if (v1 == 1) {
                if (is_step1 == false)
                    percentage = 1;

                is_step1 = true;

                if (sec_last != sec)
                    percentage++;

                if (percentage > 45)
                    percentage = 45;
            } else if (v1 == 45) {
                if (is_step2 == false)
                    percentage = 45;

                is_step2 = true;
                if (sec_last != sec)
                    percentage++;

                if (percentage > 99)
                    percentage = 99;
            } else if (v1 == 100) {
                is_step1 = false;
                is_step2 = false;

                percentage = 100;
            }

            sec_last = sec;
            process_bar_update(v0, percentage);
            lv_timer_handler();
        }

        if (count >= 10) {
            count = 0;
            sec++;
        }

        /// Progess bar
        progress_bar_update();

        count++;
        usleep(100000);
    }
    return NULL;
}

page_pack_t pp_version = {
    .p_arr = {
        .cur = 0,
        .max = ROW_COUNT,
    },
    .name = "Firmware   ", // Spaces are necessary to include alert icon.
    .create = page_version_create,
    .enter = page_version_enter,
    .exit = page_version_exit,
    .on_created = page_version_on_created,
    .on_update = page_version_on_update,
    .on_roller = page_version_on_roller,
    .on_click = page_version_on_click,
    .on_right_button = page_version_on_right_button,
};
