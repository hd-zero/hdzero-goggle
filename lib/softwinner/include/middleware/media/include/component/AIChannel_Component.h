/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : AIChannel_Component.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/05/26
  Last Modified :
  Description   : mpi functions implement
  Function List :
  History       :
******************************************************************************/
#ifndef __AI_CHANNEL_COMPONENT_H__
#define __AI_CHANNEL_COMPONENT_H__

//ref platform headers
#include <plat_defines.h>
#include <plat_errno.h>
#include <plat_math.h>
#include <plat_type.h>

//media api headers to app
#include <audio_hw.h>
#include <mm_common.h>
#include <mm_component.h>
#include <pcmBufferManager.h>
#include <tmessage.h>
#include <tsemaphore.h>

#include "ComponentCommon.h"

#ifdef CFG_AUDIO_EFFECT_AEC
#include "echo_control.h"
#endif
#ifdef CFG_AUDIO_EFFECT_RNR
#include "nosc.h"
#endif
#ifdef CFG_AUDIO_EFFECT_DRC
#include "drclog.h"
#endif
#ifdef CFG_AUDIO_EFFECT_EQ
#include "eq.h"
#endif




#define DEFAULT_REC_SAMPLERATE (8000)
#define FRMSIZE(sampleRate) ((sampleRate)*8/1000)  //8ms data
#define FRAMESIZ2TIME(size, samplerate) ((size)*1000/(samplerate))


#define AI_CHN_MAX_CACHE_FRAME (50)

typedef struct AI_AUDIO_FRAME_S {
    AUDIO_FRAME_S mFrame;
    struct list_head mList;
} AI_AUDIO_FRAME_S;

#define MAX_AI_OUTPORTS 2
typedef enum AI_OUTPORT_SUFFIX_DEFINITION{
    AI_OUTPORT_SUFFIX_AENC = 0,     // ai -> aenc
    AI_OUTPORT_SUFFIX_AO = 1,       // ai -> ao
}AI_OUTPORT_SUFFIX_DEFINITION;

typedef struct AI_CHN_DATA_S {
    COMP_STATETYPE state;
    pthread_mutex_t mStateLock;
    COMP_CALLBACKTYPE *pCallbacks;
    void *pAppData;
    COMP_HANDLETYPE hSelf;

    COMP_PORT_PARAM_TYPE sPortParam;
    COMP_PARAM_PORTDEFINITIONTYPE sPortDef[AI_CHN_MAX_PORTS];
    COMP_INTERNAL_TUNNELINFOTYPE sPortTunnelInfo[AI_CHN_MAX_PORTS];
    COMP_PARAM_BUFFERSUPPLIERTYPE sPortBufSupplier[AI_CHN_MAX_PORTS];
    BOOL mOutputPortTunnelFlag[MAX_AI_OUTPORTS];   //TRUE: tunnel mode; FALSE: non-tunnel mode.

    CompInternalMsgType eTCmd;
    pthread_t mThreadId;
    message_queue_t mCmdQueue;

    MPP_CHN_S mMppChnInfo;
    AI_CHN_PARAM_S mParam;
    volatile BOOL mbMute;
    BOOL mUseVqeLib;
    AI_VQE_CONFIG_S mVqeCfg;
    AUDIO_SAMPLE_RATE_E mResmpRate;
    PCM_CONFIG_S *mpPcmCfg;
    AIO_ATTR_S *mpAioAttr;

    PcmBufferManager *mpCapMgr;
    PcmBufferManager *mpPlayMgr;

    pthread_mutex_t mCapMgrLock;
    volatile BOOL mWaitingCapDataFlag;
    volatile BOOL mWaitingPlayDataFlag;

    //volatile BOOL mWaitingOutFrameFlag;
    cdx_sem_t mWaitOutFrameSem; //for non-tunnel mode, wait outFrame coming!

    //volatile BOOL mWaitGetAllOutFrameFlag;
    //cdx_sem_t mWaitGetAllOutFrameSem; //for non-tunnel mode, wait user get all outFrame!

    FILE *pcm_fp;
    int pcm_sz;

    cdx_sem_t mAllFrameRelSem;
    volatile int mWaitAllFrameReleaseFlag;

#ifdef CFG_AUDIO_EFFECT_AEC
    void* ec;
    ec_prms_t ec_prms;
#endif

    short *tmpBuf;
    int tmpBufLen;

#ifdef CFG_AUDIO_EFFECT_RNR
    void* nosc;
#endif
#ifdef CFG_AUDIO_EFFECT_DRC
    void* drc;
#endif

#ifdef CFG_AUDIO_EFFECT_EQ
    void* equalizer;
#endif

    volatile BOOL mSaveFileFlag;
    char *mpSaveFileFullPath;
    FILE *mFpSaveFile;
    unsigned int mSaveFileSize;
} AI_CHN_DATA_S;

ERRORTYPE AIChannel_ComponentInit(PARAM_IN COMP_HANDLETYPE hComponent);

#endif /* __AI_CHANNEL_COMPONENT_H__ */
