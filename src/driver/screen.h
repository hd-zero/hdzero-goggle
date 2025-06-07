#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "defines.h"
#include <stdint.h>

#define MIN_SCREEN_BRIGHTNESS 0
#define MAX_SCREEN_BRIGHTNESS 12

void Screen_PowerUp();
void Screen_Init();
void Screen_Startup();

void Screen_Display(int on);
void Screen_Pattern(uint8_t enable, uint8_t mode, uint8_t speed);
void Screen_Brightness(uint8_t level);

void OLED_SetTMG(int mode); // mode: 0=1080P; 1=720P

void MFPGA_Set720P90(uint8_t mode);
void MFPGA_Set720P60(uint8_t mode, uint8_t is_43);
void MFPGA_Set540P60();
void MFPGA_Set1080P30();
void MFPGA_SetRatio(int ratio);

#ifdef __cplusplus
}
#endif
