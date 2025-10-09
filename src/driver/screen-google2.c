#include "screen.h"

#if HDZGOGGLE2

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include <log/log.h>

#include "../core/common.hh"
#include "../core/defines.h"
#include "i2c.h"
#include "msp_displayport.h"
#include "uart.h"

#define _OLED_TEMP_TEST

// OLED access
/*
        V536  --m_i2c-->  AL FPGA  --I2C-->  OLED
                                  --RESX 5VEN -5VEN 1V8EN--> OLED

    SPI:
        page: 3bit, addr: 12bit, data: 32bit

    AL FPGA reg:
        reg_a0[1:0]: 1=write cmd; 2=read cmd; self-clear
        reg_a1: addr[7:0]
        reg_a2: addr[15:8]
        reg_a3: wr_data[7:0]
        reg_a4: wr_data[15:8]
        reg_a5: rd_data_right[7:0]   (read only)
        reg_a6: rd_data_right[15:8]  (read only)
        reg_a7: rd_data_left[7:0]    (read only)
        reg_a8: rd_data_left[15:8]   (read only)

*/

static void oled_write(uint16_t addr, uint16_t wdat, uint8_t sel) {
    uint8_t val;

    val = addr & 0xFF;
    I2C_Write(ADDR_FPGA, 0xa9, val);
    val = (addr >> 8) & 0xFF;
    I2C_Write(ADDR_FPGA, 0xaa, val);

    val = wdat & 0xFF;
    I2C_Write(ADDR_FPGA, 0xab, val);
    val = (wdat >> 8) & 0xFF;
    I2C_Write(ADDR_FPGA, 0xac, val);

    val = (sel << 4) | 0x01;
    I2C_Write(ADDR_FPGA, 0xa8, val);

    usleep(250);
}

static uint16_t oled_read(uint16_t addr, uint8_t sel) {
    uint8_t val;
    uint16_t rdat;

    val = addr & 0xFF;
    I2C_Write(ADDR_FPGA, 0xa9, val);
    val = (addr >> 8) & 0xFF;
    I2C_Write(ADDR_FPGA, 0xaa, val);

    val = (sel << 4) | 0x02;
    I2C_Write(ADDR_FPGA, 0xa8, val);

    usleep(500);

    val = I2C_Read(ADDR_FPGA, 0xae);
    rdat = val;
    rdat <<= 8;
    val = I2C_Read(ADDR_FPGA, 0xad);
    rdat |= val;

    usleep(250);
    return rdat;
}

static void screen_start_up() {
    uint16_t l0, l1, l2, l3, l4;
    uint16_t r0, r1, r2, r3, r4;

    I2C_Write(ADDR_FPGA, 0xa0, 0x01);
    usleep(1000);

#ifdef _OLED_TEMP_TEST
    oled_write(0xF000, 0x00AA, 2);
    oled_write(0xF001, 0x0012, 2);

    l0 = oled_read(0xD000, 0);
    l1 = oled_read(0xD001, 0);
    l2 = oled_read(0xD002, 0);
    l3 = oled_read(0xD003, 0);
    l4 = oled_read(0xD004, 0);

    r0 = oled_read(0xD000, 1);
    r1 = oled_read(0xD001, 1);
    r2 = oled_read(0xD002, 1);
    r3 = oled_read(0xD003, 1);
    r4 = oled_read(0xD004, 1);

    LOGI("OLED temp test: 0xD0 L = %x  %x  %x  %x  %x  ", l0, l1, l2, l3, l4);
    LOGI("OLED temp test: 0xD0 R = %x  %x  %x  %x  %x  ", r0, r1, r2, r3, r4);

    if (l0 == 0x05) {
        oled_write(0xD000, 0x000A, 0);
        oled_write(0xD001, 0x000A, 0);
        oled_write(0xD002, l2 + 32, 0);
        oled_write(0xD003, l3 + 32, 0);
        oled_write(0xD004, l4 + 32, 0);
    }

    if (r0 == 0x05) {
        oled_write(0xD000, 0x000A, 1);
        oled_write(0xD001, 0x000A, 1);
        oled_write(0xD002, r2 + 32, 1);
        oled_write(0xD003, r3 + 32, 1);
        oled_write(0xD004, r4 + 32, 1);
    }

    l0 = oled_read(0xD000, 0);
    l1 = oled_read(0xD001, 0);
    l2 = oled_read(0xD002, 0);
    l3 = oled_read(0xD003, 0);
    l4 = oled_read(0xD004, 0);

    r0 = oled_read(0xD000, 1);
    r1 = oled_read(0xD001, 1);
    r2 = oled_read(0xD002, 1);
    r3 = oled_read(0xD003, 1);
    r4 = oled_read(0xD004, 1);

    LOGI("OLED temp test modified: 0xD0 L = %x  %x  %x  %x  %x  ", l0, l1, l2, l3, l4);
    LOGI("OLED temp test modified: 0xD0 R = %x  %x  %x  %x  %x  ", r0, r1, r2, r3, r4);
#endif
}
// OLED display on/off
static void screen_display(bool on) {
    static int last_on = -1;

    if (last_on != on)
        last_on = on;
    else
        return;

    if (on) {
        I2C_Write(ADDR_FPGA, 0xa3, 0x83);
        usleep(1000);
        oled_write(0x8000, 0x0001, 2);
        usleep(5000);
        oled_write(0x2900, 0x0000, 2); // display on
        usleep(20000);
        oled_write(0x5300, 0x0029, 2);
        oled_write(0x5100, 0x00FF, 2);
        oled_write(0x5101, 0x0001, 2);
        oled_write(0x0300, 0x0000, 2);
        usleep(1000);
        I2C_Write(ADDR_FPGA, 0xa3, 0x03);
        LOGI("OLED: Display on");
    } else {
        I2C_Write(ADDR_FPGA, 0xa3, 0x83);
        oled_write(0x2800, 0x0000, 2); // display off
        usleep(20000);
        I2C_Write(ADDR_FPGA, 0xa3, 0x80);
        LOGI("OLED: Display off");
    }
}

