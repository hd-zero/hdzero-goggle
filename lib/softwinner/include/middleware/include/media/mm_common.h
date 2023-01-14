/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mm_common.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/03/15
  Last Modified :
  Description   : The common defination for multimedia.
  Function List :
  History       :
******************************************************************************/

#ifndef __IPCLINUX_MM_COMMON_H__
#define __IPCLINUX_MM_COMMON_H__

#include "plat_defines.h"
#include "plat_math.h"
#include "plat_type.h"

//#include <linux/videodev2.h>
//#include "mm_comm_video.h"
//#include "mm_comm_aio.h"

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

#ifndef VER_X
#define VER_X 1
#endif

#ifndef VER_Y
#define VER_Y 0
#endif

#ifndef VER_Z
#define VER_Z 0
#endif

#ifndef VER_P
#define VER_P 0
#endif

#ifndef VER_B
#define VER_B 0
#endif

#ifdef HI_DEBUG
#define VER_D " Debug"
#else
#define VER_D " Release"
#endif

#define __MK_VERSION(x, y, z, p, b) #x "." #y "." #z "." #p " B0" #b
#define MK_VERSION(x, y, z, p, b) __MK_VERSION(x, y, z, p, b)
#define MPP_VERSION CHIP_NAME MPP_VER_PRIX MK_VERSION(VER_X, VER_Y, VER_Z, VER_P, VER_B) VER_D

#define VERSION_NAME_MAXLEN 64
typedef struct MPP_VERSION_S {
    char aVersion[VERSION_NAME_MAXLEN];
} MPP_VERSION_S;

typedef struct POINT_S {
    int X;
    int Y;
} POINT_S;

typedef struct SIZE_S {
    unsigned int Width;
    unsigned int Height;
} SIZE_S;

typedef struct RECT_S {
    int X;
    int Y;
    unsigned int Width;
    unsigned int Height;
} RECT_S;

typedef struct CROP_INFO_S {
    BOOL bEnable;
    RECT_S Rect;
} CROP_INFO_S;

typedef enum ROTATE_E { ROTATE_NONE = 0, ROTATE_90 = 1, ROTATE_180 = 2, ROTATE_270 = 3, ROTATE_BUTT } ROTATE_E;

typedef struct BORDER_S {
    unsigned int mTopWidth;
    unsigned int mBottomWidth;
    unsigned int mLeftWidth;
    unsigned int mRightWidth;
    unsigned int mColor;
} BORDER_S;

typedef int AI_CHN;
typedef int AO_CHN;
typedef int AENC_CHN;
typedef int ADEC_CHN;
typedef int AUDIO_DEV;
typedef int AVENC_CHN;
typedef int VI_DEV;
typedef int VI_WAY;
typedef int VI_CHN;
typedef int VO_DEV;
typedef int VO_LAYER;
typedef int VO_CHN;
typedef int VO_WBC;
typedef int GRAPHIC_LAYER;
typedef int VENC_CHN;
typedef int VDEC_CHN;
typedef int VENC_GRP;
typedef int VO_GRP;
typedef int VDA_CHN;
typedef int IVE_HANDLE;
typedef int ISP_DEV;
typedef int SENSOR_ID;
typedef int FD_HANDLE;
typedef int MD_CHN;
typedef int DEMUX_CHN;
typedef int MUX_GRP;
typedef int MUX_CHN;
typedef int CLOCK_CHN;
typedef char* UVC_DEV;
typedef int UVC_CHN;
typedef int ISE_CHN;
typedef int ISE_GRP;
typedef int EIS_CHN;
typedef int EIS_GRP;
typedef int TENC_CHN;

#define MM_INVALID_CHN (-1)
#define MM_INVALID_WAY (-1)
#define MM_INVALID_LAYER (-1)
#define MM_INVALID_DEV (-1)
#define MM_INVALID_HANDLE (-1)
#define MM_INVALID_VALUE (-1)
#define MM_INVALID_TYPE (-1)

