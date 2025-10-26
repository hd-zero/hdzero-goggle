#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "ui/page_common.h"

void TP2825_close();
void TP2825_open();

void TP2825_init(bool is_av_in, bool is_pal);
void TP2825_Switch_Mode(bool is_pal);
void TP2825_Switch_CH(bool is_av_in);
void TP2825_Set_Clamp(int idx);

void TP2825_Set_Pclk(uint8_t inv);

#ifdef __cplusplus
}
#endif
