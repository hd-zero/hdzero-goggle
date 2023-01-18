/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : RecRender_Component.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/05/25
  Last Modified :
  Description   : mpp component implement
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_RECRENDER_COMPONENT_H__
#define __IPCLINUX_RECRENDER_COMPONENT_H__

#include <errno.h>
#include <tmessage.h>
#include <tsemaphore.h>

//#include <vencoder.h>
//#include <mp4_mux_lib.h>
//#include <type_camera.h>
//#include <record_writer.h>
//#include <include_system/cedarx_avs_counter.h>
//#include <FsWriter.h>
#include <mm_comm_mux.h>
#include "RecRenderSink.h"
#include "RecRender_cache.h"

//#include <CDX_SystemBase.h>
//#include <aenc_sw_lib.h>

#define SINKINFO_SIZE (2)

#define MUX_FIFO_LEVEL  (256)

//must match with WRITE_BLOCK_SIZE inFsCache.c
#define PAYLOAD_CACHE_SIZE_VGA (128 * 1024 * 4)                     //640*480
#define PAYLOAD_CACHE_SIZE_HD (128 * 1024 * 8 * 2)                  //(1024*1024*3)   //1280*720
#define PAYLOAD_CACHE_SIZE_COMPRESSED_JPEG_HD (128 * 1024 * 8 * 2)  //(1024*1024*3)   //1280*720
#define PAYLOAD_CACHE_SIZE_FHD (128 * 1024 * 8 * 4)                 //1920*1080

typedef enum REC_RENDER_PORT_INDEX_DEFINITION {
    RECR_PORT_INDEX_VIDEO = 0,
    RECR_PORT_INDEX_AUDIO,
    RECR_PORT_INDEX_TEXT,

    // counts of recorder render ports
    MAX_REC_RENDER_PORTS
} REC_RENDER_PORT_INDEX_DEFINITION;

typedef struct MuxChnAttr {
    MUX_CHN_ATTR_S *pChnAttr;
    MUX_CHN nChnId;
} MuxChnAttr;

typedef struct {
    MUX_CHN_ATTR_S mChnAttr;
    MUX_CHN mChnId;
    struct list_head mList;
} MuxChanAttrNode;

typedef struct RECRENDERDATATYPE {
    volatile COMP_STATETYPE state;
    pthread_mutex_t mStateMutex;
    COMP_CALLBACKTYPE *pCallbacks;
    void *pAppData;
    COMP_HANDLETYPE hSelf;

    COMP_PORT_PARAM_TYPE sPortParam;
    COMP_PARAM_PORTDEFINITIONTYPE sInPortDef[MAX_REC_RENDER_PORTS];        // port definition
    COMP_INTERNAL_TUNNELINFOTYPE sInPortTunnelInfo[MAX_REC_RENDER_PORTS];  // tunnel information
    COMP_PARAM_BUFFERSUPPLIERTYPE sPortBufSupplier[MAX_REC_RENDER_PORTS];  // buffer surpplier type?
    BOOL mInputPortTunnelFlag;                                             //TRUE: tunnel mode; FALSE: non-tunnel mode.

    MPP_CHN_S mMppChnInfo;  //mChnId is set as muxGroupId.
    pthread_t thread_id;
    CompInternalMsgType eTCmd;

    MUX_GRP_ATTR_S mGroupAttr;      //match media_inf.
    struct list_head mChnAttrList;  //MuxChanAttrNode, match mValidSinkInfoList.
    message_queue_t cmd_queue;
    volatile int mNoInputFrameFlag;  //1: no input frame to be muxed.

    CdxRecorderWriterCallbackInfo callback_writer;    //CdxRecorderWriterCallbackInfo
    int tracks_count, track_exist;                    //decided by recorder_mode.
    int64_t duration, duration_audio, duration_text;  //statistics for recRender. keep increase.
    int64_t mnBasePts;                                //us.
    int64_t mnAudioBasePts;                           //us.
    int64_t mnTextBasePts;                            //us.
    int64_t file_size_bytes;                          //statistics for recRender. keep increase.
    _media_file_inf_t media_inf;                      //match to mGroupAttr
    RECORDER_MODE recorder_mode;                      //decide by mGroupAttr
    int mSinkInfoValidNum;                            //validSinkNum + switchingSinkNum.
    int mSinkInfoTotalNum;
    struct list_head mValidSinkInfoList;      //RecSink, match to mChnAttrList
    struct list_head mSwitchingSinkInfoList;  //RecSinks which are during switching file, don't send rsPacket to them.
    struct list_head mIdleSinkInfoList;
    pthread_mutex_t mSinkInfoListMutex;

    VencHeaderData venc_extradata_info;

    BOOL mbSdCardState;  //true:exist; false:disappear

    int is_compress_source; /* gushiming compressed source */

    //int64_t             max_file_duration;  //unit:ms
    //int64_t impact_bftime;
    //int64_t impact_aftime;
    int64_t mCacheTime; //unit:ms
    RsPacketCacheManager *cache_manager;

    RecordFileDurationPolicy mRecordFileDurationPolicy;

    //FSWRITEMODE         mFsWriteMode;
    //int             mSimpleCacheSize;

    int mVideoInputFrameNum;
    struct list_head mVideoInputFrameIdleList;  //ENCODER_NODE_T
    struct list_head mVideoInputFrameReadyList;
    struct list_head mVideoInputFrameUsedList;
    pthread_mutex_t mVideoInputFrameListMutex;

    int mAudioInputFrameNum;
    struct list_head mAudioInputFrameIdleList;  //ENCODER_NODE_T
    struct list_head mAudioInputFrameReadyList;
    struct list_head mAudioInputFrameUsedList;
    pthread_mutex_t mAudioInputFrameListMutex;

    int mTextInputFrameNum;
    struct list_head mTextInputFrameIdleList;  //ENCODER_NODE_T
    struct list_head mTextInputFrameReadyList;
    struct list_head mTextInputFrameUsedList;
    pthread_mutex_t mTextInputFrameListMutex;
} RECRENDERDATATYPE;

