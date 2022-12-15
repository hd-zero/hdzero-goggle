#ifndef __ESP32_H__
#define __ESP32_H__

#include <stdint.h>
#include "../esp32/esp_loader.h"

void enable_esp32();
void disable_esp32();
void esp32_tx(uint8_t* cmd, uint8_t cmd_len);
void esp32_rx();

esp_loader_error_t loader_port_init();
void loader_port_close();

#endif //__ESP32_H__