#include "hardware.h"

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
#include "TP2825.h"
#include "beep.h"
#include "defines.h"
#include "dm5680.h"
#include "dm6302.h"
#include "dvr.h"
#include "hardware.h"
#include "i2c.h"
#include "it66021.h"
#include "it66121.h"
#include "msp_displayport.h"
#include "oled.h"
#include "uart.h"
#include "ui/ui_porting.h"
#include "util/system.h"

/////////////////////////////////////////////////////////////////////////
// global
hw_status_t g_hw_stat;
int fhd_req = 0;
// local
pthread_mutex_t hardware_mutex;

uint32_t vclk_phase_default[VIDEO_SOURCE_NUM] = {
    // 0x8d,  0x8e,  0x14,  hdmi_out
    0x00000006, // VIDEO_SOURCE_VERSION
    0x10840000, // VIDEO_SOURCE_MENU_UI
    0x10840000, // VIDEO_SOURCE_HDZERO_IN_720P60_50
    0x10840000, // VIDEO_SOURCE_HDZERO_IN_720P90
    0x10840000, // VIDEO_SOURCE_HDZERO_IN_1080P30
    0x10840000, // VIDEO_SOURCE_AV_IN
    0x10040000, // VIDEO_SOURCE_HDMI_IN_1080P50
    0x10040000, // VIDEO_SOURCE_HDMI_IN_1080P60
    0x10040000, // VIDEO_SOURCE_HDMI_IN_1080POTHER
    0x00040000, // VIDEO_SOURCE_HDMI_IN_720P50
    0x00040000, // VIDEO_SOURCE_HDMI_IN_720P60
    0x10040000, // VIDEO_SOURCE_HDMI_IN_720P100
    0x00000000, // VIDEO_SOURCE_TP2825_EX
};

