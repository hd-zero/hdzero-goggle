#include "hardware.h"

#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <log/log.h>

#include "i2c.h"
#include "uart.h"
#include "dm6302.h"
#include "oled.h"
#include "TP2825.h"
#include "dm5680.h"
#include "defines.h"
#include "msp_displayport.h"
#include "it66021.h"
#include "../core/osd.h"
#include "../core/common.hh"

/////////////////////////////////////////////////////////////////////////
// global
hw_status_t g_hw_stat;

//local
pthread_mutex_t hardware_mutex;

void hw_stat_init()
{
    g_hw_stat.source_mode = HW_SRC_MODE_UI;
    g_hw_stat.vdpo_tmg = HW_VDPO_1080P50;

    g_hw_stat.hdzero_open = 0;
    g_hw_stat.m0_open = 0;

    g_hw_stat.av_chid = 0;  
    g_hw_stat.av_pal = 0;
    g_hw_stat.av_valid[0] = g_hw_stat.av_valid[1] = 0;

    g_hw_stat.hdmiin_valid = 0;
    g_hw_stat.hdmiin_vtmg = 0;

    pthread_mutex_init(&hardware_mutex, NULL);
}

void Display_VO_SWITCH(uint8_t sel) //0 = UI;  1 = HDZERO or AV_in or HDMI_in
{
    I2C_Write(ADDR_FPGA, 0x20, sel);

    if(sel && g_hw_stat.hdzero_open && (g_hw_stat.source_mode == HW_SRC_MODE_HDZERO))
        DM6302_openM0(1);
    else
        DM6302_openM0(0);

    I2C_Write(ADDR_FPGA, 0x06, 0x0F);
}

void OLED_ON(int bON)
{
    pthread_mutex_lock(&hardware_mutex);
	OLED_display(bON);
	pthread_mutex_unlock(&hardware_mutex);
}

void Display_UI_init()
{
    g_hw_stat.source_mode = HW_SRC_MODE_UI;
    I2C_Write(ADDR_FPGA, 0x8C, 0x00);

    system("dispw -s vdpo 1080p50");
    g_hw_stat.vdpo_tmg = HW_VDPO_1080P50;
    Display_VO_SWITCH(0);
    
    I2C_Write(ADDR_FPGA, 0x8d, 0x14);
    I2C_Write(ADDR_FPGA, 0x8e, 0x80); 
    I2C_Write(ADDR_AL, 0x14, 0x00);
    I2C_Write(ADDR_FPGA, 0x80, 0x00);
    I2C_Write(ADDR_FPGA, 0x84, 0x11);

    OLED_SetTMG(0);
}

void Display_UI()
{
	pthread_mutex_lock(&hardware_mutex);
	OLED_display(0);

    Display_UI_init();

    OLED_display(1);
	pthread_mutex_unlock(&hardware_mutex);
}

void Display_720P60_50_t(int mode,uint8_t is_43) //fps: 0=50, 1=60
{
    OLED_display(0);
    I2C_Write(ADDR_FPGA, 0x8C, 0x00);

    system("dispw -s vdpo 720p60");
    g_hw_stat.vdpo_tmg = HW_VDPO_720P60;
    I2C_Write(ADDR_FPGA, 0x8d, 0x14);
    I2C_Write(ADDR_FPGA, 0x8e, 0x04); 
    I2C_Write(ADDR_AL, 0x14, 0x01);
    I2C_Write(ADDR_FPGA, 0x80, 0x00);

    DM5680_SetFPS(mode);
    MFPGA_Set720P60(mode, is_43);
    OLED_SetTMG(1);

    I2C_Write(ADDR_FPGA, 0x8C, 0x01);

    g_hw_stat.source_mode = HW_SRC_MODE_HDZERO;
    Display_VO_SWITCH(1);
    OLED_display(1);
}

void Display_720P90_t(int mode)
{
    OLED_display(0);
    I2C_Write(ADDR_FPGA, 0x8C, 0x00);

    system("dispw -s vdpo 720p90");
    g_hw_stat.vdpo_tmg = HW_VDPO_720P90;
    I2C_Write(ADDR_FPGA, 0x8d, 0x10);
    I2C_Write(ADDR_FPGA, 0x8e, 0x00); 
    I2C_Write(ADDR_AL, 0x14, 0x01);
    I2C_Write(ADDR_FPGA, 0x80, 0x03);

    DM5680_SetFPS(mode);
    MFPGA_Set720P90(mode);
    OLED_SetTMG(1);

    I2C_Write(ADDR_FPGA, 0x8C, 0x01);

    g_hw_stat.source_mode = HW_SRC_MODE_HDZERO;
    Display_VO_SWITCH(1);
    OLED_display(1);
}

