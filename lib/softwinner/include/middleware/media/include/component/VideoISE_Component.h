/******************************************************************************
 Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
 File Name     : VideoISE_Component.h
 Version       : Initial Draft
 Author        : Allwinner BU3-PD2 Team
 Created       : 2016/05/09
 Last Modified :
 Description   : mpp component implement
 Function List :
 History       :
 ******************************************************************************/

#ifndef __IPCLINUX_VIDEOISE_COMPONENT_H__
#define __IPCLINUX_VIDEOISE_COMPONENT_H__

// ref platform headers
#include "plat_defines.h"
#include "plat_errno.h"
#include "plat_math.h"
#include "plat_type.h"

// media api headers to app
#include "mm_comm_video.h"
#include "mm_common.h"
// #include "mm_comm_venc.h"
// #include "mpi_venc.h"

// media internal common headers.
#include "ComponentCommon.h"
// #include "ISE_lib.h"
#include "media_common.h"
#include "mm_component.h"
#include "tmessage.h"
#include "tsemaphore.h"
// #include "vencoder.h"

#include "ISE_common.h"
#include "ISE_lib_bi.h"
#include "ISE_lib_mo.h"
#include "ISE_lib_sti.h"
//------------------------------------------------------------------------------------

//#define VIDEO_ENC_TIME_DEBUG

// #define MAX_ISE_PORTS (2)

#define ISE_FIFO_LEVEL 20

#ifndef YUV420
#define YUV420 0
#endif
#ifndef YUV422
#define YUV422 1
#endif

// input yuv item
typedef struct {
    VIDEO_FRAME_INFO_S mInYuv;
    int                mInYuvPort;
    struct list_head mList;
} VideoInYuv;

// input yuv item manage
typedef struct bufIseMrgQ_t {
    // Chn0
    struct list_head mIdleFrameList;  // VideoFrameInfoNode
    struct list_head mReadyFrameList;
    int buf_unused;           // ISE_FIFO_LEVEL
    int mYUVTatolNum;         // get yuv num
    int mYUVProcessTatolNum;  // process yuv num
    int mYUVLossTatolNum;

    // Chn1
    struct list_head mIdleFrameList1;  // VideoFrameInfoNode
    struct list_head mReadyFrameList1;
    int buf_unused1;           // ENC_FIFO_LEVEL
    int mYUVTatolNum1;         // get yuv num
    int mYUVProcessTatolNum1;  // process yuv num
    int mYUVLossTatolNum1;
} bufIseMrgQ_t;

// output yuv item
typedef struct {
    AW_U32 mhResult;
    int mIseOutId;
    //ISE_PROCOUT_PARA mIseOut;
    ISE_PROCOUT_PARA_STI mStiOut;
    ISE_PROCOUT_PARA_BI mIseOut;
    VIDEO_FRAME_INFO_S VFrame;

    struct list_head mList;
} ISEOutBuf_t;

// output yuv item manage
typedef struct {
    ISE_GRP mIseGrp;
    ISE_CHN mIseChn;
    int mBgfgmode_enbale;

    // ISEOutBuf_t			mIseOutBuf;
    ISEOutBuf_t *mpOutBuf;
    int mIseOutBufNum;                  // default num = 5
    struct list_head mIdleOutYUVList;   //ISEOutBuf_t
    struct list_head mReadyOutYUVList;  //
    struct list_head mUsedOutYUVList;   //
    pthread_mutex_t mOutYUVListLock;

    struct list_head mList;
} ISEChnNode_t;

typedef enum ISE_CHANNEL_PORT_DEFINE_E {
	ISE_PORT_INDEX_CAP0_IN = 0,
    ISE_PORT_INDEX_CAP1_IN,
    ISE_PORT_INDEX_OUT0,
    ISE_PORT_INDEX_OUT1,
    ISE_PORT_INDEX_OUT2,
    ISE_PORT_INDEX_OUT3,
    ISE_MAX_PORT,
} ISE_CHANNEL_PORT_DEFINE_E;

