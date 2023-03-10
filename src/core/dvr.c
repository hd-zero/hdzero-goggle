#include "dvr.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <log/log.h>
#include <minIni.h>

#include "core/msp_displayport.h"
#include "core/settings.h"
#include "driver/hardware.h"
#include "ui/page_common.h"

bool dvr_is_recording = false;

static pthread_mutex_t dvr_mutex;

///////////////////////////////////////////////////////////////////
//-1=error;
// 0=idle,1=recording,2=stopped,3=No SD card,4=recorf file path error,
// 5=SD card Full,6=Encoder error
void dvr_update_status() {
    pthread_mutex_lock(&dvr_mutex);
    if (dvr_is_recording) {
        int ret = -1;
        FILE *fp = fopen("/tmp/record.dat", "r");
        if (fp) {
            fscanf(fp, "%d", &ret);
            fclose(fp);
        }
        if (ret != 1) {
            dvr_is_recording = false;
        }
    }
    pthread_mutex_unlock(&dvr_mutex);
}

void dvr_enable_line_out(bool enable) {
    char buf[128];
    if (enable) {
        sprintf(buf, "%s out_on", AUDIO_SEL_SH);
        system(buf);
        sprintf(buf, "%s out_linein_on", AUDIO_SEL_SH);
        system(buf);
        sprintf(buf, "%s out_dac_off", AUDIO_SEL_SH);
        system(buf);
    } else {
        sprintf(buf, "%s out_off", AUDIO_SEL_SH);
        system(buf);
    }
}

void dvr_select_audio_source(uint8_t source) {
    char buf[128];
    char *audio_source[3] = {
        "in_mic1",
        "in_mic2",
        "in_linein"};

    if (source > 2)
        source = 2;
    sprintf(buf, "%s %s", AUDIO_SEL_SH, audio_source[source]);
    system(buf);
}

static void dvr_update_record_conf() {
    if (g_setting.record.format_ts)
        ini_puts("record", "type", "ts", REC_CONF);
    else
        ini_puts("record", "type", "mp4", REC_CONF);

    if (g_source_info.source == SOURCE_HDZERO) {
        if (CAM_MODE == VR_540P90 || CAM_MODE == VR_540P90_CROP) { // 90fps
            ini_putl("venc", "fps", 90, REC_CONF);
            ini_putl("venc", "kbps", 34000, REC_CONF);
            ini_putl("venc", "h265", 0, REC_CONF);
            ini_putl("vi", "fps", 90, REC_CONF);
        } else {
            ini_putl("venc", "fps", 60, REC_CONF);
            ini_putl("venc", "kbps", 24000, REC_CONF);
            ini_putl("venc", "h265", 1, REC_CONF);
            ini_putl("vi", "fps", 60, REC_CONF);
        }
    } else { // AV -- HDMI no record
        ini_putl("venc", "kbps", 24000, REC_CONF);
        ini_putl("venc", "h265", 1, REC_CONF);
        ini_putl("vi", "fps", 60, REC_CONF);
        if (g_hw_stat.av_pal)
            ini_putl("venc", "fps", 50, REC_CONF);
        else
            ini_putl("venc", "fps", 60, REC_CONF);
    }

    ini_putl("record", "audio", g_setting.record.audio, REC_CONF);
    dvr_select_audio_source(g_setting.record.audio_source);
}

void dvr_cmd(osd_dvr_cmd_t cmd) {
    LOGI("dvr_cmd: sdcard=%d, recording=%d, cmd=%d", g_sdcard_enable, dvr_is_recording, cmd);

    if (!g_sdcard_enable)
        return;

    pthread_mutex_lock(&dvr_mutex);

    bool start_rec = dvr_is_recording;

    switch (cmd) {
    case DVR_TOGGLE:
        start_rec = !dvr_is_recording;
        break;
    case DVR_STOP:
        start_rec = false;
        break;
    case DVR_START:
        start_rec = true;
        break;
    }

    if (g_source_info.source == SOURCE_HDMI_IN) {
        // no record for hdmi-in :<
        start_rec = false;
    }

    if (start_rec) {
        if (!dvr_is_recording && g_sdcard_size >= 103) {
            dvr_update_record_conf();
            dvr_is_recording = true;
            system(REC_START);
            sleep(2); // wait for record process
        }
    } else {
        if (dvr_is_recording) {
            dvr_is_recording = false;
            system(REC_STOP);
            sleep(2); // wait for record process
        }
    }

    pthread_mutex_unlock(&dvr_mutex);
}