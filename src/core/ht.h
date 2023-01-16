#ifndef __HT_C__
#define __HT_C__

#define DEG_TO_RAD 0.017453295199
#define RAD_TO_DEG 57.29577951308

#define CALIBRATION_BCNT   8  //calibartion loop cnt = (1<<CALIBRATION_BCNT)
#define gyroWeightTiltRoll 0.98

#define MOVTION_GYRO_THR    3000
#define MOVTION_DUR_1MINUTE 60

#include "../bmi270/bmi2_defs.h"

typedef struct {
	struct bmi2_sens_data sensor_data;

    //offset
    int32_t acc_offset[3];
    int32_t gyr_offset[3];

    // Final angles for headtracker
    float tiltAngle;
    float tiltAngleHome;

    float rollAngle;
    float rollAngleHome;

    float panAngle;
    float panAngleHome;

    // Servo settings
    int8_t tiltInverse; // -1= inverted
    int8_t rollInverse;
    int8_t panInverse;

    float   tiltFactor; //Gain
    float   rollFactor;
    float   panFactor;

    //PPM setting
    int16_t tiltMaxPulse;
    int16_t tiltMinPulse;
    int16_t tiltCenter;
    int16_t panMaxPulse;
    int16_t panMinPulse;
    int16_t panCenter;
    int16_t rollMaxPulse;
    int16_t rollMinPulse;
    int16_t rollCenter;

    int16_t htChannels[3]; //0=Pan, 1=tilt, 2=roll
    
    // internal state
    uint8_t enable;
	
} ht_data_t;


void init_ht();
void enable_ht();
void disable_ht();
void calibrate_ht();
void set_center_position_ht();
int calc_ht();
void get_imu_data(int bCalcDiff);
int16_t* get_ht_channels();


#endif //__HT_C__