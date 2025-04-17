#include "settings.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <log/log.h>
#include <minIni.h>

#include "core/self_test.h"
#include "lang/language.h"
#include "ui/page_common.h"
#include "util/filesystem.h"
#include "util/system.h"

#define SETTINGS_INI_VERSION_UNKNOWN 0

setting_t g_setting;

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
        .voltage = 3500,
        .display_voltage = true,
        .warning_type = SETTING_POWER_WARNING_TYPE_BOTH,
        .cell_count_mode = SETTING_POWER_CELL_COUNT_MODE_AUTO,
        .cell_count = 2,
        .osd_display_mode = SETTING_POWER_OSD_DISPLAY_MODE_TOTAL,
        .power_ana = false,
        .calibration_offset = 0,
    },
    .record = {
        .mode_manual = false,
        .format_ts = true,
        .bitrate_scale = SETTING_RECORD_BITRATE_SCALE_NORMAL,
        .osd = true,
        .audio = true,
        .audio_source = SETTING_RECORD_AUDIO_SOURCE_MIC,
        .naming = SETTING_NAMING_CONTIGUOUS,
    },
    .image = {
        .oled = 8,
        .brightness = 39,
        .saturation = 28,
        .contrast = 25,
        .auto_off = 1,
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
        .alarm_state = SETTING_HT_ALARM_STATE_OFF,
        .alarm_angle = 1300,
        .alarm_delay = 5,
        .alarm_pattern = SETTING_HT_ALARM_PATTERN_2SHORT,
        .alarm_on_arm = false,
        .alarm_on_video = false,
    },
    .elrs = {
        .enable = false,
    },
    .ease = {
        .no_dial = 0,
    },
    .osd = {
        .orbit = 2,
        .embedded_mode = EMBEDDED_4x3,
        .startup_visibility = SETTING_OSD_SHOW_AT_STARTUP_SHOW,
        .is_visible = true,
        .element = {
            // OSD_GOGGLE_TOPFAN_SPEED
            {
                .show = true,
                .position = {.mode_4_3 = {.x = 160, .y = 0}, .mode_16_9 = {.x = 0, .y = 0}},
            },
            // OSD_GOGGLE_LATENCY_LOCK
            {
                .show = true,
                .position = {.mode_4_3 = {.x = 200, .y = 0}, .mode_16_9 = {.x = 40, .y = 0}},
            },
            // OSD_GOGGLE_VTX_TEMP
            {
                .show = true,
                .position = {.mode_4_3 = {.x = 240, .y = 0}, .mode_16_9 = {.x = 80, .y = 0}},
            },
            // OSD_GOGGLE_VRX_TEMP
            {
                .show = true,
                .position = {.mode_4_3 = {.x = 280, .y = 0}, .mode_16_9 = {.x = 120, .y = 0}},
            },
            // OSD_GOGGLE_BATTERY_LOW
            {
                .show = true,
                .position = {.mode_4_3 = {.x = 320, .y = 0}, .mode_16_9 = {.x = 160, .y = 0}},
            },
            // OSD_GOGGLE_BATTERY_VOLTAGE
            {
                .show = true,
                .position = {.mode_4_3 = {.x = 360, .y = 0}, .mode_16_9 = {.x = 200, .y = 0}},
            },
            // OSD_GOGGLE_CLOCK_DATE
            {
                .show = false,
                .position = {.mode_4_3 = {.x = 360, .y = 24}, .mode_16_9 = {.x = 200, .y = 24}},
            },
            // OSD_GOGGLE_CLOCK_TIME
            {
                .show = false,
                .position = {.mode_4_3 = {.x = 580, .y = 24}, .mode_16_9 = {.x = 420, .y = 24}},
            },
            // OSD_GOGGLE_CHANNEL
            {
                .show = true,
                .position = {.mode_4_3 = {.x = 580, .y = 0}, .mode_16_9 = {.x = 580, .y = 0}},
            },
            // OSD_GOGGLE_SD_REC
            {
                .show = true,
                .position = {.mode_4_3 = {.x = 840, .y = 0}, .mode_16_9 = {.x = 1000, .y = 0}},
            },
            // OSD_GOGGLE_VLQ
            {
                .show = true,
                .position = {.mode_4_3 = {.x = 880, .y = 0}, .mode_16_9 = {.x = 1040, .y = 0}},
            },
            // OSD_GOGGLE_ANT0
            {
                .show = true,
                .position = {.mode_4_3 = {.x = 960, .y = 0}, .mode_16_9 = {.x = 1120, .y = 0}},
            },
            // OSD_GOGGLE_ANT1
            {
                .show = true,
                .position = {.mode_4_3 = {.x = 920, .y = 0}, .mode_16_9 = {.x = 1080, .y = 0}},
            },
            // OSD_GOGGLE_ANT2
            {
                .show = true,
                .position = {.mode_4_3 = {.x = 1040, .y = 0}, .mode_16_9 = {.x = 1200, .y = 0}},
            },
            // OSD_GOGGLE_ANT3
            {
                .show = true,
                .position = {.mode_4_3 = {.x = 1000, .y = 0}, .mode_16_9 = {.x = 1160, .y = 0}},
            },
            // OSD_GOGGLE_TEMP_TOP
            {
                .show = true,
                .position = {.mode_4_3 = {.x = 170, .y = 50}, .mode_16_9 = {.x = 170, .y = 50}},
            },
            // OSD_GOGGLE_TEMP_LEFT
            {
                .show = true,
                .position = {.mode_4_3 = {.x = 270, .y = 50}, .mode_16_9 = {.x = 270, .y = 50}},
            },
            // OSD_GOGGLE_TEMP_RIGHT
            {
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
    },
    // Refer to `page_input.c`'s arrays `rollerFunctionPointers` and `btnFunctionPointers`
    .inputs = {
        .roller = 0,
        .left_click = 0,
        .left_press = 1,
        .right_click = 2,
        .right_press = 6,
        .right_double_click = 3,
    },
    .wifi = {
        .enable = false,
        .mode = 0,
        .clientid = {""},
        .ssid = {"HDZero", "MySSID"},
        .passwd = {"divimath", "MyPassword"},
        .dhcp = true,
        .ip_addr = "192.168.2.122",
        .netmask = "255.255.255.0",
        .gateway = "192.168.2.1",
        .dns = "192.168.2.1",
        .rf_channel = 6,
        .root_pw = "divimath",
        .ssh = false,
    },
    .storage = {
        .logging = false,
        .selftest = false,
    },
    .source = {
        .analog_format = SETTING_SOURCES_ANALOG_FORMAT_NTSC,
        .analog_ratio = SETTING_SOURCES_ANALOG_RATIO_4_3,
        .hdzero_band = SETTING_SOURCES_HDZERO_BAND_RACEBAND,
        .hdzero_bw = SETTING_SOURCES_HDZERO_BW_WIDE,
    },
    .language = {
        .lang = LANG_ENGLISH_DEFAULT,
    },
};

int settings_put_osd_element_shown(bool show, char *config_name) {
    char setting_key[128];

    snprintf(setting_key, sizeof(setting_key), "element_%s_show", config_name);
    return settings_put_bool("osd", setting_key, show);
}

int settings_put_osd_element_pos_x(const setting_osd_goggle_element_positions_t *pos, char *config_name) {
    char setting_key[128];
    int ret = 0;

    snprintf(setting_key, sizeof(setting_key), "element_%s_pos_4_3_x", config_name);
    ret = ini_putl("osd", setting_key, pos->mode_4_3.x, SETTING_INI);
    snprintf(setting_key, sizeof(setting_key), "element_%s_pos_16_9_x", config_name);
    ret &= ini_putl("osd", setting_key, pos->mode_16_9.x, SETTING_INI);
    return ret;
}

int settings_put_osd_element_pos_y(const setting_osd_goggle_element_positions_t *pos, char *config_name) {
    char setting_key[128];
    int ret = 0;

    snprintf(setting_key, sizeof(setting_key), "element_%s_pos_4_3_y", config_name);
    ret = ini_putl("osd", setting_key, pos->mode_4_3.y, SETTING_INI);
    snprintf(setting_key, sizeof(setting_key), "element_%s_pos_16_9_y", config_name);
    ret &= ini_putl("osd", setting_key, pos->mode_16_9.y, SETTING_INI);
    return ret;
}

int settings_put_osd_element(const setting_osd_goggle_element_t *element, char *config_name) {
    int ret = 0;

    ret = settings_put_osd_element_shown(element->show, config_name);
    ret &= settings_put_osd_element_pos_x(&element->position, config_name);
    ret &= settings_put_osd_element_pos_y(&element->position, config_name);
    return ret;
}

static void settings_load_osd_element(setting_osd_goggle_element_t *element, char *config_name, const setting_osd_goggle_element_t *defaults) {
    char buf[128];

    snprintf(buf, sizeof(buf), "element_%s_show", config_name);
    element->show = settings_get_bool("osd", buf, defaults->show);

    snprintf(buf, sizeof(buf), "element_%s_pos_4_3_x", config_name);
    element->position.mode_4_3.x = ini_getl("osd", buf, defaults->position.mode_4_3.x, SETTING_INI);

    snprintf(buf, sizeof(buf), "element_%s_pos_4_3_y", config_name);
    element->position.mode_4_3.y = ini_getl("osd", buf, defaults->position.mode_4_3.y, SETTING_INI);

    snprintf(buf, sizeof(buf), "element_%s_pos_16_9_x", config_name);
    element->position.mode_16_9.x = ini_getl("osd", buf, defaults->position.mode_16_9.x, SETTING_INI);

    snprintf(buf, sizeof(buf), "element_%s_pos_16_9_y", config_name);
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

void settings_reset(void) {
    char buf[256];

    snprintf(buf, sizeof(buf), "rm -f %s", SETTING_INI);
    system_exec(buf);
    usleep(50);

    snprintf(buf, sizeof(buf), "touch %s", SETTING_INI);
    system_exec(buf);
    usleep(50);

    ini_putl("settings", "file_version", SETTING_INI_VERSION, SETTING_INI);
}

void settings_init(void) {
    // check if backup of old settings file exists after goggle update
    if (fs_file_exists("/mnt/UDISK/setting.ini")) {
        char buf[256];
        snprintf(buf, sizeof(buf), "cp -f /mnt/UDISK/setting.ini %s", SETTING_INI);
        system_exec(buf);
        usleep(10);
        system_exec("rm /mnt/UDISK/setting.ini");
    }

    int file_version = ini_getl("settings", "file_version", SETTINGS_INI_VERSION_UNKNOWN, SETTING_INI);
    if (file_version != SETTING_INI_VERSION)
        settings_reset();
}

void settings_load(void) {
    // scan
    g_setting.scan.channel = ini_getl("scan", "channel", g_setting_defaults.scan.channel, SETTING_INI);

    // fans
    g_setting.fans.auto_mode = settings_get_bool("fans", "auto", g_setting_defaults.fans.auto_mode);
    g_setting.fans.top_speed = ini_getl("fans", "top_speed", g_setting_defaults.fans.top_speed, SETTING_INI);
    g_setting.fans.left_speed = ini_getl("fans", "left_speed", g_setting_defaults.fans.left_speed, SETTING_INI);
    g_setting.fans.right_speed = ini_getl("fans", "right_speed", g_setting_defaults.fans.right_speed, SETTING_INI);

    // source
    g_setting.source.analog_format = ini_getl("source", "analog_format", g_setting_defaults.source.analog_format, SETTING_INI);
    g_setting.source.analog_ratio = ini_getl("source", "analog_ratio", g_setting_defaults.source.analog_ratio, SETTING_INI);
    g_setting.source.hdzero_band = ini_getl("source", "hdzero_band", g_setting_defaults.source.hdzero_band, SETTING_INI);
    g_setting.source.hdzero_bw = ini_getl("source", "hdzero_bw", g_setting_defaults.source.hdzero_bw, SETTING_INI);

    // autoscan
    g_setting.autoscan.status = ini_getl("autoscan", "status", g_setting_defaults.autoscan.status, SETTING_INI);
    g_setting.autoscan.source = ini_getl("autoscan", "source", g_setting_defaults.autoscan.source, SETTING_INI);
    g_setting.autoscan.last_source = ini_getl("autoscan", "last_source", g_setting_defaults.autoscan.last_source, SETTING_INI);

    // osd
    g_setting.osd.orbit = ini_getl("osd", "orbit", g_setting_defaults.osd.orbit, SETTING_INI);
    g_setting.osd.embedded_mode = ini_getl("osd", "embedded_mode", g_setting_defaults.osd.embedded_mode, SETTING_INI);
    g_setting.osd.startup_visibility = ini_getl("osd", "startup_visibility", g_setting_defaults.osd.startup_visibility, SETTING_INI);

    switch (g_setting.osd.startup_visibility) {
    default:
    case SETTING_OSD_SHOW_AT_STARTUP_SHOW:
        g_setting.osd.is_visible = true;
        settings_put_bool("osd", "is_visible", g_setting.osd.is_visible);
        break;
    case SETTING_OSD_SHOW_AT_STARTUP_HIDE:
        g_setting.osd.is_visible = false;
        settings_put_bool("osd", "is_visible", g_setting.osd.is_visible);
        break;
    case SETTING_OSD_SHOW_AT_STARTUP_LAST:
        g_setting.osd.is_visible = settings_get_bool("osd", "is_visible", g_setting_defaults.osd.is_visible);
        break;
    }

    settings_load_osd_element(&g_setting.osd.element[OSD_GOGGLE_TOPFAN_SPEED], "topfan_speed", &g_setting_defaults.osd.element[OSD_GOGGLE_TOPFAN_SPEED]);
    settings_load_osd_element(&g_setting.osd.element[OSD_GOGGLE_LATENCY_LOCK], "latency_lock", &g_setting_defaults.osd.element[OSD_GOGGLE_LATENCY_LOCK]);
    settings_load_osd_element(&g_setting.osd.element[OSD_GOGGLE_VTX_TEMP], "vtx_temp", &g_setting_defaults.osd.element[OSD_GOGGLE_VTX_TEMP]);
    settings_load_osd_element(&g_setting.osd.element[OSD_GOGGLE_VRX_TEMP], "vrx_temp", &g_setting_defaults.osd.element[OSD_GOGGLE_VRX_TEMP]);
    settings_load_osd_element(&g_setting.osd.element[OSD_GOGGLE_BATTERY_LOW], "battery_low", &g_setting_defaults.osd.element[OSD_GOGGLE_BATTERY_LOW]);
    settings_load_osd_element(&g_setting.osd.element[OSD_GOGGLE_BATTERY_VOLTAGE], "battery_voltage", &g_setting_defaults.osd.element[OSD_GOGGLE_BATTERY_VOLTAGE]);
    settings_load_osd_element(&g_setting.osd.element[OSD_GOGGLE_CLOCK_DATE], "clock_date", &g_setting_defaults.osd.element[OSD_GOGGLE_CLOCK_DATE]);
    settings_load_osd_element(&g_setting.osd.element[OSD_GOGGLE_CLOCK_TIME], "clock_time", &g_setting_defaults.osd.element[OSD_GOGGLE_CLOCK_TIME]);
    settings_load_osd_element(&g_setting.osd.element[OSD_GOGGLE_CHANNEL], "channel", &g_setting_defaults.osd.element[OSD_GOGGLE_CHANNEL]);
    settings_load_osd_element(&g_setting.osd.element[OSD_GOGGLE_SD_REC], "sd_rec", &g_setting_defaults.osd.element[OSD_GOGGLE_SD_REC]);
    settings_load_osd_element(&g_setting.osd.element[OSD_GOGGLE_VLQ], "vlq", &g_setting_defaults.osd.element[OSD_GOGGLE_VLQ]);
    settings_load_osd_element(&g_setting.osd.element[OSD_GOGGLE_ANT0], "ant0", &g_setting_defaults.osd.element[OSD_GOGGLE_ANT0]);
    settings_load_osd_element(&g_setting.osd.element[OSD_GOGGLE_ANT1], "ant1", &g_setting_defaults.osd.element[OSD_GOGGLE_ANT1]);
    settings_load_osd_element(&g_setting.osd.element[OSD_GOGGLE_ANT2], "ant2", &g_setting_defaults.osd.element[OSD_GOGGLE_ANT2]);
    settings_load_osd_element(&g_setting.osd.element[OSD_GOGGLE_ANT3], "ant3", &g_setting_defaults.osd.element[OSD_GOGGLE_ANT3]);
    settings_load_osd_element(&g_setting.osd.element[OSD_GOGGLE_TEMP_TOP], "goggle_temp_top", &g_setting_defaults.osd.element[OSD_GOGGLE_TEMP_TOP]);
    settings_load_osd_element(&g_setting.osd.element[OSD_GOGGLE_TEMP_LEFT], "goggle_temp_left", &g_setting_defaults.osd.element[OSD_GOGGLE_TEMP_LEFT]);
    settings_load_osd_element(&g_setting.osd.element[OSD_GOGGLE_TEMP_RIGHT], "goggle_temp_right", &g_setting_defaults.osd.element[OSD_GOGGLE_TEMP_RIGHT]);

    // power
    g_setting.power.voltage = ini_getl("power", "voltage_mv", g_setting_defaults.power.voltage, SETTING_INI);
    g_setting.power.warning_type = ini_getl("power", "warning_type", g_setting_defaults.power.warning_type, SETTING_INI);
    g_setting.power.cell_count_mode = ini_getl("power", "cell_count_mode", g_setting_defaults.power.cell_count_mode, SETTING_INI);
    g_setting.power.cell_count = ini_getl("power", "cell_count", g_setting_defaults.power.cell_count, SETTING_INI);
    g_setting.power.osd_display_mode = ini_getl("power", "osd_display_mode", g_setting_defaults.power.osd_display_mode, SETTING_INI);
    g_setting.power.power_ana = ini_getl("power", "power_ana_rx", g_setting_defaults.power.power_ana, SETTING_INI);
    g_setting.power.calibration_offset = ini_getl("power", "calibration_offset_mv", g_setting_defaults.power.calibration_offset, SETTING_INI);

    // record
    g_setting.record.mode_manual = settings_get_bool("record", "mode_manual", g_setting_defaults.record.mode_manual);
    g_setting.record.format_ts = settings_get_bool("record", "format_ts", g_setting_defaults.record.format_ts);
    g_setting.record.bitrate_scale = ini_getl("record", "bitrate_scale", g_setting_defaults.record.bitrate_scale, SETTING_INI);
    g_setting.record.osd = settings_get_bool("record", "osd", g_setting_defaults.record.osd);
    g_setting.record.audio = settings_get_bool("record", "audio", g_setting_defaults.record.audio);
    g_setting.record.audio_source = ini_getl("record", "audio_source", g_setting_defaults.record.audio_source, SETTING_INI);
    g_setting.record.naming = ini_getl("record", "naming", g_setting_defaults.record.naming, SETTING_INI);

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
    g_setting.ht.alarm_state = ini_getl("ht", "alarm_state", g_setting_defaults.ht.alarm_state, SETTING_INI);
    g_setting.ht.alarm_angle = ini_getl("ht", "alarm_angle", g_setting_defaults.ht.alarm_angle, SETTING_INI);

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

    // inputs
    g_setting.inputs.roller = ini_getl("inputs", "roller", g_setting_defaults.inputs.roller, SETTING_INI);
    g_setting.inputs.left_click = ini_getl("inputs", "left_click", g_setting_defaults.inputs.left_click, SETTING_INI);
    g_setting.inputs.left_press = ini_getl("inputs", "left_press", g_setting_defaults.inputs.left_press, SETTING_INI);
    g_setting.inputs.right_click = ini_getl("inputs", "right_click", g_setting_defaults.inputs.right_click, SETTING_INI);
    g_setting.inputs.right_press = ini_getl("inputs", "right_press", g_setting_defaults.inputs.right_press, SETTING_INI);
    g_setting.inputs.right_double_click = ini_getl("inputs", "right_double_click", g_setting_defaults.inputs.right_double_click, SETTING_INI);

    // wifi
    g_setting.wifi.enable = settings_get_bool("wifi", "enable", g_setting_defaults.wifi.enable);
    g_setting.wifi.mode = ini_getl("wifi", "mode", g_setting_defaults.wifi.mode, SETTING_INI);
    ini_gets("wifi", "clientid", g_setting_defaults.wifi.clientid, g_setting.wifi.clientid, WIFI_CLIENTID_MAX, SETTING_INI);
    ini_gets("wifi", "ap_ssid", g_setting_defaults.wifi.ssid[0], g_setting.wifi.ssid[0], WIFI_SSID_MAX, SETTING_INI);
    ini_gets("wifi", "ap_passwd", g_setting_defaults.wifi.passwd[0], g_setting.wifi.passwd[0], WIFI_PASSWD_MAX, SETTING_INI);
    ini_gets("wifi", "sta_ssid", g_setting_defaults.wifi.ssid[1], g_setting.wifi.ssid[1], WIFI_SSID_MAX, SETTING_INI);
    ini_gets("wifi", "sta_passwd", g_setting_defaults.wifi.passwd[1], g_setting.wifi.passwd[1], WIFI_PASSWD_MAX, SETTING_INI);
    g_setting.wifi.dhcp = settings_get_bool("wifi", "dhcp", g_setting_defaults.wifi.dhcp);
    ini_gets("wifi", "ip_addr", g_setting_defaults.wifi.ip_addr, g_setting.wifi.ip_addr, WIFI_NETWORK_MAX, SETTING_INI);
    ini_gets("wifi", "netmask", g_setting_defaults.wifi.netmask, g_setting.wifi.netmask, WIFI_NETWORK_MAX, SETTING_INI);
    ini_gets("wifi", "gateway", g_setting_defaults.wifi.gateway, g_setting.wifi.gateway, WIFI_NETWORK_MAX, SETTING_INI);
    ini_gets("wifi", "dns", g_setting_defaults.wifi.dns, g_setting.wifi.dns, WIFI_NETWORK_MAX, SETTING_INI);
    g_setting.wifi.rf_channel = ini_getl("wifi", "rf_channel", g_setting_defaults.wifi.rf_channel, SETTING_INI);
    ini_gets("wifi", "root_pw", g_setting_defaults.wifi.root_pw, g_setting.wifi.root_pw, WIFI_PASSWD_MAX, SETTING_INI);
    g_setting.wifi.ssh = settings_get_bool("wifi", "ssh", g_setting_defaults.wifi.ssh);

    //  no dial under video mode
    g_setting.ease.no_dial = fs_file_exists(NO_DIAL_FILE);

    // storage
    g_setting.storage.logging = settings_get_bool("storage", "logging", g_setting_defaults.storage.logging);

    // language
    if (!language_config()) {
        g_setting.language.lang = ini_getl("language", "lang", g_setting_defaults.language.lang, SETTING_INI);
    }

    // Check
    if (fs_file_exists(SELF_TEST_FILE)) {
        unlink(SELF_TEST_FILE);
        if (log_file_open(SELF_TEST_FILE)) {
            g_setting.storage.logging = true;
            g_setting.storage.selftest = true;
        }
    } else if (g_setting.storage.logging) {
        unlink(APP_LOG_FILE);
        g_setting.storage.logging = log_file_open(APP_LOG_FILE);
    }
}
