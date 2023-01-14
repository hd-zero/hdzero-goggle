/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : AdecCompStream.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/12
  Last Modified :
  Description   : common struct definition in AdecComp and MPIAdec.
  Function List :
  History       :
******************************************************************************/

#ifndef _ADECCOMPSTREAM_H_
#define _ADECCOMPSTREAM_H_

#include <plat_type.h>

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

typedef struct ADecCompInputStream
{
    char* mpPcmBuf;
    int mSize;
    int mChannelNum;
    int mBitsPerSample;
    int mSampleRate;
    long long mTimeStamp; //unit:us
    struct list_head mList;
}ADecCompInputStream;

typedef struct ADecCompOutputFrame
{
    char* mpPcmBuf;
	int mSize;
    int mChannelNum;
    int mBitsPerSample;
    int mSampleRate;
    int mId;
	long long mTimeStamp; //unit:us
    struct list_head mList;
}ADecCompOutputFrame;


#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif  /* _ADECCOMPSTREAM_H_ */

