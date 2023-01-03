
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <log/log.h>

#include "common.h"
#include "bmi2_defs.h"
#include "i2c.h"
/******************************************************************************/
/*!                 Macro definitions                                         */
#define BMI2XY_SHUTTLE_ID  UINT16_C(0x1B8)

/*! Macro that defines read write length */
#define READ_WRITE_LEN     UINT8_C(46)

/******************************************************************************/
/*!                Static variable definition                                 */

/*! Variable that holds the I2C device address or SPI chip selection */
static uint8_t dev_addr;

/******************************************************************************/
/*!                User interface functions                                   */

/*!
 * I2C read function map to COINES platform
 */
BMI2_INTF_RETURN_TYPE bmi2_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    //uint8_t dev_addr = *(uint8_t*)intf_ptr;
    
    return BMI_I2C_READ(reg_addr,reg_data,(uint16_t)len);

}



/*!
 * I2C write function map to COINES platform
 */
BMI2_INTF_RETURN_TYPE bmi2_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    //uint8_t dev_addr = *(uint8_t*)intf_ptr;
    return BMI_I2C_WRITE(reg_addr, (uint8_t *)reg_data, (uint16_t)len);
}

/*!
 * SPI read function map to COINES platform
 */
BMI2_INTF_RETURN_TYPE bmi2_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    /*
    uint8_t dev_addr = *(uint8_t*)intf_ptr;
    return coines_read_spi(dev_addr, reg_addr, reg_data, (uint16_t)len);
    */
   return -1; 
}

/*!
 * SPI write function map to COINES platform
 */
BMI2_INTF_RETURN_TYPE bmi2_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    /*
    uint8_t dev_addr = *(uint8_t*)intf_ptr;
    return coines_write_spi(dev_addr, reg_addr, (uint8_t *)reg_data, (uint16_t)len);
    */
   return -1;
}

/*!
 * Delay function map to COINES platform
 */
void bmi2_delay_us(uint32_t period, void *intf_ptr)
{
    usleep(period);
}

/*!
 *  @brief Function to select the interface between SPI and I2C.
 *  Also to initialize coines platform
 */
