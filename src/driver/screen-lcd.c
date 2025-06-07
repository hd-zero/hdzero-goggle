#include "screen.h"

#if HDZBOXPRO

#include <log/log.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "../core/common.hh"
#include "../core/defines.h"
#include "i2c.h"

#define LCD_CMD_SA 0x5E // (0x2F<<1)

void LCD_Write(uint8_t idx, uint8_t addr, uint8_t wdat) {
    uint8_t val;

    I2C_Write(ADDR_FPGA, 0xAA, LCD_CMD_SA);
    I2C_Write(ADDR_FPGA, 0xA9, idx);
    I2C_Write(ADDR_FPGA, 0xA8, 0x01);

    I2C_Write(ADDR_FPGA, 0xAA, addr << 1);
    I2C_Write(ADDR_FPGA, 0xA9, wdat);
    I2C_Write(ADDR_FPGA, 0xA8, 0x01);

    // usleep(10);
}

void Screen_Init() {
    // Gate reverse
    LCD_Write(0x00, 0x4D, 0xAA);
    LCD_Write(0x00, 0x52, 0x13);
    LCD_Write(0x01, 0x52, 0x13);
    LCD_Write(0x02, 0x52, 0x13);
    LCD_Write(0x03, 0x52, 0x13);
    LCD_Write(0x04, 0x52, 0x12);
    LCD_Write(0x05, 0x52, 0x13);
    LCD_Write(0x06, 0x52, 0x0D);
    LCD_Write(0x07, 0x52, 0x13);
    LCD_Write(0x08, 0x52, 0x09);
    LCD_Write(0x09, 0x52, 0x0B);
    LCD_Write(0x0A, 0x52, 0x05);
    LCD_Write(0x0B, 0x52, 0x07);
    LCD_Write(0x0C, 0x52, 0x03);
    LCD_Write(0x0D, 0x52, 0x01);
    LCD_Write(0x0E, 0x52, 0x13);
    LCD_Write(0x0F, 0x52, 0x13);
    LCD_Write(0x10, 0x52, 0x13);
    LCD_Write(0x11, 0x52, 0x13);
    LCD_Write(0x12, 0x52, 0x13);
    LCD_Write(0x13, 0x52, 0x13);
    LCD_Write(0x14, 0x52, 0x13);
    LCD_Write(0x15, 0x52, 0x13);

    LCD_Write(0x00, 0x59, 0x13);
    LCD_Write(0x01, 0x59, 0x13);
    LCD_Write(0x02, 0x59, 0x13);
    LCD_Write(0x03, 0x59, 0x13);
    LCD_Write(0x04, 0x59, 0x12);
    LCD_Write(0x05, 0x59, 0x13);
    LCD_Write(0x06, 0x59, 0x0C);
    LCD_Write(0x07, 0x59, 0x13);
    LCD_Write(0x08, 0x59, 0x08);
    LCD_Write(0x09, 0x59, 0x0A);
    LCD_Write(0x0A, 0x59, 0x04);
    LCD_Write(0x0B, 0x59, 0x06);
    LCD_Write(0x0C, 0x59, 0x02);
    LCD_Write(0x0D, 0x59, 0x00);
    LCD_Write(0x0E, 0x59, 0x13);
    LCD_Write(0x0F, 0x59, 0x13);
    LCD_Write(0x10, 0x59, 0x13);
    LCD_Write(0x11, 0x59, 0x13);
    LCD_Write(0x12, 0x59, 0x13);
    LCD_Write(0x13, 0x59, 0x13);
    LCD_Write(0x14, 0x59, 0x13);
    LCD_Write(0x15, 0x59, 0x13);
    LCD_Write(0x00, 0x5C, 0x24);

    // SOURCE reverse:
    LCD_Write(0x00, 0x32, 0x12);
}

void Screen_PowerUp() {
    usleep(10000);
    I2C_Write(ADDR_FPGA, 0x8b, 0x82); // VDD->on, VS->1
    usleep(4500);
    I2C_Write(ADDR_FPGA, 0x8b, 0xc2); // VG_L->1
    usleep(1000);
    I2C_Write(ADDR_FPGA, 0x8b, 0xc6); // VG_H->1

    usleep(100000);
    Screen_Init(); // lcd reg cfg

    usleep(20000);
    I2C_Write(ADDR_FPGA, 0x8b, 0xce); // tmg_en->1

    usleep(50000);
    I2C_Write(ADDR_FPGA, 0xab, 0x80);
    usleep(50000);
    I2C_Write(ADDR_FPGA, 0xab, 0x80);
    I2C_Write(ADDR_FPGA, 0x8b, 0xde); // bl_en->1

    usleep(40000);
    I2C_Write(ADDR_FPGA, 0x8b, 0xfe); // rgb_en->1
}

void Screen_PowerDown() {
    usleep(10000);
    // I2C_Write(ADDR_FPGA, 0x8b, 0x81);

    I2C_Write(ADDR_FPGA, 0x8b, 0xce); // rgb_en->0, bl_en->0

    usleep(40000);
    I2C_Write(ADDR_FPGA, 0x8b, 0xc6); // tmg_en->0

    usleep(100000);
    I2C_Write(ADDR_FPGA, 0x8b, 0x81);
}

void Screen_Display(int on) {
    static int last_on = 1;

    if (last_on != on) {
        last_on = on;

        if (on) {
            Screen_PowerUp();
            LOGI("LCD: Display on");

        } else {
            Screen_PowerDown();
            LOGI("LCD: Display off");
        }
    }
}

void Screen_Brightness(uint8_t level) {
    level = 19 + (level * 80 / 12);
    I2C_Write(ADDR_FPGA, 0x89, level);
}

void Screen_Startup() {
}

void Screen_Pattern(uint8_t enable, uint8_t mode, uint8_t speed) {
}

#endif
