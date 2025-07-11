#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void RTC6715_Open(int power_on, int audio_on);
void RTC6715_SetCH(int ch);
int RTC6715_GetRssi();

#ifdef __cplusplus
}
#endif
