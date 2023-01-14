/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : AudioDec_Component.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/09/10
  Last Modified :
  Description   : mpp component implement
  Function List :
  History       :
******************************************************************************/

#ifndef __IPCLINUX_AUDIODEC_COMPONENT_H__
#define __IPCLINUX_AUDIODEC_COMPONENT_H__

//ref platform headers
#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "plat_math.h"

//media api headers to app
#include "mm_common.h"
//#include "mm_comm_audio.h"
#include "mm_comm_adec.h"

//media internal common headers.
#include "media_common.h"
#include "mm_component.h"
#include "ComponentCommon.h"
#include "tmessage.h"
#include "tsemaphore.h"
#include "adecoder.h"


//------------------------------------------------------------------------------------
#define MAX_ADECODER_PORTS (2)
#define ADEC_FRAME_COUNT   (64)
//#define DEBUG_SAVE_PCMDATA

typedef enum ADEC_PORT_INDEX_DEFINITION{
    ADEC_PORT_INDEX_DEMUX = 0x0,
    ADEC_OUT_PORT_INDEX_VRENDER = 0x1,
    ADEC_PORT_INDEX_CLOCK = 0x2,
}ADEC_PORT_INDEX_DEFINITION;

struct AUDIODEC_INPUT_DATA
{
    COMP_STATETYPE state;
    pthread_mutex_t mStateLock;
    pthread_cond_t mStateCond;
    
    volatile BOOL mWaitAbsValidFlag;
    int nRequestLen; //Set when not enough buffer for component A

    struct list_head mIdleAbsList;  //DMXPKT_NODE_T
    struct list_head mReadyAbsList;
    struct list_head mUsingAbsList;
    int mNodeNum;
    BOOL mbWaitAbsFullFlag;
    pthread_mutex_t mAbsListLock;
    pthread_cond_t mAbsFullCond;
    
    struct AUDIODECDATATYPE *pAudioDecData;
    message_queue_t cmd_queue;
    pthread_t thread_id;
};

struct AUDIODECDATATYPE {
    COMP_STATETYPE state;
    pthread_mutex_t mStateLock;
    COMP_CALLBACKTYPE *pCallbacks;
    void* pAppData;
    COMP_HANDLETYPE hSelf;

    COMP_PORT_PARAM_TYPE sPortParam;
    COMP_PARAM_PORTDEFINITIONTYPE sInPortDef;
    COMP_PARAM_PORTDEFINITIONTYPE sOutPortDef;
    COMP_PARAM_PORTEXTRADEFINITIONTYPE sInPortExtraDef;
    COMP_PARAM_PORTEXTRADEFINITIONTYPE sOutPortExtraDef;

    COMP_INTERNAL_TUNNELINFOTYPE sInPortTunnelInfo;
    COMP_INTERNAL_TUNNELINFOTYPE sOutPortTunnelInfo;
    COMP_PARAM_BUFFERSUPPLIERTYPE sPortBufSupplier[MAX_ADECODER_PORTS];
    BOOL mInputPortTunnelFlag;
    BOOL mOutputPortTunnelFlag;   //TRUE: tunnel mode; FALSE: non-tunnel mode.

    MPP_CHN_S mMppChnInfo;
    pthread_t thread_id;
    CompInternalMsgType eTCmd;
    message_queue_t cmd_queue;

    ADEC_CHN_ATTR_S mADecChnAttr;
    //int mOutputFrameNotifyPipeFds[2];

    int demux_type;
    int mAudioInfoVersion;
    AudioStreamInfo mAudioStreamInfo;   //for inport non-tunnel, config steamInfo by hand!

    int decode_mode;
    //int codec_type;
    int priv_flag;  //CDX_COMP_PRIV_FLAGS_REINIT
    BOOL mbEof;
    int is_raw_music_mode;

    int get_audio_info_flag;    //if get audio info, after first decode, we get audio info, then set it to 0.
    int audio_channel;    //CEDARX_AUDIO_CHANNEL_STEREO
    int volumegain;
    int mute;
    int enable_resample;
    int mNoBitstreamCounter;

    AudioDecoder*           pDecoder;
    BsInFor                 ad_cedar_info;
    int                     force_exit;
    int                     mFrameId;

    struct list_head        mIdleOutFrameList;  //ADecCompOutputFrame
    struct list_head        mReadyOutFrameList; //for non-tunnel mode. when mOutputPortTunnelFlag == FALSE, use it to store decoded frames.
    struct list_head        mUsedOutFrameList;  //for tunnel-mode; also for non-tunnel mode. when mOutputPortTunnelFlag == FALSE, use it to store user occupied frames.

    int                     mFrameNodeNum;      //ADecCompOutputFrame counter

    volatile BOOL           mWaitAbsInputFlag;
    volatile BOOL           mWaitPcmBufFlag;

    volatile BOOL           mWaitOutFrameFullFlag;
    //volatile BOOL           mWaitOutFrameFlag;  //wait outFrame coming! same meaning with mWaitPcmBufFlag