typedef struct VIDEOISEDATATYPE {
    COMP_STATETYPE state;
    pthread_mutex_t mStateLock;
    COMP_CALLBACKTYPE *pCallbacks;
    void *pAppData;
    COMP_HANDLETYPE hSelf;

    COMP_PORT_PARAM_TYPE sPortParam;
    COMP_PARAM_PORTDEFINITIONTYPE sInPortDef[ISE_MAX_PORT];
    COMP_PARAM_PORTDEFINITIONTYPE sOutPortDef[ISE_MAX_PORT];
    COMP_INTERNAL_TUNNELINFOTYPE sInPortTunnelInfo[ISE_MAX_PORT];
    COMP_INTERNAL_TUNNELINFOTYPE sOutPortTunnelInfo[ISE_MAX_PORT];
    COMP_PARAM_BUFFERSUPPLIERTYPE sPortBufSupplier[ISE_MAX_PORT];
    BOOL mInputPortTunnelFlag;
    BOOL mOutputPortTunnelFlag;  // TRUE: tunnel mode; FALSE: non-tunnel mode.
    int mOutputPortTunnelEnable[ISE_MAX_PORT];
    int mInputPortTunnelEnable[ISE_MAX_PORT];

    MPP_CHN_S mMppChnInfo;
    pthread_t thread_id;
    CompInternalMsgType eTCmd;
    message_queue_t cmd_queue;

    // ISE Handle
    ISE_HANDLE_MO fish_handle;
    ISE_HANDLE_BI dfish_handle_by_hardware;
    ISE_HANDLE_BI_SOFT dfish_handle_by_soft;
    ISE_HANDLE_STI ise_handle;

    //ISE handle default config
    ISE_CFG_PARA_MO fish_default_config;
    ISE_CFG_PARA_BI dfish_default_config;
    ISE ise_default_config;
        
    // yuv data input
    volatile int mNoInputFrameFlag;  // 1: no input frame to be encoded.
    bufIseMrgQ_t mBufQ;                 // used for managing camera frame, for uncompressed video frame.
    pthread_mutex_t mutex_fifo_ops_lock;
    
    int iseMode;
    DFISH_HANDLE_MODE dfish_handle_mode;
    ISE_GROUP_ATTR_S mIseGrpAttr;
    ISE_CHN_ATTR_S   mIseChnAttr;
    int mChangeChnAttr;
    int mISEFreq;
    
    ISEChnNode_t mIseChnNode;
    struct list_head mIdleChnAttrList;   // Add
    struct list_head mValidChnAttrList;  // Run
    int mIdleChnNodeNum;                 // Max = 32
    int mValidChnNodeNum;                // Max = 32
    pthread_mutex_t mMutexChnListLock;

	pthread_condattr_t  condAttr[4];
	BOOL mWaitOutFrameFlag[4];
	pthread_cond_t mOutFrameFullCond[4];
	pthread_mutex_t mOutDataMutex[4];
	
	BOOL mWaitSetMoConfigFlag;
	pthread_mutex_t mWaitSetMoConfigLock;

//    struct ScMemOpsS *mMemOps;
} VIDEOISEDATATYPE;

// private interface
ERRORTYPE
VideoIseGetPortDefinition(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_INOUT COMP_PARAM_PORTDEFINITIONTYPE *pPortDef);
ERRORTYPE
VideoIseSetPortDefinition(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_PARAM_PORTDEFINITIONTYPE *pPortDef);
ERRORTYPE VideoIseGetCompBufferSupplier(PARAM_IN COMP_HANDLETYPE hComponent,
                                        PARAM_INOUT COMP_PARAM_BUFFERSUPPLIERTYPE *pPortBufSupplier);
ERRORTYPE VideoIseSetCompBufferSupplier(PARAM_IN COMP_HANDLETYPE hComponent,
                                        PARAM_IN COMP_PARAM_BUFFERSUPPLIERTYPE *pPortBufSupplier);
ERRORTYPE VideoIseGetMPPChannelInfo(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_OUT MPP_CHN_S *pChn);
ERRORTYPE VideoIseSetMPPChannelInfo(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN MPP_CHN_S *pChn);
ERRORTYPE VideoIseGetChnAttr(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_OUT ISE_CHN_ATTR_S *pChnAttr);
ERRORTYPE VideoIseSetChnAttr(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_OUT ISE_CHN_ATTR_S *pChnAttr);
ERRORTYPE
VideoIseGetStreamBufInfo(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_OUT VENC_STREAM_BUF_INFO_S *pStreamBufInfo);
// public interface
ERRORTYPE VideoIseSendCommand(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_COMMANDTYPE Cmd,
                              PARAM_IN unsigned int nParam1, PARAM_IN void *pCmdData);
ERRORTYPE VideoIseGetState(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_OUT COMP_STATETYPE *pState);
ERRORTYPE VideoIseSetCallbacks(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_CALLBACKTYPE *pCallbacks,
                               PARAM_IN void *pAppData);
ERRORTYPE VideoIseGetConfig(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_INDEXTYPE nIndex,
                            PARAM_IN void *pComponentConfigStructure);
ERRORTYPE VideoIseSetConfig(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_INDEXTYPE nIndex,
                            PARAM_IN void *pComponentConfigStructure);
ERRORTYPE VideoIseComponentTunnelRequest(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN unsigned int nPort,
                                         PARAM_IN COMP_HANDLETYPE hTunneledComp, PARAM_IN unsigned int nTunneledPort,
                                         PARAM_INOUT COMP_TUNNELSETUPTYPE *pTunnelSetup);
ERRORTYPE VideoIseEmptyThisBuffer(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_BUFFERHEADERTYPE *pBuffer);
ERRORTYPE VideoIseFillThisBuffer(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_BUFFERHEADERTYPE *pBuffer);
ERRORTYPE VideoIseComponentDeInit(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE VideoIseComponentInit(PARAM_IN COMP_HANDLETYPE hComponent);

#endif /* __IPCLINUX_VIDEOENC_COMPONENT_H__ */
