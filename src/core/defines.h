#ifndef __DEFINES_H
#define __DEFINES_H

typedef enum{
    OPLEVEL_MAINMENU = 0,
    OPLEVEL_SUBMENU = 1,
    OPLEVEL_PLAYBACK = 2,
    OPLEVEL_VIDEO = 10,
    OPLEVEL_IMS = 11,
    PAGE_FAN_SLIDE = 100,
    PAGE_ANGLE_SLIDE = 101,
    PAGE_POWER_SLIDE = 102,
} op_level_t ;

typedef enum{
    LEFT_DAIL_UP = 1,
    LEFT_DAIL_DOWN = 2,
    LEFT_DAIL_CLICK = 3,
    LEFT_DAIL_LONGPRESS = 4
} left_dial_t ;

#define DIAL_SENSITIVITY    1   //slow down rate =  DIAL_SENSITIVITY+1 
#define CHANNEL_SHOWTIME	30  //must <= 127

#define GPIO_ESP32_BOOT0    128
#define GPIO_ESP32_EN       129
#define GPIO_ESP32_BOOT     130 //not used
#define GPIO_BEEP           131
#define GPIO_RXRESET        224
#define GPIO_TXRESET        228
#define GPIO_TP2825_RSTB    132

#define CLASS_PATH_GPIO		"/sys/class/gpio"
#define DEV_SPI_VTX_VRX_R	"/dev/mtd8"
#define DEV_SPI_VRX_L  		"/dev/mtd9"
#define DEV_SPI_VA  		"/dev/mtd10"


#endif //__DEFINES_H
