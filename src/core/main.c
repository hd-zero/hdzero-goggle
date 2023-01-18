#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <log/log.h>
#include <lvgl/lvgl.h>
#include <minIni.h>

#include "bmi270/accel_gyro.h"
#include "core/common.hh"
#include "core/elrs.h"
#include "core/ht.h"
#include "core/input_device.h"
#include "core/osd.h"
#include "core/self_test.h"
#include "core/thread.h"
#include "driver/TP2825.h"
#include "driver/dm5680.h"
#include "driver/esp32.h"
#include "driver/fans.h"
#include "driver/gpio.h"
#include "driver/hardware.h"
#include "driver/i2c.h"
#include "driver/it66021.h"
#include "driver/it66121.h"
#include "driver/mcp3021.h"
#include "driver/oled.h"
#include "ui/page_power.h"
#include "ui/page_scannow.h"
#include "ui/page_source.h"
#include "ui/ui_image_setting.h"
#include "ui/ui_main_menu.h"
#include "ui/ui_porting.h"
#include "ui/ui_statusbar.h"
#include "util/file.h"

static void load_ini_setting(void) {
    char str[128];

    FILE *fp;
    fp = fopen("/mnt/UDISK/setting.ini", "r");
    if (fp) {
        fclose(fp);
        sprintf(str, "cp -f /mnt/UDISK/setting.ini %s", SETTING_INI);
        system(str);
        usleep(10);
        system("rm /mnt/UDISK/setting.ini");
    }

    g_setting.scan.channel = ini_getl("scan", "channel", 1, SETTING_INI);

    // fans
    ini_gets("fans", "auto", "enable", str, sizeof(str), SETTING_INI);
    g_setting.fans.auto_mode = strcmp(str, "enable") == 0;
    g_setting.fans.top_speed = ini_getl("fans", "top_speed", 4, SETTING_INI);
    g_setting.fans.left_speed = ini_getl("fans", "left_speed", 5, SETTING_INI);
    g_setting.fans.right_speed = ini_getl("fans", "right_speed", 5, SETTING_INI);

    // autoscan
    ini_gets("autoscan", "status", "scan", str, sizeof(str), SETTING_INI);
    if (strcmp(str, "enable") == 0 || strcmp(str, "scan") == 0) {
        g_setting.autoscan.status = 0;
    } else if (strcmp(str, "disable") == 0 || strcmp(str, "last") == 0) {
        g_setting.autoscan.status = 1;
    } else {
        g_setting.autoscan.status = 2;
    }
    g_setting.autoscan.source = ini_getl("autoscan", "source", SETTING_SOURCE_LAST, SETTING_INI);
    g_setting.autoscan.last_source = ini_getl("autoscan", "last_source", SETTING_SOURCE_HDZERO, SETTING_INI);

    // embedded osd mode 4x3 default
    g_setting.osd.embedded_mode = ini_getl("osd", "embedded_mode", EMBEDDED_4x3, SETTING_INI);

    // power
    g_setting.power.voltage = ini_getl("power", "voltage", 35, SETTING_INI);
    g_setting.power.warning_type = ini_getl("power", "warning_type", 0, SETTING_INI);
    g_setting.power.cell_count_mode = ini_getl("power", "cell_count_mode", 0, SETTING_INI);
    g_setting.power.cell_count = ini_getl("power", "cell_count", 2, SETTING_INI);
    g_setting.power.osd_display_mode = ini_getl("power", "osd_display_mode", 0, SETTING_INI);
    ini_gets("record", "mode_manual", "disable", str, sizeof(str), SETTING_INI);
    g_setting.record.mode_manual = strcmp(str, "enable") == 0;
    ini_gets("record", "format_ts", "enable", str, sizeof(str), SETTING_INI);
    g_setting.record.format_ts = strcmp(str, "enable") == 0;
    ini_gets("record", "osd", "enable", str, sizeof(str), SETTING_INI);
    g_setting.record.osd = strcmp(str, "enable") == 0;
    ini_gets("record", "audio", "enable", str, sizeof(str), SETTING_INI);
    g_setting.record.audio = strcmp(str, "enable") == 0;

    g_setting.record.audio_source = ini_getl("record", "audio_source", 0, SETTING_INI);

    // image
    g_setting.image.oled = ini_getl("image", "oled", 7, SETTING_INI);
    g_setting.image.brightness = ini_getl("image", "brightness", 0, SETTING_INI);
    g_setting.image.saturation = ini_getl("image", "saturation", 0, SETTING_INI);
    g_setting.image.contrast = ini_getl("image", "contrast", 0, SETTING_INI);
    g_setting.image.auto_off = ini_getl("image", "auto_off", 2, SETTING_INI);

    // head tracker
    g_setting.ht.enable = ini_getl("ht", "enable", 0, SETTING_INI);
    g_setting.ht.max_angle = ini_getl("ht", "max_angle", 120, SETTING_INI);
    g_setting.ht.acc_x = ini_getl("ht", "acc_x", 0, SETTING_INI);
    g_setting.ht.acc_y = ini_getl("ht", "acc_y", 0, SETTING_INI);
    g_setting.ht.acc_z = ini_getl("ht", "acc_z", 0, SETTING_INI);
    g_setting.ht.gyr_x = ini_getl("ht", "gyr_x", 0, SETTING_INI);
    g_setting.ht.gyr_y = ini_getl("ht", "gyr_y", 0, SETTING_INI);
    g_setting.ht.gyr_z = ini_getl("ht", "gyr_z", 0, SETTING_INI);

    g_setting.elrs.enable = ini_getl("elrs", "enable", 0, SETTING_INI);

    // Check
    g_test_en = false;
    if (file_exists(LOG_FILE) && log_enable_file(LOG_FILE)) {
        g_test_en = true;
    }
}

