#include "settings.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <log/log.h>
#include <minIni.h>

#include "core/self_test.h"
#include "ui/page_common.h"
#include "ui/page_connections.h"
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
        .status = SETTING_AUTOSCAN_STATUS_ON,
        .last_source = SETTING_AUTOSCAN_SOURCE_LAST,
        .source = SETTING_AUTOSCAN_SOURCE_HDZERO,
    },
    .power = {
        .voltage = 35,
        .warning_type = SETTING_POWER_WARNING_TYPE_BOTH,
        .cell_count_mode = SETTING_POWER_CELL_COUNT_MODE_AUTO,
        .cell_count = 2,
        .osd_display_mode = SETTING_POWER_OSD_DISPLAY_MODE_TOTAL,
        .power_ana = false,
    },
    .source = {
        .analog_format = SETTING_SOURCES_ANALOG_FORMAT_PAL
    },
    .record = {
        .mode_manual = false,
        .format_ts = true,
        .osd = true,
        .audio = true,
        .audio_source = SETTING_RECORD_AUDIO_SOURCE_MIC,
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
    .wifi = {
        .enable = false,
    },
    .ease = {
        .no_dial = 0,
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
            .osd_tempe[0] = {
                .show = true,
                .position = {.mode_4_3 = {.x = 170, .y = 50}, .mode_16_9 = {.x = 170, .y = 50}},
            },
            .osd_tempe[1] = {
                .show = true,
                .position = {.mode_4_3 = {.x = 270, .y = 50}, .mode_16_9 = {.x = 270, .y = 50}},
            },
            .osd_tempe[2] = {
                .show = true,
                .position = {.mode_4_3 = {.x = 370, .y = 50}, .mode_16_9 = {.x = 370, .y = 50}},
            },
        },
    },
    .clock = {
        .year = 2023,
        .month = 3,
        .day = 28,
        .hour = 12,
        .min = 30,
        .sec = 30,
        .format = 0,
    }};

static void settings_load_osd_element(setting_osd_goggle_element_t *element, char *config_name, const setting_osd_goggle_element_t *defaults) {
    char buf[128];

    sprintf(buf, "element_%s_show", config_name);
    element->show = settings_get_bool("osd", buf, defaults->show);

    sprintf(buf, "element_%s_pos_4_3_x", config_name);
    element->position.mode_4_3.x = ini_getl("osd", buf, defaults->position.mode_4_3.x, SETTING_INI);

    sprintf(buf, "element_%s_pos_4_3_y", config_name);
    element->position.mode_4_3.y = ini_getl("osd", buf, defaults->position.mode_4_3.y, SETTING_INI);

    sprintf(buf, "element_%s_pos_16_9_x", config_name);
    element->position.mode_16_9.x = ini_getl("osd", buf, defaults->position.mode_16_9.x, SETTING_INI);

    sprintf(buf, "element_%s_pos_16_9_y", config_name);
    element->position.mode_16_9.y = ini_getl("osd", buf, defaults->position.mode_16_9.y, SETTING_INI);
}

bool settings_get_bool(char *section, char *key, bool default_val) {
    char buf[128];

    ini_gets(section, key, default_val ? "true" : "false", buf, sizeof(buf), SETTING_INI);
    return strcmp(buf, "true") == 0;
}

int settings_put_bool(char *section, char *key, bool value) {
    return ini_puts(section, key, value ? "true" : "false", SETTING_INI);
}