    volatile BOOL           mWaitReadyFrameFlag;  //for non-tunnel mode, wait frame ready, so can get it.
    pthread_cond_t          mReadyFrameCondition;

    pthread_mutex_t         mAbsInputMutex;

    volatile BOOL           mWaitEmptyAbsFlag;  // for non-tunnel mode, wait enough space for one new bs when Abs buffer full or almost full
    int                     mRequestAbsLen;     // coming bitstream size
    pthread_cond_t          mEmptyAbsCondition;

    pthread_mutex_t         mOutFrameListMutex;
    pthread_cond_t          mOutFrameFullCondition;
    pthread_cond_t          mOutFrameCondition; //for non-tunnel mode, wait outFrame coming!

    struct AUDIODEC_INPUT_DATA*    pInputData;
#ifdef DEBUG_SAVE_PCMDATA
    FILE*   mPcmSaveFp;
    int     mPcmSize;
#endif
};

typedef struct AUDIODECDATATYPE AUDIODECDATATYPE;
typedef struct AUDIODEC_INPUT_DATA AUDIODEC_INPUT_DATA;

// below 3 put here tmp
//typedef enum CEDARX_AUDIO_CHANNEL_TYPE
//{
//    CEDARX_AUDIO_CHANNEL_STEREO = 0,
//    CEDARX_AUDIO_CHANNEL_LEFT ,
//    CEDARX_AUDIO_CHANNEL_RIGHT,
//}CEDARX_AUDIO_CHANNEL_TYPE;

//enum CEDARXAUDIOFLAGSENUM
//{
//    ADEC_DISABLE_AAC_PACKING = (1<<0),
//};

//typedef enum CDX_DECODE_MODE {
//    CDX_DECODER_MODE_NORMAL    = 0,
//    CDX_DECODER_MODE_RAWMUSIC,
//}CDX_DECODE_MODE;

//private interface
ERRORTYPE AudioDecGetPortDefinition(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_INOUT COMP_PARAM_PORTDEFINITIONTYPE *pPortDef);
ERRORTYPE AudioDecSetPortDefinition(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN COMP_PARAM_PORTDEFINITIONTYPE *pPortDef);
ERRORTYPE AudioDecGetCompBufferSupplier(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_INOUT COMP_PARAM_BUFFERSUPPLIERTYPE *pPortBufSupplier);
ERRORTYPE AudioDecSetCompBufferSupplier(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN COMP_PARAM_BUFFERSUPPLIERTYPE *pPortBufSupplier);
ERRORTYPE AudioDecGetMPPChannelInfo(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_OUT MPP_CHN_S *pChn);
ERRORTYPE AudioDecSetMPPChannelInfo(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN MPP_CHN_S *pChn);
//ERRORTYPE AudioDecGetChannelFd(
//        PARAM_IN COMP_HANDLETYPE hComponent,
//        PARAM_OUT int *pChnFd);
ERRORTYPE AudioDecGetTunnelInfo(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_INOUT COMP_INTERNAL_TUNNELINFOTYPE *pTunnelInfo);
ERRORTYPE AudioDecGetChnAttr(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_OUT ADEC_CHN_ATTR_S *pChnAttr);
ERRORTYPE AudioDecSetChnAttr(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_OUT ADEC_CHN_ATTR_S *pChnAttr);

//public interface
ERRORTYPE AudioDecSendCommand(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN COMP_COMMANDTYPE Cmd,
        PARAM_IN unsigned int nParam1,
        PARAM_IN void* pCmdData);
ERRORTYPE AudioDecGetState(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_OUT COMP_STATETYPE* pState);
ERRORTYPE AudioDecSetCallbacks(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN COMP_CALLBACKTYPE* pCallbacks,
        PARAM_IN void* pAppData) ;
ERRORTYPE AudioDecGetConfig(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN COMP_INDEXTYPE nIndex,
        PARAM_IN void* pComponentConfigStructure);
ERRORTYPE AudioDecSetConfig(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN COMP_INDEXTYPE nIndex,
        PARAM_IN void* pComponentConfigStructure);
ERRORTYPE AudioDecComponentTunnelRequest(
        PARAM_IN  COMP_HANDLETYPE hComponent,
        PARAM_IN  unsigned int nPort,
        PARAM_IN  COMP_HANDLETYPE hTunneledComp,
        PARAM_IN  unsigned int nTunneledPort,
        PARAM_INOUT  COMP_TUNNELSETUPTYPE* pTunnelSetup);
ERRORTYPE AudioDecEmptyThisBuffer(
        PARAM_IN  COMP_HANDLETYPE hComponent,
        PARAM_IN  COMP_BUFFERHEADERTYPE* pBuffer);
ERRORTYPE AudioDecFillThisBuffer(
        PARAM_IN  COMP_HANDLETYPE hComponent,
        PARAM_IN  COMP_BUFFERHEADERTYPE* pBuffer);
ERRORTYPE AudioDecComponentDeInit(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE AudioDecComponentInit(PARAM_IN COMP_HANDLETYPE hComponent);
#endif  /* __IPCLINUX_AUDIODEC_COMPONENT_H__ */

