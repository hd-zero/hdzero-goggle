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
}

void TP2825_open() {
    gpio_set(GPIO_TP2825_RSTB, 1);
}

void TP2825_init(source_t mode, int is_pal) {
    TP2825_close();
    usleep(1000);
    TP2825_open();
    usleep(1000);

    I2C_Write(ADDR_TP2825, 0x02, SOURCE_AV_IN == mode ? 0x44 : 0x40);
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
}

void TP2825_Switch_Mode(int is_pal) {
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

void TP2825_Switch_CH(source_t mode) {
    I2C_Write(ADDR_TP2825, 0x02, SOURCE_AV_IN == mode ? 0x44 : 0x40);
    I2C_Write(ADDR_TP2825, 0x06, 0x80);
}

#endif