void Display_720P60_50(int mode,uint8_t is_43)
{
    pthread_mutex_lock(&hardware_mutex);
	Display_720P60_50_t(mode, is_43);
    pthread_mutex_unlock(&hardware_mutex);
}

void Display_720P90(int mode)
{
   pthread_mutex_lock(&hardware_mutex);
   Display_720P90_t(mode);
   pthread_mutex_unlock(&hardware_mutex);
}

void HDZero_open()
{
	if(g_hw_stat.hdzero_open == 0){
		DM6302_init(0);
		DM5680_SetBB(1);
		g_hw_stat.hdzero_open = 1;
	}

    LOGI("HDZero: open");
}

void HDZero_Close()
{
    DM5680_SetBB(0);
    DM5680_ResetRF(0);
    g_hw_stat.hdzero_open = 0;
    g_hw_stat.m0_open = 0;

    LOGI("HDZero: close");
}

void AV_Mode_Switch_fpga(int is_pal)
{
    if(is_pal){
        system("dispw -s vdpo 720p50");
        g_hw_stat.vdpo_tmg = HW_VDPO_720P50;
        I2C_Write(ADDR_FPGA, 0x80, 0x10);
    }
    else{
        system("dispw -s vdpo 720p60");
        g_hw_stat.vdpo_tmg = HW_VDPO_720P60;
        I2C_Write(ADDR_FPGA, 0x80, 0x00);
    }
    I2C_Write(ADDR_FPGA, 0x06, 0x0F);
}

