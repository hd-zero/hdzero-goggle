#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <unistd.h>

#include <log/log.h>

#include "common.hh"
#include "ht.h"
#include "osd.h"
#include "MadgwickAHRS.h"
#include "../bmi270/accel_gyro.h"
#include "../driver/hardware.h"
#include "../driver/dm6302.h"
#include "../driver/oled.h"
#include "ui/page_common.h"
#include "ui/page_scannow.h"


//#define FAST_SIM
///////////////////////////////////////////////////////////////////////////////
// local
static ht_data_t ht_data;
static uint8_t frame_period = 10;
static uint8_t sync_len = 200;


///////////////////////////////////////////////////////////////////////////////
//no motion to disable OLED display
void detect_motion(int is_moving)
{
    static uint8_t state = 0;  //0: detecting motion, 1=oled pre off mode, 2= oled off mode
	static int cnt = 0;

    if(state == 0) { //in moving 
        if(is_moving) 
            cnt = 0;
        else {
            cnt++;
            if(g_setting.image.auto_off != 3) {
            #ifdef FAST_SIM
                if(cnt > (MOVTION_DUR_1MINUTE*(g_setting.image.auto_off+1))) { 
            #else    
                if(cnt > (MOVTION_DUR_1MINUTE*(g_setting.image.auto_off*2+3))) {
            #endif        
                    state = 1;
                    cnt = 0;
                    LOGI("OLED pre-OFF for protection.");
                    OLED_Brightness(0);
                }
            }
        #ifdef FAST_SIM
            LOGI("IDLE %d",cnt);
        #endif
        }    
    }
    else if(state == 1) { //pre -off
    #ifdef FAST_SIM    
        LOGI("PRE OFF %d",cnt);
    #endif    
        if(is_moving) {
            state = 0;
            cnt = 0;
            OLED_Brightness(g_setting.image.oled);
        }
        else {
            cnt++;
            if(cnt == MOVTION_DUR_1MINUTE) { // 1-min
                LOGI("OLED OFF for protection.");
                beep(); 
                
                OLED_ON(0); //Turn off OLED

                if(g_hw_stat.source_mode ==1) 
                    HDZero_Close(); //Turn off RF

                state = 2;
                cnt = 0;
            }
        }
    }
    else { // in stationery 
    #ifdef FAST_SIM
        LOGI("OFF %d",cnt);
    #endif    
        if(is_moving) {
            cnt++;
            if(cnt == 2) {
                state = 0;
                cnt = 0;
                if(g_hw_stat.source_mode ==1) {
                    HDZero_open();
                    uint8_t ch = g_setting.scan.channel - 1;
	                DM6302_SetChannel(ch);
                }
                LOGI("OLED ON from protection.");
                OLED_Brightness(g_setting.image.oled);
                OLED_ON(1); 
            }
        }
        else {
            if(cnt) cnt = 0;
        }
    }
}

void get_imu_data(int bCalcDiff)
{
    static int dec_cnt;
    static struct bmi2_sens_axes_data gyr_last;
    int16_t  dx,dy,dz;
    uint32_t diff;
    int  is_moving;
    
    dec_cnt++;
    if(dec_cnt != 10) return;  //calibrate dec_cnt to make sure the following code runs at 1Hz
    dec_cnt = 0;

    get_bmi270(&ht_data.sensor_data);

    if(bCalcDiff) {
        dx = ht_data.sensor_data.gyr.x - gyr_last.x;
        dy = ht_data.sensor_data.gyr.y - gyr_last.y;
        dz = ht_data.sensor_data.gyr.z - gyr_last.z;
        diff = dx*dx+dy*dy+dz*dz;
        is_moving = (diff > MOVTION_GYRO_THR) | g_key;
        
        g_key = 0;
        gyr_last = ht_data.sensor_data.gyr;    
        
        //if(is_moving)
        //    LOGI("IMU: %d",diff);
       
        detect_motion(is_moving);
    }
}


/////////////////////////////////////////////////////////////////////////////////
//HT function
void init_ht()
{
    ht_data.tiltAngle = 0;      
    ht_data.tiltAngleLP = 0;    

    ht_data.rollAngle = 0;       
    ht_data.rollAngleLP = 0;    

    ht_data.panAngle = 0;       
    ht_data.panAngleLP = 0;     

    ht_data.tiltRollBeta = 0.75;
    ht_data.panBeta = 0.75; 
    
    ht_data.tiltInverse = -1; 
    ht_data.rollInverse = -1; 
    ht_data.panInverse = -1; 

    ht_data.tiltFactor = 17;
    ht_data.rollFactor = 17;
    ht_data.panFactor = 17;
    
    ht_data.tiltMaxPulse = 500;
    ht_data.tiltMinPulse = -500; 
    ht_data.tiltCenter = 1500; 
    ht_data.panMaxPulse = 500;
    ht_data.panMinPulse = -500; 
    ht_data.panCenter = 1500; 
    ht_data.rollMaxPulse = 500;
    ht_data.rollMinPulse = -500; 
    ht_data.rollCenter = 1500; 

    ht_data.htChannels[0] = 0;
    ht_data.htChannels[1] = 0;
    ht_data.htChannels[2] = 0;
    
    ht_data.is_calibrated = 0;
    ht_data.enable = 0;
}

static void calc_gyr(float* gyrAngle) //in degree
{
    gyrAngle[0] = gyr_to_dps(ht_data.sensor_data.gyr.x - ht_data.gyr_offset[0]);
    gyrAngle[1] = gyr_to_dps(ht_data.sensor_data.gyr.y - ht_data.gyr_offset[1]);
    gyrAngle[2] = gyr_to_dps(ht_data.sensor_data.gyr.z - ht_data.gyr_offset[2]);
}

static void calc_acc(float* accAngle) //in G
{
    accAngle[0] = acc_to_g(ht_data.sensor_data.acc.x - ht_data.acc_offset[0]); 

    accAngle[1] = acc_to_g(ht_data.sensor_data.acc.y - ht_data.acc_offset[1]);

    accAngle[2] = acc_to_g(ht_data.sensor_data.acc.z - ht_data.acc_offset[2]);
}

static void iir_filter(float beta,float in, float* out)
{
    *out = in*beta + (*out)*(1 - beta);
}

// Normalizes any number to an arbitrary range
// by assuming the range wraps around when going below min or above max
float normalize(float value, float start, float end)
{
  float width = end - start;          //
  float offsetValue = value - start;  // value relative to 0

  return (offsetValue - (floor(offsetValue / width) * width)) + start;
  // + start to reset back to start of original range
}

// Rotate, in Order X -> Y -> Z
void rotate(float pn[3], const float rotation[3])
{
  float rot[3] = {0, 0, 0};
  memcpy(rot, rotation, sizeof(rot[0]) * 3);

  // Passed in Degrees
  rot[0] *= DEG_TO_RAD;
  rot[1] *= DEG_TO_RAD;
  rot[2] *= DEG_TO_RAD;

  float out[3];

  // X Rotation
  if (rotation[0] != 0) {
    out[0] = pn[0] * 1 + pn[1] * 0 + pn[2] * 0;
    out[1] = pn[0] * 0 + pn[1] * cos(rot[0]) - pn[2] * sin(rot[0]);
    out[2] = pn[0] * 0 + pn[1] * sin(rot[0]) + pn[2] * cos(rot[0]);
    memcpy(pn, out, sizeof(out[0]) * 3);
  }

  // Y Rotation
  if (rotation[1] != 0) {
    out[0] = pn[0] * cos(rot[1]) - pn[1] * 0 + pn[2] * sin(rot[1]);
    out[1] = pn[0] * 0 + pn[1] * 1 + pn[2] * 0;
    out[2] = -pn[0] * sin(rot[1]) + pn[1] * 0 + pn[2] * cos(rot[1]);
    memcpy(pn, out, sizeof(out[0]) * 3);
  }

  // Z Rotation
  if (rotation[2] != 0) {
    out[0] = pn[0] * cos(rot[2]) - pn[1] * sin(rot[2]) + pn[2] * 0;
    out[1] = pn[0] * sin(rot[2]) + pn[1] * cos(rot[2]) + pn[2] * 0;
    out[2] = pn[0] * 0 + pn[1] * 0 + pn[2] * 1;
    memcpy(pn, out, sizeof(out[0]) * 3);
  }
}

void calibrate_ht()
{
    uint16_t i;
    float accAngle[3];

    LOGI("HT calibration...");
    ht_data.acc_offset[0] = ht_data.acc_offset[1] = ht_data.acc_offset[2] = 0;
    ht_data.gyr_offset[0] = ht_data.gyr_offset[1] = ht_data.gyr_offset[2] = 0;

    for (i = 0; i < (1<<CALIBRATION_BCNT); i++) {
        get_imu_data(false); usleep(100000); //0.1s
        ht_data.acc_offset[0] += ht_data.sensor_data.acc.x;
        ht_data.acc_offset[1] += ht_data.sensor_data.acc.y;
        ht_data.acc_offset[2] += ht_data.sensor_data.acc.z;
        ht_data.gyr_offset[0] += ht_data.sensor_data.gyr.x;
        ht_data.gyr_offset[1] += ht_data.sensor_data.gyr.y;
        ht_data.gyr_offset[2] += ht_data.sensor_data.gyr.z;
    }
    ht_data.acc_offset[0] >>= CALIBRATION_BCNT;
    ht_data.acc_offset[1] >>= CALIBRATION_BCNT;
    ht_data.acc_offset[2] >>= CALIBRATION_BCNT;
    ht_data.gyr_offset[0] >>= CALIBRATION_BCNT;
    ht_data.gyr_offset[1] >>= CALIBRATION_BCNT;
    ht_data.gyr_offset[2] >>= CALIBRATION_BCNT;

    LOGI("done!");
}

int calc_ht()
{
    float gyrAngle[3],accAngle[3],tmp;

    if(!ht_data.enable) return 0;
/*
    calc_gyr(gyrAngle);
    calc_acc(accAngle);

	MadgwickAHRSupdate(gyrAngle[0] * DEG_TO_RAD, gyrAngle[1] * DEG_TO_RAD, gyrAngle[2] * DEG_TO_RAD, \
					   accAngle[0],              accAngle[1],              accAngle[2], \
						0,0,0);
	
	ht_data.rollAngle = getPitch(); //getRoll();
	ht_data.tiltAngle = getRoll();  //getPitch();
	ht_data.panAngle = getYaw();

    iir_filter(ht_data.tiltRollBeta,ht_data.tiltAngle, &ht_data.tiltAngleLP);
    iir_filter(ht_data.tiltRollBeta,ht_data.rollAngle, &ht_data.rollAngleLP);
    iir_filter(ht_data.panBeta, normalize(ht_data.panAngle, -180,180), &ht_data.panAngleLP);

	LOGI("PTR=%.2f,%.2f,%.2f", ht_data.panAngleLP, ht_data.tiltAngleLP, ht_data.rollAngleLP);


    tmp = ht_data.panAngleLP * ht_data.panInverse * ht_data.panFactor;
    if ((tmp > -ht_data.panMinPulse) && (tmp < ht_data.panMaxPulse)) {
        tmp += ht_data.panCenter;
        ht_data.htChannels[0] = (int16_t)tmp;
    }    

    tmp = ht_data.tiltAngleLP * ht_data.tiltInverse * ht_data.tiltFactor;
    if((tmp > ht_data.tiltMinPulse) && (tmp < ht_data.tiltMaxPulse)) {
        tmp += ht_data.tiltCenter;
        ht_data.htChannels[1] = (int16_t)tmp;
    }

    tmp = ht_data.rollAngleLP * ht_data.rollInverse * ht_data.rollFactor;
    if((tmp > ht_data.rollMinPulse) && (tmp < ht_data.rollMaxPulse)) {
        tmp += ht_data.rollCenter;
        ht_data.htChannels[2] = (int16_t)tmp;
    }
*/
    //Set_HT_dat(1500, 1500, 1500);
	return 1;
}

void enable_ht()
{
    ht_data.enable = 1;
    Set_HT_status(ht_data.enable, frame_period, sync_len);
}

void disable_ht()
{
    ht_data.enable = 0;
    Set_HT_status(ht_data.enable, frame_period, sync_len);
}
