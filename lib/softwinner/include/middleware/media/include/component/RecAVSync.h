/*
********************************************************************************
*                           Android multimedia module
*
*          (c) Copyright 2010-2015, Allwinner Microelectronic Co., Ltd.
*                              All Rights Reserved
*
* File   : RecAVSync.h
* Version: V1.0
* By     : eric_wang
* Date   : 2015-3-7
* Description:
********************************************************************************
*/
#ifndef _RECAVSYNC_H_
#define _RECAVSYNC_H_

#include <plat_type.h>
#include <cedarx_avs_counter.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define AVS_ADJUST_PERIOD_MS 5*1000		// 5s
typedef struct RecAVSync
{
    int64_t mGetFirstVideoTime; //the time of get first video data.
    int64_t mGetFirstAudioTime; //the time of get first audio data.
    int64_t mFirstDurationDiff; //videoDuration - audioDuration, the duration diff at first av sync.
    int64_t mVideoBasePts;  //unit:ms, the pts of first video frame.
    int64_t mAudioBasePts;  //unit:ms, the pts of first audio frame.
    int64_t mAudioDataSize; //unit:byte
    int mSampleRate;
    int mChannelNum;
    int mBitsPerSample;
    int64_t mLastAdjustPts; //the video frame pts of last av sync.
    CedarxAvscounterContext *mpAvsCounter;
    ERRORTYPE (*SetVideoPts)(
        PARAM_IN struct RecAVSync *pThiz,
        PARAM_IN int64_t nPts); //unit:ms
    ERRORTYPE (*SetAudioInfo)(
        PARAM_IN struct RecAVSync *pThiz,
        PARAM_IN int nSampleRate,
        PARAM_IN int nChannelNum,
        PARAM_IN int nBitsPerSample);
    ERRORTYPE (*AddAudioDataLen)(
        PARAM_IN struct RecAVSync *pThiz,
        PARAM_IN int nLen);
    ERRORTYPE (*GetTimeDiff)(
        PARAM_IN struct RecAVSync *pThiz,
        PARAM_OUT int64_t* pDiff);  //unit:us
}RecAVSync;

ERRORTYPE RecAVSyncInit(RecAVSync *pThiz);
ERRORTYPE RecAVSyncDestroy(RecAVSync *pThiz);
RecAVSync* RecAVSyncConstruct();
void RecAVSyncDestruct(RecAVSync *pThiz);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif  /* _RECAVSYNC_H_ */