//private interface
ERRORTYPE RecRenderGetPortDefinition(PARAM_IN COMP_HANDLETYPE hComponent,
                                     PARAM_INOUT COMP_PARAM_PORTDEFINITIONTYPE *pPortDef);
ERRORTYPE RecRenderSetPortDefinition(PARAM_IN COMP_HANDLETYPE hComponent,
                                     PARAM_IN COMP_PARAM_PORTDEFINITIONTYPE *pPortDef);
ERRORTYPE RecRenderGetCompBufferSupplier(PARAM_IN COMP_HANDLETYPE hComponent,
                                         PARAM_INOUT COMP_PARAM_BUFFERSUPPLIERTYPE *pPortBufSupplier);
ERRORTYPE RecRenderSetCompBufferSupplier(PARAM_IN COMP_HANDLETYPE hComponent,
                                         PARAM_IN COMP_PARAM_BUFFERSUPPLIERTYPE *pPortBufSupplier);
ERRORTYPE RecRenderGetPortParam(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_OUT COMP_PORT_PARAM_TYPE *pPortParam);
ERRORTYPE RecRenderGetMPPChannelInfo(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_OUT MPP_CHN_S *pChn);
ERRORTYPE RecRenderGetTunnelInfo(PARAM_IN COMP_HANDLETYPE hComponent,
                                 PARAM_INOUT COMP_INTERNAL_TUNNELINFOTYPE *pTunnelInfo);
ERRORTYPE RecRenderGetGroupAttr(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_OUT MUX_GRP_ATTR_S *pGroupAttr);
ERRORTYPE RecRenderSetGroupAttr(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN MUX_GRP_ATTR_S *pGroupAttr);
ERRORTYPE RecRenderGetChnAttr(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_INOUT MuxChnAttr *pChnAttr);
ERRORTYPE RecRenderSetChnAttr(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN MuxChnAttr *pChnAttr);
//public interface
ERRORTYPE RecRenderSendCommand(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_COMMANDTYPE Cmd,
                               PARAM_IN unsigned int nParam1, PARAM_IN void *pCmdData);
ERRORTYPE RecRenderGetState(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_OUT COMP_STATETYPE *pState);
ERRORTYPE RecRenderSetCallbacks(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_CALLBACKTYPE *pCallbacks,
                                PARAM_IN void *pAppData);
ERRORTYPE RecRenderGetConfig(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_INDEXTYPE nIndex,
                             PARAM_INOUT void *pComponentConfigStructure);
ERRORTYPE RecRenderSetConfig(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_INDEXTYPE nIndex,
                             PARAM_IN void *pComponentConfigStructure);
ERRORTYPE RecRenderComponentTunnelRequest(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN unsigned int nPort,
                                          PARAM_IN COMP_HANDLETYPE hTunneledComp, PARAM_IN unsigned int nTunneledPort,
                                          PARAM_INOUT COMP_TUNNELSETUPTYPE *pTunnelSetup);
ERRORTYPE RecRenderEmptyThisBuffer(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_BUFFERHEADERTYPE *pBuffer);
ERRORTYPE RecRenderComponentDeInit(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE RecRenderComponentInit(PARAM_IN COMP_HANDLETYPE hComponent);
#endif /* __IPCLINUX_RECRENDER_COMPONENT_H__ */
