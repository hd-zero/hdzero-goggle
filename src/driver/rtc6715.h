#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
typedef struct {
    void (*init)(bool power_on, bool audio_on);
    void (*set_ch)(int ch);
    int (*get_rssi)(void);
} rtc6715_t;

extern rtc6715_t rtc6715;

#ifdef __cplusplus
}
#endif
