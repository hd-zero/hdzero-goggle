#ifndef _IT66021_H
#define _IT66021_H

void IT66021_srst();
void IT66021_close();
void IT66021_init();
int IT66021_Sig_det();
int IT66021_Get_VTMG();
int IT66021_Get_CS();
void IT66021_Set_CSMatrix(int cs);
int IT66021_Get_PCLKFREQ();

#endif