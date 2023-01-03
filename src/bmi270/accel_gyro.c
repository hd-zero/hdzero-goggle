/**\
 * Copyright (c) 2021 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

/******************************************************************************/
/*!                 Header Files                                              */
#include <stdio.h>
#include <log/log.h>
#include "bmi270.h"
#include "common.h"
/******************************************************************************/
/*!                Macro definition                                           */

/*! Earth's gravity in m/s^2 */
#define GRAVITY_EARTH  (9.80665f)

/*! Macros to select the sensors                   */
#define ACCEL          UINT8_C(0x00)
#define GYRO           UINT8_C(0x01)

/******************************************************************************/
/*!           Static Function Declaration                                     */

/*!
 *  @brief This internal API is used to set configurations for accel.
 *
 *  @param[in] dev       : Structure instance of bmi2_dev.
 *
 *  @return Status of execution.
 */
static int8_t set_accel_gyro_config(struct bmi2_dev *bmi2_dev);

/*!
 *  @brief This function converts lsb to meter per second squared for 16 bit accelerometer at
 *  range 2G, 4G, 8G or 16G.
 *
 *  @param[in] val       : LSB from each axis.
 *  @param[in] g_range   : Gravity range.
 *  @param[in] bit_width : Resolution for accel.
 *
 *  @return Gravity.
 */
static float lsb_to_mps2(int16_t val, float g_range, uint8_t bit_width);

/*!
 *  @brief This function converts lsb to degree per second for 16 bit gyro at
 *  range 125, 250, 500, 1000 or 2000dps.
 *
 *  @param[in] val       : LSB from each axis.
 *  @param[in] dps       : Degree per second.
 *  @param[in] bit_width : Resolution for gyro.
 *
 *  @return Degree per second.
 */
static float lsb_to_dps(int16_t val, float dps, uint8_t bit_width);

/////////////////////////////////////////////////
//local: bmi2_dev 
static struct bmi2_dev bmi2_dev;

void init_bmi270()
{
	    /* Status of api are returned to this variable. */
    int8_t rslt;

    rslt = bmi2_interface_init(&bmi2_dev, BMI2_I2C_INTF);
    bmi2_error_codes_print_result(rslt);

    /* Initialize bmi270. */
    rslt = bmi270_init(&bmi2_dev);
    bmi2_error_codes_print_result(rslt);
    LOGI("init_bmi270 done.");
}

void enable_bmi270()
{
    int8_t rslt;
    uint8_t sensor_list[2] = { BMI2_ACCEL, BMI2_GYRO };

    init_bmi270();
    rslt = set_accel_gyro_config(&bmi2_dev);
    bmi2_error_codes_print_result(rslt);
    if (rslt == BMI2_OK) {
        rslt = bmi270_sensor_enable(sensor_list, 2, &bmi2_dev);
        bmi2_error_codes_print_result(rslt);
        LOGI("[Pass] BMI270 enabled.");
    }
    else
        LOGE("[Error] BMI270 failed.");
}

void disable_bmi270()
{
    int8_t rslt;
    uint8_t sensor_list[2] = { BMI2_ACCEL, BMI2_GYRO };
    rslt = bmi270_sensor_disable(sensor_list, 2, &bmi2_dev);
    bmi2_error_codes_print_result(rslt);
    LOGI("disable_bmi270 done.");
}

float acc_to_mps2(int16_t acc)
{
    return lsb_to_mps2(acc, 2, bmi2_dev.resolution);
}

float acc_to_g(int16_t val)
{
	float half_scale = ((float)(1 << 16) / 2.0f);

	return (val * 2) / half_scale;
}

float gyr_to_dps(int16_t gyr)
{
    return lsb_to_dps(gyr, 2000, bmi2_dev.resolution);
}

void get_bmi270(struct bmi2_sens_data* sensor_data)
{
    int8_t rslt,ready=0;
    uint16_t int_status = 0;

	while(!ready){
	    /* To get the data ready interrupt status of accel and gyro. */
	    rslt = bmi2_get_int_status(&int_status, &bmi2_dev);
	    bmi2_error_codes_print_result(rslt);
	
	    /* To check the data ready interrupt status and print the status for 10 samples. */
	    if ((int_status & BMI2_ACC_DRDY_INT_MASK) && (int_status & BMI2_GYR_DRDY_INT_MASK)) {
	        /* Get accel and gyro data for x, y and z axis. */
	        rslt = bmi2_get_sensor_data(sensor_data, &bmi2_dev);
	        bmi2_error_codes_print_result(rslt);
	#if 0
            float x[2],y[2],z[2];
	        //LOGI("*******  Accel(Raw and m/s2) Gyro(Raw and dps) data : *******");
		        //LOGI("Acc_x = %d\t", sensor_data->acc.x);
	        //LOGI("Acc_y = %d\t", sensor_data->acc.y);
	        //LOGI("Acc_z = %d", sensor_data->acc.z);
	
	        /* Converting lsb to meter per second squared for 16 bit accelerometer at 2G range. */
	        x[0] = lsb_to_mps2(sensor_data->acc.x, 2, bmi2_dev.resolution);
	        y[0] = lsb_to_mps2(sensor_data->acc.y, 2, bmi2_dev.resolution);
	        z[0] = lsb_to_mps2(sensor_data->acc.z, 2, bmi2_dev.resolution);
	
	        /* Print the data in m/s2. */
	        //LOGI("Acc_ms2_X = %4.2f, Acc_ms2_Y = %4.2f, Acc_ms2_Z = %4.2f", x, y, z);
	
	        //LOGI("Gyr_X = %d\t", sensor_data->gyr.x);
	        //LOGI("Gyr_Y = %d\t", sensor_data->gyr.y);
	        //LOGI("Gyr_Z= %d", sensor_data->gyr.z);
	
	        /* Converting lsb to degree per second for 16 bit gyro at 2000dps range. */
	        x[1] = lsb_to_dps(sensor_data->gyr.x, 2000, bmi2_dev.resolution);
	        y[1] = lsb_to_dps(sensor_data->gyr.y, 2000, bmi2_dev.resolution);
	        z[1] = lsb_to_dps(sensor_data->gyr.z, 2000, bmi2_dev.resolution);
	
	        /* Print the data in dps. */
            //LOGI("Acc:%4.2f, %4.2f, %4.2f, Gyr: %4.2f, %4.2f, %4.2f ", x[0], y[0], z[0],x[1], y[1], z[1]); 
            LOGI("%d, %d, %d, %d, %d, %d ", sensor_data->acc.x,sensor_data->acc.y, sensor_data->acc.z,\     //For pc simulation
                                                sensor_data->gyr.x,sensor_data->gyr.y,sensor_data->gyr.z);
    #endif                
	        ready = 1;
		}
	}
}

