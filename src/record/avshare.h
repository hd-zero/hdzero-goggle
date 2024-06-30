/******************************************************************************

  Copyright (C), 2016-2017, Divimath Inc.

 ******************************************************************************
  File Name     : avshare.h
  Version       : Initial version
  Author        : Divimath Shenzhen
  Created       : 2017/01/06
  Description   : share av stream data to other modules, such as airtx
  History       :
  1.Date        : 2017/01/06
    Author      : james
    Modification: Created file

******************************************************************************/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
	MEDIA_VIDEO = 1,
	MEDIA_AUDIO
} MediaType_e;

//extend VencFrameType_e
typedef enum
{
	FRAME_I = 1,
	FRAME_P,
	FRAME_B,

	FRAME_SPS,
	FRAME_PPS,
	FRAME_SEI,
	FRAME_NUM,
} FrameType_e;

int  avshare_add(uint8_t * pstStream, int len, FrameType_e frameType, uint32_t millis, MediaType_e streamType);
void avshare_addaud(uint8_t * buf, int len, uint32_t millis );
void avshare_addvid(uint8_t * pstStream, int len, FrameType_e frameType, uint32_t millis);
void avshare_flush(void);

int avshare_init(void);
int avshare_uninit(void);

int avshare_write(uint8_t * pstStream, int len);
int avshare_addPacket(uint8_t* data, int len, uint64_t timestamp, int isNewFrame, int pesLen);
int avshare_addAudio(uint8_t* data, int len, uint64_t timestamp, int isNewFrame, int pesLen);

int  avshare_connect(MediaType_e streamType);
int  avshare_disconnect(MediaType_e streamType);
bool avshare_connected(MediaType_e streamType);
void avshare_reset(void);

#ifdef __cplusplus
}
#endif
