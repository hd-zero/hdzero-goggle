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

#include "lang/language.h"

#ifdef EMULATOR_BUILD
#include "SDLaccess.h"
SDL_mutex *global_sdl_mutex;
#endif

#include "bmi270/accel_gyro.h"
#include "core/app_state.h"
#include "core/common.hh"
#include "core/elrs.h"
#include "core/ht.h"
#include "core/input_device.h"
#include "core/osd.h"
#include "core/self_test.h"
#include "core/settings.h"
#include "core/sleep_mode.h"
#include "core/thread.h"
#include "driver/TP2825.h"
#include "driver/beep.h"
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
#include "driver/rtc.h"
#include "ui/page_power.h"
#include "ui/page_scannow.h"
#include "ui/page_source.h"
#include "ui/page_storage.h"
#include "ui/ui_image_setting.h"
#include "ui/ui_main_menu.h"
#include "ui/ui_osd_element_pos.h"
#include "ui/ui_porting.h"
#include "ui/ui_statusbar.h"
#include "util/sdcard.h"

int gif_cnt = 0;

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
    if (g_setting.autoscan.source == SETTING_AUTOSCAN_SOURCE_LAST)
        source = g_setting.autoscan.last_source;
    else
        source = g_setting.autoscan.source;

    if (source == SETTING_AUTOSCAN_SOURCE_HDZERO) { // HDZero
        g_source_info.source = SOURCE_HDZERO;
        // go autoscan only if no dial up/down during initialization
        if ((g_setting.autoscan.status == SETTING_AUTOSCAN_STATUS_ON) && (g_init_done == 0)) {
            pthread_t pid;
            g_autoscan_exit = false;
            pthread_create(&pid, NULL, thread_autoscan, NULL);
        } else if (g_setting.autoscan.status == SETTING_AUTOSCAN_STATUS_LAST) {
            app_state_push(APP_STATE_VIDEO);
            app_switch_to_hdzero(true);
        } else { // auto scan disabled, go to go directly to last saved channel
            app_state_push(APP_STATE_MAINMENU);
        }
    } else {
        app_state_push(APP_STATE_VIDEO);
        if (source == SETTING_AUTOSCAN_SOURCE_EXPANSION) { // module Bay
            g_hw_stat.av_pal[1] = g_setting.source.analog_format;
            app_switch_to_analog(1);
            g_source_info.source = SOURCE_EXPANSION;
        } else if (source == SETTING_AUTOSCAN_SOURCE_AV_IN) { // AV in
            g_hw_stat.av_pal[0] = g_setting.source.analog_format;
            app_switch_to_analog(0);
            g_source_info.source = SOURCE_AV_IN;
        } else { // HDMI in
            sleep(2);
            // g_source_info.hdmi_in_status = IT66021_Sig_det();
            // if (g_source_info.hdmi_in_status) {
            app_switch_to_hdmi_in();
            g_source_info.source = SOURCE_HDMI_IN;
            //} else {
            //    g_source_info.source = SOURCE_HDZERO;
            //    app_state_push(APP_STATE_MAINMENU);
            //}
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

#ifdef EMULATOR_BUILD
    global_sdl_mutex = SDL_CreateMutex();
    if (global_sdl_mutex == NULL) {
        LOGE("Failed to create an SDL mutex!");
    }
#endif

    // 1. Recall configuration
    settings_init();
    settings_load();
    language_init();
    vclk_phase_init();
    pclk_phase_init();

    // 2. Initialize communications.
    rtc_init();
    iic_init();
    gpio_init();
    uart_init();

    // 3. Initialize core devices.
    mcp3021_init();
    input_device_init();
    hw_stat_init();
    device_init();
    esp32_init();
    elrs_init();
    ht_init();
    beep_init();

    // 4. Initilize UI
    lvgl_init();
    main_menu_init();
    statusbar_init();
    lv_timer_handler();

    // 5. Prepare Display
    OLED_Startup();
    Display_UI_init();
    OLED_Pattern(0, 0, 0);
    osd_init();
    ims_init();
    ui_osd_element_pos_init();

    // 6. Enable functionality
    if (g_setting.ht.enable) {
        ht_enable();
    } else {
        ht_disable();
    }

    // 7. Start threads
    g_bootup_sdcard_state = sdcard_mounted() ? 1 : 0;
    start_running();
    create_threads();

    // 8. Synthetic counter for gif refresh
    gif_cnt = 0;

    // 8.1 set initial analog module power state
    Analog_Module_Power(0);

    // Head alarm
    head_alarm_init();

    // 10. Execute main loop
    g_init_done = 1;
    for (;;) {
        pthread_mutex_lock(&lvgl_mutex);
        main_menu_update();
        sleep_reminder();
        statubar_update();
        osd_hdzero_update();
        ims_update();
        ui_osd_element_pos_update();
        ht_detect_motion();
        lv_timer_handler();
        source_status_timer();
        pthread_mutex_unlock(&lvgl_mutex);
        usleep(5000);
        gif_cnt++;
    }
    return 0;
}