/*
bit[0]   hdmi in
bit[1]   analog in
bit[2]   osd
bit[3]   dvr
bit[5:4] hdmi out
bit[6]   oled
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

uint32_t vclk_phase_load[VIDEO_SOURCE_NUM];
uint32_t vclk_phase[VIDEO_SOURCE_NUM];
uint32_t pclk_phase_load[VIDEO_SOURCE_NUM];
uint32_t pclk_phase[VIDEO_SOURCE_NUM];

uint8_t vclk_phase_read_file(char *file_path) {
    FILE *file;
    char line[256];
    char type_str[128];
    char value_str[16];
    char *endptr;
    uint32_t value;
    uint8_t i = 0;

    LOGI("vclk_phase_read_file: %s", file_path);
    for (i = 0; i < VIDEO_SOURCE_NUM; i++) {
        vclk_phase_load[i] = 0xffffffff;
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
                LOGI("vclk parse error: %s, %s", type_str, value_str);
                break;
            }
            LOGI("%s    0x%08x", type_str, value);

            if (strcmp(type_str, "VIDEO_SOURCE_VERSION") == 0)
                vclk_phase_load[VIDEO_SOURCE_VERSION] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_MENU_UI") == 0)
                vclk_phase_load[VIDEO_SOURCE_MENU_UI] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_HDZERO_IN_720P60_50") == 0)
                vclk_phase_load[VIDEO_SOURCE_HDZERO_IN_720P60_50] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_HDZERO_IN_720P90") == 0)
                vclk_phase_load[VIDEO_SOURCE_HDZERO_IN_720P90] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_HDZERO_IN_1080P30") == 0)
                vclk_phase_load[VIDEO_SOURCE_HDZERO_IN_1080P30] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_AV_IN") == 0)
                vclk_phase_load[VIDEO_SOURCE_AV_IN] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_HDMI_IN_1080P50") == 0)
                vclk_phase_load[VIDEO_SOURCE_HDMI_IN_1080P50] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_HDMI_IN_1080P60") == 0)
                vclk_phase_load[VIDEO_SOURCE_HDMI_IN_1080P60] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_HDMI_IN_1080POTHER") == 0)
                vclk_phase_load[VIDEO_SOURCE_HDMI_IN_1080POTHER] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_HDMI_IN_720P50") == 0)
                vclk_phase_load[VIDEO_SOURCE_HDMI_IN_720P50] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_HDMI_IN_720P60") == 0)
                vclk_phase_load[VIDEO_SOURCE_HDMI_IN_720P60] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_HDMI_IN_720P100") == 0)
                vclk_phase_load[VIDEO_SOURCE_HDMI_IN_720P100] = value;
            else if (strcmp(type_str, "VIDEO_SOURCE_TP2825_EX") == 0)
                vclk_phase_load[VIDEO_SOURCE_TP2825_EX] = value;
        }
    }

    fclose(file);
    return 0;
}

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

uint8_t vclk_phase_inv_read_file(char *file_path) {
    FILE *file;
    char line[256];
    char type_str[128];
    char value_str[16];
    char *endptr;
    uint8_t mode = 0;
    uint32_t value;
    uint8_t i = 0;

    LOGI("vclk_phase_inv_read_file: %s", file_path);

    file = fopen(file_path, "r");
    if (file == NULL) {
        LOGI("%s open failed", file_path);
        return 1;
    } else {
        while (fgets(line, sizeof(line), file)) {
            sscanf(line, "%s %s", type_str, value_str);
            value = strtoul(value_str, &endptr, 0);
            if (*endptr != '\0') {
                LOGI("vclk parse inv error: %s, %s", type_str, value_str);
                break;
            }

            if (strcmp(type_str, "MODE1") == 0 || strcmp(type_str, "mode1") == 0)
                mode = 1;
            else if (strcmp(type_str, "MODE2") == 0 || strcmp(type_str, "mode2") == 0)
                mode = 2;
            else if (strcmp(type_str, "MODE3") == 0 || strcmp(type_str, "mode3") == 0)
                mode = 3;
            else if (strcmp(type_str, "MODE4") == 0 || strcmp(type_str, "mode4") == 0)
                mode = 4;
            else if (strcmp(type_str, "MODE5") == 0 || strcmp(type_str, "mode5") == 0)
                mode = 5;
            else if (strcmp(type_str, "MODE6") == 0 || strcmp(type_str, "mode6") == 0)
                mode = 6;
            else if (strcmp(type_str, "MODE7") == 0 || strcmp(type_str, "mode7") == 0)
                mode = 7;
            else if (strcmp(type_str, "MODE8") == 0 || strcmp(type_str, "mode8") == 0)
                mode = 8;
            else if (strcmp(type_str, "MODE9") == 0 || strcmp(type_str, "mode9") == 0)
                mode = 9;
            else if (strcmp(type_str, "MODE10") == 0 || strcmp(type_str, "mode10") == 0)
                mode = 10;
            else if (strcmp(type_str, "MODE11") == 0 || strcmp(type_str, "mode11") == 0)
                mode = 11;
            else if (strcmp(type_str, "MODE12") == 0 || strcmp(type_str, "mode12") == 0)
                mode = 12;
            else
                break;

            LOGI("mode %d", mode);

            vclk_phase[mode] = vclk_phase_default[mode];

            if (mode == VIDEO_SOURCE_TP2825_EX) {
                vclk_phase[mode] = value;
            } else {
                if ((value >> 0) & 1) // 14[0]
                    vclk_phase[mode] ^= (1 << 8);

                if ((value >> 1) & 1) // 8e[7]
                    vclk_phase[mode] ^= (1 << 23);

                if ((value >> 2) & 1) // 8d[2]
                    vclk_phase[mode] ^= (1 << 26);

                if ((value >> 3) & 1) // hdmi out_0
                    vclk_phase[mode] ^= (1 << 0);

                if ((value >> 4) & 1) // hdmi out_1
                    vclk_phase[mode] ^= (1 << 1);

                if ((value >> 5) & 1) // 8d[4]
                    vclk_phase[mode] ^= (1 << 28);

                if ((value >> 6) & 1) // 8d[3]
                    vclk_phase[mode] ^= (1 << 27);

                if ((value >> 7) & 1) // 8e[2]
                    vclk_phase[mode] ^= (1 << 18);
            }

            LOGI("%s 0x%02x, ori:0x%08x new:0x%08x", type_str, value, vclk_phase_default[mode], vclk_phase[mode]);
        }
    }

    fclose(file);
    return 0;
}

uint8_t vclk_phase_write_file(char *file_path, uint32_t phase_p[]) {
    FILE *file;

    LOGI("vclk_phase_write_file %s", file_path);

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

void vclk_phase_load_system() {
    uint8_t i;

    if (vclk_phase_read_file("/etc/vclk_phase.cfg")) {
        // if no .cfg file, write it.
        // vclk_phase_write_file("/etc/vclk_phase.cfg", vclk_phase_default);

        for (i = 0; i < VIDEO_SOURCE_NUM; i++) {
            vclk_phase[i] = vclk_phase_default[i];
        }
    } else if (vclk_phase_load[VIDEO_SOURCE_VERSION] != 0xffffffff && vclk_phase_load[VIDEO_SOURCE_VERSION] != vclk_phase_default[VIDEO_SOURCE_VERSION]) {
        // newer .cfg file version
        // vclk_phase_write_file("/etc/vclk_phase.cfg", vclk_phase_default);

        for (i = 0; i < VIDEO_SOURCE_NUM; i++) {
            vclk_phase[i] = vclk_phase_default[i];
        }
    } else {
        for (i = 0; i < VIDEO_SOURCE_NUM; i++) {
            if (vclk_phase_load[i] != 0xffffffff)
                vclk_phase[i] = vclk_phase_load[i];
        }
    }
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

void vclk_phase_inv_load_sdcard() {
    if (vclk_phase_inv_read_file("/mnt/extsd/vclk_phase_inv.txt")) {
        return;
    }

    // vclk_phase_write_file("/etc/vclk_phase.cfg", vclk_phase);

    system_exec("rm /mnt/extsd/vclk_phase_inv.txt");
}

void vclk_phase_dump() {
    FILE *file;
    char *file_path = "/mnt/extsd/vclk_phase_dump.cfg";

    file = fopen(file_path, "r");
    if (file == NULL)
        return;
    fclose(file);

    vclk_phase_write_file(file_path, vclk_phase);
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

void vclk_phase_init() {
    vclk_phase_load_system();
    // vclk_phase_inv_load_sdcard();
    vclk_phase_dump();
}

void pclk_phase_init() {
    pclk_phase_load_system();
    pclk_phase_dump();
}

void vclk_phase_set(video_source_t source, uint8_t reg_8d_sel) {
    if (reg_8d_sel)
        I2C_Write(ADDR_FPGA, 0x8d, (((vclk_phase[source] >> 24) & 0xff) ^ (1 << 4)));
    else
        I2C_Write(ADDR_FPGA, 0x8d, (vclk_phase[source] >> 24) & 0xff);

    I2C_Write(ADDR_FPGA, 0x8e, (vclk_phase[source] >> 16) & 0xff);
    I2C_Write(ADDR_AL, 0x14, (vclk_phase[source] >> 8) & 0xff);

    IT66121_set_phase(vclk_phase[source] & 3, 0);
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
    TP2825_Set_Pclk((pclk_phase[source] >> 1) & 1);

    // bit[2] osd
    vdpo_sync_ctrl_set((pclk_phase[source] >> 2) & 1, 0, 0);

    // bit[3] dvr
    csic_pclk_invert_set((pclk_phase[source] >> 3) & 1);

    // bit [5:4] hdmi out  bit[7]
    IT66121_set_phase((pclk_phase[source] >> 4) & 3, (pclk_phase[source] >> 7) & 1);

    // bit[6] oled
    I2C_Write(ADDR_AL, 0x14, (pclk_phase[source] >> 6) & 1);
}
void hw_stat_init() {
    g_hw_stat.source_mode = SOURCE_MODE_UI;
    g_hw_stat.vdpo_tmg = VDPO_TMG_1080P50;

    g_hw_stat.hdz_bw = 0;
    g_hw_stat.hdzero_open = 0;
    g_hw_stat.m0_open = 0;

    g_hw_stat.av_chid = 0;
    g_hw_stat.av_pal[0] = g_hw_stat.av_pal[1] = g_hw_stat.av_pal_w = 0;
    g_hw_stat.av_valid[0] = g_hw_stat.av_valid[1] = 0;

    g_hw_stat.hdmiin_valid = 0;
    g_hw_stat.hdmiin_vtmg = HDMIIN_VTMG_1080P50;
    g_hw_stat.IS_TP2825_L = 0;

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

void OLED_ON(int bON) {
    pthread_mutex_lock(&hardware_mutex);
    OLED_display(bON);
    pthread_mutex_unlock(&hardware_mutex);
}

void Display_UI_init() {
    g_hw_stat.source_mode = SOURCE_MODE_UI;
    I2C_Write(ADDR_FPGA, 0x8C, 0x00);

    system_exec("dispw -s vdpo 1080p50");
    g_hw_stat.vdpo_tmg = VDPO_TMG_1080P50;
    Display_VO_SWITCH(0);

    vclk_phase_set(VIDEO_SOURCE_MENU_UI, 0);
    pclk_phase_set(VIDEO_SOURCE_MENU_UI);
    I2C_Write(ADDR_FPGA, 0x80, 0x00);
    I2C_Write(ADDR_FPGA, 0x84, 0x11);

    OLED_SetTMG(0);
    system_exec("aww 0x0300b084 0x00015565"); // Set vdpo clock driver strength to level 2. Refer datasheet 12.7.5.11
    system_exec("aww 0x06542018 0x00000044"); // disable horizontal chroma FIR filter.
}

void Display_UI() {
    pthread_mutex_lock(&hardware_mutex);
    OLED_display(0);

    Display_UI_init();

    OLED_display(1);
    pthread_mutex_unlock(&hardware_mutex);
}

void Display_720P60_50_t(int mode, uint8_t is_43) // fps: 0=50, 1=60
{
    OLED_display(0);
    I2C_Write(ADDR_FPGA, 0x8C, 0x00);

    system_exec("dispw -s vdpo 720p60");
    g_hw_stat.vdpo_tmg = VDPO_TMG_720P60;
    vclk_phase_set(VIDEO_SOURCE_HDZERO_IN_720P60_50, 0);
    pclk_phase_set(VIDEO_SOURCE_HDZERO_IN_720P60_50);

    I2C_Write(ADDR_FPGA, 0x80, (mode == VR_540P60) ? 0x01 : 0x00);

    DM5680_SetFPS(mode);
    if (mode == VR_540P60)
        MFPGA_Set540P60();
    else
        MFPGA_Set720P60(mode, is_43);
    OLED_SetTMG(1);

    I2C_Write(ADDR_FPGA, 0x8C, 0x01);

    g_hw_stat.source_mode = SOURCE_MODE_HDZERO;
    Display_VO_SWITCH(1);
    OLED_display(1);
    system_exec("aww 0x06542018 0x00000044"); // disable horizontal chroma FIR filter.
}

void Display_720P90_t(int mode) {
    OLED_display(0);
    I2C_Write(ADDR_FPGA, 0x8C, 0x00);

    system_exec("dispw -s vdpo 720p90");
    g_hw_stat.vdpo_tmg = VDPO_TMG_720P90;
    vclk_phase_set(VIDEO_SOURCE_HDZERO_IN_720P90, 0);
    pclk_phase_set(VIDEO_SOURCE_HDZERO_IN_720P90);
    I2C_Write(ADDR_FPGA, 0x80, 0x03);

    DM5680_SetFPS(mode);
    MFPGA_Set720P90(mode);
    OLED_SetTMG(1);

    I2C_Write(ADDR_FPGA, 0x8C, 0x01);

    g_hw_stat.source_mode = SOURCE_MODE_HDZERO;
    Display_VO_SWITCH(1);
    OLED_display(1);
    system_exec("aww 0x06542018 0x00000044"); // disable horizontal chroma FIR filter.
}

void Display_1080P30_t(int mode) {
    OLED_display(0);
    I2C_Write(ADDR_FPGA, 0x8C, 0x00);

    system_exec("dispw -s vdpo 1080p60");
    g_hw_stat.vdpo_tmg = VDPO_TMG_1080P60;
    vclk_phase_set(VIDEO_SOURCE_HDZERO_IN_1080P30, 0);
    pclk_phase_set(VIDEO_SOURCE_HDZERO_IN_1080P30);

    I2C_Write(ADDR_FPGA, 0x80, 0x04);
    // I2C_Write(ADDR_FPGA, 0x84, 0x00); // close OSD

    DM5680_SetFPS(mode);
    MFPGA_Set1080P30();
    OLED_SetTMG(2);

    I2C_Write(ADDR_FPGA, 0x8C, 0x01);

    g_hw_stat.source_mode = SOURCE_MODE_HDZERO;
    Display_VO_SWITCH(1);
    OLED_display(1);
    system_exec("aww 0x06542018 0x00000044"); // disable horizontal chroma FIR filter.
}

void Display_720P60_50(int mode, uint8_t is_43) {
    pthread_mutex_lock(&hardware_mutex);
    Display_720P60_50_t(mode, is_43);
    pthread_mutex_unlock(&hardware_mutex);
}

void Display_720P90(int mode) {
    pthread_mutex_lock(&hardware_mutex);
    Display_720P90_t(mode);
    pthread_mutex_unlock(&hardware_mutex);
}

void Display_1080P30(int mode) {
    pthread_mutex_lock(&hardware_mutex);
    Display_1080P30_t(mode);
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
            switch (CAM_MODE) {
            case VR_720P50:
            case VR_720P60:
            case VR_960x720P60:
            case VR_540P60:
                Display_720P60_50_t(CAM_MODE, cam_4_3);
                break;
            case VR_720P30:
                perror("cam_mode =2 fix me!!\n ");
                break;
            case VR_540P90:
            case VR_540P90_CROP:
                Display_720P90_t(CAM_MODE);
                break;
            case VR_1080P30:
                Display_1080P30_t(CAM_MODE);

                break;
            default:
                LOGW("cam_mode =%d not suppored!!\n ", CAM_MODE);
                break;
            }

            if (CAM_MODE == VR_1080P30)
                fhd_req = 1;
            else if (cam_mode_last == VR_1080P30)
                fhd_req = -1;
            dvr_update_vi_conf(CAM_MODE);
            system_script(REC_STOP_LIVE);
            cam_mode_last = CAM_MODE;
            ret = 1;
        }

        if (cam_4_3_last != cam_4_3) {
            LOGI("cam_4_3: %d  CAM_MODE=%d", cam_4_3, CAM_MODE);
            cam_4_3_last = cam_4_3;

            if (CAM_MODE != VR_720P50)
                MFPGA_SetRatio(cam_4_3);
            else
                MFPGA_SetRatio(0);
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
}

void AV_Mode_Switch(int is_pal) {
    int tmg = is_pal ? VDPO_TMG_720P50 : VDPO_TMG_720P60;

    if (g_hw_stat.vdpo_tmg != tmg) {
        OLED_display(0);
        AV_Mode_Switch_fpga(is_pal);
        OLED_display(1);
    }
}

void Source_AV(uint8_t sel) // 0=AV in, 1=AV module
{
    pthread_mutex_lock(&hardware_mutex);
    OLED_display(0);
    I2C_Write(ADDR_FPGA, 0x8C, 0x00);

    g_hw_stat.av_chid = sel ? 1 : 0;

    TP2825_Config(sel, g_setting.source.analog_format);

    // TP2825_Switch_Mode(g_hw_stat.av_pal[g_hw_stat.av_chid]);
    TP2825_Switch_Mode(g_setting.source.analog_format);
    TP2825_Switch_CH(g_hw_stat.av_chid);

    // AV_Mode_Switch_fpga(g_hw_stat.av_pal[g_hw_stat.av_chid]);
    // g_hw_stat.av_pal_w = g_hw_stat.av_pal[g_hw_stat.av_chid];
    AV_Mode_Switch_fpga(g_setting.source.analog_format);
    g_hw_stat.av_pal_w = g_setting.source.analog_format;

    vclk_phase_set(VIDEO_SOURCE_AV_IN, 0);
    pclk_phase_set(VIDEO_SOURCE_AV_IN);

    I2C_Write(ADDR_FPGA, 0x89, 0x01);

    HDZero_Close();
    OLED_SetTMG(1);

    if (g_setting.source.analog_ratio == SETTING_SOURCES_ANALOG_RATIO_4_3)
        I2C_Write(ADDR_FPGA, 0x8f, 0x80); // bit[7]: 0=16:9, 1=original
    else
        I2C_Write(ADDR_FPGA, 0x8f, 0x00); // bit[7]: 0=16:9, 1=original

    I2C_Write(ADDR_FPGA, 0x8C, 0x02);

    g_hw_stat.source_mode = SOURCE_MODE_AV;
    Display_VO_SWITCH(1);
    OLED_display(1);

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

    rdat = I2C_Read(ADDR_TP2825, 0x01);

    if (g_hw_stat.source_mode == SOURCE_MODE_UI) { // detect in UI mode
        TP2825_Set_Clamp(0);

        det = (rdat & 0x80) ? 0 : 1;
        if (det_last != det) {
            det_last = det;
        } else {
            g_hw_stat.av_valid[g_hw_stat.av_chid] = det;
            g_hw_stat.av_pal[g_hw_stat.av_chid] = ((rdat & 0xAC) == 0x2c) ? 1 : 0;

            g_hw_stat.av_chid = g_hw_stat.av_chid ? 0 : 1;
            TP2825_Switch_CH(g_hw_stat.av_chid);
            det_last = -1;
        }

        det_cnt = det2_cnt = 0;
    } else if (g_hw_stat.source_mode == SOURCE_MODE_AV) { // detect in AV_in/Module_bay mode
        det = ((rdat & 0xAE) == (g_hw_stat.av_pal_w ? 0x28 : 0x2C)) ? 1 : 0;

        if (det_last != det) {
            det_last = det;
            det_cnt = 0;
        } else if (det_cnt < AV_DET_SWITCH_CNT) {
            det_cnt++;
        }

        if (det && det_cnt == AV_DET_SWITCH_CNT) {
            g_hw_stat.av_pal_w = g_hw_stat.av_pal_w ? 0 : 1;

            TP2825_Switch_Mode(g_hw_stat.av_pal_w);
            // LOGI("Switch mode:%d", g_hw_stat.av_pal_w);

            if (g_hw_stat.av_pal[g_hw_stat.av_chid])
                I2C_Write(ADDR_FPGA, 0x80, 0x10);
            else
                I2C_Write(ADDR_FPGA, 0x80, 0x00);

            if (g_hw_stat.av_pal_w == g_hw_stat.av_pal[g_hw_stat.av_chid])
                I2C_Write(ADDR_FPGA, 0x89, 0x01);
            else
                I2C_Write(ADDR_FPGA, 0x89, 0x00);

            g_hw_stat.av_valid[g_hw_stat.av_chid] = 0;

            LOGI("AV_in_detect -- switch: av_pal = %d,  rdat = %02x\n", g_hw_stat.av_pal_w, rdat);
        } else {
            int vloss, h_lock, vh_lock;
            vloss = (rdat & 0x80) ? 1 : 0;
            h_lock = ((rdat & 0x28) == 0x28) ? 1 : 0;
            vh_lock = ((rdat & 0x68) == 0x68) ? 1 : 0;

            // LOGI("rdat=%x, state=%d, dcnt2=%d", rdat,g_hw_stat.av_valid[g_hw_stat.av_chid],det2_cnt);

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
                        ret = 1;
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

            int clamp_idx = g_hw_stat.av_valid[g_hw_stat.av_chid];
            if (clamp_idx == 1) {
                clamp_idx = vh_lock ? 2 : 1;
            }
            TP2825_Set_Clamp(clamp_idx);
        }
    }

    pthread_mutex_unlock(&hardware_mutex);
    return ret;
}

void Source_HDMI_in() {
    pthread_mutex_lock(&hardware_mutex);
    OLED_display(0);
    I2C_Write(ADDR_FPGA, 0x8C, 0x00);

    HDZero_Close();
    // OLED_SetTMG(0);

    I2C_Write(ADDR_FPGA, 0x8C, 0x04);
    I2C_Write(ADDR_FPGA, 0x84, 0x00);

    g_hw_stat.source_mode = SOURCE_MODE_HDMIIN;
    Display_VO_SWITCH(1);
    // OLED_display(1);

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

                    OLED_display(0);
                    I2C_Write(ADDR_FPGA, 0x8C, 0x00);

                    switch (vtmg) {
                    case HDMIIN_VTMG_UNKNOW:
                        break;

                    case HDMIIN_VTMG_1080P60:
                        system_exec("dispw -s vdpo 1080p60");
                        dvr_update_vi_conf(VR_1080P60);
                        g_hw_stat.vdpo_tmg = VDPO_TMG_1080P60;
                        vclk_phase_set(VIDEO_SOURCE_HDMI_IN_1080P60, (freq_ref < 63));
                        pclk_phase_set(VIDEO_SOURCE_HDMI_IN_1080P60);
                        I2C_Write(ADDR_FPGA, 0x80, 0x00);

                        OLED_SetTMG(2);

                        I2C_Write(ADDR_FPGA, 0x8C, 0x04);
                        I2C_Write(ADDR_FPGA, 0x06, 0x0F);
                        OLED_display(1);
                        g_hw_stat.hdmiin_vtmg = HDMIIN_VTMG_1080P60;
                        break;

                    case HDMIIN_VTMG_1080P50:
                        system_exec("dispw -s vdpo 1080p50");
                        dvr_update_vi_conf(VR_1080P50);
                        g_hw_stat.vdpo_tmg = VDPO_TMG_1080P50;
                        vclk_phase_set(VIDEO_SOURCE_HDMI_IN_1080P50, (freq_ref < 63));
                        pclk_phase_set(VIDEO_SOURCE_HDMI_IN_1080P50);

                        I2C_Write(ADDR_FPGA, 0x80, 0x20);

                        OLED_SetTMG(0);

                        I2C_Write(ADDR_FPGA, 0x8C, 0x04);
                        I2C_Write(ADDR_FPGA, 0x06, 0x0F);
                        OLED_display(1);
                        g_hw_stat.hdmiin_vtmg = HDMIIN_VTMG_1080P50;
                        break;

                    case HDMIIN_VTMG_1080Pother:
                        system_exec("dispw -s vdpo 1080p50");
                        dvr_update_vi_conf(VR_1080P50);
                        g_hw_stat.vdpo_tmg = VDPO_TMG_1080P50;
                        vclk_phase_set(VIDEO_SOURCE_HDMI_IN_1080POTHER, (freq_ref < 63));
                        pclk_phase_set(VIDEO_SOURCE_HDMI_IN_1080POTHER);

                        I2C_Write(ADDR_FPGA, 0x80, 0x40);

                        OLED_SetTMG(0);

                        I2C_Write(ADDR_FPGA, 0x8C, 0x04);
                        I2C_Write(ADDR_FPGA, 0x06, 0x0F);
                        OLED_display(1);
                        g_hw_stat.hdmiin_vtmg = HDMIIN_VTMG_1080Pother;
                        break;

                    case HDMIIN_VTMG_720P50:
                        system_exec("dispw -s vdpo 720p50");
                        dvr_update_vi_conf(VR_720P50);
                        g_hw_stat.vdpo_tmg = VDPO_TMG_720P50;
                        vclk_phase_set(VIDEO_SOURCE_HDMI_IN_720P50, (freq_ref < 63));
                        pclk_phase_set(VIDEO_SOURCE_HDMI_IN_720P50);

                        I2C_Write(ADDR_FPGA, 0x80, 0x60);

                        OLED_SetTMG(1);

                        I2C_Write(ADDR_FPGA, 0x8C, 0x04);
                        I2C_Write(ADDR_FPGA, 0x06, 0x0F);
                        OLED_display(1);
                        g_hw_stat.hdmiin_vtmg = HDMIIN_VTMG_720P50;
                        break;

                    case HDMIIN_VTMG_720P60:
                        system_exec("dispw -s vdpo 720p60");
                        dvr_update_vi_conf(VR_720P60);
                        g_hw_stat.vdpo_tmg = VDPO_TMG_720P60;
                        vclk_phase_set(VIDEO_SOURCE_HDMI_IN_720P60, (freq_ref < 63));
                        pclk_phase_set(VIDEO_SOURCE_HDMI_IN_720P60);
                        I2C_Write(ADDR_FPGA, 0x80, 0x80);

                        OLED_SetTMG(1);

                        I2C_Write(ADDR_FPGA, 0x8C, 0x04);
                        I2C_Write(ADDR_FPGA, 0x06, 0x0F);
                        OLED_display(1);
                        g_hw_stat.hdmiin_vtmg = HDMIIN_VTMG_720P60;
                        break;

                    case HDMIIN_VTMG_720P100:
                        system_exec("dispw -s vdpo 720p30"); // 100fps actually
                        dvr_update_vi_conf(VR_540P90);
                        g_hw_stat.vdpo_tmg = VDPO_TMG_720P100;
                        vclk_phase_set(VIDEO_SOURCE_HDMI_IN_720P100, (freq_ref < 63));
                        pclk_phase_set(VIDEO_SOURCE_HDMI_IN_720P100);

                        I2C_Write(ADDR_FPGA, 0x80, 0xA0);

                        OLED_SetTMG(1);

                        I2C_Write(ADDR_FPGA, 0x8C, 0x04);
                        I2C_Write(ADDR_FPGA, 0x06, 0x0F);
                        OLED_display(1);
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
            if (g_source_info.source != SOURCE_EXPANSION) {
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

    if (rdat >= 0xE4 && rdat <= 0xE7)
        rdat = 1;
    else
        rdat = 0;

    I2C_Write(ADDR_FPGA, 0x81, 0x00);

    return rdat;
}
