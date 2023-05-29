#ifndef __DEFINES_H
#define __DEFINES_H

#define DIAL_SENSITIVITY 2    // number of clicks before dial event is triggered
#define DIAL_SENSITIVTY_TIMEOUT_MS 1000 // ms
#define CHANNEL_SHOWTIME 30 // must <= 127

#define GPIO_BEEP 131

#define GPIO_ESP32_BOOT0 128
#define GPIO_ESP32_EN    129
#define GPIO_ESP32_BOOT  130

#define GPIO_TP2825_RSTB 132

#define GPIO_HDZ_RX_RESET 224
#define GPIO_HDZ_TX_RESET 228
#define GPIO_FPGA_RESET   258

#define DEV_SPI_VTX_VRX_R "/dev/mtd8"
#define DEV_SPI_VRX_L     "/dev/mtd9"
#define DEV_SPI_VA        "/dev/mtd10"

#endif //__DEFINES_H
