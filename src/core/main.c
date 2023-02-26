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
#include "core/app_state.h"
#include "core/common.hh"
#include "core/elrs.h"
#include "core/ht.h"
#include "core/input_device.h"
#include "core/osd.h"
#include "core/self_test.h"
#include "core/settings.h"
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

static void *thread_autoscan(void *ptr) {
    for (;;) {
        pthread_mutex_lock(&lvgl_mutex);
        main_menu_show(true);
        app_state_push(APP_STATE_SUBMENU);
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
            app_state_push(APP_STATE_VIDEO);
            app_switch_to_hdzero(true);
        } else { // auto scan disabled, go to go directly to last saved channel
            app_state_push(APP_STATE_MAINMENU);
        }
    } else {
        app_state_push(APP_STATE_VIDEO);
        if (source == SETTING_SOURCE_EXPANSION) { // module Bay
            app_switch_to_analog(1);
            g_source_info.source = SOURCE_EXPANSION;
        } else if (source == SETTING_SOURCE_AV_IN) { // AV in
            app_switch_to_analog(0);
            g_source_info.source = SOURCE_AV_IN;
        } else { // HDMI in
            sleep(2);
            g_source_info.hdmi_in_status = IT66021_Sig_det();
            if (g_source_info.hdmi_in_status) {
                app_switch_to_hdmi_in();
                g_source_info.source = SOURCE_HDMI_IN;
            } else {
                g_source_info.source = SOURCE_HDZERO;
                app_state_push(APP_STATE_MAINMENU);
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
    settings_load();
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
