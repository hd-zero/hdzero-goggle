#include "settings.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <log/log.h>
#include <minIni.h>

#include "core/self_test.h"
#include "ui/page_common.h"
#include "util/file.h"

setting_t g_setting;
bool g_test_en = false;

const setting_t g_setting_defaults = {
    .scan = {
        .channel = 1,
    },
    .fans = {
        .top_speed = 4,
        .auto_mode = true,
        .left_speed = 5,
        .right_speed = 5,
    },
    .autoscan = {
        .status = SETTING_AUTOSCAN_SCAN,
        .last_source = SETTING_SOURCE_LAST,
        .source = SETTING_SOURCE_HDZERO,
    },
    .power = {
        .voltage = 35,
        .display_voltage = true,
        .warning_type = 2,
        .cell_count_mode = SETTING_POWER_CELL_COUNT_MODE_AUTO,
        .cell_count = 2,
        .osd_display_mode = SETTING_POWER_OSD_DISPLAY_MODE_TOTAL,
    },
    .record = {
        .mode_manual = false,
        .format_ts = true,
        .osd = true,
        .audio = true,
        .audio_source = 0,
    },
    .image = {
        .oled = 7,
        .brightness = 0,
        .saturation = 0,
        .contrast = 0,
        .auto_off = 2,
    },
    .ht = {
        .enable = false,
        .max_angle = 120,
        .acc_x = 0,
        .acc_y = 0,
        .acc_z = 0,
        .gyr_x = 0,
        .gyr_y = 0,
        .gyr_z = 0,
    },
    .elrs = {
        .enable = false,
    },
    .osd = {
        .embedded_mode = EMBEDDED_4x3,
        .elements = {
            .topfan_speed = {
                .show = true,
                .position = {.mode_4_3 = {.x = 160, .y = 0}, .mode_16_9 = {.x = 0, .y = 0}},
            },
            .latency_lock = {
                .show = true,
                .position = {.mode_4_3 = {.x = 200, .y = 0}, .mode_16_9 = {.x = 40, .y = 0}},
            },
            .vtx_temp = {
                .show = true,
                .position = {.mode_4_3 = {.x = 240, .y = 0}, .mode_16_9 = {.x = 80, .y = 0}},
            },
            .vrx_temp = {
                .show = true,
                .position = {.mode_4_3 = {.x = 280, .y = 0}, .mode_16_9 = {.x = 120, .y = 0}},
            },
            .battery_low = {
                .show = true,
                .position = {.mode_4_3 = {.x = 320, .y = 0}, .mode_16_9 = {.x = 160, .y = 0}},
            },
            .channel = {
                .show = true,
                .position = {.mode_4_3 = {.x = 540, .y = 0}, .mode_16_9 = {.x = 540, .y = 0}},
            },
            .sd_rec = {
                .show = true,
                .position = {.mode_4_3 = {.x = 840, .y = 0}, .mode_16_9 = {.x = 1000, .y = 0}},
            },
            .vlq = {
                .show = true,
                .position = {.mode_4_3 = {.x = 880, .y = 0}, .mode_16_9 = {.x = 1040, .y = 0}},
            },
            .ant0 = {
                .show = true,
                .position = {.mode_4_3 = {.x = 960, .y = 0}, .mode_16_9 = {.x = 1120, .y = 0}},
            },
            .ant1 = {
                .show = true,
                .position = {.mode_4_3 = {.x = 920, .y = 0}, .mode_16_9 = {.x = 1080, .y = 0}},
            },
            .ant2 = {
                .show = true,
                .position = {.mode_4_3 = {.x = 1040, .y = 0}, .mode_16_9 = {.x = 1200, .y = 0}},
            },
            .ant3 = {
                .show = true,
                .position = {.mode_4_3 = {.x = 1000, .y = 0}, .mode_16_9 = {.x = 1160, .y = 0}},
            },
            .goggle_temp_top = {
                .show = true,
                .position = {.mode_4_3 = {.x = 170, .y = 50}, .mode_16_9 = {.x = 170, .y = 50}},
            },
            .goggle_temp_left = {
                .show = true,
                .position = {.mode_4_3 = {.x = 270, .y = 50}, .mode_16_9 = {.x = 270, .y = 50}},
            },
            .goggle_temp_right = {
                .show = true,
                .position = {.mode_4_3 = {.x = 370, .y = 50}, .mode_16_9 = {.x = 370, .y = 50}},
            },
        },
    }};

