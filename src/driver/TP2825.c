#include "TP2825.h"
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

void TP2825_Config(int ch_sel, int is_pal) // ch_sel: 0=AV in; 1=Module bay
{
    TP2825_close();
    usleep(1000);
    TP2825_open();
    usleep(1000);

    TP2825_VER();

    I2C_Write(ADDR_TP2825, 0x41, ch_sel);

    if (is_pal) {
        I2C_Write(ADDR_TP2825, 0x02, 0xCE);
        I2C_Write(ADDR_TP2825, 0x06, 0x32);
        I2C_Write(ADDR_TP2825, 0x07, 0xC0);
        I2C_Write(ADDR_TP2825, 0x08, 0x00);
        I2C_Write(ADDR_TP2825, 0x09, 0x24);
        I2C_Write(ADDR_TP2825, 0x0A, 0x48);
        I2C_Write(ADDR_TP2825, 0x0B, 0xC0);
        I2C_Write(ADDR_TP2825, 0x0C, 0x53);
        I2C_Write(ADDR_TP2825, 0x0D, 0x11);
        I2C_Write(ADDR_TP2825, 0x0E, 0x00);
        I2C_Write(ADDR_TP2825, 0x0F, 0x00);

        // I2C_Write(ADDR_TP2825, 0x15, 0x13);
        I2C_Write(ADDR_TP2825, 0x16, 0x4A);
        I2C_Write(ADDR_TP2825, 0x17, 0xC0);
        I2C_Write(ADDR_TP2825, 0x18, 0x17);
        I2C_Write(ADDR_TP2825, 0x19, 0x20);
        I2C_Write(ADDR_TP2825, 0x1A, 0x17);
        I2C_Write(ADDR_TP2825, 0x1C, 0x09);
        I2C_Write(ADDR_TP2825, 0x1D, 0x48);

        I2C_Write(ADDR_TP2825, 0x20, g_hw_stat.IS_TP2825_L ? 0x40 : 0xB0);
        I2C_Write(ADDR_TP2825, 0x21, 0x86);
        I2C_Write(ADDR_TP2825, 0x22, 0x38);
        I2C_Write(ADDR_TP2825, 0x23, 0x3C);
        // I2C_Write(ADDR_TP2825, 0x24, 0x56);
        I2C_Write(ADDR_TP2825, 0x25, 0xFF);
        I2C_Write(ADDR_TP2825, 0x26, 0x02);
        I2C_Write(ADDR_TP2825, 0x27, 0x2D);
        I2C_Write(ADDR_TP2825, 0x28, 0xC5); //---
        I2C_Write(ADDR_TP2825, 0x29, 0x18); //---
        I2C_Write(ADDR_TP2825, 0x2A, 0x30); //---
        I2C_Write(ADDR_TP2825, 0x2B, 0x70);
        I2C_Write(ADDR_TP2825, 0x2C, 0x1A);
        I2C_Write(ADDR_TP2825, 0x2D, 0x60);
        I2C_Write(ADDR_TP2825, 0x2E, 0x5E);
        I2C_Write(ADDR_TP2825, 0x2F, 0x00);

        I2C_Write(ADDR_TP2825, 0x30, 0x7A);
        I2C_Write(ADDR_TP2825, 0x31, 0x4A);
        I2C_Write(ADDR_TP2825, 0x32, 0x4D);
        I2C_Write(ADDR_TP2825, 0x33, 0xF0);
        I2C_Write(ADDR_TP2825, 0x34, 0x00);
        I2C_Write(ADDR_TP2825, 0x35, 0x65);
        I2C_Write(ADDR_TP2825, 0x36, 0xDC);
        I2C_Write(ADDR_TP2825, 0x37, 0x00);
        I2C_Write(ADDR_TP2825, 0x38, 0x40);
        I2C_Write(ADDR_TP2825, 0x39, 0x84);
        I2C_Write(ADDR_TP2825, 0x3A, 0x00);
        I2C_Write(ADDR_TP2825, 0x3B, 0x03);
        I2C_Write(ADDR_TP2825, 0x3C, 0x00);
        I2C_Write(ADDR_TP2825, 0x3D, 0x60);
        I2C_Write(ADDR_TP2825, 0x3E, 0x00);
        I2C_Write(ADDR_TP2825, 0x3F, 0x00);

        I2C_Write(ADDR_TP2825, 0x40, 0x00);
        // I2C_Write(ADDR_TP2825, 0x41, 0x00);
        I2C_Write(ADDR_TP2825, 0x42, 0x00);
        I2C_Write(ADDR_TP2825, 0x43, 0x12);
        I2C_Write(ADDR_TP2825, 0x44, 0x07);
        I2C_Write(ADDR_TP2825, 0x45, 0x49);
        I2C_Write(ADDR_TP2825, 0x46, 0x00);
        I2C_Write(ADDR_TP2825, 0x47, 0x00);
        I2C_Write(ADDR_TP2825, 0x48, 0x00);
        I2C_Write(ADDR_TP2825, 0x49, 0x00);
        I2C_Write(ADDR_TP2825, 0x4A, 0x00);
        I2C_Write(ADDR_TP2825, 0x4B, 0x00);
        I2C_Write(ADDR_TP2825, 0x4C, 0xC3);
        I2C_Write(ADDR_TP2825, 0x4D, 0x01);
        I2C_Write(ADDR_TP2825, 0x4E, 0x37);
        I2C_Write(ADDR_TP2825, 0x4F, 0x00);
    } else {
        I2C_Write(ADDR_TP2825, 0x02, 0xCF);
        I2C_Write(ADDR_TP2825, 0x06, 0x32);
        I2C_Write(ADDR_TP2825, 0x07, 0xC0);
        I2C_Write(ADDR_TP2825, 0x08, 0x00);
        I2C_Write(ADDR_TP2825, 0x09, 0x24);
        I2C_Write(ADDR_TP2825, 0x0A, 0x48);
        I2C_Write(ADDR_TP2825, 0x0B, 0xC0);
        I2C_Write(ADDR_TP2825, 0x0C, 0x53);
        I2C_Write(ADDR_TP2825, 0x0D, 0x10);
        I2C_Write(ADDR_TP2825, 0x0E, 0x00);
        I2C_Write(ADDR_TP2825, 0x0F, 0x00);

        // I2C_Write(ADDR_TP2825, 0x15, 0x13);
        I2C_Write(ADDR_TP2825, 0x16, 0x3C);
        I2C_Write(ADDR_TP2825, 0x17, 0xC0);
        I2C_Write(ADDR_TP2825, 0x18, 0x13);
        I2C_Write(ADDR_TP2825, 0x19, 0xF0);
        I2C_Write(ADDR_TP2825, 0x1A, 0x07);
        I2C_Write(ADDR_TP2825, 0x1C, 0x09);
        I2C_Write(ADDR_TP2825, 0x1D, 0x38);

        I2C_Write(ADDR_TP2825, 0x20, g_hw_stat.IS_TP2825_L ? 0x30 : 0xA0);
        I2C_Write(ADDR_TP2825, 0x21, 0x86);
        I2C_Write(ADDR_TP2825, 0x22, 0x38);
        I2C_Write(ADDR_TP2825, 0x23, 0x3c);
        // I2C_Write(ADDR_TP2825, 0x24, 0x56);
        I2C_Write(ADDR_TP2825, 0x25, 0xFF);
        I2C_Write(ADDR_TP2825, 0x26, 0x12);
        I2C_Write(ADDR_TP2825, 0x27, 0x2D);
        I2C_Write(ADDR_TP2825, 0x28, 0xC5); //---
        I2C_Write(ADDR_TP2825, 0x29, 0x18); //---
        I2C_Write(ADDR_TP2825, 0x2A, 0x30); //---
        I2C_Write(ADDR_TP2825, 0x2B, 0x70);
        I2C_Write(ADDR_TP2825, 0x2C, 0x1A);
        I2C_Write(ADDR_TP2825, 0x2D, 0x68);
        I2C_Write(ADDR_TP2825, 0x2E, 0x5E);
        I2C_Write(ADDR_TP2825, 0x2F, 0x00);

        I2C_Write(ADDR_TP2825, 0x30, 0x62);
        I2C_Write(ADDR_TP2825, 0x31, 0xBB);
        I2C_Write(ADDR_TP2825, 0x32, 0x96);
        I2C_Write(ADDR_TP2825, 0x33, 0xC0);
        I2C_Write(ADDR_TP2825, 0x34, 0x00);
        I2C_Write(ADDR_TP2825, 0x35, 0x65);
        I2C_Write(ADDR_TP2825, 0x36, 0xDC);
        I2C_Write(ADDR_TP2825, 0x37, 0x00);
        I2C_Write(ADDR_TP2825, 0x38, 0x40);
        I2C_Write(ADDR_TP2825, 0x39, 0x84);
        I2C_Write(ADDR_TP2825, 0x3A, 0x00);
        I2C_Write(ADDR_TP2825, 0x3B, 0x03);
        I2C_Write(ADDR_TP2825, 0x3C, 0x00);
        I2C_Write(ADDR_TP2825, 0x3D, 0x60);
        I2C_Write(ADDR_TP2825, 0x3E, 0x00);
        I2C_Write(ADDR_TP2825, 0x3F, 0x00);

        I2C_Write(ADDR_TP2825, 0x40, 0x00);
        // I2C_Write(ADDR_TP2825, 0x41, 0x00);
        I2C_Write(ADDR_TP2825, 0x42, 0x00);
        I2C_Write(ADDR_TP2825, 0x43, 0x12);
        I2C_Write(ADDR_TP2825, 0x44, 0x07);
        I2C_Write(ADDR_TP2825, 0x45, 0x49);
        I2C_Write(ADDR_TP2825, 0x46, 0x00);
        I2C_Write(ADDR_TP2825, 0x47, 0x00);
        I2C_Write(ADDR_TP2825, 0x48, 0x00);
        I2C_Write(ADDR_TP2825, 0x49, 0x00);
        I2C_Write(ADDR_TP2825, 0x4A, 0x00);
        I2C_Write(ADDR_TP2825, 0x4B, 0x00);
        I2C_Write(ADDR_TP2825, 0x4C, 0x03);
        I2C_Write(ADDR_TP2825, 0x4D, 0x03);
        I2C_Write(ADDR_TP2825, 0x4E, 0x37);
        I2C_Write(ADDR_TP2825, 0x4F, 0x01);
    }

    I2C_Write(ADDR_TP2825, 0x06, TP2825_REG06);
}

