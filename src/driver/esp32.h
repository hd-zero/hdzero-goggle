#ifndef __ESP32_H__
#define __ESP32_H__

#include <stdint.h>
#include "msp.h"
#include "../esp32/esp_loader.h"

void esp32_init();
void enable_esp32();
void disable_esp32();
void esp32_tx(uint8_t* cmd, uint8_t cmd_len);
void esp32_rx();

esp_loader_error_t loader_port_init();
void loader_port_close();

void msp_send_packet(uint16_t function, mspPacketType_e type, uint16_t payload_size, uint8_t *payload);
bool msp_await_resposne(uint16_t function, uint16_t payload_size, uint8_t *payload, uint32_t timeout_ms);

#endif //__ESP32_H__