#ifndef __DM6302_H_
#define __DM6302_H_

#include <stdbool.h>
#include <stdint.h>

//#define _DEBUG_DM6300

#define FREQ_NUM  10
typedef enum{
	DM5680_L = 0,
	DM5680_R,
}l_or_r_t;

typedef enum{
	CMD_GET_VERSION = 0,
	CMD_SET_FAN,
	CMD_SET_BEEP,
	CMD_RESET_6302,
	CMD_RESET_IT,
	CMD_5680_ENABLE,
	CMD_GET_STATUS,
	CMD_GET_DETAIL,
	CMD_SET_SCALE,
	CMD_SET_VTX,
	CMD_GET_OSD,
	CMD_CLEAR_OSD,
	CMD_R_BTN,
}cmd_5680_t;

int DM6302_init(uint8_t freq);
void DM6302_SetChannel(uint8_t ch);
void DM6302_openM0(uint32_t open);
void DM6302_get_gain(uint8_t* gain);

void DM6302_Init0(uint8_t sel);
void SPI_Read (uint8_t page, uint16_t addr, uint32_t* dat0, uint32_t* dat1);

#endif // __DM6302_H_
