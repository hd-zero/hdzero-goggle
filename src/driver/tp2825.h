#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "ui/page_common.h"

void TP2825_close();
void TP2825_open();

void TP2825_init(source_t mode, int is_pal);
void TP2825_Switch_Mode(int is_pal);
void TP2825_Switch_CH(source_t mode);
void TP2825_Set_Clamp(int);

void TP2825_Set_Pclk(uint8_t inv);

#ifdef __cplusplus
}
#endif
