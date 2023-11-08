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

#define  MAXSTREAMBUF 1024*1024   //65535 * 12

typedef enum
{
	MEDIA_VIDEO = 1,
	MEDIA_AUDIO
} MediaType_e;

typedef enum
{
	FRAME_I = 1,
	FRAME_P,
	FRAME_B,

	FRAME_SPS,
	FRAME_NUM,
} FrameType_e;

int  avshare_add(uint8_t* pstStream, int len, FrameType_e frame_type, uint32_t millis, MediaType_e streamType);
void avshare_addaud(uint8_t * buf, int len, uint32_t millis );
void avshare_addvid(uint8_t* pstStream, int len, FrameType_e frameType, uint32_t millis);
void avshare_flush(void);

int  avshare_init(void);
int  avshare_uninit(void);

int  avshare_connect(int media_type);
int  avshare_disconnect(int media_type);

int  avshare_read(uint8_t*sFrameBuf, int nLen);
int  avshare_readFrame(uint8_t* sFrameBuf, int nLen, struct timeval* timestamp, unsigned* fDurationInMicroseconds);
int  avshare_readAudio(uint8_t* sFrameBuf, int nLen, struct timeval* timestamp, unsigned* fDurationInMicroseconds);
int  avshare_readFrame2(uint8_t*sFrameBuf, int nBufLen, uint32_t* timestamp);
int  avshare_readPipe(uint8_t* sFrameBuf, int nLen);

bool avshare_start(void);
void avshare_stop(void);

#ifdef __cplusplus
}
#endif
