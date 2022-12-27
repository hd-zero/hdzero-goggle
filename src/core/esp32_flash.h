#ifndef __ESP32_FLASH_H__
#define __ESP32_FLASH_H__

#include <stdint.h>
#include "../esp32/esp_loader.h"
#include "../esp32/serial_io.h"

esp_loader_error_t loader_port_init();
void loader_port_close();

#endif //__ESP32_FLASH_H__