typedef enum MOD_ID_E {
    MOD_ID_CMPI = 0,
    MOD_ID_VB = 1,
    MOD_ID_SYS = 2,
    MOD_ID_RGN = 3,
    MOD_ID_CHNL = 4,
    MOD_ID_VDEC = 5,
    MOD_ID_GROUP = 6,
    MOD_ID_VPSS = 7,
    MOD_ID_VENC = 8,
    //MOD_ID_VDA = 9,
    MOD_ID_H264E = 10,
    MOD_ID_JPEGE = 11,
    MOD_ID_MPEG4E = 12,
    MOD_ID_H264D = 13,
    MOD_ID_JPEGD = 14,
    MOD_ID_VOU = 15,
    MOD_ID_VIU = 16,
    MOD_ID_DSU = 17,
    MOD_ID_VALG = 18,
    MOD_ID_RC = 19,
    MOD_ID_AIO = 20,
    MOD_ID_AI = 21,
    MOD_ID_AO = 22,
    MOD_ID_AENC = 23,
    MOD_ID_ADEC = 24,
    MOD_ID_AVENC = 25,
    MOD_ID_PCIV = 26,
    MOD_ID_PCIVFMW = 27,
    MOD_ID_ISP = 28,
    MOD_ID_IVE = 29,
    MOD_ID_UVC = 30,

    MOD_ID_DCCM = 31,
    MOD_ID_DCCS = 32,
    MOD_ID_PROC = 33,
    MOD_ID_LOG = 34,
    MOD_ID_MST_LOG = 35,
    MOD_ID_VD = 36,

    MOD_ID_VCMP = 38,
    MOD_ID_FB = 39,
    MOD_ID_HDMI = 40,
    MOD_ID_VOIE = 41,
    MOD_ID_TDE = 42,
    MOD_ID_USR = 43,
    MOD_ID_VEDU = 44,
    MOD_ID_VGS = 45,
    MOD_ID_H265E = 46,
    MOD_ID_FD = 47,
    MOD_ID_ODT = 48,  //Object detection trace
    MOD_ID_VQA = 49,  //Video quality  analysis
    MOD_ID_LPR = 50,  // License Plate Recognition

    MOD_ID_COMPCORE = 100,
    MOD_ID_DEMUX = 101,
    MOD_ID_MUX = 102,
    MOD_ID_CLOCK = 103,
    MOD_ID_CSI = 104,

    MOD_ID_ISE = 229,
    MOD_ID_EIS = 230,

    MOD_ID_TENC = 231,
    MOD_ID_BUTT,
} MOD_ID_E;

typedef struct MPP_CHN_S {
    MOD_ID_E mModId;
    int mDevId;  //VO set VoLayer to it. ISE set grpId to it.
    int mChnId;  //muxer group set grpId to it;
} MPP_CHN_S;

typedef enum PROFILE_TYPE_E {
    PROFILE_1080P_30 = 0,
    PROFILE_3M_30,
    PROFILE_1080P_60,
    PROFILE_5M_30,
    PROFILE_BUTT,
} PROFILE_TYPE_E;

#define MPP_MOD_VIU "vi"
#define MPP_MOD_VOU "vo"
#define MPP_MOD_HDMI "hdmi"
#define MPP_MOD_DSU "dsu"
#define MPP_MOD_VGS "vgs"

#define MPP_MOD_CHNL "chnl"
#define MPP_MOD_VENC "venc"
#define MPP_MOD_GRP "grp"
#define MPP_MOD_VDA "vda"
#define MPP_MOD_VPSS "vpss"
#define MPP_MOD_RGN "rgn"
#define MPP_MOD_IVE "ive"
#define MPP_MOD_FD "fd"

#define MPP_MOD_H264E "h264e"
#define MPP_MOD_H265E "h265e"
#define MPP_MOD_JPEGE "jpege"
#define MPP_MOD_MPEG4E "mpeg4e"

#define MPP_MOD_VDEC "vdec"
#define MPP_MOD_H264D "h264d"
#define MPP_MOD_JPEGD "jpegd"

#define MPP_MOD_AI "ai"
#define MPP_MOD_AO "ao"
#define MPP_MOD_AENC "aenc"
#define MPP_MOD_ADEC "adec"
#define MPP_MOD_AIO "aio"
#define MPP_MOD_ACODEC "acodec"

