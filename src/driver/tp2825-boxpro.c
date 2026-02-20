#include "tp2825.h"

#if HDZBOXPRO

#include "core/common.hh"
#include "driver/gpio.h"
#include "hardware.h"
#include "i2c.h"
#include <log/log.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

void TP2825_close() {
    gpio_set(GPIO_TP2825_RSTB, 0);
    LOGI("TP2825 close");
}

void TP2825_open() {
    gpio_set(GPIO_TP2825_RSTB, 1);
    LOGI("TP2825 open");
}

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

    I2C_Write(ADDR_TP2825, 0x10, 0x04); // brightness
    I2C_Write(ADDR_TP2825, 0x11, 0x32); // contrast
    I2C_Write(ADDR_TP2825, 0x13, 0x48); // hue
    I2C_Write(ADDR_TP2825, 0x14, 0x70); // saturation
    I2C_Write(ADDR_TP2825, 0x2C, 0x38); // output sharpness
    // I2C_Write(ADDR_TP2825, 0x14, 0x80); // saturation
    // I2C_Write(ADDR_TP2825, 0x15, 0x00);
    // I2C_Write(ADDR_TP2825, 0x17, 0x00); // sharpness? bit 3-0 strength
    // I2C_Write(ADDR_TP2825, 0x19, 0x00); // black blamp bit 7: pedestal En
    //                                                      6-4: clamp filter/speed
    //                                                      3-0: calibration/offset

    // AGC params
    I2C_Write(ADDR_TP2825, 0x21, 0xFF); //
    I2C_Write(ADDR_TP2825, 0x22, 0xFF); //
    I2C_Write(ADDR_TP2825, 0x23, 0xFF); //
    I2C_Write(ADDR_TP2825, 0x24, 0x80); // Black level
    I2C_Write(ADDR_TP2825, 0x27, 0x17); // White level
    I2C_Write(ADDR_TP2825, 0x25, 0x28); // AGC gain

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

    LOGI("TP2825 switch channel: %d", is_av_in);
}

void TP2825_Set_Clamp(int idx) {}
void TP2825_Set_Pclk(uint8_t inv) {}

#endif
