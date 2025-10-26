#include "tp2825.h"

#if HDZGOGGLE2

#include "core/common.hh"
#include "driver/gpio.h"
#include "hardware.h"
#include "i2c.h"
#include <log/log.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#define TP2825_REG06 0xB2 // AGC enabled

void TP2825_close() {
    gpio_set(GPIO_TP2825_RSTB, 0);
}

void TP2825_open() {
    gpio_set(GPIO_TP2825_RSTB, 1);
}

void TP2825_VER() {
    int dat = I2C_Read(ADDR_TP2825, 0x24);
    g_hw_stat.IS_TP2825_L = (dat == 0x04) ? 1 : 0;

    if (g_hw_stat.IS_TP2825_L) {
        I2C_Write(ADDR_TP2825, 0xf5, 0x00);
        I2C_Write(ADDR_TP2825, 0xfa, 0x01);
        LOGI("TP2825L detected");
    }
}

/*
is_av_in:
        0: AV line in
        1: Internal/External Analog Module

*/
void TP2825_init(bool is_av_in, bool is_pal) {
    TP2825_close();
    usleep(1000);
    TP2825_open();
    usleep(1000);

    I2C_Write(ADDR_TP2825, 0x02, is_av_in ? 0x44 : 0x40);
    I2C_Write(ADDR_TP2825, 0x03, 0x25);
    I2C_Write(ADDR_TP2825, 0x05, 0x77);

    if (is_pal) {
        I2C_Write(ADDR_TP2825, 0x07, 0x12);
        I2C_Write(ADDR_TP2825, 0x08, 0x18);
        I2C_Write(ADDR_TP2825, 0x09, 0x20);
        I2C_Write(ADDR_TP2825, 0x0A, 0x10);
        I2C_Write(ADDR_TP2825, 0x0B, 0xD0);
    } else {
        I2C_Write(ADDR_TP2825, 0x07, 0x02);
        I2C_Write(ADDR_TP2825, 0x08, 0x12);
        I2C_Write(ADDR_TP2825, 0x09, 0xF0);
        I2C_Write(ADDR_TP2825, 0x0A, 0x10);
        I2C_Write(ADDR_TP2825, 0x0B, 0xD0);
    }

    I2C_Write(ADDR_TP2825, 0x10, 0x10);
    I2C_Write(ADDR_TP2825, 0x11, 0x48);
    // I2C_Write(ADDR_TP2825, 0x12, 0x51);
    // I2C_Write(ADDR_TP2825, 0x13, 0x80);
    // I2C_Write(ADDR_TP2825, 0x14, 0x80);
    // I2C_Write(ADDR_TP2825, 0x15, 0x00);

    I2C_Write(ADDR_TP2825, 0x25, 0x28);

    I2C_Write(ADDR_TP2825, 0x06, 0x80);

    LOGI("TP2825 init: is_av_in=%d, is_pal=%d", is_av_in, is_pal);
}

void TP2825_Switch_Mode(bool is_pal) {
    if (is_pal) {
        I2C_Write(ADDR_TP2825, 0x07, 0x12);
        I2C_Write(ADDR_TP2825, 0x08, 0x18);
        I2C_Write(ADDR_TP2825, 0x09, 0x20);
        I2C_Write(ADDR_TP2825, 0x0A, 0x10);
        I2C_Write(ADDR_TP2825, 0x0B, 0xD0);
    } else {
        I2C_Write(ADDR_TP2825, 0x07, 0x02);
        I2C_Write(ADDR_TP2825, 0x08, 0x12);
        I2C_Write(ADDR_TP2825, 0x09, 0xF0);
        I2C_Write(ADDR_TP2825, 0x0A, 0x10);
        I2C_Write(ADDR_TP2825, 0x0B, 0xD0);
    }

    I2C_Write(ADDR_TP2825, 0x06, 0x80);
}

void TP2825_Switch_CH(bool is_av_in) {
    I2C_Write(ADDR_TP2825, 0x02, is_av_in ? 0x44 : 0x40);
    I2C_Write(ADDR_TP2825, 0x06, 0x80);

    // LOGI("TP2825 switch channel: %d", is_av_in);
}

void TP2825_Set_Clamp(int idx) {}
void TP2825_Set_Pclk(uint8_t inv) {}
#endif
