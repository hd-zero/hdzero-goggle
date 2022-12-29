#include "fans.h"

#include <stdio.h>
#include <stdint.h>

#include <log/log.h>

#include "i2c.h"
#include "uart.h"
#include "dm5680.h"
#include "../core/common.hh"

uint8_t fan_speeds[3]; //0=right,1=left,2=top

uint8_t get_topfan_value(uint8_t level)
{
	uint8_t topfan_tbl[6] = {0,17,33,41,62,100};
	if(level>5) level = 5; 
	fan_speeds[2] = level;
	return topfan_tbl[level];
}

void fans_top_setspeed(uint8_t speed)
{
	LOGI("fans_top_setspeed: %d",speed);
	speed = get_topfan_value(speed);
	i2c_write(2, 0x64, 0x83, speed);
}

void fans_left_setspeed(uint8_t speed)
{
	fan_speeds[1] = speed;
	DM5680_SetFanSpeed(1, speed); 
	LOGI("fans_left_setspeed: %d",speed);
}

void fans_right_setspeed(uint8_t speed)
{
	fan_speeds[0] = speed;
	DM5680_SetFanSpeed(0, speed); 
	LOGI("fans_right_setspeed: %d",speed);
}

