/******************************************************************************
 Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
 File Name     : media_common.h
 Version       : Initial Draft
 Author        : Allwinner BU3-PD2 Team
 Created       : 2016/05/09
 Last Modified :
 Description   : component internal common.
 Function List :
 History       :
 ******************************************************************************/
#ifndef __IPCLINUX_COMPONENTCOMMON_H__
#define __IPCLINUX_COMPONENTCOMMON_H__

// ref platform headers
#include "plat_defines.h"
#include "plat_errno.h"
#include "plat_math.h"
#include "plat_type.h"

// media api headers to app
#include "mm_comm_video.h"
#include "mm_comm_aio.h"
#include "mm_common.h"
//#include "mm_comm_venc.h"
//#include "cve_dtca_result.h"

// media internal common headers.
//#include "media_common.h"
#include "mm_component.h"

typedef enum CompInternalMsgType {
    SetState,
    Flush,
    StopPort,
    RestartPort,
    MarkBuf,
    Stop,
    FillBuf,
    EmptyBuf,
    VDecComp_VbsAvailable = 0x100,
    VDecComp_OutFrameAvailable,
    VDecComp_ReopenVideoEngine,
    VDecComp_ChangeGraphicBufferProducer,
    VDecComp_CompFrameBufferThread_InputFrameAvailable,
    VDecComp_CompFrameBufferThread_OutFrameAvailable,
    VDecComp_InputData_UsedVbsAvailable,
    VRenderComp_InputFrameAvailable = 0x200,
    VRenderComp_ChangeANativeWindow,
    VRenderComp_ResolutionChange,
    VRenderComp_StoreFrame,
    ADecComp_AbsAvailable = 0x300,
    ADecComp_PcmBufAvailable,
    ARenderComp_InputFrameAvailable = 0x400,
    ARenderComp_FillBuffer,
    ARenderComp_ReturnAllBuffer,
    SDecComp_InputChunkAvailable = 0x500,
    SDecComp_OutFrameAvailable,
    SRenderComp_InputFrameAvailable = 0x600,
    SRenderComp_ReturnAllBuffer,
    VSourceComp_ReleaseFrameAvailable = 0x700,
    VEncComp_InputFrameAvailable = 0x800,
    VEncComp_OutputFrameReturn,
    AEncComp_OutFrameAvailable = 0x900,
    AEncComp_InputPcmAvailable,
    TSrcComp_InputTextAvailable = 0xA00,
    TEncComp_InputTextAvailable = 0xB00,
    TEncComp_OutFrameAvailable,
    RecRenderComp_InputFrameAvailable = 0xC00,
    VendorAddOutputSinkInfo,
    VendorRemoveOutputSinkInfo,
    SwitchFile, //deprecated.
    SwitchFileDone,
    SwitchFileNormal,
    RecSink_InputPacketAvailable,

    AIChannel_CapDataAvailable = 0xD00,
    AIChannel_PlayDataAvailable,
    AOChannel_InputPcmAvailable = 0xE00,
    AOChannel_OutBufAvailable,
	//VVdaComp_InputFrameAvailable,
	VViComp_InputFrameAvailable = 0xF00,
	VViComp_LongExpEvent,
	VViComp_StoreFrame,
    VIseComp_InputFrameAvailable = 0x1000,
    VendorAddIseChn,
    VendorRemoveIseChn,

    DemuxComp_VideoBufferAvailable = 0x1100,
    DemuxComp_AudioBufferAvailable,
    DemuxComp_SubtitleBufferAvailable,

    EisComp_InputFrameAvailable = 0x1200,
    EisComp_StoreFrame,

    UvcComp_InputFrameAvailable = 0x1300,
} CompInternalMsgType;

typedef enum CDX_comp_PRIV_FLAGS {
    CDX_comp_PRIV_FLAGS_REINIT = 0x01,
    CDX_comp_PRIV_FLAGS_STREAMEOF = 0x02,
} CDX_comp_PRIV_FLAGS;

typedef struct COMP_BUFFERSTATE {
    unsigned int nValidSizePercent;
    unsigned int nElementCounter;    // valid frame number.
    unsigned int video_stream_type;  // VIDEO_TYPE_MAJOR, CDX_VIDEO_STREAM_MAJOR
} COMP_BUFFERSTATE;

typedef struct CacheState {
    unsigned int mValidSizePercent;  // 0~100
    unsigned int mValidSize;         // unit:kB
    unsigned int mTotalSize;         // unit:kB
} CacheState;

typedef struct CDX_NotifyStartToRunTYPE {
    unsigned int nPortIndex;  // notify at which port.
    BOOL mbNotify;
} CDX_NotifyStartToRunTYPE;

#endif /* __IPCLINUX_COMPONENTCOMMON_H__ */
