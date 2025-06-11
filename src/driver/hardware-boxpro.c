#include "hardware.h"

#if HDZBOXPRO

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <log/log.h>

#include "../core/app_state.h"
#include "../core/common.hh"
#include "../core/osd.h"
#include "../core/settings.h"
#include "../ui/page_common.h"
#include "beep.h"
#include "defines.h"
#include "dm5680.h"
#include "dm6302.h"
#include "dvr.h"
#include "hardware.h"
#include "i2c.h"
#include "it66021.h"
#include "it66121.h"
#include "minIni.h"
#include "msp_displayport.h"
#include "rtc6715.h"
#include "screen.h"
#include "tp2825.h"
#include "uart.h"
#include "ui/ui_porting.h"
#include "util/system.h"

/////////////////////////////////////////////////////////////////////////
// global
hw_status_t g_hw_stat;
int fhd_req = 0;
// local
pthread_mutex_t hardware_mutex;

/*
bit[0]   hdmi in
bit[1]   analog in
bit[2]   osd
bit[3]   dvr
bit[5:4] hdmi out
*/
uint32_t pclk_phase_default[VIDEO_SOURCE_NUM] = {
    0x00000001,
    0x00000000, // VIDEO_SOURCE_MENU_UI
    0x00000000, // VIDEO_SOURCE_HDZERO_IN_720P60_50
    0x00000000, // VIDEO_SOURCE_HDZERO_IN_720P90
    0x00000000, // VIDEO_SOURCE_HDZERO_IN_1080P30
    0x00000000, // VIDEO_SOURCE_AV_IN
    0x00000000, // VIDEO_SOURCE_HDMI_IN_1080P50
    0x00000000, // VIDEO_SOURCE_HDMI_IN_1080P60
    0x00000000, // VIDEO_SOURCE_HDMI_IN_1080POTHER
    0x00000000, // VIDEO_SOURCE_HDMI_IN_720P50
    0x00000000, // VIDEO_SOURCE_HDMI_IN_720P60
    0x00000000, // VIDEO_SOURCE_HDMI_IN_720P100
    0x00000000, // VIDEO_SOURCE_TP2825_EX, DO NOT USE
};

uint32_t pclk_phase_load[VIDEO_SOURCE_NUM];
uint32_t pclk_phase[VIDEO_SOURCE_NUM];

uint8_t pclk_phase_read_file(char *file_path) {
    FILE *file;
    char line[256];
    char type_str[128];
    char value_str[16];
    char *endptr;
    uint32_t value;
    uint8_t i = 0;

    LOGI("pclk_phase_read_file: %s", file_path);
    for (i = 0; i < VIDEO_SOURCE_NUM; i++) {
        pclk_phase_load[i] = 0xffffffff;
    }

    file = fopen(file_path, "r");
    if (file == NULL) {
        LOGI("%s open failed", file_path);
        return 1;
    } else {
        while (fgets(line, sizeof(line), file)) {
            sscanf(line, "%s %s", type_str, value_str);
            value = strtoul(value_str, &endptr, 0);
            if (*endptr != '\0') {
                LOGI("pclk parse error: %s, %s", type_str, value_str);
                break;
            }
            LOGI("%s    0x%08x", type_str, value);

            if (strcmp(type_str, "VIDEO_SOURCE_VERSION") == 0)
                pclk_phase_load[VIDEO_SOURCE_VERSION] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_MENU_UI") == 0)
                pclk_phase_load[VIDEO_SOURCE_MENU_UI] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_HDZERO_IN_720P60_50") == 0)
                pclk_phase_load[VIDEO_SOURCE_HDZERO_IN_720P60_50] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_HDZERO_IN_720P90") == 0)
                pclk_phase_load[VIDEO_SOURCE_HDZERO_IN_720P90] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_HDZERO_IN_1080P30") == 0)
                pclk_phase_load[VIDEO_SOURCE_HDZERO_IN_1080P30] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_AV_IN") == 0)
                pclk_phase_load[VIDEO_SOURCE_AV_IN] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_HDMI_IN_1080P50") == 0)
                pclk_phase_load[VIDEO_SOURCE_HDMI_IN_1080P50] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_HDMI_IN_1080P60") == 0)
                pclk_phase_load[VIDEO_SOURCE_HDMI_IN_1080P60] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_HDMI_IN_1080POTHER") == 0)
                pclk_phase_load[VIDEO_SOURCE_HDMI_IN_1080POTHER] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_HDMI_IN_720P50") == 0)
                pclk_phase_load[VIDEO_SOURCE_HDMI_IN_720P50] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_HDMI_IN_720P60") == 0)
                pclk_phase_load[VIDEO_SOURCE_HDMI_IN_720P60] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_HDMI_IN_720P100") == 0)
                pclk_phase_load[VIDEO_SOURCE_HDMI_IN_720P100] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_TP2825_EX") == 0)
                pclk_phase_load[VIDEO_SOURCE_TP2825_EX] = value;
        }
    }

    fclose(file);
    return 0;
}

