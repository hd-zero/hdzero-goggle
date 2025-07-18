#include "rtc6715.h"

#if defined(HDZBOXPRO) || defined(HDZGOGGLE2)

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include <log/log.h>

#include "../core/common.hh"
#include "../core/defines.h"
#include "driver/dm5680.h"
#include "driver/gpio.h"
#include "gpadc.h"
#include "i2c.h"

static void MM_Write(uint8_t addr, uint32_t dat) {
    uint8_t val;
#if defined(HDZBOXPRO)
    // spi_addr
    I2C_Write(ADDR_FPGA, 0xa1, addr);
    // spi_wdat
    val = dat & 0xFF;
    I2C_Write(ADDR_FPGA, 0xa2, val);
    val = (dat >> 8) & 0xFF;
    I2C_Write(ADDR_FPGA, 0xa3, val);
    val = (dat >> 16) & 0xFF;
    I2C_Write(ADDR_FPGA, 0xa4, val);
    // wrte cmd
    I2C_Write(ADDR_FPGA, 0xa0, 0x01);
#elif defined(HDZGOGGLE2)
    // spi_addr
    I2C_Write(ADDR_FPGA, 0xb1, addr);

    // spi_wdat
    val = dat & 0xFF;
    I2C_Write(ADDR_FPGA, 0xb2, val);
    val = (dat >> 8) & 0xFF;
    I2C_Write(ADDR_FPGA, 0xb3, val);
    val = (dat >> 16) & 0xFF;
    I2C_Write(ADDR_FPGA, 0xb4, val);

    // wrte cmd
    I2C_Write(ADDR_FPGA, 0xb0, 0x01);
#endif
    usleep(10000);
}

static void rtc6715_power(bool is_on) {
#if defined(HDZBOXPRO)
    gpio_set(GPIO_RTC6715_ON, is_on);
#elif defined(HDZGOGGLE2)
    DM5680_InternalAnalog_Power(is_on);
#endif
    I2C_Write(ADDR_FPGA, 0x8C, (is_on << 1)); // rf switch

    if (is_on) {
        usleep(200 * 1000); // wait for power stable
    }
    LOGI("RTC6715 power %s", is_on ? "on" : "off");
}

static void rtc6715_audio(bool is_on) {
    uint32_t audio_val[2] = {0x10df3, 0x10c13};
    MM_Write(0x0a, audio_val[is_on]);
    LOGI("RTC6715 audio %s", is_on ? "on" : "off");
}

static void rtc6715_init(bool power_on, bool audio_on) {

    gpadc_on(power_on);
    rtc6715_power(power_on);
    rtc6715_audio(audio_on);
}

static void rtc6715_set_ch(int ch) {
    const uint32_t tab[48] = {
        0x2a05, 0x299b, 0x2991, 0x2987, 0x291d, 0x2913, 0x2909, 0x289f, // band A
        0x2903, 0x290c, 0x2916, 0x291f, 0x2989, 0x2992, 0x299c, 0x2a05, // band B
        0x2895, 0x288b, 0x2881, 0x2817, 0x2a0f, 0x2a19, 0x2a83, 0x2a8d, // band E
        0x2906, 0x2910, 0x291a, 0x2984, 0x298e, 0x2998, 0x2a02, 0x2a0c, // band F
        0x281d, 0x2890, 0x2902, 0x2915, 0x2987, 0x299a, 0x2a0c, 0x2a1f, // band R
        0x2609, 0x2614, 0x2686, 0x2701, 0x2713, 0x2786, 0x2798, 0x280b, // band L
    };

    MM_Write(0x00, 0x08);
    MM_Write(0x01, tab[ch]);

    LOGI("Set_RTC6715: %d", (uint16_t)ch);
}

int rtc6715_get_rssi() {
    static int rssi_adc = 0;
    int value = gpdac0_get();

    if (value < 0)
        return rssi_adc;

    rssi_adc = 3300 * value / 4096;
    // LOGI("rssi voltage: %02f", (float)rssi_adc / 1000);
    return rssi_adc;
}
#else
static void rtc6715_init(bool power_on, bool audio_on) {
}
static void rtc6715_set_ch(int ch) {
}
static int rtc6715_get_rssi() {
    return 0;
}
#endif

rtc6715_t rtc6715 = {
    .init = rtc6715_init,
    .set_ch = rtc6715_set_ch,
    .get_rssi = rtc6715_get_rssi,
};
