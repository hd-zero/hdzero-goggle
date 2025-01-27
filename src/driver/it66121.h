#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

void IT66121_close();
void IT66121_init();
void IT66121_set_phase(uint8_t phase, uint8_t inv);

#ifdef __cplusplus
}
#endif