static void settings_load_osd_element(setting_osd_goggle_element_t *element, char *config_name, const setting_osd_goggle_element_t *defaults) {
    char buf[128];

    sprintf(buf, "element_%s_show", config_name);
    element->show = ini_getl("osd", buf, defaults->show, SETTING_INI);

    sprintf(buf, "element_%s_pos_4_3_x", config_name);
    element->position.mode_4_3.x = ini_getl("osd", buf, defaults->position.mode_4_3.x, SETTING_INI);

    sprintf(buf, "element_%s_pos_4_3_y", config_name);
    element->position.mode_4_3.y = ini_getl("osd", buf, defaults->position.mode_4_3.y, SETTING_INI);

    sprintf(buf, "element_%s_pos_16_9_x", config_name);
    element->position.mode_16_9.x = ini_getl("osd", buf, defaults->position.mode_16_9.x, SETTING_INI);

    sprintf(buf, "element_%s_pos_16_9_y", config_name);
    element->position.mode_16_9.y = ini_getl("osd", buf, defaults->position.mode_16_9.y, SETTING_INI);
}

void settings_load(void) {
    char str[128];

    if (file_exists("/mnt/UDISK/setting.ini")) {
        sprintf(str, "cp -f /mnt/UDISK/setting.ini %s", SETTING_INI);
        system(str);
        usleep(10);
        system("rm /mnt/UDISK/setting.ini");
    }

    g_setting.scan.channel = ini_getl("scan", "channel", g_setting_defaults.scan.channel, SETTING_INI);

    // fans
    ini_gets("fans", "auto", "enable", str, sizeof(str), SETTING_INI);
    g_setting.fans.auto_mode = strcmp(str, "enable") == 0;
    g_setting.fans.top_speed = ini_getl("fans", "top_speed", g_setting_defaults.fans.top_speed, SETTING_INI);
    g_setting.fans.left_speed = ini_getl("fans", "left_speed", g_setting_defaults.fans.left_speed, SETTING_INI);
    g_setting.fans.right_speed = ini_getl("fans", "right_speed", g_setting_defaults.fans.right_speed, SETTING_INI);

    // autoscan
    ini_gets("autoscan", "status", "scan", str, sizeof(str), SETTING_INI);
    if (strcmp(str, "enable") == 0 || strcmp(str, "scan") == 0) {
        g_setting.autoscan.status = 0;
    } else if (strcmp(str, "disable") == 0 || strcmp(str, "last") == 0) {
        g_setting.autoscan.status = 1;
    } else {
        g_setting.autoscan.status = 2;
    }
    g_setting.autoscan.source = ini_getl("autoscan", "source", g_setting_defaults.autoscan.source, SETTING_INI);
    g_setting.autoscan.last_source = ini_getl("autoscan", "last_source", g_setting_defaults.autoscan.last_source, SETTING_INI);

    // osd
    g_setting.osd.embedded_mode = ini_getl("osd", "embedded_mode", g_setting_defaults.osd.embedded_mode, SETTING_INI);
    settings_load_osd_element(&g_setting.osd.elements.topfan_speed, "topfan_speed", &g_setting_defaults.osd.elements.topfan_speed);
    settings_load_osd_element(&g_setting.osd.elements.latency_lock, "latency_lock", &g_setting_defaults.osd.elements.latency_lock);
    settings_load_osd_element(&g_setting.osd.elements.vtx_temp, "vtx_temp", &g_setting_defaults.osd.elements.vtx_temp);
    settings_load_osd_element(&g_setting.osd.elements.vrx_temp, "vrx_temp", &g_setting_defaults.osd.elements.vrx_temp);
    settings_load_osd_element(&g_setting.osd.elements.battery_low, "battery_low", &g_setting_defaults.osd.elements.battery_low);
    settings_load_osd_element(&g_setting.osd.elements.channel, "channel", &g_setting_defaults.osd.elements.channel);
    settings_load_osd_element(&g_setting.osd.elements.sd_rec, "sd_rec", &g_setting_defaults.osd.elements.sd_rec);
    settings_load_osd_element(&g_setting.osd.elements.vlq, "vlq", &g_setting_defaults.osd.elements.vlq);
    settings_load_osd_element(&g_setting.osd.elements.ant0, "ant0", &g_setting_defaults.osd.elements.ant0);
    settings_load_osd_element(&g_setting.osd.elements.ant1, "ant1", &g_setting_defaults.osd.elements.ant1);
    settings_load_osd_element(&g_setting.osd.elements.ant2, "ant2", &g_setting_defaults.osd.elements.ant2);
    settings_load_osd_element(&g_setting.osd.elements.ant3, "ant3", &g_setting_defaults.osd.elements.ant3);
    settings_load_osd_element(&g_setting.osd.elements.goggle_temp_top, "goggle_temp_top", &g_setting_defaults.osd.elements.goggle_temp_top);
    settings_load_osd_element(&g_setting.osd.elements.goggle_temp_left, "goggle_temp_left", &g_setting_defaults.osd.elements.goggle_temp_left);
    settings_load_osd_element(&g_setting.osd.elements.goggle_temp_right, "goggle_temp_right", &g_setting_defaults.osd.elements.goggle_temp_right);

    // power
    g_setting.power.voltage = ini_getl("power", "voltage", g_setting_defaults.power.voltage, SETTING_INI);
    g_setting.power.warning_type = ini_getl("power", "warning_type", g_setting_defaults.power.warning_type, SETTING_INI);
    g_setting.power.cell_count_mode = ini_getl("power", "cell_count_mode", g_setting_defaults.power.cell_count_mode, SETTING_INI);
    g_setting.power.cell_count = ini_getl("power", "cell_count", g_setting_defaults.power.cell_count, SETTING_INI);
    g_setting.power.osd_display_mode = ini_getl("power", "osd_display_mode", g_setting_defaults.power.osd_display_mode, SETTING_INI);

    // record
    ini_gets("record", "mode_manual", "disable", str, sizeof(str), SETTING_INI);
    g_setting.record.mode_manual = strcmp(str, "enable") == 0;
    ini_gets("record", "format_ts", "enable", str, sizeof(str), SETTING_INI);
    g_setting.record.format_ts = strcmp(str, "enable") == 0;
    ini_gets("record", "osd", "enable", str, sizeof(str), SETTING_INI);
    g_setting.record.osd = strcmp(str, "enable") == 0;
    ini_gets("record", "audio", "enable", str, sizeof(str), SETTING_INI);
    g_setting.record.audio = strcmp(str, "enable") == 0;

    g_setting.record.audio_source = ini_getl("record", "audio_source", g_setting_defaults.record.audio_source, SETTING_INI);

    // image
    g_setting.image.oled = ini_getl("image", "oled", g_setting_defaults.image.oled, SETTING_INI);
    g_setting.image.brightness = ini_getl("image", "brightness", g_setting_defaults.image.brightness, SETTING_INI);
    g_setting.image.saturation = ini_getl("image", "saturation", g_setting_defaults.image.saturation, SETTING_INI);
    g_setting.image.contrast = ini_getl("image", "contrast", g_setting_defaults.image.contrast, SETTING_INI);
    g_setting.image.auto_off = ini_getl("image", "auto_off", g_setting_defaults.image.auto_off, SETTING_INI);

    // head tracker
    g_setting.ht.enable = ini_getl("ht", "enable", g_setting_defaults.ht.enable, SETTING_INI);
    g_setting.ht.max_angle = ini_getl("ht", "max_angle", g_setting_defaults.ht.max_angle, SETTING_INI);
    g_setting.ht.acc_x = ini_getl("ht", "acc_x", g_setting_defaults.ht.acc_x, SETTING_INI);
    g_setting.ht.acc_y = ini_getl("ht", "acc_y", g_setting_defaults.ht.acc_y, SETTING_INI);
    g_setting.ht.acc_z = ini_getl("ht", "acc_z", g_setting_defaults.ht.acc_z, SETTING_INI);
    g_setting.ht.gyr_x = ini_getl("ht", "gyr_x", g_setting_defaults.ht.gyr_x, SETTING_INI);
    g_setting.ht.gyr_y = ini_getl("ht", "gyr_y", g_setting_defaults.ht.gyr_y, SETTING_INI);
    g_setting.ht.gyr_z = ini_getl("ht", "gyr_z", g_setting_defaults.ht.gyr_z, SETTING_INI);

    g_setting.elrs.enable = ini_getl("elrs", "enable", g_setting_defaults.elrs.enable, SETTING_INI);

    // Check
    g_test_en = false;
    if (file_exists(LOG_FILE) && log_enable_file(LOG_FILE)) {
        g_test_en = true;
    }
}