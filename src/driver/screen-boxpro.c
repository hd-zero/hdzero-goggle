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

static void lcd_write(uint8_t idx, uint8_t addr, uint8_t wdat) {
    uint8_t val;

    I2C_Write(ADDR_FPGA, 0xAA, LCD_CMD_SA);
    I2C_Write(ADDR_FPGA, 0xA9, idx);
    I2C_Write(ADDR_FPGA, 0xA8, 0x01);

    I2C_Write(ADDR_FPGA, 0xAA, addr << 1);
    I2C_Write(ADDR_FPGA, 0xA9, wdat);
    I2C_Write(ADDR_FPGA, 0xA8, 0x01);

    // usleep(10);
}

static void screen_init() {
    // Gate reverse
    lcd_write(0x00, 0x4D, 0xAA);
    lcd_write(0x00, 0x52, 0x13);
    lcd_write(0x01, 0x52, 0x13);
    lcd_write(0x02, 0x52, 0x13);
    lcd_write(0x03, 0x52, 0x13);
    lcd_write(0x04, 0x52, 0x12);
    lcd_write(0x05, 0x52, 0x13);
    lcd_write(0x06, 0x52, 0x0D);
    lcd_write(0x07, 0x52, 0x13);
    lcd_write(0x08, 0x52, 0x09);
    lcd_write(0x09, 0x52, 0x0B);
    lcd_write(0x0A, 0x52, 0x05);
    lcd_write(0x0B, 0x52, 0x07);
    lcd_write(0x0C, 0x52, 0x03);
    lcd_write(0x0D, 0x52, 0x01);
    lcd_write(0x0E, 0x52, 0x13);
    lcd_write(0x0F, 0x52, 0x13);
    lcd_write(0x10, 0x52, 0x13);
    lcd_write(0x11, 0x52, 0x13);
    lcd_write(0x12, 0x52, 0x13);
    lcd_write(0x13, 0x52, 0x13);
    lcd_write(0x14, 0x52, 0x13);
    lcd_write(0x15, 0x52, 0x13);

    lcd_write(0x00, 0x59, 0x13);
    lcd_write(0x01, 0x59, 0x13);
    lcd_write(0x02, 0x59, 0x13);
    lcd_write(0x03, 0x59, 0x13);
    lcd_write(0x04, 0x59, 0x12);
    lcd_write(0x05, 0x59, 0x13);
    lcd_write(0x06, 0x59, 0x0C);
    lcd_write(0x07, 0x59, 0x13);
    lcd_write(0x08, 0x59, 0x08);
    lcd_write(0x09, 0x59, 0x0A);
    lcd_write(0x0A, 0x59, 0x04);
    lcd_write(0x0B, 0x59, 0x06);
    lcd_write(0x0C, 0x59, 0x02);
    lcd_write(0x0D, 0x59, 0x00);
    lcd_write(0x0E, 0x59, 0x13);
    lcd_write(0x0F, 0x59, 0x13);
    lcd_write(0x10, 0x59, 0x13);
    lcd_write(0x11, 0x59, 0x13);
    lcd_write(0x12, 0x59, 0x13);
    lcd_write(0x13, 0x59, 0x13);
    lcd_write(0x14, 0x59, 0x13);
    lcd_write(0x15, 0x59, 0x13);
    lcd_write(0x00, 0x5C, 0x24);

    // SOURCE reverse:
    lcd_write(0x00, 0x32, 0x12);
}

static void screen_power_up() {
    usleep(10000);
    I2C_Write(ADDR_FPGA, 0x8b, 0x82); // VDD->on, VS->1
    usleep(4500);
    I2C_Write(ADDR_FPGA, 0x8b, 0xc2); // VG_L->1
    usleep(1000);
    I2C_Write(ADDR_FPGA, 0x8b, 0xc6); // VG_H->1

    usleep(100000);
    screen_init(); // lcd reg cfg

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

static void screen_power_down() {
    usleep(10000);
    // I2C_Write(ADDR_FPGA, 0x8b, 0x81);

    I2C_Write(ADDR_FPGA, 0x8b, 0xce); // rgb_en->0, bl_en->0

    usleep(40000);
    I2C_Write(ADDR_FPGA, 0x8b, 0xc6); // tmg_en->0

    usleep(100000);
    I2C_Write(ADDR_FPGA, 0x8b, 0x81);
}

static void screen_display(bool on) {
    static int last_on = 1;

    if (last_on != on) {
        last_on = on;
        if (on) {
            screen_power_up();
            LOGI("LCD: Display on");
        } else {
            screen_power_down();
            LOGI("LCD: Display off");
        }
    }
}

static void screen_brightness(uint8_t level) {
    level = 19 + (level * 80 / 12);
    I2C_Write(ADDR_FPGA, 0x89, level);
}

static void screen_start_up() {}
static void screen_pattern(bool enable, uint8_t mode, uint8_t speed) {}
static void screen_vtmg(int mode) {}

static void MFPGA_Set720P90(uint8_t mode) {};
static void MFPGA_Set720P60(uint8_t mode, bool is_43) {};
static void MFPGA_Set540P60() {};
static void MFPGA_Set1080P30() {};
static void MFPGA_SetRatio(bool ratio) {};

screen_t screen = {
    .start_up = screen_start_up,
    .display = screen_display,
    .brightness = screen_brightness,
    .pattern = screen_pattern,
    .vtmg = screen_vtmg,
    .mfpga = {
        .set720p90 = MFPGA_Set720P90,
        .set720p60 = MFPGA_Set720P60,
        .set540p60 = MFPGA_Set540P60,
        .set1080p30 = MFPGA_Set1080P30,
        .set_ratio = MFPGA_SetRatio,
    },
};

#endif
