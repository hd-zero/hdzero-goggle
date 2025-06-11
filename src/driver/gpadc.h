#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define GPADC_INIT "/mnt/app/script/set_gpadc.sh"
#define ADC0_FILE  "/tmp/adc0"

void gpadc_init();
void gpadc_on(uint8_t is_on);
int gpdac0_get();

#ifdef __cplusplus
}
#endif