uint8_t pclk_phase_write_file(char *file_path, uint32_t phase_p[]) {
    FILE *file;

    LOGI("pclk_phase_write_file %s", file_path);

    file = fopen(file_path, "w");
    if (file == NULL) {
        LOGI("%s open failed", file_path);
        return 1;
    }

    fprintf(file, "VIDEO_SOURCE_VERSION 0x%08x\r\n", phase_p[VIDEO_SOURCE_VERSION]);
    fprintf(file, "VIDEO_SOURCE_MENU_UI 0x%08x\r\n", phase_p[VIDEO_SOURCE_MENU_UI]);
    fprintf(file, "VIDEO_SOURCE_HDZERO_IN_720P60_50 0x%08x\r\n", phase_p[VIDEO_SOURCE_HDZERO_IN_720P60_50]);
    fprintf(file, "VIDEO_SOURCE_HDZERO_IN_720P90 0x%08x\r\n", phase_p[VIDEO_SOURCE_HDZERO_IN_720P90]);
    fprintf(file, "VIDEO_SOURCE_HDZERO_IN_1080P30 0x%08x\r\n", phase_p[VIDEO_SOURCE_HDZERO_IN_1080P30]);
    fprintf(file, "VIDEO_SOURCE_AV_IN 0x%08x\r\n", phase_p[VIDEO_SOURCE_AV_IN]);
    fprintf(file, "VIDEO_SOURCE_HDMI_IN_1080P50 0x%08x\r\n", phase_p[VIDEO_SOURCE_HDMI_IN_1080P50]);
    fprintf(file, "VIDEO_SOURCE_HDMI_IN_1080P60 0x%08x\r\n", phase_p[VIDEO_SOURCE_HDMI_IN_1080P60]);
    fprintf(file, "VIDEO_SOURCE_HDMI_IN_1080POTHER 0x%08x\r\n", phase_p[VIDEO_SOURCE_HDMI_IN_1080POTHER]);
    fprintf(file, "VIDEO_SOURCE_HDMI_IN_720P50 0x%08x\r\n", phase_p[VIDEO_SOURCE_HDMI_IN_720P50]);
    fprintf(file, "VIDEO_SOURCE_HDMI_IN_720P60 0x%08x\r\n", phase_p[VIDEO_SOURCE_HDMI_IN_720P60]);
    fprintf(file, "VIDEO_SOURCE_HDMI_IN_720P100 0x%08x\r\n", phase_p[VIDEO_SOURCE_HDMI_IN_720P100]);
    fprintf(file, "VIDEO_SOURCE_TP2825_EX 0x%08x\r\n", phase_p[VIDEO_SOURCE_TP2825_EX]);

    fclose(file);
}

void pclk_phase_load_system() {
    uint8_t i;

    system_exec("cp /mnt/extsd/pclk_phase.cfg /etc/pclk_phase.cfg");
    system_exec("rm /mnt/extsd/pclk_phase.cfg");

    if (pclk_phase_read_file("/etc/pclk_phase.cfg")) {
        // if no .cfg file, write it.
        pclk_phase_write_file("/etc/pclk_phase.cfg", pclk_phase_default);

        for (i = 0; i < VIDEO_SOURCE_NUM; i++) {
            pclk_phase[i] = pclk_phase_default[i];
        }
    } else if (pclk_phase_load[VIDEO_SOURCE_VERSION] != 0xffffffff && pclk_phase_load[VIDEO_SOURCE_VERSION] != pclk_phase_default[VIDEO_SOURCE_VERSION]) {
        // newer .cfg file version
        pclk_phase_write_file("/etc/pclk_phase.cfg", pclk_phase_default);

        for (i = 0; i < VIDEO_SOURCE_NUM; i++) {
            pclk_phase[i] = pclk_phase_default[i];
        }
    } else {
        for (i = 0; i < VIDEO_SOURCE_NUM; i++) {
            if (pclk_phase_load[i] != 0xffffffff)
                pclk_phase[i] = pclk_phase_load[i];
        }
    }
}

void pclk_phase_dump() {
    FILE *file;
    char *file_path = "/mnt/extsd/pclk_phase_dump.cfg";

    file = fopen(file_path, "r");
    if (file == NULL)
        return;
    fclose(file);

    pclk_phase_write_file(file_path, pclk_phase);
}

void pclk_phase_init() {
    pclk_phase_load_system();
    pclk_phase_dump();
}

/*
DCLK_INVERT
    0: Disable dclk invert
    1: Enable dclk invert
DCLK_DLY_EN
    0: Disable
    1: Enable
DCLK_DLY_NUM[5:0]
    Number= num+1
*/
void vdpo_sync_ctrl_set(bool dclk_invert, bool dclk_dly_en, uint8_t dvlk_dly_num) {
    const uint32_t addr = 0x06542008;
    uint32_t dat = 0x00000003;
    char buf[32];

    dat |= (dclk_invert << 3);
    dat |= (dvlk_dly_num << 4);
    dat |= (dclk_dly_en << 10);

    snprintf(buf, sizeof(buf), "aww 0x%08x 0x%08x", addr, dat);
    system_exec(buf);
}

/*
used for dvr
Pclk_dly
default value : 9
32 Step for adjust, 1 step = 0.2ns
*/
void csic_pclk_dly_set(uint8_t pclk_dly_num) {
    const uint32_t addr = 0x06601500;
    uint32_t dat = pclk_dly_num;
    char buf[32];

    snprintf(buf, sizeof(buf), "aww 0x%08x 0x%08x", addr, dat);
    system_exec(buf);
}

void csic_pclk_invert_set(uint8_t is_invert) {
    const uint32_t addr = 0x06601004;
    uint32_t dat = is_invert ? 0x010000A0 : 0x010100A0;
    char buf[32];

    snprintf(buf, sizeof(buf), "aww 0x%08x 0x%08x", addr, dat);
    system_exec(buf);
}

void pclk_phase_set(video_source_t source) {
    LOGI("pclk_phase_set %d", pclk_phase[source]);
    // bit[0] hdmi in
    IT66021_Set_Pclk((pclk_phase[source] >> 0) & 1);

    // bit[1] analog in
    // TP2825_Set_Pclk((pclk_phase[source] >> 1) & 1);

    // bit[2] osd
    vdpo_sync_ctrl_set((pclk_phase[source] >> 2) & 1, 0, 0);

    // bit[3] dvr
    csic_pclk_invert_set((pclk_phase[source] >> 3) & 1);

    // bit [5:4] hdmi out  bit[7]
    IT66121_set_phase((pclk_phase[source] >> 4) & 3, (pclk_phase[source] >> 7) & 1);
}
void hw_stat_init() {
    g_hw_stat.source_mode = SOURCE_MODE_UI;
    g_hw_stat.vdpo_tmg = VDPO_TMG_720P60;

    g_hw_stat.hdz_bw = 0;
    g_hw_stat.hdzero_open = 0;
    g_hw_stat.m0_open = 0;

    g_hw_stat.av_chid = 0;
    g_hw_stat.av_pal[0] = g_hw_stat.av_pal[1] = g_hw_stat.av_pal_w = 0;
    g_hw_stat.av_valid[0] = g_hw_stat.av_valid[1] = 0;

    g_hw_stat.hdmiin_valid = 0;
    g_hw_stat.hdmiin_vtmg = HDMIIN_VTMG_1080P50;

    pthread_mutex_init(&hardware_mutex, NULL);
}

