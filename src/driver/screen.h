#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "defines.h"
#include <stdbool.h>
#include <stdint.h>

#define MIN_SCREEN_BRIGHTNESS 0
#define MAX_SCREEN_BRIGHTNESS 12

typedef struct {
    void (*set720p90)(uint8_t mode);
    void (*set720p60)(uint8_t mode, bool is_43);
    void (*set540p60)(void);
    void (*set1080p30)(void);
    void (*set_ratio)(bool is_43);
} mfpga_t;

typedef struct {
    void (*start_up)(void);
    void (*display)(bool on);
    void (*brightness)(uint8_t level);
    void (*pattern)(bool enable, uint8_t mode, uint8_t speed);
    void (*vtmg)(int mode);
    mfpga_t mfpga;
} screen_t;

extern screen_t screen;

#ifdef __cplusplus
}
#endif