/*!
 * @brief This internal API is used to set configurations for accel and gyro.
 */
static int8_t set_accel_gyro_config(struct bmi2_dev *bmi2_dev)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Structure to define accelerometer and gyro configuration. */
    struct bmi2_sens_config config[2];

    /* Configure the type of feature. */
    config[ACCEL].type = BMI2_ACCEL;
    config[GYRO].type = BMI2_GYRO;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi270_get_sensor_config(config, 2, bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    /* Map data ready interrupt to interrupt pin. */
    rslt = bmi2_map_data_int(BMI2_DRDY_INT, BMI2_INT1, bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* NOTE: The user can change the following configuration parameters according to their requirement. */
        /* Set Output Data Rate */
        config[ACCEL].cfg.acc.odr = BMI2_ACC_ODR_200HZ;

        /* Gravity range of the sensor (+/- 2G, 4G, 8G, 16G). */
        config[ACCEL].cfg.acc.range = BMI2_ACC_RANGE_2G;

        /* The bandwidth parameter is used to configure the number of sensor samples that are averaged
         * if it is set to 2, then 2^(bandwidth parameter) samples
         * are averaged, resulting in 4 averaged samples.
         * Note1 : For more information, refer the datasheet.
         * Note2 : A higher number of averaged samples will result in a lower noise level of the signal, but
         * this has an adverse effect on the power consumed.
         */
        config[ACCEL].cfg.acc.bwp = BMI2_ACC_NORMAL_AVG4;

        /* Enable the filter performance mode where averaging of samples
         * will be done based on above set bandwidth and ODR.
         * There are two modes
         *  0 -> Ultra low power mode
         *  1 -> High performance mode(Default)
         * For more info refer datasheet.
         */
        config[ACCEL].cfg.acc.filter_perf = BMI2_PERF_OPT_MODE;

        /* The user can change the following configuration parameters according to their requirement. */
        /* Set Output Data Rate */
        config[GYRO].cfg.gyr.odr = BMI2_GYR_ODR_200HZ;

        /* Gyroscope Angular Rate Measurement Range.By default the range is 2000dps. */
        config[GYRO].cfg.gyr.range = BMI2_GYR_RANGE_2000;

        /* Gyroscope bandwidth parameters. By default the gyro bandwidth is in normal mode. */
        config[GYRO].cfg.gyr.bwp = BMI2_GYR_NORMAL_MODE;

        /* Enable/Disable the noise performance mode for precision yaw rate sensing
         * There are two modes
         *  0 -> Ultra low power mode(Default)
         *  1 -> High performance mode
         */
        config[GYRO].cfg.gyr.noise_perf = BMI2_POWER_OPT_MODE;

        /* Enable/Disable the filter performance mode where averaging of samples
         * will be done based on above set bandwidth and ODR.
         * There are two modes
         *  0 -> Ultra low power mode
         *  1 -> High performance mode(Default)
         */
        config[GYRO].cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;

        /* Set the accel and gyro configurations. */
        rslt = bmi270_set_sensor_config(config, 2, bmi2_dev);
        bmi2_error_codes_print_result(rslt);
    }

    return rslt;
}

/*!
 * @brief This function converts lsb to meter per second squared for 16 bit accelerometer at
 * range 2G, 4G, 8G or 16G.
 */
static float lsb_to_mps2(int16_t val, float g_range, uint8_t bit_width)
{
    float half_scale = ((float)(1 << bit_width) / 2.0f);

    return (GRAVITY_EARTH * val * g_range) / half_scale;
}

/*!
 * @brief This function converts lsb to degree per second for 16 bit gyro at
 * range 125, 250, 500, 1000 or 2000dps.
 */
static float lsb_to_dps(int16_t val, float dps, uint8_t bit_width)
{
    float half_scale = ((float)(1 << bit_width) / 2.0f);

    return (dps / ((half_scale))) * (val);
}