// Set OLED to pattern mode
// enable: 0=disable; 1=enable
// mode: 0=color bar; 1=grid; 2=all black; 3=all white; 4=boot screen
// speed: color bar move speed (0~15)
static void screen_pattern(bool enable, uint8_t mode, uint8_t speed) {
    mode = (enable & 0x01) | ((mode & 0x07) << 1) | ((speed & 0x0F) << 4);

    screen_display(0);
    I2C_Write(ADDR_FPGA, 0xa4, mode);
    screen_display(1);
}

static void screen_vtmg(int mode) {
    static int last_mode = 0;

    if (last_mode != mode) {
        last_mode = mode;
        switch (mode) {
        case 0:
            oled_write(0x8001, 0x00E0, 2);
            oled_write(0x6900, 0x0000, 2);
            break;
        case 1:
            oled_write(0x8001, 0x0040, 2);
            oled_write(0x6900, 0x0002, 2);
            break;
        case 2:
            oled_write(0x8001, 0x0068, 2);
            oled_write(0x6900, 0x0001, 2);
            break;
        }
        LOGI("OLED: Set to mode %d.", mode);
    }
}

static void MFPGA_SetRatio(bool ratio) {
    if (ratio)
        I2C_Write(ADDR_FPGA, 0x8f, 0x80);
    else
        I2C_Write(ADDR_FPGA, 0x8f, 0x00);
    // LOGI("MFPGA_SetRatio %d",ratio);
}

static void MFPGA_Set720P90(uint8_t mode) {
    I2C_Write(ADDR_FPGA, 0x40, 0xc0);
    I2C_Write(ADDR_FPGA, 0x41, 0x23);
    I2C_Write(ADDR_FPGA, 0x42, 0x1c);
    I2C_Write(ADDR_FPGA, 0x43, 0xaa);
    I2C_Write(ADDR_FPGA, 0x44, 0x45);
    I2C_Write(ADDR_FPGA, 0x45, 0x39);
    I2C_Write(ADDR_FPGA, 0x46, 0x00);
    I2C_Write(ADDR_FPGA, 0x47, 0x00);
    I2C_Write(ADDR_FPGA, 0x48, 0x28);
    I2C_Write(ADDR_FPGA, 0x49, 0xdd);
    I2C_Write(ADDR_FPGA, 0x4a, 0x01);
    I2C_Write(ADDR_FPGA, 0x4b, 0x05);
    I2C_Write(ADDR_FPGA, 0x4c, 0x11);

    if (mode == VR_540P90_CROP) {
        I2C_Write(ADDR_FPGA, 0x4d, 0xE2);
        I2C_Write(ADDR_FPGA, 0x4e, 0x04);
    } else {
        I2C_Write(ADDR_FPGA, 0x4d, 0x30);
        I2C_Write(ADDR_FPGA, 0x4e, 0x05);
    }

    I2C_Write(ADDR_FPGA, 0x4f, 0x00);
    I2C_Write(ADDR_FPGA, 0x52, 0x48);
    I2C_Write(ADDR_FPGA, 0x53, 0x48);
    I2C_Write(ADDR_FPGA, 0x54, 0x66);
    I2C_Write(ADDR_FPGA, 0x61, 0x71);
    I2C_Write(ADDR_FPGA, 0x63, 0x5a);
    I2C_Write(ADDR_FPGA, 0x65, 0x96);
    I2C_Write(ADDR_FPGA, 0x66, 0x00);

    MFPGA_SetRatio(1);
    I2C_Write(ADDR_FPGA, 0x06, 0x0F);
}