void AV_Mode_Switch(int is_pal)
{
    int tmg = is_pal ? HW_VDPO_720P50 : HW_VDPO_720P60;

    if(g_hw_stat.vdpo_tmg != tmg){
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
    
    g_hw_stat.av_chid = sel;
    g_hw_stat.av_pal = 0; //(g_hw_stat.av_valid[sel] == 2) ? 1 : 0;
    TP2825_Config(sel, g_hw_stat.av_pal);
    AV_Mode_Switch_fpga(g_hw_stat.av_pal);

    I2C_Write(ADDR_FPGA, 0x8d, 0x14);
    I2C_Write(ADDR_FPGA, 0x8e, 0x04);
    I2C_Write(ADDR_AL, 0x14, 0x01);

    HDZero_Close();
    OLED_SetTMG(1);

    I2C_Write(ADDR_FPGA, 0x8C, 0x02);
    
    g_hw_stat.source_mode = HW_SRC_MODE_AV;
    Display_VO_SWITCH(1);
    OLED_display(1);

    pthread_mutex_unlock(&hardware_mutex);
}

void Source_HDMI_in()
{
    pthread_mutex_lock(&hardware_mutex);
    OLED_display(0);
    I2C_Write(ADDR_FPGA, 0x8C, 0x00);

    I2C_Write(ADDR_FPGA, 0x8d, 0x10);
    I2C_Write(ADDR_FPGA, 0x8e, 0x00);
    I2C_Write(ADDR_AL, 0x14, 0x01);

    HDZero_Close();
    //OLED_SetTMG(0);

    I2C_Write(ADDR_FPGA, 0x8C, 0x04);
    I2C_Write(ADDR_FPGA, 0x84, 0x00);

    g_hw_stat.source_mode = HW_SRC_MODE_HDMIIN;
    Display_VO_SWITCH(1);
    //OLED_display(1);
    
    pthread_mutex_unlock(&hardware_mutex);
}

int HDZERO_detect() // return = 1: vtmg to V536 changed
{
    static video_resolution_t cam_mode_last = VR_720P60;
    static uint8_t cam_4_3_last=0;
    int ret = 0;

    pthread_mutex_lock(&hardware_mutex);

    if(g_hw_stat.source_mode == HW_SRC_MODE_HDZERO) {

        if(cam_mode_last != CAM_MODE){ //Camera mode changed
            LOGI("CAM_mode: %d->%d", cam_mode_last,CAM_MODE);
            cam_mode_last = CAM_MODE;

            // 1. Change fps
            switch(CAM_MODE){
                case VR_720P50: 
                case VR_720P60: 
                case VR_960x720P60: 
                    Display_720P60_50_t(CAM_MODE, cam_4_3);
                    break;
                case VR_720P30: 
                    perror("cam_mode =2 fix me!!\n ");
                    break;
                case VR_540P90:
                case VR_540P90_CROP:
                    Display_720P90_t(CAM_MODE);
                    break;
                default:
                    LOGW("cam_mode =%d not suppored!!\n ",CAM_MODE);
                    break;
            }

            ret = 1;
        }

        if(cam_4_3_last != cam_4_3) {
            LOGI("cam_4_3: %d  CAM_MODE=%d", cam_4_3, CAM_MODE);
            cam_4_3_last = cam_4_3;

            if(CAM_MODE != VR_720P50)
                MFPGA_SetRatio(cam_4_3);
        }
    }    
    pthread_mutex_unlock(&hardware_mutex);
    return ret;
}

#define AV_DET_CNT1  1
#define AV_DET_CNT2  3
int AV_in_detect() // return = 1: vtmg to V536 changed
{
    static int det_last = -1, det2_last = -1;
    static int det_cnt = 0, det2_cnt = 0;
    int det, det2;
    int ret = 0;

    pthread_mutex_lock(&hardware_mutex);

    det = I2C_Read(ADDR_TP2825, 0x01);

    if(g_hw_stat.source_mode == HW_SRC_MODE_UI) { //detect in UI mode
        TP2825_Set_Clamp(0);

        det = ((det & 0x80) == 0x80) ? 0 : 1;
        if(det_last != det) {
            det_last = det;
        }
        else {
            g_hw_stat.av_valid[g_hw_stat.av_chid] = det;
            g_hw_stat.av_chid = g_hw_stat.av_chid ? 0 : 1;
            TP2825_Switch_CH(g_hw_stat.av_chid);
            det_last = -1;
        }

        det_cnt = det2_cnt = 0;
    }
    else if(g_hw_stat.source_mode == HW_SRC_MODE_AV) { //detect in AV_in/Module_bay mode
        det &= 0xAF;
        det2 = (g_hw_stat.av_pal == 1 && det == 0x2C) ? 2 :
               (g_hw_stat.av_pal == 0 && det == 0x28) ? 1 : 0;
        det  = (det == (g_hw_stat.av_pal ? 0x28 : 0x2C)) ? 1 : 0;

        if(det_last != det) {
            det_last = det;
            det_cnt = 0;
        }
        else if(det_cnt < AV_DET_CNT1) {
            det_cnt++;
        }

        if(det2_last != det2) {
            det2_last = det2;
            det2_cnt = 0;
        }
        else if(det2_cnt < AV_DET_CNT2) {
            det2_cnt++;
        }

        if(g_hw_stat.av_valid[g_hw_stat.av_chid]) {
            if(det2 == 0 && det2_cnt == AV_DET_CNT2) {
                TP2825_Set_Clamp(0);
                g_hw_stat.av_valid[g_hw_stat.av_chid] = 0;
            }
        }
        else {
            if(det && det_cnt == AV_DET_CNT1) {
                g_hw_stat.av_pal = g_hw_stat.av_pal ? 0 : 1;
                TP2825_Config(g_hw_stat.av_chid, g_hw_stat.av_pal);
                AV_Mode_Switch(g_hw_stat.av_pal);
            }

            if(det2_cnt == AV_DET_CNT2) {
                if(det2){
                    TP2825_Set_Clamp(1);
                    g_hw_stat.av_valid[g_hw_stat.av_chid] = det2;
                    ret = 1;
                }
                else
                    TP2825_Set_Clamp(0);
            }
        }
    }

    pthread_mutex_unlock(&hardware_mutex);
    return ret;
}

void HDMI_in_detect()
{
    static int vtmg_last = -1;
    static int cs_last = -1;
    static int last_vld = 0;
    int vtmg, cs;

    pthread_mutex_lock(&hardware_mutex);

    if((g_hw_stat.source_mode == HW_SRC_MODE_UI) || (g_hw_stat.source_mode == HW_SRC_MODE_HDMIIN)) {

        last_vld = g_hw_stat.hdmiin_valid;
        g_hw_stat.hdmiin_valid = IT66021_Sig_det();

        if(g_hw_stat.source_mode == HW_SRC_MODE_HDMIIN){
            if(g_hw_stat.hdmiin_valid){
                vtmg = IT66021_Get_VTMG();
                if(vtmg_last != vtmg){
                    vtmg_last = vtmg;
                    LOGI("IT66021: VTMG change: %d", vtmg);

                    OLED_display(0);
                    I2C_Write(ADDR_FPGA, 0x8C, 0x00);

                    if(vtmg == 1){
                        system("dispw -s vdpo 1080p50");
                        g_hw_stat.vdpo_tmg = HW_VDPO_1080P50;
                        //I2C_Write(ADDR_FPGA, 0x8d, 0x10);
                        I2C_Write(ADDR_FPGA, 0x8e, 0x80);
                        I2C_Write(ADDR_AL, 0x14, 0x00);
                        I2C_Write(ADDR_FPGA, 0x80, 0x00);
                        
                        OLED_SetTMG(0);

                        I2C_Write(ADDR_FPGA, 0x8C, 0x04);
                        OLED_display(1);
                        g_hw_stat.hdmiin_vtmg = 1;
                    }
                    else if(vtmg == 2){
                        system("dispw -s vdpo 720p30"); // 100fps actually
                        g_hw_stat.vdpo_tmg = HW_VDPO_720P100;
                        //I2C_Write(ADDR_FPGA, 0x8d, 0x04);
                        I2C_Write(ADDR_FPGA, 0x8e, 0x80);
                        I2C_Write(ADDR_AL, 0x14, 0x00);
                        I2C_Write(ADDR_FPGA, 0x80, 0x80);
                        
                        OLED_SetTMG(1);

                        I2C_Write(ADDR_FPGA, 0x8C, 0x04);
                        OLED_display(1);
                        g_hw_stat.hdmiin_vtmg = 2;
                    }
                }

                cs = IT66021_Get_PCLKFREQ();
                if(cs > 0) {
                    if(cs < 63)
                        I2C_Write(ADDR_FPGA, 0x8d, 0x10);
                    else
                        I2C_Write(ADDR_FPGA, 0x8d, 0x04);
                }

                cs = IT66021_Get_CS();
                if(cs_last != cs) {
                    cs_last = cs;
                    LOGI("IT66021: Color space change: %d\n", cs);

                    IT66021_Set_CSMatrix(cs);
                }
            }
            else {
                if(last_vld)
                    I2C_Write(ADDR_FPGA, 0x06, 0x0F);

                vtmg_last = -1;
                cs_last = -1;
            }
        }
        else{
            vtmg_last = -1;
            cs_last = -1;
        }
    }
    pthread_mutex_unlock(&hardware_mutex);
}

void Display_Osd(bool enable)
{
    I2C_Write(ADDR_FPGA, 0x84, enable? 0x11 : 0x01);
}


void Set_Brightness(uint8_t bri)
{
    int8_t val = 0x80+bri-39;
    I2C_Write(ADDR_FPGA, 0x85, val);
}

void Set_Contrast(uint8_t con)
{
    int8_t val = con<<2;
    I2C_Write(ADDR_FPGA, 0x86, val);
}

void Set_Saturation(uint8_t sat)
{
    int8_t val = sat<<2;
    I2C_Write(ADDR_FPGA, 0x87, val);
}

void Set_HT_status(uint8_t is_open, uint8_t frame_period, uint8_t sync_len) // frame_period unit = 2ms
{
    I2C_Write(ADDR_FPGA, 0x71, sync_len);
    I2C_Write(ADDR_FPGA, 0x70, (frame_period << 4) | is_open);
}

void Set_HT_dat(uint16_t ch0, uint16_t ch1, uint16_t ch2)
{
    I2C_Write(ADDR_FPGA, 0x72, ch0 & 0xff);
    I2C_Write(ADDR_FPGA, 0x73, (ch0 >> 8) & 0xff);
    I2C_Write(ADDR_FPGA, 0x74, ch1 & 0xff);
    I2C_Write(ADDR_FPGA, 0x75, (ch1 >> 8) & 0xff);
    I2C_Write(ADDR_FPGA, 0x76, ch2 & 0xff);
    I2C_Write(ADDR_FPGA, 0x77, (ch2 >> 8) & 0xff);
}

int Get_VideoLatancy_status() // ret: 0=unlocked, 1=locked
{
    uint8_t rdat = 0;
    
    if(g_hw_stat.source_mode == HW_SRC_MODE_AV || g_hw_stat.source_mode == HW_SRC_MODE_HDZERO) {
        rdat = I2C_Read(ADDR_FPGA, 0x11);
        rdat = (rdat < 0x04) ? 1 : 0;
    }
    
    return rdat;
}

int Get_HAN_status() //ret: 0=error; 1=ok
{
    uint8_t rdat;

    I2C_Write(ADDR_FPGA, 0x81, 0x01);
    sleep(1);

    rdat = I2C_Read(ADDR_FPGA, 0x19);

    if(rdat > 0x9C && rdat < 0xA3)
        rdat =  1;
    else
        rdat =  0;

    I2C_Write(ADDR_FPGA, 0x81, 0x00);

    return rdat;
}