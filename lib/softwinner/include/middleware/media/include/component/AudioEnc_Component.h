/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : AudioEnc_Component.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/05/09
  Last Modified :
  Description   : mpp component implement
  Function List :
  History       :
******************************************************************************/

#ifndef __IPCLINUX_AUDIOENC_COMPONENT_H__
#define __IPCLINUX_AUDIOENC_COMPONENT_H__

//ref platform headers
#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "plat_math.h"

//media api headers to app
#include "mm_common.h"
//#include "mm_comm_audio.h"
#include "mm_comm_aenc.h"
#include "mpi_aenc.h"

//media internal common headers.
#include "media_common.h"
#include "mm_component.h"
#include "ComponentCommon.h"
#include "tmessage.h"
#include "tsemaphore.h"


//------------------------------------------------------------------------------------

//#define AUDIO_ENC_TIME_DEBUG

#define MAX_AENCODER_PORTS (2)

#define WAIT_PCMBUF_READY   (200*1000)   //us
#define AENC_FIFO_LEVEL  (FIFO_LEVEL+1)

typedef struct AUDIOENCDATATYPE {
    COMP_STATETYPE state;
    pthread_mutex_t mStateLock;
    COMP_CALLBACKTYPE *pCallbacks;
    void* pAppData;
    COMP_HANDLETYPE hSelf;

    COMP_PORT_PARAM_TYPE sPortParam;
    COMP_PARAM_PORTDEFINITIONTYPE sInPortDef;
    COMP_PARAM_PORTDEFINITIONTYPE sOutPortDef;
    COMP_INTERNAL_TUNNELINFOTYPE sInPortTunnelInfo;
    COMP_INTERNAL_TUNNELINFOTYPE sOutPortTunnelInfo;

    COMP_PARAM_BUFFERSUPPLIERTYPE sPortBufSupplier[MAX_AENCODER_PORTS];
    BOOL mInputPortTunnelFlag;
    BOOL mOutputPortTunnelFlag;   //TRUE: tunnel mode; FALSE: non-tunnel mode.

    MPP_CHN_S mMppChnInfo;
    pthread_t thread_id;
    CompInternalMsgType eTCmd;
    message_queue_t  cmd_queue;
    volatile int mNoInputPcmFlag; //1: aenclib:no input pcm data to be encoded.
    volatile int mNoOutFrameFlag; //1: aenclib:no out frame to store encoded audio data.

    AudioEncoder *pCedarA;  //AUDIOENCCONTENT_t* pCedarA
    //int mAudioEncodeType;  //AUDIO_ENCODER_AAC_TYPE
    AudioEncConfig mAudioInfo;  //__audio_enc_inf_t 
    FILE *bs_fp;
    int bs_sz;
    FILE *pcm_fp;
    int pcm_sz;

    int64_t mSendPcmFailCnt;    // for update pts
    int64_t mFailPcmLen;

    AENC_CHN_ATTR_S mEncChnAttr;
    int mOutputFrameNotifyPipeFds[2];

    struct list_head    mIdleOutFrameList;  //AEncCompOutputBuffer
    struct list_head    mReadyOutFrameList; //AEncCompOutputBuffer, for non-tunnel mode. when mOutputPortTunnelFlag == FALSE, use it to store encoded frames.
    struct list_head    mUsedOutFrameList; //AEncCompOutputBuffer, for non-tunnel mode. when mOutputPortTunnelFlag == FALSE, use it to store user occupied frames.
    int                 mFrameNodeNum;
    BOOL                mWaitOutFrameFullFlag;
    BOOL                mWaitOutFrameFlag;  //for non-tunnel mode, wait outFrame coming!
    pthread_mutex_t     mInputPcmMutex;
    pthread_mutex_t     mOutFrameListMutex;
    pthread_cond_t      mOutFrameFullCondition;
    pthread_cond_t      mOutFrameCondition; //for non-tunnel mode, wait outFrame coming!


    long long first_audio_pts;
} AUDIOENCDATATYPE;

//private interface
ERRORTYPE AudioEncGetPortDefinition(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_INOUT COMP_PARAM_PORTDEFINITIONTYPE *pPortDef);
ERRORTYPE AudioEncSetPortDefinition(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN COMP_PARAM_PORTDEFINITIONTYPE *pPortDef);
ERRORTYPE AudioEncGetCompBufferSupplier(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_INOUT COMP_PARAM_BUFFERSUPPLIERTYPE *pPortBufSupplier);
ERRORTYPE AudioEncSetCompBufferSupplier(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN COMP_PARAM_BUFFERSUPPLIERTYPE *pPortBufSupplier);
ERRORTYPE AudioEncGetMPPChannelInfo(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_OUT MPP_CHN_S *pChn);
ERRORTYPE AudioEncSetMPPChannelInfo(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN MPP_CHN_S *pChn);
ERRORTYPE AudioEncGetChannelFd(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_OUT int *pChnFd);
ERRORTYPE AudioEncGetTunnelInfo(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_INOUT COMP_INTERNAL_TUNNELINFOTYPE *pTunnelInfo);
ERRORTYPE AudioEncGetChnAttr(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_OUT AENC_CHN_ATTR_S *pChnAttr);
ERRORTYPE AudioEncSetChnAttr(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_OUT AENC_CHN_ATTR_S *pChnAttr);

//public interface
ERRORTYPE AudioEncSendCommand(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN COMP_COMMANDTYPE Cmd,
        PARAM_IN unsigned int nParam1,
        PARAM_IN void* pCmdData);
ERRORTYPE AudioEncGetState(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_OUT COMP_STATETYPE* pState);
ERRORTYPE AudioEncSetCallbacks(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN COMP_CALLBACKTYPE* pCallbacks,
        PARAM_IN void* pAppData) ;
ERRORTYPE AudioEncGetConfig(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN COMP_INDEXTYPE nIndex,
        PARAM_IN void* pComponentConfigStructure);
ERRORTYPE AudioEncSetConfig(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN COMP_INDEXTYPE nIndex,
        PARAM_IN void* pComponentConfigStructure);
ERRORTYPE AudioEncComponentTunnelRequest(
        PARAM_IN  COMP_HANDLETYPE hComponent,
        PARAM_IN  unsigned int nPort,
        PARAM_IN  COMP_HANDLETYPE hTunneledComp,
        PARAM_IN  unsigned int nTunneledPort,
        PARAM_INOUT  COMP_TUNNELSETUPTYPE* pTunnelSetup);
ERRORTYPE AudioEncEmptyThisBuffer(
        PARAM_IN  COMP_HANDLETYPE hComponent,
        PARAM_IN  COMP_BUFFERHEADERTYPE* pBuffer);
ERRORTYPE AudioEncFillThisBuffer(
        PARAM_IN  COMP_HANDLETYPE hComponent,
        PARAM_IN  COMP_BUFFERHEADERTYPE* pBuffer);
ERRORTYPE AudioEncComponentDeInit(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE AudioEncComponentInit(PARAM_IN COMP_HANDLETYPE hComponent);
#endif  /* __IPCLINUX_AUDIOENC_COMPONENT_H__ */

