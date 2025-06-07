#include "fans.h"

#include <stdint.h>
#include <stdio.h>

#include <log/log.h>

#include "../core/common.hh"
#include "dm5680.h"
#include "i2c.h"
#include "uart.h"

fan_speed_t fan_speed;

uint8_t get_topfan_value(uint8_t level) {
#if HDZGOGGLE
    uint8_t topfan_tbl[6] = {0, 17, 33, 41, 62, 100};
#elif HDZBOXPRO
    uint8_t topfan_tbl[6] = {0, 24, 33, 41, 62, 100};
#endif
    if (level > 5) {
        level = 5;
    }
    fan_speed.top = level;
    return topfan_tbl[level];
}

void fans_top_setspeed(uint8_t speed) {
    LOGI("fans_top_setspeed: %d", speed);
    speed = get_topfan_value(speed);
    i2c_write(2, 0x64, 0x83, speed);
}

void fans_left_setspeed(uint8_t speed) {
    if (TARGET_GOGGLE == getTargetType()) {
        fan_speed.left = speed;
        DM5680_SetFanSpeed(1, speed);
        LOGI("fans_left_setspeed: %d", speed);
    }
}

void fans_right_setspeed(uint8_t speed) {
    if (TARGET_GOGGLE == getTargetType()) {
        fan_speed.right = speed;
        DM5680_SetFanSpeed(0, speed);
        LOGI("fans_right_setspeed: %d", speed);
    }
}
