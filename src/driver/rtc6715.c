#include "rtc6715.h"

#if HDZBOXPRO

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include <log/log.h>

#include "../core/common.hh"
#include "../core/defines.h"
#include "driver/gpio.h"
#include "gpadc.h"
#include "i2c.h"

static uint8_t rtc6715_rssi;
uint32_t MM_Read(uint8_t addr) {
    uint32_t rdat;

    // spi_addr
    I2C_Write(ADDR_FPGA, 0xa1, addr);

    // read cmd
    I2C_Write(ADDR_FPGA, 0xa0, 0x10);

    // read dat
    rdat = I2C_Read(ADDR_FPGA, 0xa7);
    rdat <<= 8;
    rdat |= I2C_Read(ADDR_FPGA, 0xa6);
    rdat <<= 8;
    rdat |= I2C_Read(ADDR_FPGA, 0xa5);

    usleep(10000);

#ifdef _DEBUG_MM
    LOGI("MM READ: addr = %x  data = %x\n", addr, rdat);
#endif

    return rdat;
}

void MM_Write(uint8_t addr, uint32_t dat) {
    uint8_t val;

    // spi_addr
    I2C_Write(ADDR_FPGA, 0xa1, addr);

    // spi_wdat
    val = dat & 0xFF;
    I2C_Write(ADDR_FPGA, 0xa2, val);
    val = (dat >> 8) & 0xFF;
    I2C_Write(ADDR_FPGA, 0xa3, val);
    val = (dat >> 16) & 0xFF;
    I2C_Write(ADDR_FPGA, 0xa4, val);

    // wrte cmd
    I2C_Write(ADDR_FPGA, 0xa0, 0x01);

    usleep(10000);

#ifdef _DEBUG_MM
    uint32_t rdat = MM_Read(addr);
    if (dat != rdat)
        LOGI("                           --- W or R error !!   wdat = %x", dat);
#endif
}

void RTC6715_Open(int on) {
    gpio_set(GPIO_RTC6715_ON, on);
    gpadc_on(on);
    LOGI("RTC6715_Open:%d", on);
}

void RTC6715_SetCH(int ch) {

    const uint32_t tab[48] = {
        0x2a05, 0x299b, 0x2991, 0x2987, 0x291d, 0x2913, 0x2909, 0x289f, // band A
        0x2903, 0x290c, 0x2916, 0x291f, 0x2989, 0x2992, 0x299c, 0x2a05, // band B
        0x2895, 0x288b, 0x2881, 0x2817, 0x2a0f, 0x2a19, 0x2a83, 0x2a8d, // band E
        0x2906, 0x2910, 0x291a, 0x2984, 0x298e, 0x2998, 0x2a02, 0x2a0c, // band F
        0x281d, 0x2890, 0x2902, 0x2915, 0x2987, 0x299a, 0x2a0c, 0x2a1f, // band R
        0x2604, 0x261c, 0x268e, 0x2701, 0x2713, 0x2786, 0x2798, 0x280b, // band L
    };

    MM_Write(0x00, 0x08);

    MM_Write(0x01, tab[ch]);

    LOGI("Set_RTC6715: %d", (uint16_t)ch);
}

int RTC6715_GetRssi() {
    static int rssi_adc = 0;
    int value = gpdac0_get();

    if (value < 0)
        return rssi_adc;

    rssi_adc = 3300 * value / 4096;
    LOGI("rssi voltage: %02f", (float)rssi_adc / 1000);
    return rssi_adc;
}

#endif