static void MFPGA_Set540P60() {
    I2C_Write(ADDR_FPGA, 0x40, 0xc0);
    I2C_Write(ADDR_FPGA, 0x41, 0x23);
    I2C_Write(ADDR_FPGA, 0x42, 0x1c);
    I2C_Write(ADDR_FPGA, 0x43, 0x4b);
    I2C_Write(ADDR_FPGA, 0x44, 0x44);
    I2C_Write(ADDR_FPGA, 0x45, 0x33);
    I2C_Write(ADDR_FPGA, 0x46, 0x00);
    I2C_Write(ADDR_FPGA, 0x47, 0x00);
    I2C_Write(ADDR_FPGA, 0x48, 0x28);
    I2C_Write(ADDR_FPGA, 0x49, 0x84);
    I2C_Write(ADDR_FPGA, 0x4a, 0x00);
    I2C_Write(ADDR_FPGA, 0x4b, 0x05);
    I2C_Write(ADDR_FPGA, 0x4c, 0x11);

    I2C_Write(ADDR_FPGA, 0x4d, 0xd0);
    I2C_Write(ADDR_FPGA, 0x4e, 0x07);

    I2C_Write(ADDR_FPGA, 0x4f, 0x00);
    I2C_Write(ADDR_FPGA, 0x52, 0x48);
    I2C_Write(ADDR_FPGA, 0x53, 0x48);
    I2C_Write(ADDR_FPGA, 0x54, 0x66);
    I2C_Write(ADDR_FPGA, 0x61, 0x71);
    I2C_Write(ADDR_FPGA, 0x63, 0x5a);
    I2C_Write(ADDR_FPGA, 0x65, 0x96);
    I2C_Write(ADDR_FPGA, 0x66, 0x00);

    MFPGA_SetRatio(1);
    I2C_Write(ADDR_FPGA, 0x06, 0x0F);
}

static void MFPGA_Set720P60(uint8_t mode, bool is_43) {
    I2C_Write(ADDR_FPGA, 0x40, 0x00);
    I2C_Write(ADDR_FPGA, 0x41, 0x25);
    I2C_Write(ADDR_FPGA, 0x42, 0xd0);
    I2C_Write(ADDR_FPGA, 0x43, 0x72);
    I2C_Write(ADDR_FPGA, 0x44, 0x46);
    I2C_Write(ADDR_FPGA, 0x45, 0xee);
    I2C_Write(ADDR_FPGA, 0x46, 0x00);
    I2C_Write(ADDR_FPGA, 0x47, 0x00);
    I2C_Write(ADDR_FPGA, 0x48, 0x28);
    I2C_Write(ADDR_FPGA, 0x49, 0xf7);
    I2C_Write(ADDR_FPGA, 0x4a, 0x00);
    I2C_Write(ADDR_FPGA, 0x4b, 0x05);
    I2C_Write(ADDR_FPGA, 0x4c, 0x19);

    if (mode == VR_960x720P60) {
        I2C_Write(ADDR_FPGA, 0x4d, 0xDC);
        I2C_Write(ADDR_FPGA, 0x4e, 0x05);
    } else if (mode == VR_720P50) {
        I2C_Write(ADDR_FPGA, 0x4d, 0xBC);
        I2C_Write(ADDR_FPGA, 0x4e, 0x07);
    } else {
        I2C_Write(ADDR_FPGA, 0x4d, 0x72);
        I2C_Write(ADDR_FPGA, 0x4e, 0x06);
    }

    I2C_Write(ADDR_FPGA, 0x4f, 0x00);
    I2C_Write(ADDR_FPGA, 0x52, 0x5f);
    I2C_Write(ADDR_FPGA, 0x53, 0x5f);
    I2C_Write(ADDR_FPGA, 0x54, 0x88);
    I2C_Write(ADDR_FPGA, 0x61, 0x96);
    I2C_Write(ADDR_FPGA, 0x63, 0x78);
    I2C_Write(ADDR_FPGA, 0x65, 0xc8);
    I2C_Write(ADDR_FPGA, 0x66, 0x00);

    MFPGA_SetRatio(is_43);
    I2C_Write(ADDR_FPGA, 0x06, 0x0F);
}

