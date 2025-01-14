#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void IT66021_srst();
void IT66021_close();
void IT66021_init();
int IT66021_Sig_det();
int IT66021_Get_VTMG(int *freq_ref);
int IT66021_Get_CS();
void IT66021_Set_CSMatrix(int cs);
int IT66021_Get_PCLKFREQ();
void IT66021_edid();

void IT66021_Set_Pclk(int inv);

#ifdef __cplusplus
}
#endif
