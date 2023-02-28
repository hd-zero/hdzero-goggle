#include "it66121.h"
#include "../core/common.hh"
#include "dm5680.h"
#include "i2c.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
    I2C_R_Write(ADDR_IT66121, 0x61, 0x20);
    I2C_R_Write(ADDR_IT66121, 0x62, 0x18);
    I2C_R_Write(ADDR_IT66121, 0x64, 0x1d);
    I2C_R_Write(ADDR_IT66121, 0x68, 0x10);
    I2C_R_Write(ADDR_IT66121, 0x04, 0x3c);
    I2C_R_Write(ADDR_IT66121, 0x04, 0x3d);
    I2C_R_Write(ADDR_IT66121, 0x0f, 0x08);
    I2C_R_Write(ADDR_IT66121, 0xd1, 0x0c);
    I2C_R_Write(ADDR_IT66121, 0x65, 0x00);
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
    I2C_R_Write(ADDR_IT66121, 0x6a, 0xff);
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

    I2C_R_Write(ADDR_IT66121, 0x90, 0x20);
    I2C_R_Write(ADDR_IT66121, 0x91, 0x67);
    I2C_R_Write(ADDR_IT66121, 0x92, 0x70);
    I2C_R_Write(ADDR_IT66121, 0x93, 0x70);
    I2C_R_Write(ADDR_IT66121, 0x94, 0x61);
    I2C_R_Write(ADDR_IT66121, 0x95, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x96, 0x28);
    I2C_R_Write(ADDR_IT66121, 0x97, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x98, 0xee);
    I2C_R_Write(ADDR_IT66121, 0x99, 0x02);
    I2C_R_Write(ADDR_IT66121, 0x9a, 0x1d);
    I2C_R_Write(ADDR_IT66121, 0x9b, 0xed);
    I2C_R_Write(ADDR_IT66121, 0x9c, 0x20);
    I2C_R_Write(ADDR_IT66121, 0xa0, 0x00);
    I2C_R_Write(ADDR_IT66121, 0xa1, 0x50);
    I2C_R_Write(ADDR_IT66121, 0xa2, 0x00);
    I2C_R_Write(ADDR_IT66121, 0xa3, 0x00);
    I2C_R_Write(ADDR_IT66121, 0xa6, 0x00);
    I2C_R_Write(ADDR_IT66121, 0xb1, 0x00);
    I2C_R_Write(ADDR_IT66121, 0xc0, 0x01);
    I2C_R_Write(ADDR_IT66121, 0x4f, 0xf0);
    I2C_R_Write(ADDR_IT66121, 0xc1, 0x00);
    I2C_R_Write(ADDR_IT66121, 0xc6, 0x03);
    I2C_R_Write(ADDR_IT66121, 0x61, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x0f, 0x09);
    I2C_R_Write(ADDR_IT66121, 0x68, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x69, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x6a, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x6b, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x6c, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x6d, 0x71);
    I2C_R_Write(ADDR_IT66121, 0x0f, 0x09);
    I2C_R_Write(ADDR_IT66121, 0x30, 0x0a);
    I2C_R_Write(ADDR_IT66121, 0x31, 0x22);
    I2C_R_Write(ADDR_IT66121, 0x32, 0x01);
    I2C_R_Write(ADDR_IT66121, 0x33, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x34, 0x01);
    I2C_R_Write(ADDR_IT66121, 0x35, 0x00);
    I2C_R_Write(ADDR_IT66121, 0xf8, 0xc3);
    I2C_R_Write(ADDR_IT66121, 0xc5, 0x06);
    I2C_R_Write(ADDR_IT66121, 0xf8, 0xff);
    I2C_R_Write(ADDR_IT66121, 0x0f, 0x08);
    I2C_R_Write(ADDR_IT66121, 0x04, 0x04);
    I2C_R_Write(ADDR_IT66121, 0xe0, 0x01);
    I2C_R_Write(ADDR_IT66121, 0xe1, 0x05);
    I2C_R_Write(ADDR_IT66121, 0xe2, 0xe4);
    I2C_R_Write(ADDR_IT66121, 0xe3, 0x00);
    I2C_R_Write(ADDR_IT66121, 0xe4, 0x00);
    I2C_R_Write(ADDR_IT66121, 0xe5, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x04, 0x00);
    I2C_R_Write(ADDR_IT66121, 0x0f, 0x09);
    I2C_R_Write(ADDR_IT66121, 0x91, 0x01);
    I2C_R_Write(ADDR_IT66121, 0x93, 0x01);
    I2C_R_Write(ADDR_IT66121, 0x98, 0x02);
    I2C_R_Write(ADDR_IT66121, 0x99, 0xd2);
    I2C_R_Write(ADDR_IT66121, 0x0f, 0x08);
}