void Display_VO_SWITCH(uint8_t sel) // 0 = UI;  1 = HDZERO or AV_in or HDMI_in
{
    I2C_Write(ADDR_FPGA, 0x20, sel);

    if (sel && g_hw_stat.hdzero_open && (g_hw_stat.source_mode == SOURCE_MODE_HDZERO))
        DM6302_openM0(1);
    else
        DM6302_openM0(0);

    I2C_Write(ADDR_FPGA, 0x06, 0x0F);
}

void Screen_ON(int bON) {
    pthread_mutex_lock(&hardware_mutex);
    Screen_Display(bON);
    pthread_mutex_unlock(&hardware_mutex);
}

void MFPGA_HDZ_Ratio(int ratio) {
    I2C_Write(ADDR_FPGA, 0x8f, ratio << 7);
    // LOGI("MFPGA_SetRatio %d",ratio);
}

void MFPGA_HDZ_VTMG(int mode, int is_43) {
    switch (mode) {
    case VR_720P50:
        I2C_Write(ADDR_FPGA, 0x40, 0x00);
        I2C_Write(ADDR_FPGA, 0x41, 0x25);
        I2C_Write(ADDR_FPGA, 0x42, 0xd0);
        I2C_Write(ADDR_FPGA, 0x43, 0xdc);
        I2C_Write(ADDR_FPGA, 0x44, 0x45);
        I2C_Write(ADDR_FPGA, 0x45, 0xee);
        I2C_Write(ADDR_FPGA, 0x48, 0x28);
        I2C_Write(ADDR_FPGA, 0x49, 0x4c);
        I2C_Write(ADDR_FPGA, 0x4a, 0x00);
        I2C_Write(ADDR_FPGA, 0x4b, 0x05);
        I2C_Write(ADDR_FPGA, 0x4c, 0x17);
        I2C_Write(ADDR_FPGA, 0x4d, 0xbc);
        I2C_Write(ADDR_FPGA, 0x4e, 0x07);

        I2C_Write(ADDR_FPGA, 0x52, 0x5f);
        I2C_Write(ADDR_FPGA, 0x53, 0x5f);
        I2C_Write(ADDR_FPGA, 0x54, 0x88);
        I2C_Write(ADDR_FPGA, 0x61, 0x96);
        I2C_Write(ADDR_FPGA, 0x63, 0x78);
        I2C_Write(ADDR_FPGA, 0x65, 0xc8);
        I2C_Write(ADDR_FPGA, 0x66, 0x00);
        break;
    case VR_720P60:
        I2C_Write(ADDR_FPGA, 0x40, 0x00);
        I2C_Write(ADDR_FPGA, 0x41, 0x25);
        I2C_Write(ADDR_FPGA, 0x42, 0xd0);
        I2C_Write(ADDR_FPGA, 0x43, 0xdc);
        I2C_Write(ADDR_FPGA, 0x44, 0x45);
        I2C_Write(ADDR_FPGA, 0x45, 0xee);
        I2C_Write(ADDR_FPGA, 0x48, 0x28);
        I2C_Write(ADDR_FPGA, 0x49, 0x4c);
        I2C_Write(ADDR_FPGA, 0x4a, 0x00);
        I2C_Write(ADDR_FPGA, 0x4b, 0x05);
        I2C_Write(ADDR_FPGA, 0x4c, 0x17);
        I2C_Write(ADDR_FPGA, 0x4d, 0x72);
        I2C_Write(ADDR_FPGA, 0x4e, 0x06);

        I2C_Write(ADDR_FPGA, 0x52, 0x5f);
        I2C_Write(ADDR_FPGA, 0x53, 0x5f);
        I2C_Write(ADDR_FPGA, 0x54, 0x88);
        I2C_Write(ADDR_FPGA, 0x61, 0x96);
        I2C_Write(ADDR_FPGA, 0x63, 0x78);
        I2C_Write(ADDR_FPGA, 0x65, 0xc8);
        I2C_Write(ADDR_FPGA, 0x66, 0x00);
        break;
    case VR_960x720P60:
        I2C_Write(ADDR_FPGA, 0x40, 0x00);
        I2C_Write(ADDR_FPGA, 0x41, 0x25);
        I2C_Write(ADDR_FPGA, 0x42, 0xd0);
        I2C_Write(ADDR_FPGA, 0x43, 0xdc);
        I2C_Write(ADDR_FPGA, 0x44, 0x45);
        I2C_Write(ADDR_FPGA, 0x45, 0xee);
        I2C_Write(ADDR_FPGA, 0x48, 0x28);
        I2C_Write(ADDR_FPGA, 0x49, 0x4c);
        I2C_Write(ADDR_FPGA, 0x4a, 0x00);
        I2C_Write(ADDR_FPGA, 0x4b, 0x05);
        I2C_Write(ADDR_FPGA, 0x4c, 0x17);
        I2C_Write(ADDR_FPGA, 0x4d, 0xdc);
        I2C_Write(ADDR_FPGA, 0x4e, 0x05);

        I2C_Write(ADDR_FPGA, 0x52, 0x5f);
        I2C_Write(ADDR_FPGA, 0x53, 0x5f);
        I2C_Write(ADDR_FPGA, 0x54, 0x88);
        I2C_Write(ADDR_FPGA, 0x61, 0x96);
        I2C_Write(ADDR_FPGA, 0x63, 0x78);
        I2C_Write(ADDR_FPGA, 0x65, 0xc8);
        I2C_Write(ADDR_FPGA, 0x66, 0x00);
        break;
    case VR_540P60:
        I2C_Write(ADDR_FPGA, 0x40, 0xc0);
        I2C_Write(ADDR_FPGA, 0x41, 0x23);
        I2C_Write(ADDR_FPGA, 0x42, 0x1c);
        I2C_Write(ADDR_FPGA, 0x43, 0xe7);
        I2C_Write(ADDR_FPGA, 0x44, 0x43);
        I2C_Write(ADDR_FPGA, 0x45, 0x33);
        I2C_Write(ADDR_FPGA, 0x48, 0x28);
        I2C_Write(ADDR_FPGA, 0x49, 0x20);
        I2C_Write(ADDR_FPGA, 0x4a, 0x00);
        I2C_Write(ADDR_FPGA, 0x4b, 0x05);
        I2C_Write(ADDR_FPGA, 0x4c, 0x10);
        I2C_Write(ADDR_FPGA, 0x4d, 0xd0);
        I2C_Write(ADDR_FPGA, 0x4e, 0x07);

        I2C_Write(ADDR_FPGA, 0x52, 0x48);
        I2C_Write(ADDR_FPGA, 0x53, 0x48);
        I2C_Write(ADDR_FPGA, 0x54, 0x66);
        I2C_Write(ADDR_FPGA, 0x61, 0x71);
        I2C_Write(ADDR_FPGA, 0x63, 0x5a);
        I2C_Write(ADDR_FPGA, 0x65, 0x96);
        I2C_Write(ADDR_FPGA, 0x66, 0x00);
        break;
    case VR_540P90:
        I2C_Write(ADDR_FPGA, 0x40, 0xc0);
        I2C_Write(ADDR_FPGA, 0x41, 0x23);
        I2C_Write(ADDR_FPGA, 0x42, 0x1c);
        I2C_Write(ADDR_FPGA, 0x43, 0xe7);
        I2C_Write(ADDR_FPGA, 0x44, 0x43);
        I2C_Write(ADDR_FPGA, 0x45, 0x33);
        I2C_Write(ADDR_FPGA, 0x48, 0x28);
        I2C_Write(ADDR_FPGA, 0x49, 0x20);
        I2C_Write(ADDR_FPGA, 0x4a, 0x00);
        I2C_Write(ADDR_FPGA, 0x4b, 0x05);
        I2C_Write(ADDR_FPGA, 0x4c, 0x10);
        I2C_Write(ADDR_FPGA, 0x4d, 0x30);
        I2C_Write(ADDR_FPGA, 0x4e, 0x05);

        I2C_Write(ADDR_FPGA, 0x52, 0x48);
        I2C_Write(ADDR_FPGA, 0x53, 0x48);
        I2C_Write(ADDR_FPGA, 0x54, 0x66);
        I2C_Write(ADDR_FPGA, 0x61, 0x71);
        I2C_Write(ADDR_FPGA, 0x63, 0x5a);
        I2C_Write(ADDR_FPGA, 0x65, 0x96);
        I2C_Write(ADDR_FPGA, 0x66, 0x00);
        break;
    case VR_540P90_CROP:
        I2C_Write(ADDR_FPGA, 0x40, 0xc0);
        I2C_Write(ADDR_FPGA, 0x41, 0x23);
        I2C_Write(ADDR_FPGA, 0x42, 0x1c);
        I2C_Write(ADDR_FPGA, 0x43, 0xe7);
        I2C_Write(ADDR_FPGA, 0x44, 0x43);
        I2C_Write(ADDR_FPGA, 0x45, 0x33);
        I2C_Write(ADDR_FPGA, 0x48, 0x28);
        I2C_Write(ADDR_FPGA, 0x49, 0x20);
        I2C_Write(ADDR_FPGA, 0x4a, 0x00);
        I2C_Write(ADDR_FPGA, 0x4b, 0x05);
        I2C_Write(ADDR_FPGA, 0x4c, 0x10);
        I2C_Write(ADDR_FPGA, 0x4d, 0xe2);
        I2C_Write(ADDR_FPGA, 0x4e, 0x04);

        I2C_Write(ADDR_FPGA, 0x52, 0x48);
        I2C_Write(ADDR_FPGA, 0x53, 0x48);
        I2C_Write(ADDR_FPGA, 0x54, 0x66);
        I2C_Write(ADDR_FPGA, 0x61, 0x71);
        I2C_Write(ADDR_FPGA, 0x63, 0x5a);
        I2C_Write(ADDR_FPGA, 0x65, 0x96);
        I2C_Write(ADDR_FPGA, 0x66, 0x00);
        break;
    case VR_1080P30:
        I2C_Write(ADDR_FPGA, 0x40, 0x00);
        I2C_Write(ADDR_FPGA, 0x41, 0x45);
        I2C_Write(ADDR_FPGA, 0x42, 0x38);
        I2C_Write(ADDR_FPGA, 0x43, 0xd0);
        I2C_Write(ADDR_FPGA, 0x44, 0x87);
        I2C_Write(ADDR_FPGA, 0x45, 0x65);
        I2C_Write(ADDR_FPGA, 0x48, 0x14);
        I2C_Write(ADDR_FPGA, 0x49, 0x1e);
        I2C_Write(ADDR_FPGA, 0x4a, 0x00);
        I2C_Write(ADDR_FPGA, 0x4b, 0x05);
        I2C_Write(ADDR_FPGA, 0x4c, 0x22);
        I2C_Write(ADDR_FPGA, 0x4d, 0x98);
        I2C_Write(ADDR_FPGA, 0x4e, 0x08);

        I2C_Write(ADDR_FPGA, 0x52, 0x8f);
        I2C_Write(ADDR_FPGA, 0x53, 0x8f);
        I2C_Write(ADDR_FPGA, 0x54, 0xcc);
        I2C_Write(ADDR_FPGA, 0x61, 0xe1);
        I2C_Write(ADDR_FPGA, 0x63, 0xb4);
        I2C_Write(ADDR_FPGA, 0x65, 0x2c);
        I2C_Write(ADDR_FPGA, 0x66, 0x01);
        break;
    }

    MFPGA_HDZ_Ratio(is_43);
    I2C_Write(ADDR_FPGA, 0x06, 0x0F);
}

