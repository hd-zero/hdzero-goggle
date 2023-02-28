#ifndef _OLED_H
#define _OLED_H
#include "defines.h"
#include <stdint.h>

void OLED_write(uint16_t addr, uint16_t wdat, uint8_t sel);
uint16_t OLED_read(uint16_t addr, uint8_t sel);

void OLED_power_up();
void OLED_init();
void OLED_Startup();

void OLED_SetTMG(int mode); // mode: 0=1080P; 1=720P

void OLED_display(int on);
void OLED_Pattern(uint8_t enable, uint8_t mode, uint8_t speed);
void OLED_Brightness(uint8_t level);

void MFPGA_Set720P90(uint8_t mode);
void MFPGA_Set720P60(uint8_t mode, uint8_t is_43);
void MFPGA_SetRatio(int ratio);

#endif
