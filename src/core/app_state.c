#include "app_state.h"

#include <log/log.h>
#include <minIni.h>
#include <stdlib.h>
#include <unistd.h>

#include "core/dvr.h"
#include "core/input_device.h"
#include "core/msp_displayport.h"
#include "core/osd.h"
#include "driver/dm5680.h"
#include "driver/dm6302.h"
#include "driver/hardware.h"
#include "driver/it66121.h"
#include "ui/page_common.h"
#include "ui/page_imagesettings.h"
#include "ui/ui_image_setting.h"
#include "ui/ui_main_menu.h"
#include "ui/ui_porting.h"
#include "util/system.h"

app_state_t g_app_state = APP_STATE_MAINMENU;

extern int valid_channel_tb[10];
extern int user_select_index;

void app_state_push(app_state_t state) {
    g_app_state = state;
}

void app_switch_to_menu() {
    if (g_app_state == APP_STATE_IMS) {
        ims_save();
        set_slider_value();
    }

    app_state_push(APP_STATE_MAINMENU);

    // Stop recording if switching to menu mode from video mode regardless
    dvr_cmd(DVR_STOP);
    dvr_update_vi_conf(VR_1080P30);

    Display_UI();
    lvgl_switch_to_1080p();
    exit_tune_channel();
    osd_show(false);
    g_bShowIMS = false;
    main_menu_show(true);
    HDZero_Close();
    g_sdcard_det_req = 1;
    if (g_source_info.source == SOURCE_HDMI_IN) // HDMI
        IT66121_init();

    system_script(REC_STOP_LIVE);
}

void app_switch_to_analog(bool is_bay) {
    Source_AV(is_bay);

    dvr_update_vi_conf(VR_720P50);
    osd_fhd(0);
    osd_show(true);
    lvgl_switch_to_720p();
    osd_clear();
    lv_timer_handler();
    Display_Osd(g_setting.record.osd);

    g_setting.autoscan.last_source = is_bay ? SETTING_AUTOSCAN_SOURCE_EXPANSION : SETTING_AUTOSCAN_SOURCE_AV_IN;
    ini_putl("autoscan", "last_source", g_setting.autoscan.last_source, SETTING_INI);

    // usleep(300*1000);
    sleep(1);
    system_script(REC_STOP_LIVE);
}

void app_switch_to_hdmi_in() {
    Source_HDMI_in();
    IT66121_close();
    sleep(2);

    if (g_hw_stat.hdmiin_vtmg == 1)
        lvgl_switch_to_1080p();
    else
        lvgl_switch_to_720p();

    osd_show(true);
    osd_clear();
    lv_timer_handler();

    dvr_update_vi_conf((g_hw_stat.hdmiin_vtmg == 1) ? VR_1080P30 : VR_720P60);

    app_state_push(APP_STATE_VIDEO);
    g_source_info.source = SOURCE_HDMI_IN;
    dvr_enable_line_out(false);
    g_setting.autoscan.last_source = SETTING_AUTOSCAN_SOURCE_HDMI_IN;
    ini_putl("autoscan", "last_source", g_setting.autoscan.last_source, SETTING_INI);

    system_script(REC_STOP_LIVE);
}

//////////////////////////////////////////////////////////////////////
// is_default:
//    true = load from g_settings
//    false = user selected from auto scan page
void app_switch_to_hdzero(bool is_default) {
    int ch;

    if (is_default) {
        ch = g_setting.scan.channel - 1;
    } else {
        ch = valid_channel_tb[user_select_index];
        g_setting.scan.channel = ch + 1;
        ini_putl("scan", "channel", g_setting.scan.channel, SETTING_INI);
    }

    HDZero_open(g_setting.source.hdzero_bw);
    ch &= 0x7f;

    LOGI("switch to bw:%d, band:%d, ch:%d, CAM_MODE=%d 4:3=%d", g_setting.source.hdzero_bw, g_setting.source.hdzero_band, g_setting.scan.channel, CAM_MODE, cam_4_3);
    DM6302_SetChannel(g_setting.source.hdzero_band, ch);
    DM5680_clear_vldflg();
    DM5680_req_vldflg();
    progress_bar.start = 0;

    switch (CAM_MODE) {
    case VR_720P50:
    case VR_720P60:
    case VR_960x720P60:
    case VR_540P60:
        Display_720P60_50(CAM_MODE, cam_4_3);
        break;

    case VR_540P90:
    case VR_540P90_CROP:
        Display_720P90(CAM_MODE);
        break;

    case VR_1080P30:
        Display_1080P30(CAM_MODE);
        break;

    default:
        perror("switch_to_video CaM_MODE error");
    }

    channel_osd_mode = CHANNEL_SHOWTIME;

    if (CAM_MODE == VR_1080P30)
        lvgl_switch_to_1080p();
    else
        lvgl_switch_to_720p();
    osd_fhd(CAM_MODE == VR_1080P30);
    osd_clear();
    osd_show(true);
    lv_timer_handler();
    Display_Osd(g_setting.record.osd);

    g_setting.autoscan.last_source = SETTING_AUTOSCAN_SOURCE_HDZERO;
    ini_putl("autoscan", "last_source", g_setting.autoscan.last_source, SETTING_INI);

    dvr_update_vi_conf(CAM_MODE);
    system_script(REC_STOP_LIVE);
}