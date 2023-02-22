#include "TP2825.h"

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <log/log.h>

#include "core/common.hh"
#include "driver/gpio.h"
#include "driver/i2c.h"

#define RESET_VALUE 0xb3

static uint8_t clamp_value = TP2825_CLAMP_MIN;

void TP2825_close() {
    gpio_set(GPIO_TP2825_RSTB, 0);
}

void TP2825_open() {
    gpio_set(GPIO_TP2825_RSTB, 1);
}

void TP2825_Init(int ch_sel, int is_pal) {
    TP2825_close();
    usleep(10000);
    TP2825_open();
    usleep(10000);

    TP2825_Config(ch_sel, is_pal, TP2825_CLAMP_MIN);
}

// ch_sel: 0=AV in; 1=Module bay
void TP2825_Config(int ch_sel, int is_pal, uint8_t clamp) {
    I2C_Write(ADDR_TP2825, 0x06, RESET_VALUE);
    I2C_Write(ADDR_TP2825, 0x41, ch_sel);

    if (is_pal) {
        LOGD("TP2825_Config: pal");
        I2C_Write(ADDR_TP2825, 0x02, 0xCE);
        I2C_Write(ADDR_TP2825, 0x0D, 0x11);

        I2C_Write(ADDR_TP2825, 0x15, 0x13);
        I2C_Write(ADDR_TP2825, 0x16, 0x4A);

        I2C_Write(ADDR_TP2825, 0x1A, 0x17);
        I2C_Write(ADDR_TP2825, 0x17, 0xC0);
        I2C_Write(ADDR_TP2825, 0x19, 0x20);

        I2C_Write(ADDR_TP2825, 0x18, 0x13);

        I2C_Write(ADDR_TP2825, 0x1C, 0x09);
        I2C_Write(ADDR_TP2825, 0x1D, 0x48);

        I2C_Write(ADDR_TP2825, 0x2D, 0x60);

        I2C_Write(ADDR_TP2825, 0x30, 0x7A);
        I2C_Write(ADDR_TP2825, 0x31, 0x4A);
        I2C_Write(ADDR_TP2825, 0x32, 0x4D);
        I2C_Write(ADDR_TP2825, 0x33, 0xF0);
    } else {
        LOGD("TP2825_Config: ntsc");
        I2C_Write(ADDR_TP2825, 0x02, 0xCF);
        I2C_Write(ADDR_TP2825, 0x0D, 0x10);

        I2C_Write(ADDR_TP2825, 0x15, 0x13);
        I2C_Write(ADDR_TP2825, 0x16, 0x3C);

        I2C_Write(ADDR_TP2825, 0x1A, 0x07);
        I2C_Write(ADDR_TP2825, 0x17, 0xC0);
        I2C_Write(ADDR_TP2825, 0x19, 0xF0);

        I2C_Write(ADDR_TP2825, 0x18, 0x11);

        I2C_Write(ADDR_TP2825, 0x1C, 0x09);
        I2C_Write(ADDR_TP2825, 0x1D, 0x38);

        I2C_Write(ADDR_TP2825, 0x2D, 0x68);

        I2C_Write(ADDR_TP2825, 0x30, 0x62);
        I2C_Write(ADDR_TP2825, 0x31, 0xBB);
        I2C_Write(ADDR_TP2825, 0x32, 0x96);
        I2C_Write(ADDR_TP2825, 0x33, 0xC0);
    }

    I2C_Write(ADDR_TP2825, 0x09, 0xa4);
    I2C_Write(ADDR_TP2825, 0x0A, 0x2a);
    I2C_Write(ADDR_TP2825, 0x0C, 0x7a);

    // I2C_Write(ADDR_TP2825, 0x10, 0x0);  // brightness
    // I2C_Write(ADDR_TP2825, 0x11, 0x3c); // contrast
    // I2C_Write(ADDR_TP2825, 0x12, 0x40); // saturation
    // I2C_Write(ADDR_TP2825, 0x13, 0x0);  // hue
    // I2C_Write(ADDR_TP2825, 0x14, 0x2);  // sharpness

    clamp_value = clamp;

    I2C_Write(ADDR_TP2825, 0x20, 0xB0);
    I2C_Write(ADDR_TP2825, 0x22, 0x39);
    I2C_Write(ADDR_TP2825, 0x23, clamp_value);
    I2C_Write(ADDR_TP2825, 0x24, 0x57);
    I2C_Write(ADDR_TP2825, 0x25, 0xfa);
    I2C_Write(ADDR_TP2825, 0x26, 0x12);
    I2C_Write(ADDR_TP2825, 0x28, 0x04);
    I2C_Write(ADDR_TP2825, 0x29, 0x18);
    I2C_Write(ADDR_TP2825, 0x2A, 0xb3); // diable color kill
    I2C_Write(ADDR_TP2825, 0x2C, 0x4a);

    I2C_Write(ADDR_TP2825, 0x39, 0x4);

    I2C_Write(ADDR_TP2825, 0x35, 0x65);
    I2C_Write(ADDR_TP2825, 0x4C, 0x03);
    I2C_Write(ADDR_TP2825, 0x4D, 0x03);
    I2C_Write(ADDR_TP2825, 0x4E, 0x37);
}

// 0 = AV in; 1 = Module bay
void TP2825_Switch_CH(uint8_t sel) {
    I2C_Write(ADDR_TP2825, 0x41, sel);
    I2C_Write(ADDR_TP2825, 0x06, RESET_VALUE);
}

void TP2825_Set_Clamp(uint8_t val) {
    if (clamp_value == val) {
        return;
    }

    LOGD("TP2825_Set_Clamp: %d", val);
    I2C_Write(ADDR_TP2825, 0x23, val);
    I2C_Write(ADDR_TP2825, 0x06, RESET_VALUE);
    clamp_value = val;
}