#define MPP_MOD_VB "vb"
#define MPP_MOD_SYS "sys"
#define MPP_MOD_CMPI "cmpi"

#define MPP_MOD_PCIV "pciv"
#define MPP_MOD_PCIVFMW "pcivfmw"

#define MPP_MOD_PROC "proc"
#define MPP_MOD_LOG "logmpp"
#define MPP_MOD_MST_LOG "mstlog"

#define MPP_MOD_DCCM "dccm"
#define MPP_MOD_DCCS "dccs"

#define MPP_MOD_VCMP "vcmp"
#define MPP_MOD_FB "fb"

#define MPP_MOD_RC "rc"

#define MPP_MOD_VOIE "voie"

#define MPP_MOD_TDE "tde"
#define MPP_MOD_ISP "isp"
#define MPP_MOD_USR "usr"

/* We just coyp this value of payload type from RTP/RTSP definition */
typedef enum {
    PT_PCMU = 0,
    PT_1016 = 1,
    PT_G721 = 2,
    PT_GSM = 3,
    PT_G723 = 4,
    PT_DVI4_8K = 5,
    PT_DVI4_16K = 6,
    PT_LPC = 7,
    PT_PCMA = 8,
    PT_G722 = 9,
    PT_S16BE_STEREO = 10,
    PT_S16BE_MONO = 11,
    PT_QCELP = 12,
    PT_CN = 13,
    PT_MPEGAUDIO = 14,
    PT_G728 = 15,
    PT_DVI4_3 = 16,
    PT_DVI4_4 = 17,
    PT_G729 = 18,
    PT_G711A = 19,
    PT_G711U = 20,
    PT_G726 = 21,
    PT_G729A = 22,
    PT_LPCM = 23,
    PT_CelB = 25,
    PT_JPEG = 26,
    PT_CUSM = 27,
    PT_NV = 28,
    PT_PICW = 29,
    PT_CPV = 30,
    PT_H261 = 31,
    PT_MPEGVIDEO = 32,
    PT_MPEG2TS = 33,
    PT_H263 = 34,
    PT_SPEG = 35,
    PT_MPEG2VIDEO = 36,
    PT_AAC = 37,
    PT_WMA9STD = 38,
    PT_HEAAC = 39,
    PT_PCM_VOICE = 40,
    PT_PCM_AUDIO = 41,
    PT_AACLC = 42,
    PT_MP3 = 43,
    PT_ADPCMA = 49,
    PT_AEC = 50,
    PT_X_LD = 95,
    PT_H264 = 96,
    PT_D_GSM_HR = 200,
    PT_D_GSM_EFR = 201,
    PT_D_L8 = 202,
    PT_D_RED = 203,
    PT_D_VDVI = 204,
    PT_D_BT656 = 220,
    PT_D_H263_1998 = 221,
    PT_D_MP1S = 222,
    PT_D_MP2P = 223,
    PT_D_BMPEG = 224,
    PT_MP4VIDEO = 230,
    PT_MP4AUDIO = 237,
    PT_VC1 = 238,
    PT_JVC_ASF = 255,
    PT_D_AVI = 256,
    PT_DIVX3 = 257,
    PT_AVS = 258,
    PT_REAL8 = 259,
    PT_REAL9 = 260,
    PT_VP6 = 261,
    PT_VP6F = 262,
    PT_VP6A = 263,
    PT_SORENSON = 264,
    PT_H265 = 265,
    PT_MAX = 266,
    /* add by hisilicon */
    PT_AMR = 1001,
    PT_MJPEG = 1002,
    PT_AMRWB = 1003,
    PT_TEXT = 1004,
    PT_BUTT
} PAYLOAD_TYPE_E;

typedef enum {
    MEDIA_FILE_FORMAT_DEFAULT = 0,
    MEDIA_FILE_FORMAT_MP4 = 0,
    MEDIA_FILE_FORMAT_TS,
    MEDIA_FILE_FORMAT_MP3,
    MEDIA_FILE_FORMAT_AAC,
    MEDIA_FILE_FORMAT_RAW,
    MEDIA_FILE_FORMAT_UNKNOWN,
} MEDIA_FILE_FORMAT_E;
BOOL judgeAudioFileFormat(MEDIA_FILE_FORMAT_E fileFormat);