void Display_HDZ_t(int mode, int is_43) {
    Screen_Display(0);
    I2C_Write(ADDR_FPGA, 0x8C, 0x00);
    I2C_Write(ADDR_FPGA, 0x8d, 0x15);

    switch (mode) {
    case VR_720P50:
        system_exec("dispw -s vdpo 720p50");
        g_hw_stat.vdpo_tmg = VDPO_TMG_720P50;
        I2C_Write(ADDR_FPGA, 0x80, 0x00);
        pclk_phase_set(VIDEO_SOURCE_HDZERO_IN_720P60_50);
        break;
    case VR_720P60:
        system_exec("dispw -s vdpo 720p60");
        g_hw_stat.vdpo_tmg = VDPO_TMG_720P60;
        I2C_Write(ADDR_FPGA, 0x80, 0x00);
        pclk_phase_set(VIDEO_SOURCE_HDZERO_IN_720P60_50);
        break;
    case VR_960x720P60:
        system_exec("dispw -s vdpo 720p60");
        g_hw_stat.vdpo_tmg = VDPO_TMG_720P60;
        I2C_Write(ADDR_FPGA, 0x80, 0x00);
        pclk_phase_set(VIDEO_SOURCE_HDZERO_IN_720P60_50);
        break;
    case VR_540P60:
        system_exec("dispw -s vdpo 720p60");
        g_hw_stat.vdpo_tmg = VDPO_TMG_720P60;
        I2C_Write(ADDR_FPGA, 0x80, 0x01);
        pclk_phase_set(VIDEO_SOURCE_HDZERO_IN_720P60_50);
        break;
    case VR_540P90:
        system_exec("dispw -s vdpo 720p90");
        g_hw_stat.vdpo_tmg = VDPO_TMG_720P90;
        I2C_Write(ADDR_FPGA, 0x80, 0x03);
        pclk_phase_set(VIDEO_SOURCE_HDZERO_IN_720P90);
        break;
    case VR_540P90_CROP:
        system_exec("dispw -s vdpo 720p90");
        g_hw_stat.vdpo_tmg = VDPO_TMG_720P90;
        I2C_Write(ADDR_FPGA, 0x80, 0x03);
        pclk_phase_set(VIDEO_SOURCE_HDZERO_IN_720P90);
        break;
    case VR_1080P30:
        system_exec("dispw -s vdpo 720p60");
        g_hw_stat.vdpo_tmg = VDPO_TMG_720P60;
        I2C_Write(ADDR_FPGA, 0x80, 0x04);
        pclk_phase_set(VIDEO_SOURCE_HDZERO_IN_720P60_50);
        break;
    }

    DM5680_SetFPS(mode);
    MFPGA_HDZ_VTMG(mode, is_43);

    g_hw_stat.source_mode = SOURCE_MODE_HDZERO;
    Display_VO_SWITCH(1);
    Screen_Display(1);
    I2C_Write(ADDR_FPGA, 0x8C, 0x01);
    system_exec("aww 0x06542018 0x00000044"); // disable horizontal chroma FIR filter.
}

