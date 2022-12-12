#ifndef _ACCEL_GYRO_H
#define _ACCEL_GYRO_H
#include "bmi2_defs.h"

void init_bmi270();

void enable_bmi270();

void disable_bmi270();

void get_bmi270(struct bmi2_sens_data* sensor_data);

float acc_to_mps2(int16_t acc);

float acc_to_g(int16_t val);

float gyr_to_dps(int16_t gyr);

#endif //_ACCEL_GYRO_H