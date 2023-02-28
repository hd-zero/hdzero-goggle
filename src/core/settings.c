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
    // ### scan ###
    {
        // channel
        1},

    // ### fans ###
    {
        // top_speed, auto_mode, left_speed, right_speed
        4, true, 5, 5},

    // ### autoscan ###
    {
        // status, last_source, source
        SETTING_AUTOSCAN_SCAN, SETTING_SOURCE_LAST, SETTING_SOURCE_HDZERO},

    // ### power ###
    {
        // voltage, display_voltage, warning_type, cell_count_mode, cell_count, osd_display_mode
        35, true, 2, SETTING_POWER_CELL_COUNT_MODE_AUTO, 2, SETTING_POWER_OSD_DISPLAY_MODE_TOTAL},

    // ### record ###
    {
        // mode_manual, format_ts, osd, audio, audio_source
        false, true, true, true, 0},

    // ### image ###
    {
        // oled, brightness, saturation, contrast, auto_off
        7, 0, 0, 0, 2},

    // ### head tracker ###
    {
        // enable, max_angle, acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z
        false, 120, 0, 0, 0, 0, 0, 0},

    // ### ELRS ###
    {
        // enable
        false},

    // ### OSD ###
    {
        // embedded_mode
        EMBEDDED_4x3,
        //      elements
        // show       position
        //       mode_4_3   mode_16_9
        //        x    y    x   y
        {
            {true, {{160, 0}, {0, 0}}},     // topfan_speed
            {true, {{200, 0}, {40, 0}}},    // latency_lock
            {true, {{240, 0}, {80, 0}}},    // vtx_temp
            {true, {{280, 0}, {120, 0}}},   // vrx_temp
            {true, {{320, 0}, {160, 0}}},   // battery_low
            {true, {{540, 0}, {540, 0}}},   // channel
            {true, {{840, 0}, {1000, 0}}},  // sd_rec
            {true, {{880, 0}, {1040, 0}}},  // vlq
            {true, {{960, 0}, {1120, 0}}},  // ant0
            {true, {{920, 0}, {1080, 0}}},  // ant1
            {true, {{1040, 0}, {1200, 0}}}, // ant2
            {true, {{1000, 0}, {1160, 0}}}, // ant3
            {true, {{170, 50}, {170, 50}}}, // goggle_temp_top
            {true, {{270, 50}, {270, 50}}}, // goggle_temp_left
            {true, {{370, 50}, {370, 50}}},
        } // goggle_temp_right
    },
};

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