void Display_HDZ(int mode, int is_43) {
    pthread_mutex_lock(&hardware_mutex);
    Display_HDZ_t(mode, is_43);
    pthread_mutex_unlock(&hardware_mutex);
}

// Set to pattern mode
// enable: 0=disable; 1=enable
// mode: 0=color bar; 1=grid; 2=all black; 3=all white; 4=boot screen(do not use)
// speed: color bar move speed (0~15)
void MFPGA_Pattern(uint8_t enable, uint8_t mode, uint8_t speed) {
    mode = (enable & 0x01) | ((mode & 0x07) << 1) | ((speed & 0x0F) << 4);
    I2C_Write(ADDR_FPGA, 0x8A, mode);
}

void Display_Pattern(int enable, uint8_t mode, uint8_t speed) {
    static int last_en = -1;
    int tmg_ch = 0;

    pthread_mutex_lock(&hardware_mutex);

    if (last_en != enable) {
        last_en = enable;
        tmg_ch = 1;
    }

    if (tmg_ch)
        Screen_Display(0);

    MFPGA_Pattern(enable, mode, speed);

    if (tmg_ch)
        Screen_Display(1);

    pthread_mutex_unlock(&hardware_mutex);
}

void Display_UI_init() {
    g_hw_stat.source_mode = SOURCE_MODE_UI;
    Screen_Display(0);
    I2C_Write(ADDR_FPGA, 0x8C, 0x00);

    MFPGA_Pattern(0, 0, 0);
    RTC6715_Open(0);

    I2C_Write(ADDR_FPGA, 0x8d, 0x14);
    I2C_Write(ADDR_FPGA, 0x8e, 0x84);
    I2C_Write(ADDR_FPGA, 0x80, 0x00);
    I2C_Write(ADDR_FPGA, 0x84, 0x11);

    system_exec("dispw -s vdpo 720p60");
    g_hw_stat.vdpo_tmg = VDPO_TMG_720P60;
    Display_VO_SWITCH(0);

    pclk_phase_set(VIDEO_SOURCE_MENU_UI);

    Screen_Display(1);
    system_exec("aww 0x06542018 0x00000044"); // disable horizontal chroma FIR filter.
}

void Display_UI() {
    pthread_mutex_lock(&hardware_mutex);
    Display_UI_init();
    pthread_mutex_unlock(&hardware_mutex);
}

void HDZero_open(int bw) {
    if (bw != g_hw_stat.hdz_bw) // reopen with different bw
        HDZero_Close();

    if (g_hw_stat.hdzero_open == 0) {
        g_hw_stat.hdz_bw = bw;
        DM5680_SetBR(g_hw_stat.hdz_bw);
        DM6302_init(0, g_hw_stat.hdz_bw);
        DM5680_SetBB(1);
        g_hw_stat.hdzero_open = 1;
    }
    LOGI("HDZero: open");
}

void HDZero_Close() {
    DM5680_SetBB(0);
    DM5680_ResetRF(0);
    g_hw_stat.hdzero_open = 0;
    g_hw_stat.m0_open = 0;

    LOGI("HDZero: close");
}