static void MFPGA_Set1080P30() {
    I2C_Write(ADDR_FPGA, 0x40, 0x80);
    I2C_Write(ADDR_FPGA, 0x41, 0x47);
    I2C_Write(ADDR_FPGA, 0x42, 0x38);
    I2C_Write(ADDR_FPGA, 0x43, 0x9a);
    I2C_Write(ADDR_FPGA, 0x44, 0x88);
    I2C_Write(ADDR_FPGA, 0x45, 0x64);
    I2C_Write(ADDR_FPGA, 0x46, 0x00);
    I2C_Write(ADDR_FPGA, 0x47, 0x00);
    I2C_Write(ADDR_FPGA, 0x48, 0x2c);
    I2C_Write(ADDR_FPGA, 0x49, 0xa9);
    I2C_Write(ADDR_FPGA, 0x4a, 0x00);
    I2C_Write(ADDR_FPGA, 0x4b, 0x05);
    I2C_Write(ADDR_FPGA, 0x4c, 0x28);

    I2C_Write(ADDR_FPGA, 0x4d, 0x98);
    I2C_Write(ADDR_FPGA, 0x4e, 0x08);

    I2C_Write(ADDR_FPGA, 0x4f, 0x00);
    I2C_Write(ADDR_FPGA, 0x52, 0x8f);
    I2C_Write(ADDR_FPGA, 0x53, 0x8f);
    I2C_Write(ADDR_FPGA, 0x54, 0xcc);
    I2C_Write(ADDR_FPGA, 0x61, 0xe1);
    I2C_Write(ADDR_FPGA, 0x63, 0xb4);
    I2C_Write(ADDR_FPGA, 0x65, 0x2c);
    I2C_Write(ADDR_FPGA, 0x66, 0x01);

    MFPGA_SetRatio(0);
    I2C_Write(ADDR_FPGA, 0x06, 0x0F);
}

// OLED brightness setting
static void screen_brightness(uint8_t level) {
    uint16_t dh = 0, dl = 0;

    switch (level) {
    case 12:
        dh = 0x0002;
        dl = 0x00F0;
        break;
    case 11:
        dh = 0x0002;
        dl = 0x00C0;
        break;
    case 10:
        dh = 0x0002;
        dl = 0x0090;
        break;
    case 9:
        dh = 0x0002;
        dl = 0x0060;
        break;
    case 8:
        dh = 0x0002;
        dl = 0x0030;
        break;
    case 7:
        dh = 0x0002;
        dl = 0x0000;
        break;
    case 6:
        dh = 0x0001;
        dl = 0x00D0;
        break;
    case 5:
        dh = 0x0001;
        dl = 0x00A0;
        break;
    case 4:
        dh = 0x0001;
        dl = 0x0070;
        break;
    case 3:
        dh = 0x0001;
        dl = 0x0040;
        break;
    case 2:
        dh = 0x0001;
        dl = 0x0010;
        break;
    case 1:
        dh = 0x0000;
        dl = 0x00E0;
        break;
    case 0:
        dh = 0x0000;
        dl = 0x0020;
        break;
    }

    oled_write(0xF000, 0x00AA, 2);
    oled_write(0xF001, 0x0011, 2);

    oled_write(0xC200, dh, 2);
    oled_write(0xC201, dl, 2);
    oled_write(0xC202, dh, 2);
    oled_write(0xC203, dl, 2);
    oled_write(0xC204, dh, 2);
    oled_write(0xC205, dl, 2);
    oled_write(0xC206, dh, 2);
    oled_write(0xC207, dl, 2);
}

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
