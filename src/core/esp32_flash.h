#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "serial_io.h"
#include <stdint.h>

esp_loader_error_t loader_port_init();
void loader_port_close();

#ifdef __cplusplus
}
#endif
