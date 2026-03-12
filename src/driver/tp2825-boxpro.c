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

uint8_t orbit_x = 0, orbit_y = 0;
uint8_t orbit_move = 0;

void TP2825_orbit(int orbit_setting) {
    static int orbit_level = 0;

    if (orbit_setting > 0 && orbit_x > 0 && orbit_y > 0) {

        if (orbit_level != orbit_setting) {
            orbit_level = orbit_setting;
            orbit_move = 0;
        }

        switch (orbit_move) {
        case 0:
            I2C_Write(ADDR_TP2825, 0x0A, orbit_x);
            I2C_Write(ADDR_TP2825, 0x08, orbit_y);
            orbit_move = 1;
            break;
        case 1:
            if (orbit_level > 1) {
                I2C_Write(ADDR_TP2825, 0x0A, orbit_x + 1);
                I2C_Write(ADDR_TP2825, 0x08, orbit_y + 1);
            } else {
                I2C_Write(ADDR_TP2825, 0x0A, orbit_x);
                I2C_Write(ADDR_TP2825, 0x08, orbit_y + 1);
            }
            orbit_move = 2;
            break;
        case 2:
            if (orbit_level > 1) {
                I2C_Write(ADDR_TP2825, 0x0A, orbit_x + 2);
                I2C_Write(ADDR_TP2825, 0x08, orbit_y);
            } else {
                I2C_Write(ADDR_TP2825, 0x0A, orbit_x + 1);
                I2C_Write(ADDR_TP2825, 0x08, orbit_y + 1);
            }
            orbit_move = 3;
            break;
        case 3:
            if (orbit_level > 1) {
                I2C_Write(ADDR_TP2825, 0x0A, orbit_x + 1);
                I2C_Write(ADDR_TP2825, 0x08, orbit_y - 1);
            } else {
                I2C_Write(ADDR_TP2825, 0x0A, orbit_x + 1);
                I2C_Write(ADDR_TP2825, 0x08, orbit_y);
            }
            orbit_move = 0;
            break;
        default:
            orbit_move = 0;
            break;
        }
    }
}

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
        I2C_Write(ADDR_TP2825, 0x08, orbit_y = 0x18);
        I2C_Write(ADDR_TP2825, 0x09, 0x20);
        I2C_Write(ADDR_TP2825, 0x0A, orbit_x = 0x10);
        I2C_Write(ADDR_TP2825, 0x0B, 0xD0);
    } else {
        I2C_Write(ADDR_TP2825, 0x07, 0x02);
        I2C_Write(ADDR_TP2825, 0x08, orbit_y = 0x12);
        I2C_Write(ADDR_TP2825, 0x09, 0xF0);
        I2C_Write(ADDR_TP2825, 0x0A, orbit_x = 0x10);
        I2C_Write(ADDR_TP2825, 0x0B, 0xD0);
    }
    orbit_move = 0;

    I2C_Write(ADDR_TP2825, 0x10, 0x10); // brightness
    I2C_Write(ADDR_TP2825, 0x11, 0x48); // contrast
    I2C_Write(ADDR_TP2825, 0x2C, 0x28); // horizontal sharpness
    I2C_Write(ADDR_TP2825, 0x13, 0x80); // hue
    I2C_Write(ADDR_TP2825, 0x14, 0x80); // saturation
    I2C_Write(ADDR_TP2825, 0x17, 0x31); // vertical sharpness

    I2C_Write(ADDR_TP2825, 0x25, 0x28);

    I2C_Write(ADDR_TP2825, 0x06, 0x80);

    LOGI("TP2825 init: is_av_in=%d, is_pal=%d", is_av_in, is_pal);
}

void TP2825_Switch_Mode(bool is_pal) {
    if (is_pal) {
        I2C_Write(ADDR_TP2825, 0x07, 0x12);
        I2C_Write(ADDR_TP2825, 0x08, orbit_y = 0x18);
        I2C_Write(ADDR_TP2825, 0x09, 0x20);
        I2C_Write(ADDR_TP2825, 0x0A, orbit_x = 0x10);
        I2C_Write(ADDR_TP2825, 0x0B, 0xD0);
    } else {
        I2C_Write(ADDR_TP2825, 0x07, 0x02);
        I2C_Write(ADDR_TP2825, 0x08, orbit_y = 0x12);
        I2C_Write(ADDR_TP2825, 0x09, 0xF0);
        I2C_Write(ADDR_TP2825, 0x0A, orbit_x = 0x10);
        I2C_Write(ADDR_TP2825, 0x0B, 0xD0);
    }

    I2C_Write(ADDR_TP2825, 0x06, 0x80);
    orbit_move = 0;
}

void TP2825_Switch_CH(bool is_av_in) {
    I2C_Write(ADDR_TP2825, 0x02, is_av_in ? 0x44 : 0x40);
    I2C_Write(ADDR_TP2825, 0x06, 0x80);

    LOGI("TP2825 switch channel: %d", is_av_in);
}

void TP2825_Set_Clamp(int idx) {}
void TP2825_Set_Pclk(uint8_t inv) {}

#endif