int8_t bmi2_interface_init(struct bmi2_dev *bmi, uint8_t intf)
{
    int8_t rslt = BMI2_OK;

    if (bmi != NULL)
    {
        /* Bus configuration : I2C */
        if (intf == BMI2_I2C_INTF)
        {
            LOGI("I2C Interface ");

            /* To initialize the user I2C function */
            dev_addr = BMI2_I2C_PRIM_ADDR;
            bmi->intf = BMI2_I2C_INTF;
            bmi->read = bmi2_i2c_read;
            bmi->write = bmi2_i2c_write;

            /* SDO to Ground */
            /* Make CSB pin HIGH */
        }
        /* Bus configuration : SPI */
        else if (intf == BMI2_SPI_INTF)
        {
            LOGI("SPI Interface not supported");
        }

        /* Assign device address to interface pointer */
        bmi->intf_ptr = &dev_addr;

        /* Configure delay in microseconds */
        bmi->delay_us = bmi2_delay_us;

        /* Configure max read/write length (in bytes) ( Supported length depends on target machine) */
        bmi->read_write_len = READ_WRITE_LEN;

        /* Assign to NULL to load the default config file. */
        bmi->config_file_ptr = NULL;

    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;

}

/*!
 *  @brief Prints the execution status of the APIs.
 */
void bmi2_error_codes_print_result(int8_t rslt)
{
    switch (rslt)
    {
        case BMI2_OK:

            /* Do nothing */
            break;

        case BMI2_W_FIFO_EMPTY:
            LOGW("Warning [%d] : FIFO empty", rslt);
            break;
        case BMI2_W_PARTIAL_READ:
            LOGW("Warning [%d] : FIFO partial read", rslt);
            break;
        case BMI2_E_NULL_PTR:
            LOGE(
                "Error [%d] : Null pointer error. It occurs when the user tries to assign value (not address) to a pointer," " which has been initialized to NULL.",
                rslt);
            break;

        case BMI2_E_COM_FAIL:
            LOGE(
                "Error [%d] : Communication failure error. It occurs due to read/write operation failure and also due " "to power failure during communication",
                rslt);
            break;

        case BMI2_E_DEV_NOT_FOUND:
            LOGE("Error [%d] : Device not found error. It occurs when the device chip id is incorrectly read",
                   rslt);
            break;

        case BMI2_E_INVALID_SENSOR:
            LOGE(
                "Error [%d] : Invalid sensor error. It occurs when there is a mismatch in the requested feature with the " "available one",
                rslt);
            break;

        case BMI2_E_SELF_TEST_FAIL:
            LOGE(
                "Error [%d] : Self-test failed error. It occurs when the validation of accel self-test data is " "not satisfied",
                rslt);
            break;

        case BMI2_E_INVALID_INT_PIN:
            LOGE(
                "Error [%d] : Invalid interrupt pin error. It occurs when the user tries to configure interrupt pins " "apart from INT1 and INT2",
                rslt);
            break;

        case BMI2_E_OUT_OF_RANGE:
            LOGE(
                "Error [%d] : Out of range error. It occurs when the data exceeds from filtered or unfiltered data from " "fifo and also when the range exceeds the maximum range for accel and gyro while performing FOC",
                rslt);
            break;

        case BMI2_E_ACC_INVALID_CFG:
            LOGE(
                "Error [%d] : Invalid Accel configuration error. It occurs when there is an error in accel configuration" " register which could be one among range, BW or filter performance in reg address 0x40",
                rslt);
            break;

        case BMI2_E_GYRO_INVALID_CFG:
            LOGE(
                "Error [%d] : Invalid Gyro configuration error. It occurs when there is a error in gyro configuration" "register which could be one among range, BW or filter performance in reg address 0x42",
                rslt);
            break;

        case BMI2_E_ACC_GYR_INVALID_CFG:
            LOGE(
                "Error [%d] : Invalid Accel-Gyro configuration error. It occurs when there is a error in accel and gyro" " configuration registers which could be one among range, BW or filter performance in reg address 0x40 " "and 0x42",
                rslt);
            break;

        case BMI2_E_CONFIG_LOAD:
            LOGE(
                "Error [%d] : Configuration load error. It occurs when failure observed while loading the configuration " "into the sensor",
                rslt);
            break;

        case BMI2_E_INVALID_PAGE:
            LOGE(
                "Error [%d] : Invalid page error. It occurs due to failure in writing the correct feature configuration " "from selected page",
                rslt);
            break;

        case BMI2_E_SET_APS_FAIL:
            LOGE(
                "Error [%d] : APS failure error. It occurs due to failure in write of advance power mode configuration " "register",
                rslt);
            break;

        case BMI2_E_AUX_INVALID_CFG:
            LOGE(
                "Error [%d] : Invalid AUX configuration error. It occurs when the auxiliary interface settings are not " "enabled properly",
                rslt);
            break;

        case BMI2_E_AUX_BUSY:
            LOGE(
                "Error [%d] : AUX busy error. It occurs when the auxiliary interface buses are engaged while configuring" " the AUX",
                rslt);
            break;

        case BMI2_E_REMAP_ERROR:
            LOGE(
                "Error [%d] : Remap error. It occurs due to failure in assigning the remap axes data for all the axes " "after change in axis position",
                rslt);
            break;

        case BMI2_E_GYR_USER_GAIN_UPD_FAIL:
            LOGE(
                "Error [%d] : Gyro user gain update fail error. It occurs when the reading of user gain update status " "fails",
                rslt);
            break;

        case BMI2_E_SELF_TEST_NOT_DONE:
            LOGE(
                "Error [%d] : Self-test not done error. It occurs when the self-test process is ongoing or not " "completed",
                rslt);
            break;

        case BMI2_E_INVALID_INPUT:
            LOGE("Error [%d] : Invalid input error. It occurs when the sensor input validity fails", rslt);
            break;

        case BMI2_E_INVALID_STATUS:
            LOGE("Error [%d] : Invalid status error. It occurs when the feature/sensor validity fails", rslt);
            break;

        case BMI2_E_CRT_ERROR:
            LOGE("Error [%d] : CRT error. It occurs when the CRT test has failed", rslt);
            break;

        case BMI2_E_ST_ALREADY_RUNNING:
            LOGE(
                "Error [%d] : Self-test already running error. It occurs when the self-test is already running and " "another has been initiated",
                rslt);
            break;

        case BMI2_E_CRT_READY_FOR_DL_FAIL_ABORT:
            LOGE(
                "Error [%d] : CRT ready for download fail abort error. It occurs when download in CRT fails due to wrong " "address location",
                rslt);
            break;

        case BMI2_E_DL_ERROR:
            LOGE(
                "Error [%d] : Download error. It occurs when write length exceeds that of the maximum burst length",
                rslt);
            break;

        case BMI2_E_PRECON_ERROR:
            LOGE(
                "Error [%d] : Pre-conditional error. It occurs when precondition to start the feature was not " "completed",
                rslt);
            break;

        case BMI2_E_ABORT_ERROR:
            LOGE("Error [%d] : Abort error. It occurs when the device was shaken during CRT test", rslt);
            break;

        case BMI2_E_WRITE_CYCLE_ONGOING:
            LOGE(
                "Error [%d] : Write cycle ongoing error. It occurs when the write cycle is already running and another " "has been initiated",
                rslt);
            break;

        case BMI2_E_ST_NOT_RUNING:
            LOGE(
                "Error [%d] : Self-test is not running error. It occurs when self-test running is disabled while it's " "running",
                rslt);
            break;

        case BMI2_E_DATA_RDY_INT_FAILED:
            LOGE(
                "Error [%d] : Data ready interrupt error. It occurs when the sample count exceeds the FOC sample limit " "and data ready status is not updated",
                rslt);
            break;

        case BMI2_E_INVALID_FOC_POSITION:
            LOGE(
                "Error [%d] : Invalid FOC position error. It occurs when average FOC data is obtained for the wrong" " axes",
                rslt);
            break;

        default:
            LOGE("Error [%d] : Unknown error code", rslt);
            break;
    }
}
