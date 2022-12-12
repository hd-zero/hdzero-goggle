#ifndef __ESP32_H__
#define __ESP32_H__

#include <stdint.h>

void init_esp32();
void  esp32_tx(uint8_t* cmd, uint8_t cmd_len);
void esp32_rx();

#endif //__ESP32_H__