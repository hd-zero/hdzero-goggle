#include <stdint.h>
#include <stdlib.h>
 #include <unistd.h>
#include "i2c.h"
#include "../core/common.hh"

void TP2825_close()
{
    set_gpio(GPIO_TP2825_RSTB,0);
}

void TP2825_open()
{
    set_gpio(GPIO_TP2825_RSTB,1);
}

void TP2825_Config(int ch_sel, int is_pal) //ch_sel: 0=AV in; 1=Module bay
{
    TP2825_close();
    usleep(1000);
    TP2825_open();
    usleep(1000);

    I2C_Write(ADDR_TP2825, 0x41, ch_sel);

    if(is_pal) {
        I2C_Write(ADDR_TP2825, 0x02, 0xCE);
        I2C_Write(ADDR_TP2825, 0x07, 0xC0);
        I2C_Write(ADDR_TP2825, 0x0B, 0xC0);
        I2C_Write(ADDR_TP2825, 0x0C, 0x53);
        I2C_Write(ADDR_TP2825, 0x0D, 0x11);
        //I2C_Write(ADDR_TP2825, 0x15, 0x13);
        I2C_Write(ADDR_TP2825, 0x16, 0x4A);
        I2C_Write(ADDR_TP2825, 0x17, 0xC0);
        I2C_Write(ADDR_TP2825, 0x18, 0x17);
        I2C_Write(ADDR_TP2825, 0x19, 0x20);
        I2C_Write(ADDR_TP2825, 0x1A, 0x17);
        I2C_Write(ADDR_TP2825, 0x20, 0xB0);
        //I2C_Write(ADDR_TP2825, 0x22, 0x39);
        //I2C_Write(ADDR_TP2825, 0x23, 0x3c);
        I2C_Write(ADDR_TP2825, 0x26, 0x02);
        //I2C_Write(ADDR_TP2825, 0x28, 0x41);
        I2C_Write(ADDR_TP2825, 0x2B, 0x70);
        I2C_Write(ADDR_TP2825, 0x2D, 0x60);
        I2C_Write(ADDR_TP2825, 0x2E, 0x5E);
        I2C_Write(ADDR_TP2825, 0x30, 0x7A);
        I2C_Write(ADDR_TP2825, 0x31, 0x4A);
        I2C_Write(ADDR_TP2825, 0x32, 0x4D);
        I2C_Write(ADDR_TP2825, 0x33, 0xF0);
        I2C_Write(ADDR_TP2825, 0x35, 0x65);
        I2C_Write(ADDR_TP2825, 0x39, 0x84);
        I2C_Write(ADDR_TP2825, 0x4C, 0x03);
        I2C_Write(ADDR_TP2825, 0x4D, 0x03);
        I2C_Write(ADDR_TP2825, 0x4E, 0x37);
        I2C_Write(ADDR_TP2825, 0x1C, 0x09);
        I2C_Write(ADDR_TP2825, 0x1D, 0x48);
    }
    else {
        I2C_Write(ADDR_TP2825, 0x02, 0xCF);
        I2C_Write(ADDR_TP2825, 0x07, 0xC0);
        I2C_Write(ADDR_TP2825, 0x0B, 0xC0);
        I2C_Write(ADDR_TP2825, 0x0C, 0x53);
        I2C_Write(ADDR_TP2825, 0x0D, 0x10);
        //I2C_Write(ADDR_TP2825, 0x15, 0x13);
        I2C_Write(ADDR_TP2825, 0x16, 0x3C);
        I2C_Write(ADDR_TP2825, 0x17, 0xC0);
        I2C_Write(ADDR_TP2825, 0x18, 0x13);
        I2C_Write(ADDR_TP2825, 0x19, 0xF0);
        I2C_Write(ADDR_TP2825, 0x1A, 0x07);
        I2C_Write(ADDR_TP2825, 0x20, 0xA0);
        //I2C_Write(ADDR_TP2825, 0x22, 0x39);
        //I2C_Write(ADDR_TP2825, 0x23, 0x3c);
        I2C_Write(ADDR_TP2825, 0x26, 0x12);
        //I2C_Write(ADDR_TP2825, 0x28, 0x41);
        I2C_Write(ADDR_TP2825, 0x2B, 0x70);
        I2C_Write(ADDR_TP2825, 0x2D, 0x68);
        I2C_Write(ADDR_TP2825, 0x2E, 0x5E);
        I2C_Write(ADDR_TP2825, 0x30, 0x62);
        I2C_Write(ADDR_TP2825, 0x31, 0xBB);
        I2C_Write(ADDR_TP2825, 0x32, 0x96);
        I2C_Write(ADDR_TP2825, 0x33, 0xC0);
        I2C_Write(ADDR_TP2825, 0x35, 0x65);
        I2C_Write(ADDR_TP2825, 0x39, 0x84);
        I2C_Write(ADDR_TP2825, 0x4C, 0x03);
        I2C_Write(ADDR_TP2825, 0x4D, 0x03);
        I2C_Write(ADDR_TP2825, 0x4E, 0x37);
        I2C_Write(ADDR_TP2825, 0x1C, 0x09);
        I2C_Write(ADDR_TP2825, 0x1D, 0x38);
    }
}

void TP2825_Switch_CH(uint8_t sel) // 0 = AV in; 1 = Module bay
{
    I2C_Write(ADDR_TP2825, 0x41, sel);
    I2C_Write(ADDR_TP2825, 0x06, 0xB2);
}
