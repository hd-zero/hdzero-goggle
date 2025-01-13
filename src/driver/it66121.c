#include "it66121.h"
#include "../core/common.hh"
#include "dm5680.h"
#include "i2c.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

uint8_t it66121_vi_phase = 0;

void IT66121_close() {
    DM5680_ResetHDMI_TX(0);
}

void IT66121_init() {
    DM5680_ResetHDMI_TX(0);
    usleep(1000);
    DM5680_ResetHDMI_TX(1);
    usleep(10000);

    I2C_R_Write(ADDR_IT66121, 0x0f, 0x08);
    I2C_R_Write(ADDR_IT66121, 0x05, 0x60);
    I2C_R_Write(ADDR_IT66121, 0x61, 0x30);
    I2C_R_Write(ADDR_IT66121, 0x62, 0x80);
    I2C_R_Write(ADDR_IT66121, 0x64, 0x90);
    usleep(1000);

    I2C_R_Write(ADDR_IT66121, 0x62, 0x18);
    I2C_R_Write(ADDR_IT66121, 0x64, 0x1d);
    I2C_R_Write(ADDR_IT66121, 0x68, 0x10);
    I2C_R_Write(ADDR_IT66121, 0x04, 0x3c);
    I2C_R_Write(ADDR_IT66121, 0x04, 0x3d);
    usleep(1000);

    I2C_R_Write(ADDR_IT66121, 0x0f, 0x08);
    I2C_R_Write(ADDR_IT66121, 0xd1, 0x0c);
    I2C_R_Write(ADDR_IT66121, 0x71, 0x18);
    I2C_R_Write(ADDR_IT66121, 0x0f, 0x08);
    I2C_R_Write(ADDR_IT66121, 0xf8, 0xc3);
    I2C_R_Write(ADDR_IT66121, 0xf8, 0xa5);
    I2C_R_Write(ADDR_IT66121, 0x20, 0x88);
    I2C_R_Write(ADDR_IT66121, 0x37, 0x62);
    I2C_R_Write(ADDR_IT66121, 0x20, 0x08);
    I2C_R_Write(ADDR_IT66121, 0xf8, 0xff);
    I2C_R_Write(ADDR_IT66121, 0x59, 0x40);
    I2C_R_Write(ADDR_IT66121, 0xe1, 0x41);
    I2C_R_Write(ADDR_IT66121, 0x05, 0x60);
    I2C_R_Write(ADDR_IT66121, 0x0c, 0xff);
    I2C_R_Write(ADDR_IT66121, 0x0d, 0xff);
    I2C_R_Write(ADDR_IT66121, 0x0e, 0x4f);
    I2C_R_Write(ADDR_IT66121, 0x0c, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x0d, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x0e, 0x4c);
    I2C_R_Write(ADDR_IT66121, 0x09, 0xff);
    I2C_R_Write(ADDR_IT66121, 0x20, 0x08);
    I2C_R_Write(ADDR_IT66121, 0x6a, 0xff);
    I2C_R_Write(ADDR_IT66121, 0xc1, 0x01);
    I2C_R_Write(ADDR_IT66121, 0x72, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x70, 0x48);
    I2C_R_Write(ADDR_IT66121, 0x0f, 0x08);
    I2C_R_Write(ADDR_IT66121, 0x72, 0x03);
    I2C_R_Write(ADDR_IT66121, 0x73, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x74, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x75, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x76, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x77, 0x08);
    I2C_R_Write(ADDR_IT66121, 0x78, 0x55);
    I2C_R_Write(ADDR_IT66121, 0x79, 0x3c);
    I2C_R_Write(ADDR_IT66121, 0x7a, 0x88);
    I2C_R_Write(ADDR_IT66121, 0x7b, 0x3e);
    I2C_R_Write(ADDR_IT66121, 0x7c, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x7d, 0x08);
    I2C_R_Write(ADDR_IT66121, 0x7e, 0x51);
    I2C_R_Write(ADDR_IT66121, 0x7f, 0x0c);
    I2C_R_Write(ADDR_IT66121, 0x80, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x81, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x82, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x83, 0x08);
    I2C_R_Write(ADDR_IT66121, 0x84, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x85, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x86, 0x84);
    I2C_R_Write(ADDR_IT66121, 0x87, 0x0e);
    I2C_R_Write(ADDR_IT66121, 0x88, 0x03);
    I2C_R_Write(ADDR_IT66121, 0x04, 0x14);
    I2C_R_Write(ADDR_IT66121, 0x90, 0x70);
    I2C_R_Write(ADDR_IT66121, 0x91, 0x89);
    I2C_R_Write(ADDR_IT66121, 0x92, 0xc0);
    I2C_R_Write(ADDR_IT66121, 0x93, 0x40);
    I2C_R_Write(ADDR_IT66121, 0x94, 0x80);
    I2C_R_Write(ADDR_IT66121, 0x95, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x96, 0x2c);
    I2C_R_Write(ADDR_IT66121, 0x97, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x98, 0x64);
    I2C_R_Write(ADDR_IT66121, 0x99, 0x04);
    I2C_R_Write(ADDR_IT66121, 0x9a, 0x28);
    I2C_R_Write(ADDR_IT66121, 0x9b, 0x60);
    I2C_R_Write(ADDR_IT66121, 0x9c, 0x40);
    I2C_R_Write(ADDR_IT66121, 0x9d, 0xff);
    I2C_R_Write(ADDR_IT66121, 0x9e, 0xff);
    I2C_R_Write(ADDR_IT66121, 0x9f, 0xff);
    I2C_R_Write(ADDR_IT66121, 0xa0, 0x00);
    I2C_R_Write(ADDR_IT66121, 0xa1, 0x50);
    I2C_R_Write(ADDR_IT66121, 0xa2, 0xff);
    I2C_R_Write(ADDR_IT66121, 0xa3, 0xff);
    I2C_R_Write(ADDR_IT66121, 0xa4, 0x4c);
    I2C_R_Write(ADDR_IT66121, 0xa5, 0x04);
    I2C_R_Write(ADDR_IT66121, 0xa6, 0xf0);
    I2C_R_Write(ADDR_IT66121, 0xb1, 0x00);
    I2C_R_Write(ADDR_IT66121, 0xb2, 0x00);
    I2C_R_Write(ADDR_IT66121, 0xc0, 0x01);
    I2C_R_Write(ADDR_IT66121, 0x4f, 0xf0);
    I2C_R_Write(ADDR_IT66121, 0x64, 0x94);
    I2C_R_Write(ADDR_IT66121, 0x68, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x61, 0x02);
    I2C_R_Write(ADDR_IT66121, 0x63, 0x20);
    I2C_R_Write(ADDR_IT66121, 0x66, 0x17);
    I2C_R_Write(ADDR_IT66121, 0x65, 0x80);
    I2C_R_Write(ADDR_IT66121, 0x61, 0x02);
    I2C_R_Write(ADDR_IT66121, 0xc1, 0x00);
    I2C_R_Write(ADDR_IT66121, 0xc6, 0x03);
}

void IT66121_set_phase(uint8_t phase, uint8_t inv) {
    uint8_t rdat;

    inv &= 1;
    I2C_R_Write(ADDR_IT66121, 0x59, 0x40 | (inv << 3));

    rdat = I2C_R_Read(ADDR_IT66121, 0x02);
    if (rdat == 0x12) {
        I2C_R_Write(ADDR_IT66121, 0x70, (0x48 | phase));
    } else if (rdat == 0x22) {
        I2C_R_Write(ADDR_IT66121, 0x72, (0x03 | (phase << 6)));
    }
}