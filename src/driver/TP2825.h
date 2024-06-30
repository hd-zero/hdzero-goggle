#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void TP2825_close();
void TP2825_open();

void TP2825_Config(int ch_sel, int is_pal); // ch_sel: 0=AV in; 1=Module bay
void TP2825_Switch_Mode(int is_pal);
void TP2825_Switch_CH(uint8_t sel); // 0 = AV in; 1 = Module bay
void TP2825_Set_Clamp(int);

#ifdef __cplusplus
}
#endif