typedef enum MPP_EVENT_TYPE {
    MPP_EVENT_NONE = 0,
    MPP_EVENT_RELEASE_VIDEO_BUFFER,  //VIDEO_FRAME_INFO_S for recorder/VIChannel::DoVdaThread, ISE, VO, VENC
    MPP_EVENT_VENC_TIMEOUT, //uint64_t*
    MPP_EVENT_VENC_BUFFER_FULL, // NULL, the venc outbuffer  is full or too samall.
    MPP_EVENT_RELEASE_ISE_VIDEO_BUFFER0,  //VIDEO_FRAME_INFO_S for recorder/VIChannel::DoVdaThread, ISE, VO, VENC
    MPP_EVENT_RELEASE_ISE_VIDEO_BUFFER1,  //VIDEO_FRAME_INFO_S for recorder/VIChannel::DoVdaThread, ISE, VO, VENC
    MPP_EVENT_RELEASE_AUDIO_BUFFER,  //AUDIO_FRAME_S
    MPP_EVENT_BSFRAME_AVAILABLE,     //CDXRecorderBsInfo
    MPP_EVENT_ERROR_ENCBUFFER_OVERFLOW,
    MPP_EVENT_NEED_NEXT_FD,  // int muxerId
    MPP_EVENT_RECORD_DONE,   // int muxerId
    MPP_EVENT_WRITE_DISK_ERROR, // int muxerId
    MPP_EVENT_CAPTURE_AUDIO_DATA, // unsigned int size;

    MPP_EVENT_NOTIFY_EOF = 0x100,
    MPP_EVENT_SET_VIDEO_SIZE,  //SIZE_S
    MPP_EVENT_RENDERING_START,
    MPP_EVENT_VDEC_NOTIFY_NO_FRAME_BUFFER,
} MPP_EVENT_TYPE;

typedef ERRORTYPE (*MPPCallbackFuncType)(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData);
typedef struct MPPCallbackInfo {
    void *cookie;                  //EyeseeRecorder*
    MPPCallbackFuncType callback;  //MPPCallbackWrapper
} MPPCallbackInfo;

#define CHECK_MPP_CALLBACK(FuncPtr)                                 \
    do {                                                            \
        if (FuncPtr == NULL) {                                      \
            aloge("Error! User should RegisterCallback to mpp!");   \
            return -1;                                              \
        }                                                           \
    } while (0)

typedef enum {
    AUDIO_SOURCE_DEFAULT = 0,
    AUDIO_SOURCE_MIC = 1,
    AUDIO_SOURCE_VOICE_UPLINK = 2,
    AUDIO_SOURCE_VOICE_DOWNLINK = 3,
    AUDIO_SOURCE_VOICE_CALL = 4,
    AUDIO_SOURCE_CAMCORDER = 5,
    AUDIO_SOURCE_VOICE_RECOGNITION = 6,
    AUDIO_SOURCE_VOICE_COMMUNICATION = 7,
    AUDIO_SOURCE_REMOTE_SUBMIX = 8,
    AUDIO_SOURCE_AF = 9,
    AUDIO_SOURCE_FM = 10,

    AUDIO_SOURCE_LIST_END  // must be last - used to validate audio source type
} AUDIO_SOURCE_E;

typedef enum {
    AUDIO_ENCODER_DEFAULT = 0,
    AUDIO_ENCODER_AMR_NB = 1,
    AUDIO_ENCODER_AMR_WB = 2,
    AUDIO_ENCODER_AAC = 3,
    AUDIO_ENCODER_HE_AAC = 4,
    AUDIO_ENCODER_AAC_ELD = 5,

    AUDIO_ENCODER_PCM = 0x100,
    AUDIO_ENCODER_MP3 = 0x101,

    AUDIO_ENCODER_LIST_END  // must be the last - used to validate the audio encoder type
} AUDIO_ENCODER_E;

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif /* __IPCLINUX_MM_COMMON_H__ */