void TP2825_Switch_Mode(int is_pal) {
    if (is_pal) {
        I2C_Write(ADDR_TP2825, 0x02, 0xCE);
        I2C_Write(ADDR_TP2825, 0x0D, 0x11);

        I2C_Write(ADDR_TP2825, 0x19, 0x20);
        I2C_Write(ADDR_TP2825, 0x1A, 0x17);
        I2C_Write(ADDR_TP2825, 0x1C, 0x09);
        I2C_Write(ADDR_TP2825, 0x1D, 0x48);
        I2C_Write(ADDR_TP2825, 0x16, 0x4A);
        I2C_Write(ADDR_TP2825, 0x18, 0x17);

        I2C_Write(ADDR_TP2825, 0x20, g_hw_stat.IS_TP2825_L ? 0x40 : 0xB0);
        I2C_Write(ADDR_TP2825, 0x26, 0x02);
        I2C_Write(ADDR_TP2825, 0x2D, 0x60);

        I2C_Write(ADDR_TP2825, 0x30, 0x7A);
        I2C_Write(ADDR_TP2825, 0x31, 0x4A);
        I2C_Write(ADDR_TP2825, 0x32, 0x4D);
        I2C_Write(ADDR_TP2825, 0x33, 0xF0);

        I2C_Write(ADDR_TP2825, 0x4C, 0xC3);
        I2C_Write(ADDR_TP2825, 0x4D, 0x01);
        I2C_Write(ADDR_TP2825, 0x4F, 0x00);
    } else {
        I2C_Write(ADDR_TP2825, 0x02, 0xCF);
        I2C_Write(ADDR_TP2825, 0x0D, 0x10);

        I2C_Write(ADDR_TP2825, 0x19, 0xF0);
        I2C_Write(ADDR_TP2825, 0x1A, 0x07);
        I2C_Write(ADDR_TP2825, 0x1C, 0x09);
        I2C_Write(ADDR_TP2825, 0x1D, 0x38);
        I2C_Write(ADDR_TP2825, 0x16, 0x3C);
        I2C_Write(ADDR_TP2825, 0x18, 0x13);

        I2C_Write(ADDR_TP2825, 0x20, g_hw_stat.IS_TP2825_L ? 0x30 : 0xA0);
        I2C_Write(ADDR_TP2825, 0x26, 0x12);
        I2C_Write(ADDR_TP2825, 0x2D, 0x68);

        I2C_Write(ADDR_TP2825, 0x30, 0x62);
        I2C_Write(ADDR_TP2825, 0x31, 0xBB);
        I2C_Write(ADDR_TP2825, 0x32, 0x96);
        I2C_Write(ADDR_TP2825, 0x33, 0xC0);

        I2C_Write(ADDR_TP2825, 0x4C, 0x03);
        I2C_Write(ADDR_TP2825, 0x4D, 0x03);
        I2C_Write(ADDR_TP2825, 0x4F, 0x01);
    }

    I2C_Write(ADDR_TP2825, 0x06, TP2825_REG06);
}

void TP2825_Switch_CH(uint8_t sel) // 0 = AV in; 1 = Module bay
{
    I2C_Write(ADDR_TP2825, 0x41, sel);
    I2C_Write(ADDR_TP2825, 0x06, TP2825_REG06);
}

void TP2825_Set_Clamp(int idx) {
    static int clamp = -1;
    int clamps[2][3] = {{0x7f, 0x4f, 0x3e}, {0x4f, 0x48, 0x3e}};

    if (clamp != idx) {
        I2C_Write(ADDR_TP2825, 0x23, clamps[g_hw_stat.IS_TP2825_L][idx]);
        // I2C_Write(ADDR_TP2825, 0x06, TP2825_REG06);
        LOGI("Clamp = %x", clamps[g_hw_stat.IS_TP2825_L][idx]);
        clamp = idx;
    }
}
