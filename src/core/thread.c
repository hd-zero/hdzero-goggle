#include "thread.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <unistd.h>

#include <log/log.h>

#include "core/app_state.h"
#include "core/battery.h"
#include "core/common.hh"
#include "core/defines.h"
#include "core/dvr.h"
#include "core/input_device.h"
#include "core/msp_displayport.h"
#include "core/osd.h"
#include "core/settings.h"
#include "driver/dm5680.h"
#include "driver/hardware.h"
#include "driver/it66021.h"
#include "driver/nct75.h"
#include "driver/oled.h"
#include "ui/page_fans.h"
#include "ui/page_storage.h"
#include "ui/page_version.h"
#include "ui/ui_porting.h"
#include "util/sdcard.h"

///////////////////////////////////////////////////////////////////////////////
// SD card exist
static void detect_sdcard(void) {
    static bool sdcard_init_scan = true;
    static bool sdcard_enable_last = false;

    if (!page_storage_is_sd_repair_active()) {
        g_sdcard_enable = sdcard_mounted();

        if ((g_sdcard_enable && !sdcard_enable_last) || g_sdcard_det_req) {
            struct statfs info;
            if (statfs("/mnt/extsd", &info) == 0)
                g_sdcard_size = (info.f_bsize * info.f_bavail) >> 20; // in MB
            else
                g_sdcard_size = 0;
            g_sdcard_det_req = 0;
        }

        // Only repair card at bootup or when inserted
        if (g_init_done) {
            if (sdcard_init_scan && g_sdcard_enable) {
                page_storage_init_auto_sd_repair();
                sdcard_init_scan = false;
            } else if (!g_sdcard_enable && sdcard_enable_last) {
                sdcard_init_scan = true;
            }
        }

        sdcard_enable_last = g_sdcard_enable;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Signal loss|accquire processing
#define SIGNAL_LOSS_DURATION_THR 20 // 25=4 seconds,
#define SIGNAL_ACCQ_DURATION_THR 10
static void check_hdzero_signal(int vtmg_change) {
    static uint8_t cnt = 0;
    uint8_t is_valid;

    // HDZero digital
    if (g_source_info.source == SOURCE_HDZERO) {
        DM5680_req_rssi();
        DM5680_req_vldflg();
        tune_channel_timer();
    }

    // exit if no SD card or not in video mode
    if (g_setting.record.mode_manual || (!g_sdcard_enable) || (g_app_state != APP_STATE_VIDEO))
        return;

    // exit if HDMI in
    if (g_source_info.source == SOURCE_HDMI_IN)
        return;

    // Analog VTMG change -> Restart recording
    if (g_source_info.source >= SOURCE_AV_IN) {
        if (vtmg_change && dvr_is_recording) {
            LOGI("AV VTMG change");
            dvr_cmd(DVR_STOP);
            dvr_cmd(DVR_START);
        }
    }

    // Auto DVT HDZero VTMG change -> stop recording first
    if ((g_source_info.source == SOURCE_HDZERO) && vtmg_change) {
        LOGI("HDZero VTMG change (A)\n");
        dvr_cmd(DVR_STOP);
        system(REC_STOP_LIVE);
        cnt = 0;
    }

    if (g_source_info.source == SOURCE_AV_IN)
        is_valid = g_source_info.av_in_status;
    else if (g_source_info.source == SOURCE_EXPANSION)
        is_valid = g_source_info.av_bay_status;
    else
        is_valid = (rx_status[0].rx_valid || rx_status[1].rx_valid);

    if (dvr_is_recording) { // in-recording
        if (!is_valid) {
            cnt++;
            if (cnt >= SIGNAL_LOSS_DURATION_THR) {
                cnt = 0;
                LOGI("Signal lost");
                dvr_cmd(DVR_STOP);
            }
        } else
            cnt = 0;
    } else { // not in-recording
        if (is_valid) {
            cnt++;
            if (cnt >= SIGNAL_ACCQ_DURATION_THR) {
                cnt = 0;
                LOGI("Signal accquired");
                dvr_cmd(DVR_START);
            }
        } else
            cnt = 0;
    }
}

static void *thread_peripheral(void *ptr) {
    int record_vtmg_change = 0;
    int j = 0, k = 0;

    for (;;) {
        if (j > 50) {
            j = 0;

            fans_auto_ctrl();
            detect_sdcard();
            if (k++ == 4) {
                k = 0;
                battery_update();
                g_temperature.top = nct_read_temperature(NCT_TOP);
                g_temperature.left = nct_read_temperature(NCT_LEFT) + 100;
                g_temperature.right = nct_read_temperature(NCT_RIGHT);
                dvr_update_status();
            }
            // detect HDZERO
            record_vtmg_change = HDZERO_detect();

            // detect AV_in/Moudle_bay
            record_vtmg_change |= AV_in_detect();
            g_source_info.av_in_status = g_hw_stat.av_valid[0];
            g_source_info.av_bay_status = g_hw_stat.av_valid[1];

            // detect HDMI in
            HDMI_in_detect();
            g_source_info.hdmi_in_status = g_hw_stat.hdmiin_valid;

            g_latency_locked = (bool)Get_VideoLatancy_status();
            check_hdzero_signal(record_vtmg_change);
            record_vtmg_change = 0;
        }
        j++;
        usleep(2000);
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////
// local
static threads_obj_t threads_obj;

static void threads_instance(threads_obj_t *obj) {
    obj->instance[0] = thread_peripheral;
    obj->instance[1] = thread_version;
    obj->instance[2] = thread_osd;
}

int create_threads() {
    int ret = 0;

    threads_obj_t *obj = &threads_obj;
    threads_instance(obj);
    for (int i = 0; i < THREAD_COUNT; i++) {
        ret = pthread_create(&obj->pid[i],
                             NULL,
                             obj->instance[i],
                             NULL);

        if (ret != 0)
            goto thread_create_err;
    }
    return 0;

thread_create_err:
    return -1;
}
