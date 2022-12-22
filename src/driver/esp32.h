#ifndef __ESP32_H__
#define __ESP32_H__

#include <stdint.h>
#include "../esp32/esp_loader.h"

void esp32_init();
void enable_esp32();
void disable_esp32();
void esp32_tx(uint8_t* cmd, uint8_t cmd_len);
void esp32_rx();

// Functions that need to be implemented by the serial handler connected to the ESP32
void esp32_handler_set_uart(uint32_t fd_uart);
bool esp32_handler_process_byte(uint8_t byte);
void esp32_handler_timeout();   // handle a character read timeout

#endif //__ESP32_H__