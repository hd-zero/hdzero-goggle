#ifndef _TP2825_H
#define _TP2825_H

#include <stdint.h>

void TP2825_close();
void TP2825_open();

void TP2825_Config(int ch_sel, int is_pal); //ch_sel: 0=AV in; 1=Module bay

void TP2825_Switch_CH(uint8_t sel); // 0 = AV in; 1 = Module bay
void TP2825_Set_Clamp(int set_default);

#endif