void settings_load(void) {
    if (file_exists("/mnt/UDISK/setting.ini")) {
        char str[128];
        sprintf(str, "cp -f /mnt/UDISK/setting.ini %s", SETTING_INI);
        system(str);
        usleep(10);
        system("rm /mnt/UDISK/setting.ini");
    }

    // scan
    g_setting.scan.channel = ini_getl("scan", "channel", g_setting_defaults.scan.channel, SETTING_INI);

    // fans
    g_setting.fans.auto_mode = settings_get_bool("fans", "auto", g_setting_defaults.fans.auto_mode);
    g_setting.fans.top_speed = ini_getl("fans", "top_speed", g_setting_defaults.fans.top_speed, SETTING_INI);
    g_setting.fans.left_speed = ini_getl("fans", "left_speed", g_setting_defaults.fans.left_speed, SETTING_INI);
    g_setting.fans.right_speed = ini_getl("fans", "right_speed", g_setting_defaults.fans.right_speed, SETTING_INI);

    // source
    g_setting.source.analog_format = ini_getl("source", "analog_format", g_setting_defaults.source.analog_format, SETTING_INI);

    // autoscan
    g_setting.autoscan.status = ini_getl("autoscan", "status", g_setting_defaults.autoscan.status, SETTING_INI);
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
    settings_load_osd_element(&g_setting.osd.elements.osd_tempe[0], "goggle_temp_top", &g_setting_defaults.osd.elements.osd_tempe[0]);
    settings_load_osd_element(&g_setting.osd.elements.osd_tempe[1], "goggle_temp_left", &g_setting_defaults.osd.elements.osd_tempe[1]);
    settings_load_osd_element(&g_setting.osd.elements.osd_tempe[2], "goggle_temp_right", &g_setting_defaults.osd.elements.osd_tempe[2]);

    // power
    g_setting.power.voltage = ini_getl("power", "voltage", g_setting_defaults.power.voltage, SETTING_INI);
    g_setting.power.warning_type = ini_getl("power", "warning_type", g_setting_defaults.power.warning_type, SETTING_INI);
    g_setting.power.cell_count_mode = ini_getl("power", "cell_count_mode", g_setting_defaults.power.cell_count_mode, SETTING_INI);
    g_setting.power.cell_count = ini_getl("power", "cell_count", g_setting_defaults.power.cell_count, SETTING_INI);
    g_setting.power.osd_display_mode = ini_getl("power", "osd_display_mode", g_setting_defaults.power.osd_display_mode, SETTING_INI);
    g_setting.power.power_ana = ini_getl("power", "power_ana_rx", g_setting_defaults.power.power_ana, SETTING_INI);

    // record
    g_setting.record.mode_manual = settings_get_bool("record", "mode_manual", g_setting_defaults.record.mode_manual);
    g_setting.record.format_ts = settings_get_bool("record", "format_ts", g_setting_defaults.record.format_ts);
    g_setting.record.osd = settings_get_bool("record", "osd", g_setting_defaults.record.osd);
    g_setting.record.audio = settings_get_bool("record", "audio", g_setting_defaults.record.audio);
    g_setting.record.audio_source = ini_getl("record", "audio_source", g_setting_defaults.record.audio_source, SETTING_INI);

    // image
    g_setting.image.oled = ini_getl("image", "oled", g_setting_defaults.image.oled, SETTING_INI);
    g_setting.image.brightness = ini_getl("image", "brightness", g_setting_defaults.image.brightness, SETTING_INI);
    g_setting.image.saturation = ini_getl("image", "saturation", g_setting_defaults.image.saturation, SETTING_INI);
    g_setting.image.contrast = ini_getl("image", "contrast", g_setting_defaults.image.contrast, SETTING_INI);
    g_setting.image.auto_off = ini_getl("image", "auto_off", g_setting_defaults.image.auto_off, SETTING_INI);

    // head tracker
    g_setting.ht.enable = settings_get_bool("ht", "enable", g_setting_defaults.ht.enable);
    g_setting.ht.max_angle = ini_getl("ht", "max_angle", g_setting_defaults.ht.max_angle, SETTING_INI);
    g_setting.ht.acc_x = ini_getl("ht", "acc_x", g_setting_defaults.ht.acc_x, SETTING_INI);
    g_setting.ht.acc_y = ini_getl("ht", "acc_y", g_setting_defaults.ht.acc_y, SETTING_INI);
    g_setting.ht.acc_z = ini_getl("ht", "acc_z", g_setting_defaults.ht.acc_z, SETTING_INI);
    g_setting.ht.gyr_x = ini_getl("ht", "gyr_x", g_setting_defaults.ht.gyr_x, SETTING_INI);
    g_setting.ht.gyr_y = ini_getl("ht", "gyr_y", g_setting_defaults.ht.gyr_y, SETTING_INI);
    g_setting.ht.gyr_z = ini_getl("ht", "gyr_z", g_setting_defaults.ht.gyr_z, SETTING_INI);

    // elrs
    g_setting.elrs.enable = settings_get_bool("elrs", "enable", g_setting_defaults.elrs.enable);

    // clock
    g_setting.clock.year = ini_getl("clock", "year", g_setting_defaults.clock.year, SETTING_INI);
    g_setting.clock.month = ini_getl("clock", "month", g_setting_defaults.clock.month, SETTING_INI);
    g_setting.clock.day = ini_getl("clock", "day", g_setting_defaults.clock.day, SETTING_INI);
    g_setting.clock.hour = ini_getl("clock", "hour", g_setting_defaults.clock.hour, SETTING_INI);
    g_setting.clock.min = ini_getl("clock", "min", g_setting_defaults.clock.min, SETTING_INI);
    g_setting.clock.sec = ini_getl("clock", "sec", g_setting_defaults.clock.sec, SETTING_INI);
    g_setting.clock.format = ini_getl("clock", "format", g_setting_defaults.clock.format, SETTING_INI);

    // disable wifi on boot
    g_setting.wifi.enable = false;
    settings_put_bool("wifi", "enable", g_setting.wifi.enable);
    if (file_exists(WIFI_SSID_FILE)) {
        ini_gets("wifi", "ssid", "HDZero", g_setting.wifi.ssid, 16, WIFI_SSID_FILE);
        ini_gets("wifi", "passwd", "divimath", g_setting.wifi.passwd, 16, WIFI_SSID_FILE);
        ini_puts("wifi", "ssid", g_setting.wifi.ssid, SETTING_INI);
        ini_puts("wifi", "passwd", g_setting.wifi.passwd, SETTING_INI); // passwd length is 8+
    } else {
        ini_gets("wifi", "ssid", "HDZero", g_setting.wifi.ssid, 16, SETTING_INI);
        ini_gets("wifi", "passwd", "divimath", g_setting.wifi.passwd, 16, SETTING_INI);
    }
    update_hostpad_conf();

    // no dial under video mode
    g_setting.ease.no_dial = file_exists(NO_DIAL_FILE);

    // Check
    g_test_en = false;
    if (file_exists(LOG_FILE) && log_enable_file(LOG_FILE)) {
        g_test_en = true;
    }
}
