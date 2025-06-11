#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void RTC6715_Open(int on);
void RTC6715_SetCH(int ch);
int RTC6715_GetRssi();

#ifdef __cplusplus
}
#endif