static void *thread_autoscan(void *ptr) {
    for (;;) {
        pthread_mutex_lock(&lvgl_mutex);
        main_menu_show(true);
        g_menu_op = OPLEVEL_SUBMENU;
        submenu_enter();
        pthread_mutex_unlock(&lvgl_mutex);

        if (g_autoscan_exit)
            goto a_exit;

        sleep(5);

        if (g_autoscan_exit)
            goto a_exit;
    }

a_exit:
    pthread_exit(NULL);
    return NULL;
}

void start_running(void) {
    int source;
    if (g_setting.autoscan.source == SETTING_SOURCE_LAST)
        source = g_setting.autoscan.last_source;
    else
        source = g_setting.autoscan.source;

    if (source == SETTING_SOURCE_HDZERO) { // HDZero
        g_source_info.source = SOURCE_HDZERO;
        HDZero_open();
        if (g_setting.autoscan.status == SETTING_AUTOSCAN_SCAN) {
            pthread_t pid;
            g_autoscan_exit = false;
            pthread_create(&pid, NULL, thread_autoscan, NULL);
        } else if (g_setting.autoscan.status == SETTING_AUTOSCAN_LAST) {
            g_menu_op = OPLEVEL_VIDEO;
            switch_to_video(true);
        } else { // auto scan disabled, go to go directly to last saved channel
            g_menu_op = OPLEVEL_MAINMENU;
        }
    } else {
        g_menu_op = OPLEVEL_VIDEO;
        if (source == SETTING_SOURCE_EXPANSION) { // module Bay
            switch_to_analog(1);
            g_source_info.source = SOURCE_EXPANSION;
        } else if (source == SETTING_SOURCE_AV_IN) { // AV in
            switch_to_analog(0);
            g_source_info.source = SOURCE_AV_IN;
        } else { // HDMI in
            sleep(2);
            g_source_info.hdmi_in_status = IT66021_Sig_det();
            if (g_source_info.hdmi_in_status) {
                switch_to_hdmiin();
                g_source_info.source = SOURCE_HDMI_IN;
            } else {
                g_source_info.source = SOURCE_HDZERO;
                g_menu_op = OPLEVEL_MAINMENU;
            }
        }
    }

    if (g_setting.elrs.enable)
        enable_esp32();
}

static void device_init(void) {
    self_test();
    enable_bmi270();
    IT66021_init();
    IT66121_init();
    TP2825_Config(0, 0);
    DM5680_req_ver();
    fans_top_setspeed(g_setting.fans.top_speed);
}

void lvgl_init() {
    lv_init();
    style_init();
    lvgl_init_porting();
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_color_make(0xff, 0xff, 0xff), lv_palette_main(LV_PALETTE_RED),
                                              false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_make(64, 64, 64), 0);
}

int main(int argc, char *argv[]) {
    pthread_mutex_init(&lvgl_mutex, NULL);

    gpio_init();
    lvgl_init();
    load_ini_setting();
    main_menu_init();
    statusbar_init();
    lv_timer_handler();
    input_device_init();

    iic_init();
    OLED_Startup();
    Display_UI_init();
    OLED_Pattern(0, 0, 0);

    uart_init();
    hw_stat_init();
    device_init();

    osd_init();
    ims_init();

    esp32_init();
    elrs_init();
    ht_init();

    start_running(); // start to run from saved settings
    create_threads();

    if (g_setting.ht.enable)
        ht_enable();
    else
        ht_disable();

    g_init_done = 1;
    for (;;) {
        pthread_mutex_lock(&lvgl_mutex);
        statubar_update();
        osd_hdzero_update();
        ims_update();
        ht_detect_motion();
        lv_timer_handler();
        source_status_timer();
        pthread_mutex_unlock(&lvgl_mutex);
        usleep(5000);
    }
    return 0;
}
