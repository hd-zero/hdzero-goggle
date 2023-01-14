/*
********************************************************************************
*                           Android multimedia module
*
*          (c) Copyright 2010-2015, Allwinner Microelectronic Co., Ltd.
*                              All Rights Reserved
*
* File   : RecRenderCdxOutputSinkInfo.h
* Version: V1.0
* By     : eric_wang
* Date   : 2015-1-23
* Description:
********************************************************************************
*/
#ifndef _RECRENDERSINK_H_
#define _RECRENDERSINK_H_

#include <stdint.h>
//#include <include_omx/OMX_Types.h>
//#include <include_omx/OMX_Core.h>
//#include <FsWriter.h>
#include <mm_component.h>
#include <aencoder.h>
//#include <encoder_type.h>
#include <vencoder.h>
//#include <CDX_Recorder.h>
//#include <CDX_Common.h>
//#include <CDX_Component.h>
#include <record_writer.h>
#include <FsWriter.h>
#include <tmessage.h>
#include <tsemaphore.h>
#include "ComponentCommon.h"
#include <RecorderMode.h>

#define RECSINK_MAX_PACKET_NUM  (256)   //(BITSTREAM_FRAME_SIZE+FIFO_LEVEL)
#define MAX_TRACK_COUNT (3)
typedef enum REC_FILE_STATE {
	FILE_NORMAL = 0,
	FILE_NEED_SWITCH_TO_IMPACT,
	FILE_IMPACT_RECDRDING,
	FILE_NEED_SWITCH_TO_NORMAL, //normal switch file.
} REC_FILE_STATE;

typedef enum SourceType 
{
    SOURCE_TYPE_UNKNOWN = -1,
    SOURCE_TYPE_COMPONENT = 0,
    SOURCE_TYPE_CACHEMANAGER = 1,
    SOURCE_TYPE_OTHER,
}SourceType;

typedef struct RecSinkPacket {
    int     mId;
    int     mStreamType;   //CodecType
    int     mFlags;  //AVPACKET_FLAGS
    int64_t     mPts;   //unit:us
    char*     mpData0;
    int     mSize0;
    char*     mpData1;
    int     mSize1;

    //for video frame.
    int mCurrQp;         //qp of current frame
    int mavQp;
	int mnGopIndex;      //index of gop
	int mnFrameIndex;    //index of current frame in gop.
	int mnTotalIndex;    //index of current frame in whole encoded frames

    int  mSourceType;    //SourceType of [RecRenderSink.h]. indicate where RSPacket's data come from.
    int mRefCnt;
    struct list_head mList;
} RecSinkPacket;

typedef struct RecSinkCallbackType
{
    ERRORTYPE (*EventHandler)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN void* pAppData,
        PARAM_IN COMP_EVENTTYPE eEvent,
        PARAM_IN unsigned int nData1,
        PARAM_IN unsigned int nData2,
        PARAM_IN void* pEventData);

    ERRORTYPE (*EmptyBufferDone)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN void* pAppData,
        PARAM_IN RecSinkPacket* pRSPacket);

} RecSinkCallbackType;

