#ifndef _HARDWARE_H
#define _HARDWARE_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    SOURCE_MODE_UI = 0,
    SOURCE_MODE_HDZERO = 1,
    SOURCE_MODE_AV = 2,
    SOURCE_MODE_HDMIIN = 3,
} source_mode_t;

typedef enum {
    HDMIIN_VTMG_UNKNOW = 0,
    HDMIIN_VTMG_1080P = 1,
    HDMIIN_VTMG_720P50 = 2,
    HDMIIN_VTMG_720P60 = 3,
    HDMIIN_VTMG_720P100 = 4,
} hdmiin_vtmg_t;

typedef enum {
    VDPO_TMG_720P50 = 0,
    VDPO_TMG_720P60 = 1,
    VDPO_TMG_720P90 = 2,
    VDPO_TMG_1080P50 = 3,
    VDPO_TMG_720P100 = 4,
    VDPO_TMG_1080P60 = 5,
} vdpo_tmg_t;

typedef struct {
    source_mode_t source_mode;
    vdpo_tmg_t vdpo_tmg;

    // hdzero
    int hdz_bw; // 0=27MHz; 1=17MHz
    int hdzero_open;
    int m0_open;

    // av in
    int av_chid; // 0=AV in; 1=Module bay
    int av_pal[2];
    int av_pal_w;
    int av_valid[2]; // 0=invalid; 1=AV in; 2=Module bay

    // hdmi in
    int hdmiin_valid;
    hdmiin_vtmg_t hdmiin_vtmg;

    int IS_TP2825_L;

} hw_status_t;

extern hw_status_t g_hw_stat;
extern int fhd_req;

void hw_stat_init();

void OLED_ON(int bON);
void HDZero_open(int bw);
void HDZero_Close();

void Source_HDMI_in();
void Source_AV(uint8_t sel); // 0=AV in, 1=AV module
void Display_UI_init();
void Display_UI();
void Display_720P90(int mode);
void Display_720P60_50(int mode, uint8_t is_43);
void Display_1080P30(int mode);

void Display_Osd(bool enable);

void Set_Brightness(uint8_t bri);
void Set_Contrast(uint8_t con);
void Set_Saturation(uint8_t sat);

void Set_HT_status(uint8_t is_open, uint8_t frame_period, uint8_t sync_len);
void Set_HT_dat(uint16_t ch0, uint16_t ch1, uint16_t ch2);

void Analog_Module_Power(bool Force);

int HDZERO_detect();
int AV_in_detect();
void HDMI_in_detect();

int Get_VideoLatancy_status(); // ret: 0=unlocked, 1=locked
int Get_HAN_status();          // ret: 0=error; 1=ok

#endif
