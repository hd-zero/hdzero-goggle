#pragma once

#define DIAL_SENSITIVITY           1    // number of clicks before dial event is triggered
#define DIAL_SENSITIVTY_TIMEOUT_MS 1000 // ms
#define CHANNEL_SHOWTIME           30   // must <= 127

#define GPIO_BEEP 131

#define GPIO_ESP32_BOOT0 128
#define GPIO_ESP32_EN    129
#define GPIO_ESP32_BOOT  130

#define GPIO_TP2825_RSTB 132
#define GPIO_TP2825_PDN  133
#define GPIO_RTC6715_ON  134
#define GPIO_IS_PRO      136

#define GPIO_HDZ_RX_RESET 224
#define GPIO_HDZ_TX_RESET 228
#define GPIO_FPGA_RESET   258

#define DEV_SPI_VTX_VRX_R "/dev/mtd8"
#define DEV_SPI_VRX_L     "/dev/mtd9"
#define DEV_SPI_VA        "/dev/mtd10"

#define SELF_TEST_FILE "/mnt/extsd/self_test.txt"
#define NO_DIAL_FILE   "/mnt/extsd/no_dial.txt"
#define APP_LOG_FILE   "/mnt/extsd/HDZGOGGLE.log"
#define APP_BIN_FILE   "/mnt/extsd/HDZGOGGLE"
#define DEVELOP_SCRIPT "/mnt/extsd/develop.sh"
