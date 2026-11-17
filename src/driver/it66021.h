#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

void IT66021_srst();
void IT66021_close();
void IT66021_init();
int IT66021_Sig_det();
int IT66021_Get_VTMG(int *freq_ref);
int IT66021_Get_CS();
void IT66021_Set_CSMatrix(int cs);
int IT66021_Get_PCLKFREQ();
void IT66021_edid();

void IT66021_Set_Pclk(int inv, int dly);
void IT66021_Mask_WR(uint8_t is_ring, uint8_t addr, uint8_t mask, uint8_t wdat);

#ifdef __cplusplus
}
#endif