int HDZERO_detect() // return = 1: vtmg to V536 changed
{
    static video_resolution_t cam_mode_last = VR_720P60;
    static uint8_t cam_4_3_last = 0;
    int ret = 0;

    pthread_mutex_lock(&hardware_mutex);

    if (g_hw_stat.source_mode == SOURCE_MODE_HDZERO) {

        if (cam_mode_last != CAM_MODE) { // Camera mode changed
            LOGI("CAM_mode: %d->%d", cam_mode_last, CAM_MODE);

            // 1. Change fps
            Display_HDZ_t(CAM_MODE, cam_4_3);
            dvr_update_vi_conf(CAM_MODE);
            system_script(REC_STOP_LIVE);
            cam_mode_last = CAM_MODE;
            ret = 1;
        }

        if (cam_4_3_last != cam_4_3) {
            LOGI("cam_4_3: %d  CAM_MODE=%d", cam_4_3, CAM_MODE);
            cam_4_3_last = cam_4_3;

            if (CAM_MODE != VR_720P50)
                MFPGA_HDZ_Ratio(cam_4_3);
            else
                MFPGA_HDZ_Ratio(0);
        }
    }
    pthread_mutex_unlock(&hardware_mutex);
    return ret;
}

void AV_Mode_Switch_fpga(int is_pal) {
    if (is_pal) {
        system_exec("dispw -s vdpo 720p50");
        g_hw_stat.vdpo_tmg = VDPO_TMG_720P50;
        I2C_Write(ADDR_FPGA, 0x80, 0x10);
    } else {
        system_exec("dispw -s vdpo 720p60");
        g_hw_stat.vdpo_tmg = VDPO_TMG_720P60;
        I2C_Write(ADDR_FPGA, 0x80, 0x00);
    }
    I2C_Write(ADDR_FPGA, 0x06, 0x0F);
    system_exec("aww 0x06542018 0x00000044"); // disable horizontal chroma FIR filter.
    pclk_phase_set(VIDEO_SOURCE_AV_IN);
}

void AV_Mode_Switch(int is_pal) {
    if (g_hw_stat.vdpo_tmg != (is_pal ? VDPO_TMG_720P50 : VDPO_TMG_720P60)) {
        Screen_Display(0);
        AV_Mode_Switch_fpga(is_pal);
        Screen_Display(1);
    }
}

void Source_AV(source_t mode) // 0=rtc6715; 1=AV_in
{
    pthread_mutex_lock(&hardware_mutex);
    Screen_Display(0);
    I2C_Write(ADDR_FPGA, 0x8C, 0x00);

    g_hw_stat.av_chid = SOURCE_AV_MODULE == mode ? 0 : 1;

    if (SOURCE_AV_MODULE == mode) {
        RTC6715_Open(1);
        usleep(100 * 1000);
        RTC6715_SetCH(g_setting.source.analog_channel - 1);
    }

    TP2825_Switch_Mode(g_setting.source.analog_format);
    TP2825_Switch_CH(g_hw_stat.av_chid ? SOURCE_AV_IN : SOURCE_AV_MODULE);

    AV_Mode_Switch_fpga(g_setting.source.analog_format);
    g_hw_stat.av_pal_w = g_setting.source.analog_format;

    pclk_phase_set(VIDEO_SOURCE_AV_IN);

    HDZero_Close();

    I2C_Write(ADDR_FPGA, 0x8d, 0x1C);

    if (g_setting.source.analog_ratio == SETTING_SOURCES_ANALOG_RATIO_4_3)
        I2C_Write(ADDR_FPGA, 0x8f, 0x80); // bit[7]: 0=15:9, 1=original
    else
        I2C_Write(ADDR_FPGA, 0x8f, 0x00); // bit[7]: 0=15:9, 1=original

    I2C_Write(ADDR_FPGA, 0x8c, 0x02);

    g_hw_stat.source_mode = SOURCE_MODE_AV;
    Display_VO_SWITCH(1);
    Screen_Display(1);

    pthread_mutex_unlock(&hardware_mutex);
}

#define AV_DET_SWITCH_CNT 1
#define AV_DET_LOCK_CNT   10
#define AV_DET_DROP_CNT   5
int AV_in_detect() // return = 1: vtmg to V536 changed
{
    static int det_last = -1;
    static int det_cnt = 0, det2_cnt = 0;
    int rdat, det;
    int ret = 0;

    pthread_mutex_lock(&hardware_mutex);

    if (g_hw_stat.source_mode == SOURCE_MODE_UI) { // detect in UI mode
        if (g_hw_stat.av_chid == 0) {
            g_hw_stat.av_chid = 1; // 1=AV_in
            TP2825_Switch_CH(g_hw_stat.av_chid ? SOURCE_AV_IN : SOURCE_AV_MODULE);
        }

        rdat = I2C_Read(ADDR_TP2825, 0x01);

        det = (rdat & 0x80) ? 0 : 1;
        if (det_last != det) {
            det_last = det;
        } else {
            g_hw_stat.av_valid[g_hw_stat.av_chid] = det;
            g_hw_stat.av_pal[g_hw_stat.av_chid] = (rdat & 0x01) ? 0 : 1;
        }

        det_cnt = 0;
    } else if (g_hw_stat.source_mode == SOURCE_MODE_AV) {
        rdat = I2C_Read(ADDR_TP2825, 0x01);

        det = ((rdat & 0xC9) == (g_hw_stat.av_pal_w ? 0x48 : 0x49)) ? 1 : 0;

        if (det_last != det) {
            det_last = det;
            det_cnt = 0;
        } else if (det_cnt < AV_DET_SWITCH_CNT) {
            det_cnt++;
        }

        if (det && det_cnt == AV_DET_SWITCH_CNT) {
            g_hw_stat.av_pal_w = g_hw_stat.av_pal_w ? 0 : 1;
            g_hw_stat.av_valid[g_hw_stat.av_chid] = 0;

            TP2825_Switch_Mode(g_hw_stat.av_pal_w);
            AV_Mode_Switch(g_hw_stat.av_pal_w);
            // LOGI("Switch mode:%d", g_hw_stat.av_pal_w);

            if (g_hw_stat.av_pal)
                I2C_Write(ADDR_FPGA, 0x80, 0x10);
            else
                I2C_Write(ADDR_FPGA, 0x80, 0x00);

            ret = 1;

            g_setting.source.analog_format = g_hw_stat.av_pal_w;
            ini_putl("source", "analog_format", g_setting.source.analog_format, SETTING_INI);

            LOGI("AV_in_detect -- switch: av_pal = %d,  rdat = %02x\n", g_hw_stat.av_pal, rdat);
        } else {
            int vloss, h_lock, vh_lock;
            vloss = ((rdat & 0x80) == 0x80);
            h_lock = ((rdat & 0x60) == 0x60);
            vh_lock = ((rdat & 0x68) == 0x68);

            switch (g_hw_stat.av_valid[g_hw_stat.av_chid]) {
            case 0: // video loss
                if (h_lock || vh_lock) {
                    g_hw_stat.av_valid[g_hw_stat.av_chid] = 1;
                }
                break;

            case 1: // search
                if (vloss) {
                    det2_cnt = 0;
                    g_hw_stat.av_valid[g_hw_stat.av_chid] = 0;
                } else if (vh_lock) {
                    det2_cnt++;
                    if (det2_cnt >= AV_DET_LOCK_CNT) {
                        det2_cnt = 0;
                        g_hw_stat.av_valid[g_hw_stat.av_chid] = 2;
                        // ret = 1;
                    }
                } else
                    det2_cnt = 0;
                break;

            case 2: // locked
                if (vloss) {
                    det2_cnt = 0;
                    g_hw_stat.av_valid[g_hw_stat.av_chid] = 0;
                } else if (vh_lock == 0) {
                    det2_cnt++;
                    if (det2_cnt >= AV_DET_DROP_CNT) {
                        det2_cnt = 0;
                        g_hw_stat.av_valid[g_hw_stat.av_chid] = 1;
                    }
                } else
                    det2_cnt = 0;
                break;
            }
        }
    }

    pthread_mutex_unlock(&hardware_mutex);
    return ret;
}

