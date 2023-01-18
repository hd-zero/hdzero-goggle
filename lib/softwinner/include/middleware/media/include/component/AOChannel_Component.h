/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : AOChannel_Component.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/05/26
  Last Modified :
  Description   : mpi functions implement
  Function List :
  History       :
******************************************************************************/
#ifndef __AO_CHANNEL_COMPONENT_H__
#define __AO_CHANNEL_COMPONENT_H__

//ref platform headers
#include <plat_defines.h>
#include <plat_errno.h>
#include <plat_math.h>
#include <plat_type.h>

//media api headers to app
#include <audio_hw.h>
#include <mm_common.h>
#include <mm_component.h>
//#include <pcmBufferManager.h>
#include <tmessage.h>
#include <tsemaphore.h>

#include <AdecCompStream.h>
#include <Clock_Component.h>
#include "ComponentCommon.h"

#ifdef CFG_AUDIO_EFFECT_RESAMPLE
#include "aumixcom.h"
#include "doResample.h"
#endif

#ifdef CFG_AUDIO_EFFECT_GAIN
#include "auGaincom.h"
#endif

#ifdef CFG_AUDIO_EFFECT_EQ
#include "eq.h"
#endif


#define AO_CHN_MAX_CACHE_FRAME (64) // keep same with ADEC_FRAME_COUNT

#define MAX_AO_INPORTS 2
typedef enum AO_INPORT_SUFFIX_DEFINITION{
    AO_INPORT_SUFFIX_CLOCK = 0,
    AO_INPORT_SUFFIX_AUDIO = 1,
}AO_INPORT_SUFFIX_DEFINITION;

typedef struct AOCompInputFrame
{
    AUDIO_FRAME_S mAFrame;
    struct list_head mList;
}AOCompInputFrame;

typedef struct AO_CHN_DATA_S {
    COMP_STATETYPE state;
    pthread_mutex_t mStateLock;
    COMP_CALLBACKTYPE *pCallbacks;
    void *pAppData;
    COMP_HANDLETYPE hSelf;

    COMP_PORT_PARAM_TYPE sPortParam;
    COMP_PARAM_PORTDEFINITIONTYPE sPortDef[AO_CHN_MAX_PORTS];
    COMP_INTERNAL_TUNNELINFOTYPE sPortTunnelInfo[AO_CHN_MAX_PORTS];
    COMP_PARAM_BUFFERSUPPLIERTYPE sPortBufSupplier[AO_CHN_MAX_PORTS];
    BOOL mInputPortTunnelFlag[MAX_AO_INPORTS];
    BOOL mOutputPortTunnelFlag;   //TRUE: tunnel mode; FALSE: non-tunnel mode. AO -> AI

    MPP_CHN_S mMppChnInfo;
    CompInternalMsgType eTCmd;
    pthread_t mThreadId;
    message_queue_t mCmdQueue;

    volatile BOOL start_to_play;
    volatile int priv_flag;
    int audio_rend_flag;
    PCM_CARD_TYPE_E card_id;

    int wait_time_out;
    BOOL is_ref_clock;
    BOOL av_sync;
    volatile int seek_flag;

    struct list_head        mAudioInputFrameIdleList;   // AOCompInputFrame
    struct list_head        mAudioInputFrameReadyList;
    struct list_head        mAudioInputFrameUsedList;
    int                     mFrameNodeNum;
    AOCompInputFrame        *mpCurrentInputFrame;
    int                     mCurrentFrameUsedSize;
    BOOL                    mWaitInputFrameFlag; //1: no input frame from ADec, wait!
    pthread_mutex_t         mInputFrameListMutex;
    BOOL                    mWaitOutputBufFlag; //1: no space in sound-card's RingBuf, wait!
    pthread_mutex_t         mOutputBufMutex;

    BOOL mUseVqeLib;
    AO_VQE_CONFIG_S mVqeCfg;
    PCM_CONFIG_S *mpPcmCfg;
    AIO_ATTR_S *mpAioAttr;
    //PcmBufferManager *mpPcmMgr;
    int mWaitingPlayDataFlag;

    cdx_sem_t mAllFrameRelSem;
    int mWaitAllFrameReleaseFlag;

    cdx_sem_t mWaitDrainPcmSem;
    BOOL mWaitDrainPcmSemFlag;

    char *tmpHandleBuf;
	int tmpBufSize;

#ifdef CFG_AUDIO_EFFECT_RESAMPLE
    Resampler *res;
#endif

#ifdef CFG_AUDIO_EFFECT_EQ
    void* equalizer;
#endif

    volatile BOOL mSaveFileFlag;
    char *mpSaveFileFullPath;
    FILE *mFpSaveFile;
    unsigned int mSaveFileSize;
} AO_CHN_DATA_S;

ERRORTYPE AOChannel_ComponentInit(PARAM_IN COMP_HANDLETYPE hComponent);

#endif /* __AO_CHANNEL_COMPONENT_H__ */