typedef struct RecSink
{
    int                 mMuxerId;
    MUXERMODES          nMuxerMode;
    char                *mPath;
    int                 nOutputFd;
    unsigned int                 nFallocateLen;
    int                 nCallbackOutFlag;
    BOOL                bBufFromCacheFlag;

    //FILE                *pOutputFile;
    CDX_RecordWriter    *pWriter;
    void                *pMuxerCtx;
    volatile BOOL   mbMuxerInit;
    BOOL            mbTrackInit[MAX_TRACK_COUNT];   //for write fd, clear when switch fd.
    int             mDuration;                      //for write fd, clear when switch fd. unit:ms
    int             mDurationAudio;                 //for write fd, clear when switch fd.
    int             mDurationText;                  //for write fd, clear when switch fd.
    int             mPrevDuration;                  //duration sum before current pkt. unit:ms. mPrevDuration + CurPktDuration = mDuration
    int             mPrevDurationAudio;
    int             mPrevDurationText;
    int64_t             mLoopDuration;                  //total duration from start point when loop recording. unit:ms 
    int64_t             mLoopDurationAudio;                  //total duration from start point when loop recording. unit:ms
    int64_t             mFileSizeBytes;                 //for write fd, clear when switch fd.
    BOOL                bNeedSw;                    // flag used to indicate that the condition for filw switch has been checked and need to switch,no need to check any more. 
    BOOL                bNeedSwAudio;                    // flag used to indicate that the condition for filw switch has been checked and need to switch,no need to check any more.
    int             mVideoFrameCounter;             //count video frame number during one file, clear when switch fd.
    int64_t             mPrevPts[MAX_TRACK_COUNT];  //calculated pts. clear when switch fd, unit:us
    int64_t             mBasePts[MAX_TRACK_COUNT];  //first source pts from encoder. clear when switch fd, unit:us
    int64_t             mInputPrevPts[MAX_TRACK_COUNT];  //record input previous frame source pts, unit:us
    int64_t             mOrigBasePts[MAX_TRACK_COUNT];  //first source pts. Keep valid during recSink Record, not clear when switch fd. for future, not use now. unit:us
    int64_t             mDebugInputPts[MAX_TRACK_COUNT];    //for debug, source pts, unit:us

    pthread_mutex_t     mutex_reset_writer_lock;    //lock nSwitchFd(mSwitchFilePath), reset_fd_flag, mbMuxerInit, rec_file, nOutputFd(mPath), pWriter, pMuxerCtx
    int                 nSwitchFd;  //need dup fd.
    char                *mSwitchFilePath;
    unsigned int                 nSwitchFdFallocateSize;
    //int             mSwitchFdImpactFlag;    //1: nSwitchFd is impact switchFd; 0: nSwitchFd is common switchFd; after thread process impact, thread will clear its value.
    volatile BOOL            reset_fd_flag;  //true:new fd is ready and wait to be used; false: new fd is used, no new fd has come, there is not waiting fd.
    volatile BOOL            need_set_next_fd;   //true:need next fd, but not notify app; false: has already notify app need next fd, or not need next fd.
    volatile REC_FILE_STATE      rec_file;
    //int64_t             mImpactStartTm; //unit:ms
    //int64_t             mImpactAgainTm;
    //int64_t             mImpactPrevTm;
    int64_t             mCurMaxFileDuration;  //unit:ms
    int64_t             mCurFileEndTm;  //unit:ms
    int64_t             mMaxFileDuration;  //unit:ms
    //int64_t             mImpactFileDuration;  //unit:ms
    int64_t             mMaxFileSizeBytes;  //unit:byte

    RECORDER_MODE       mRecordMode;
    RecordFileDurationPolicy mFileDurationPolicy;
    FSWRITEMODE         mFsWriteMode;
    int             mFsSimpleCacheSize;
    _media_file_inf_t   *mpMediaInf;    //RECRENDERDATATYPE->media_inf
    VencHeaderData      *mpVencExtraData;   //RECRENDERDATATYPE->venc_extradata_info
    //__extra_data_t      mAudioExtraDataForMp4;
    CdxRecorderWriterCallbackInfo *mpCallbackWriter;    //RECRENDERDATATYPE->callback_writer
    volatile BOOL   mbSdCardState; //true:exist; false:disappear
    
    RecSinkCallbackType*    mpCallbacks;
    void*                 mpAppData;

    message_queue_t mMsgQueue;
    cdx_sem_t       mSemStateComplete;
    //cdx_sem_t       mSemCmdComplete;    //provide for caller, use it in EventHandler().
    pthread_t       mThreadId;
    //struct list_head    mRSPacketBufList;   //DynamicBuffer, sizeof(RecSinkPacket)*RECSINK_MAX_PACKET_NUM
    BOOL                mPrefetchFlag;  //true: non-audio data, validList->prefetchList
    BOOL                mPrefetchFlagAudio;
    BOOL                bTimeMeetAudio;
    int                 mVideoFrmCntWriteMore;
    int64_t             mVideoPtsWriteMoreSt;
    int64_t             mVideoPtsWriteMoreEnd;
    struct list_head    mPrefetchRSPacketList;  //valid->prefetch, at next file begin, move back to valid list. 
    struct list_head    mPrefetchRSPacketListAudio;  //valid->prefetch, at next file begin, move back to valid list.
    struct list_head    mValidRSPacketList; //RecSinkPacket
    struct list_head    mIdleRSPacketList;
    pthread_mutex_t     mRSPacketListMutex;
    volatile BOOL   mNoInputPacketFlag; //1: no input frame to be muxed.

    BOOL mbShutDownNowFlag;

    volatile COMP_STATETYPE   mStatus;   //OMX_StateInvalid, OMX_StateLoaded, OMX_StateExecuting, OMX_StatePause.

    ERRORTYPE (*ConfigByCdxSink)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN CdxOutputSinkInfo *pCdxSink);
    
    ERRORTYPE (*SetCallbacks)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN RecSinkCallbackType* pCallbacks,
        PARAM_IN void* pAppData);

    ERRORTYPE (*EmptyThisBuffer)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN RecSinkPacket* pRSPacket);

    ERRORTYPE (*SetRecordMode)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN RECORDER_MODE nMode);

    ERRORTYPE (*SetMediaInf)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN _media_file_inf_t* pMediaInf);

    ERRORTYPE (*SetVencExtraData)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN VencHeaderData* pExtraData);

    ERRORTYPE (*SetMaxFileDuration)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN int64_t nDuration);

//    ERRORTYPE (*SetImpactFileDuration)(
//        PARAM_IN COMP_HANDLETYPE hComponent,
//        PARAM_IN int64_t nDuration);

    ERRORTYPE (*SetFileDurationPolicy)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN RecordFileDurationPolicy nPolicy);
    
    ERRORTYPE (*SetMaxFileSize)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN int64_t nSize);

    ERRORTYPE (*SetFsWriteMode)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN FSWRITEMODE nFsWriteMode);

    ERRORTYPE (*SetFsSimpleCacheSize)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN int nSize);

    ERRORTYPE (*SetCallbackWriter)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN CdxRecorderWriterCallbackInfo *pCallbackWriter);

    ERRORTYPE (*SwitchFd)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN int nFd,
        PARAM_IN int nFallocateLen,
        PARAM_IN int nIsImpact);

    ERRORTYPE (*SendCmdSwitchFile)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN BOOL nCacheFlag);

    ERRORTYPE (*SendCmdSwitchFileNormal)(
        PARAM_IN COMP_HANDLETYPE hComponent);

    ERRORTYPE (*SetSdcardState)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN BOOL bSdcardState);

    ERRORTYPE (*Reset)(
        PARAM_IN COMP_HANDLETYPE hComponent);

    ERRORTYPE (*SetShutDownNow)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN BOOL bShutDownNowFlag);

    struct list_head    mList;
}RecSink;
ERRORTYPE RecSinkInit(PARAM_IN RecSink *pThiz);
ERRORTYPE RecSinkDestroy(RecSink *pThiz);

#endif  /* _RECRENDERSINK_H_ */

