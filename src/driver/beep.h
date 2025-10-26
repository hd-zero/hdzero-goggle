#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define BEEP_VERY_SHORT 1
#define BEEP_SHORT      50
#define BEEP_LONG       200

#define beep() beep_dur(BEEP_SHORT)

void beep_init(void);
void beep_dur(int ms);

#ifdef __cplusplus
}
#endif