void Source_HDMI_in() {
    pthread_mutex_lock(&hardware_mutex);
    Screen_Display(0);
    I2C_Write(ADDR_FPGA, 0x8C, 0x00);

    HDZero_Close();

    // I2C_Write(ADDR_FPGA, 0x8C, 0x04);
    I2C_Write(ADDR_FPGA, 0x84, 0x00);

    g_hw_stat.source_mode = SOURCE_MODE_HDMIIN;
    Display_VO_SWITCH(1);
    // Screen_Display(1);

    pthread_mutex_unlock(&hardware_mutex);
}

int HDMI_in_detect() {
    static int vtmg_last = -1;
    static int cs_last = -1;
    static int last_vld = 0;
    int vtmg, cs, freq_ref;
    int ret = 0;

    pthread_mutex_lock(&hardware_mutex);

    if ((g_hw_stat.source_mode == SOURCE_MODE_UI) || (g_hw_stat.source_mode == SOURCE_MODE_HDMIIN)) {

        last_vld = g_hw_stat.hdmiin_valid;
        g_hw_stat.hdmiin_valid = IT66021_Sig_det();

        if (g_hw_stat.source_mode == SOURCE_MODE_HDMIIN) {
            if (g_hw_stat.hdmiin_valid) {
                vtmg = IT66021_Get_VTMG(&freq_ref);
                if (vtmg_last != vtmg) {
                    vtmg_last = vtmg;
                    ret = 1;
                    LOGI("IT66021: VTMG change: %d", vtmg);

                    Screen_Display(0);
                    I2C_Write(ADDR_FPGA, 0x8C, 0x00);

                    switch (vtmg) {
                    case HDMIIN_VTMG_UNKNOW:
                        break;

                    case HDMIIN_VTMG_1080P60:
                        system_exec("dispw -s vdpo 720p60");
                        dvr_update_vi_conf(VR_1080P60);
                        g_hw_stat.vdpo_tmg = VDPO_TMG_720P60;
                        I2C_Write(ADDR_FPGA, 0x8D, 0x04);
                        pclk_phase_set(VIDEO_SOURCE_HDMI_IN_1080P60);
                        I2C_Write(ADDR_FPGA, 0x80, 0x00);
                        I2C_Write(ADDR_FPGA, 0x8C, 0x04);
                        I2C_Write(ADDR_FPGA, 0x06, 0x0F);
                        Screen_Display(1);
                        g_hw_stat.hdmiin_vtmg = HDMIIN_VTMG_1080P60;
                        break;

                    case HDMIIN_VTMG_1080P50:
                        system_exec("dispw -s vdpo 720p50");
                        dvr_update_vi_conf(VR_1080P50);
                        g_hw_stat.vdpo_tmg = VDPO_TMG_720P50;
                        I2C_Write(ADDR_FPGA, 0x8D, 0x04);
                        I2C_Write(ADDR_FPGA, 0x80, 0x20);
                        I2C_Write(ADDR_FPGA, 0x8C, 0x04);
                        I2C_Write(ADDR_FPGA, 0x06, 0x0F);
                        pclk_phase_set(VIDEO_SOURCE_HDMI_IN_1080P50);

                        Screen_Display(1);
                        g_hw_stat.hdmiin_vtmg = HDMIIN_VTMG_1080P50;
                        break;

                    case HDMIIN_VTMG_1080Pother:
                        system_exec("dispw -s vdpo 720p60");
                        dvr_update_vi_conf(VR_1080P50);
                        g_hw_stat.vdpo_tmg = VDPO_TMG_720P60;
                        I2C_Write(ADDR_FPGA, 0x8D, 0x14);
                        pclk_phase_set(VIDEO_SOURCE_HDMI_IN_1080P60);
                        I2C_Write(ADDR_FPGA, 0x80, 0x40);
                        I2C_Write(ADDR_FPGA, 0x8C, 0x04);
                        I2C_Write(ADDR_FPGA, 0x06, 0x0F);

                        Screen_Display(1);
                        g_hw_stat.hdmiin_vtmg = HDMIIN_VTMG_1080Pother;
                        break;

                    case HDMIIN_VTMG_720P50:
                        system_exec("dispw -s vdpo 720p50");
                        dvr_update_vi_conf(VR_720P50);
                        g_hw_stat.vdpo_tmg = VDPO_TMG_720P50;
                        I2C_Write(ADDR_FPGA, 0x8D, 0x14);
                        pclk_phase_set(VIDEO_SOURCE_HDMI_IN_720P50);

                        I2C_Write(ADDR_FPGA, 0x80, 0x60);
                        I2C_Write(ADDR_FPGA, 0x8C, 0x04);
                        I2C_Write(ADDR_FPGA, 0x06, 0x0F);
                        Screen_Display(1);
                        g_hw_stat.hdmiin_vtmg = HDMIIN_VTMG_720P50;
                        break;

                    case HDMIIN_VTMG_720P60:
                        system_exec("dispw -s vdpo 720p60");
                        dvr_update_vi_conf(VR_720P60);
                        g_hw_stat.vdpo_tmg = VDPO_TMG_720P60;
                        I2C_Write(ADDR_FPGA, 0x8D, 0x14);
                        pclk_phase_set(VIDEO_SOURCE_HDMI_IN_720P60);
                        I2C_Write(ADDR_FPGA, 0x80, 0x80);
                        I2C_Write(ADDR_FPGA, 0x8C, 0x04);
                        I2C_Write(ADDR_FPGA, 0x06, 0x0F);
                        Screen_Display(1);
                        g_hw_stat.hdmiin_vtmg = HDMIIN_VTMG_720P60;
                        break;

                    case HDMIIN_VTMG_720P100:
                        system_exec("dispw -s vdpo 720p30"); // 100fps actually
                        dvr_update_vi_conf(VR_540P90);
                        g_hw_stat.vdpo_tmg = VDPO_TMG_720P100;
                        I2C_Write(ADDR_FPGA, 0x8D, 0x04);
                        pclk_phase_set(VIDEO_SOURCE_HDMI_IN_720P100);
                        I2C_Write(ADDR_FPGA, 0x80, 0xA0);
                        I2C_Write(ADDR_FPGA, 0x8C, 0x04);
                        I2C_Write(ADDR_FPGA, 0x06, 0x0F);
                        Screen_Display(1);
                        g_hw_stat.hdmiin_vtmg = HDMIIN_VTMG_720P100;
                        break;
                    }
                }

                cs = IT66021_Get_CS();
                if (cs_last != cs) {
                    cs_last = cs;
                    LOGI("IT66021: Color space change: %d\n", cs);

                    IT66021_Set_CSMatrix(cs);
                }
            } else {
                if (last_vld)
                    I2C_Write(ADDR_FPGA, 0x06, 0x0F);

                vtmg_last = -1;
                cs_last = -1;
            }
        } else {
            vtmg_last = -1;
            cs_last = -1;
        }
    }
    pthread_mutex_unlock(&hardware_mutex);

    return ret;
}

