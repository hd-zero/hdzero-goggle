#ifndef _HARDWARE_H
#define _HARDWARE_H

#include <stdint.h>
#include <stdbool.h>

#define HW_SRC_MODE_UI      0
#define HW_SRC_MODE_HDZERO  1
#define HW_SRC_MODE_AV      2
#define HW_SRC_MODE_HDMIIN  3

#define HW_VDPO_720P50      0
#define HW_VDPO_720P60      1
#define HW_VDPO_720P90      2
#define HW_VDPO_1080P50     3
#define HW_VDPO_720P100     4

typedef struct{
    int source_mode; // 0=UI; 1=HDZERO; 2=AV_in/Module_bay; 3=HDMI in
    int vdpo_tmg;    // 0=720p50; 1=720P60; 2=720P90; 3=1080P50;

    // hdzero
    int hdzero_open;
    int m0_open;
    
    // av in
    int av_chid;     // 0=AV in; 1=Module bay
    int av_pal;
    int av_valid[2]; // 0=invalid; 1=AV in; 2=Module bay

    // hdmi in
    int hdmiin_valid;
    int hdmiin_vtmg; //1=1080p,2=720p

} hw_status_t;

extern hw_status_t g_hw_stat;

void hw_stat_init();

void OLED_ON(int bON);
void HDZero_open();
void HDZero_Close();
void Source_HDMI_in();
void Source_AV(uint8_t sel); // 0=AV in, 1=AV module
void Display_UI_init();
void Display_UI();
void Display_720P90(int mode);
void Display_720P60_50(int mode,uint8_t is_43);

void Display_Osd(bool enable);

void Set_Brightness(uint8_t bri);
void Set_Contrast(uint8_t con);
void Set_Saturation(uint8_t sat);

void Set_HT_status(uint8_t is_open, uint8_t frame_period, uint8_t sync_len);
void Set_HT_dat(uint16_t ch0, uint16_t ch1, uint16_t ch2);

int HDZERO_detect();
int AV_in_detect();
void HDMI_in_detect();

int Get_VideoLatancy_status();  // ret: 0=unlocked, 1=locked
int Get_HAN_status(); //ret: 0=error; 1=ok

#endif