void Display_Osd(bool enable) {
    I2C_Write(ADDR_FPGA, 0x84, enable ? 0x11 : 0x01);
}

void Set_Brightness(uint8_t bri) {
    int8_t val = 0x80 + bri - 39;
    I2C_Write(ADDR_FPGA, 0x85, val);
}

void Set_Contrast(uint8_t con) {
    int8_t val = con << 2;
    I2C_Write(ADDR_FPGA, 0x86, val);
}

void Set_Saturation(uint8_t sat) {
    int8_t val = sat << 2;
    I2C_Write(ADDR_FPGA, 0x87, val);
}

void Set_HT_status(uint8_t is_open, uint8_t frame_period, uint8_t sync_len) // frame_period unit = 2ms
{
    I2C_Write(ADDR_FPGA, 0x71, sync_len);
    I2C_Write(ADDR_FPGA, 0x70, (frame_period << 4) | is_open);
}

void Set_HT_dat(uint16_t ch0, uint16_t ch1, uint16_t ch2) {
    I2C_Write(ADDR_FPGA, 0x72, ch0 & 0xff);
    I2C_Write(ADDR_FPGA, 0x73, (ch0 >> 8) & 0xff);
    I2C_Write(ADDR_FPGA, 0x74, ch1 & 0xff);
    I2C_Write(ADDR_FPGA, 0x75, (ch1 >> 8) & 0xff);
    I2C_Write(ADDR_FPGA, 0x76, ch2 & 0xff);
    I2C_Write(ADDR_FPGA, 0x77, (ch2 >> 8) & 0xff);
}

void Analog_Module_Power(bool ForceSet) {
    // Batch 2 goggles only
    if (getHwRevision() >= HW_REV_2) {
        static bool Analog_Module_Power_State = 0;
        static bool Analog_Module_Power_State_Last = 0;
        if (g_setting.power.power_ana == 0) {
            Analog_Module_Power_State = 0;
        } else {
            if (g_source_info.source != SOURCE_AV_MODULE) {
                Analog_Module_Power_State = 1;
            } else {
                Analog_Module_Power_State = 0;
            }
        }
        if ((Analog_Module_Power_State_Last != Analog_Module_Power_State) || (ForceSet == 1)) {
            beep();
            Analog_Module_Power_State_Last = Analog_Module_Power_State;
            DM5680_Power_AnalogModule(Analog_Module_Power_State);
        }
    }
}

int Get_VideoLatancy_status() // ret: 0=unlocked, 1=locked
{
    uint8_t rdat = 0;

    if (g_hw_stat.source_mode == SOURCE_MODE_AV || g_hw_stat.source_mode == SOURCE_MODE_HDZERO) {
        rdat = I2C_Read(ADDR_FPGA, 0x11);
        rdat = (rdat < 0x04) ? 1 : 0;
    }

    return rdat;
}

int Get_HAN_status() // ret: 0=error; 1=ok
{
    uint8_t rdat;

    I2C_Write(ADDR_FPGA, 0x81, 0x01);
    sleep(1);

    rdat = I2C_Read(ADDR_FPGA, 0x18);

    if (rdat >= 0x4F && rdat <= 0x52)
        rdat = 1;
    else
        rdat = 0;

    I2C_Write(ADDR_FPGA, 0x81, 0x00);

    return rdat;
}

#endif
