
/*
* Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : vdecoder.h
* Description :
* History :
*   Author  : xyliu <xyliu@allwinnertech.com>
*   Date    : 2016/04/13
*   Comment :
*
*
*/

#ifndef VDECODER_H
#define VDECODER_H

#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include "typedef.h"
#include <sc_interface.h>
#include "veInterface.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AFBC_VERSION   4
#define AFBC_FILEHEADER_SIZE 32

#define AFBC_ENABLE_SIZE_WIDTH (3840)  //* enable afbc when (pic_size) >= (the_value)
#define AFBC_ENABLE_SIZE_HEIGHT (2160)

#define ENABLE_HIGH_CHANNAL_SIZE_WIDTH (3840)
#define ENABLE_HIGH_CHANNAL_SIZE_HEIGHT (2160)
#define ENABLE_SBM_FRAME_INTERFACE (1)
#define ENABLE_NEW_MEMORY_OPTIMIZATION_PROGRAM (0)

typedef struct VeProcDecInfo {
    unsigned int     nChannelNum;
    char             nProfile[10];

    unsigned int     nWidth;
    unsigned int     nHeight;
    s32              nCurFrameRef;
    int              nFrameRate;
}VeProcDecInfo;


typedef enum CONTROL_AFBC_MODE {
    DISABLE_AFBC_ALL_SIZE     = 0,
    ENABLE_AFBC_JUST_BIG_SIZE = 1, //* >= 4k
    ENABLE_AFBC_ALL_SIZE      = 2,
}eControlAfbcMode;

typedef enum CONTROL_IPTV_MODE {
    DISABLE_IPTV_ALL_SIZE     = 0,
    ENABLE_IPTV_JUST_SMALL_SIZE = 1, //* < 4k
    ENABLE_IPTV_ALL_SIZE      = 2,
}eControlIptvMode;

enum EVIDEOCODECFORMAT
{
    VIDEO_CODEC_FORMAT_UNKNOWN          = 0,
    VIDEO_CODEC_FORMAT_MJPEG            = 0x101,
    VIDEO_CODEC_FORMAT_MPEG1            = 0x102,
    VIDEO_CODEC_FORMAT_MPEG2            = 0x103,
    VIDEO_CODEC_FORMAT_MPEG4            = 0x104,
    VIDEO_CODEC_FORMAT_MSMPEG4V1        = 0x105,
    VIDEO_CODEC_FORMAT_MSMPEG4V2        = 0x106,
    VIDEO_CODEC_FORMAT_DIVX3            = 0x107,
    VIDEO_CODEC_FORMAT_DIVX4            = 0x108,
    VIDEO_CODEC_FORMAT_DIVX5            = 0x109,
    VIDEO_CODEC_FORMAT_XVID             = 0x10a,
    VIDEO_CODEC_FORMAT_H263             = 0x10b,
    VIDEO_CODEC_FORMAT_SORENSSON_H263   = 0x10c,
    VIDEO_CODEC_FORMAT_RXG2             = 0x10d,
    VIDEO_CODEC_FORMAT_WMV1             = 0x10e,
    VIDEO_CODEC_FORMAT_WMV2             = 0x10f,
    VIDEO_CODEC_FORMAT_WMV3             = 0x110,
    VIDEO_CODEC_FORMAT_VP6              = 0x111,
    VIDEO_CODEC_FORMAT_VP8              = 0x112,
    VIDEO_CODEC_FORMAT_VP9              = 0x113,
    VIDEO_CODEC_FORMAT_RX               = 0x114,
    VIDEO_CODEC_FORMAT_H264             = 0x115,
    VIDEO_CODEC_FORMAT_H265             = 0x116,
    VIDEO_CODEC_FORMAT_AVS              = 0x117,

    VIDEO_CODEC_FORMAT_MAX              = VIDEO_CODEC_FORMAT_AVS,
    VIDEO_CODEC_FORMAT_MIN              = VIDEO_CODEC_FORMAT_MJPEG,
};

enum EPIXELFORMAT
{
    PIXEL_FORMAT_DEFAULT            = 0,

    PIXEL_FORMAT_YUV_PLANER_420     = 1,
    PIXEL_FORMAT_YUV_PLANER_422     = 2,
    PIXEL_FORMAT_YUV_PLANER_444     = 3,

    PIXEL_FORMAT_YV12               = 4,
    PIXEL_FORMAT_NV21               = 5,
    PIXEL_FORMAT_NV12               = 6,
    PIXEL_FORMAT_YUV_MB32_420       = 7,
    PIXEL_FORMAT_YUV_MB32_422       = 8,
    PIXEL_FORMAT_YUV_MB32_444       = 9,

    PIXEL_FORMAT_RGBA                = 10,
    PIXEL_FORMAT_ARGB                = 11,
    PIXEL_FORMAT_ABGR                = 12,
    PIXEL_FORMAT_BGRA                = 13,

    PIXEL_FORMAT_YUYV                = 14,
    PIXEL_FORMAT_YVYU                = 15,
    PIXEL_FORMAT_UYVY                = 16,
    PIXEL_FORMAT_VYUY                = 17,

    PIXEL_FORMAT_PLANARUV_422        = 18,
    PIXEL_FORMAT_PLANARVU_422        = 19,
    PIXEL_FORMAT_PLANARUV_444        = 20,
    PIXEL_FORMAT_PLANARVU_444        = 21,
    PIXEL_FORMAT_P010_UV             = 22,
    PIXEL_FORMAT_P010_VU             = 23,

    PIXEL_FORMAT_MIN = PIXEL_FORMAT_DEFAULT,
    PIXEL_FORMAT_MAX = PIXEL_FORMAT_PLANARVU_444,
};

enum EVDECODERESULT
{
    VDECODE_RESULT_UNSUPPORTED       = -1,
    VDECODE_RESULT_OK                = 0,
    VDECODE_RESULT_FRAME_DECODED     = 1,
    VDECODE_RESULT_CONTINUE          = 2,
    VDECODE_RESULT_KEYFRAME_DECODED  = 3,
    VDECODE_RESULT_NO_FRAME_BUFFER   = 4,
    VDECODE_RESULT_NO_BITSTREAM      = 5,
    VDECODE_RESULT_RESOLUTION_CHANGE = 6,

    VDECODE_RESULT_MIN = VDECODE_RESULT_UNSUPPORTED,
    VDECODE_RESULT_MAX = VDECODE_RESULT_RESOLUTION_CHANGE,
};

//*for new display
typedef struct FBMBUFINFO
{
    int nBufNum;
    int nBufWidth;
    int nBufHeight;
    int ePixelFormat;
    int nAlignValue;
    int bProgressiveFlag;
    int bIsSoftDecoderFlag;

    int bHdrVideoFlag;
    int b10bitVideoFlag;
    int bAfbcModeFlag;

    int nTopOffset;
    int nBottomOffset;
    int nLeftOffset;
    int nRightOffset;
}FbmBufInfo;

typedef struct VIDEOSTREAMINFO
{
    int   eCodecFormat;
    int   nWidth;
    int   nHeight;
    int   nFrameRate;
    int   nFrameDuration;
    int   nAspectRatio;
    int   bIs3DStream;
    int   nCodecSpecificDataLen;
    char* pCodecSpecificData;
    int   bSecureStreamFlag;
    int   bSecureStreamFlagLevel1;
    int   bIsFramePackage; /* 1: frame package;  0: stream package */
    int   h265ReferencePictureNum;
    int   bReOpenEngine;
    int   bIsFrameCtsTestFlag;
}VideoStreamInfo;

typedef struct VCONFIG
{
    int bScaleDownEn;
    int bRotationEn;
    int bSecOutputEn;
    int nHorizonScaleDownRatio;
    int nVerticalScaleDownRatio;
    int nSecHorizonScaleDownRatio;
    int nSecVerticalScaleDownRatio;
    int nRotateDegree;
    int bThumbnailMode;
    int eOutputPixelFormat;
    int eSecOutputPixelFormat;
    int bNoBFrames;
    int bDisable3D;
    int bSupportMaf;    //not use
    int bDispErrorFrame;
    int nVbvBufferSize;
    int nFrameBufferNum;
    int bSecureosEn;
    int  bGpuBufValid;
    int  nAlignStride;
    int  bIsSoftDecoderFlag;
    int  bVirMallocSbm;
    int  bSupportPallocBufBeforeDecode;
    //only used for xuqi, set this flag to 1 meaning palloc the fbm buffer before
    // decode the sequence, to short the first frame decoing time
    int nDeInterlaceHoldingFrameBufferNum;
    int nDisplayHoldingFrameBufferNum;
    int nRotateHoldingFrameBufferNum;
    int nDecodeSmoothFrameBufferNum;
    int bIsTvStream;

    struct ScMemOpsS *memops;
    eControlAfbcMode  eCtlAfbcMode;
    eControlIptvMode  eCtlIptvMode;

    VeOpsS*           veOpsS;
    void*             pVeOpsSelf;
    int               bConvertVp910bitTo8bit;
    unsigned int      nVeFreq;

    int               bCalledByOmxFlag;

    int               bSetProcInfoEnable; //* for check the decoder info by cat devices-note
    int               nSetProcInfoFreq;
    int               nChannelNum;
    int               nSupportMaxWidth;  //the max width of mjpeg continue decode
    int               nSupportMaxHeight; //the max height of mjpeg continue decode
}VConfig;

typedef struct VIDEOSTREAMDATAINFO
{
    char*   pData;
    int     nLength;
    int64_t nPts;
    int64_t nPcr;
    int     bIsFirstPart;
    int     bIsLastPart;
    int     nID;
    int     nStreamIndex;
    int     bValid;
    unsigned int     bVideoInfoFlag;
    void*   pVideoInfo;
}VideoStreamDataInfo;

typedef enum VIDEO_TRANSFER
{
    VIDEO_TRANSFER_RESERVED_0      = 0,
    VIDEO_TRANSFER_BT1361          = 1,
    VIDEO_TRANSFER_UNSPECIFIED     = 2,
    VIDEO_TRANSFER_RESERVED_1      = 3,
    VIDEO_TRANSFER_GAMMA2_2        = 4,
    VIDEO_TRANSFER_GAMMA2_8        = 5,
    VIDEO_TRANSFER_SMPTE_170M      = 6,
    VIDEO_TRANSFER_SMPTE_240M      = 7,
    VIDEO_TRANSFER_LINEAR          = 8,
    VIDEO_TRANSFER_LOGARITHMIC_0   = 9,
    VIDEO_TRANSFER_LOGARITHMIC_1   = 10,
    VIDEO_TRANSFER_IEC61966        = 11,
    VIDEO_TRANSFER_BT1361_EXTENDED = 12,
    VIDEO_TRANSFER_SRGB     = 13,
    VIDEO_TRANSFER_BT2020_0 = 14,
    VIDEO_TRANSFER_BT2020_1 = 15,
    VIDEO_TRANSFER_ST2084   = 16,
    VIDEO_TRANSFER_ST428_1  = 17,
    VIDEO_TRANSFER_HLG      = 18,
    VIDEO_TRANSFER_RESERVED = 19, //* 19~255
}VIDEO_TRANSFER;

typedef enum VIDEO_MATRIX_COEFFS
{
    VIDEO_MATRIX_COEFFS_IDENTITY       = 0,
    VIDEO_MATRIX_COEFFS_BT709          = 1,
    VIDEO_MATRIX_COEFFS_UNSPECIFIED_0  = 2,
    VIDEO_MATRIX_COEFFS_RESERVED_0     = 3,
    VIDEO_MATRIX_COEFFS_BT470M         = 4,
    VIDEO_MATRIX_COEFFS_BT601_625_0    = 5,
    VIDEO_MATRIX_COEFFS_BT601_625_1    = 6,
    VIDEO_MATRIX_COEFFS_SMPTE_240M     = 7,
    VIDEO_MATRIX_COEFFS_YCGCO          = 8,
    VIDEO_MATRIX_COEFFS_BT2020         = 9,
    VIDEO_MATRIX_COEFFS_BT2020_CONSTANT_LUMINANCE = 10,
    VIDEO_MATRIX_COEFFS_SOMPATE                   = 11,
    VIDEO_MATRIX_COEFFS_CD_NON_CONSTANT_LUMINANCE = 12,
    VIDEO_MATRIX_COEFFS_CD_CONSTANT_LUMINANCE     = 13,
    VIDEO_MATRIX_COEFFS_BTICC                     = 14,
    VIDEO_MATRIX_COEFFS_RESERVED                  = 15, //* 15~255
}VIDEO_MATRIX_COEFFS;

typedef enum VIDEO_FULL_RANGE_FLAG
{
    VIDEO_FULL_RANGE_LIMITED = 0,
    VIDEO_FULL_RANGE_FULL    = 1,
}VIDEO_FULL_RANGE_FLAG;

typedef enum VID_FRAME_TYPE
{
    VIDEO_FORMAT_TYPE_UNKONWN = 0,
    VIDEO_FORMAT_TYPE_I,
    VIDEO_FORMAT_TYPE_P,
    VIDEO_FORMAT_TYPE_B,
    VIDEO_FORMAT_TYPE_IDR,
    VIDEO_FORMAT_TYPE_BUTT,
}VID_FRAME_TYPE;

typedef struct VIDEO_FRM_MV_INFO
{
    s16              nMaxMv_x;
    s16              nMinMv_x;
    s16              nAvgMv_x;
    s16              nMaxMv_y;
    s16              nMinMv_y;
    s16              nAvgMv_y;
    s16              nMaxMv;
    s16              nMinMv;
    s16              nAvgMv;
    s16              SkipRatio;
}VIDEO_FRM_MV_INFO;

typedef struct VIDEO_FRM_STATUS_INFO
{
    VID_FRAME_TYPE   enVidFrmType;
    int              nVidFrmSize;
    int              nVidFrmDisW;
    int              nVidFrmDisH;
    int              nVidFrmQP;
    double           nAverBitRate;
    double           nFrameRate;
    int64_t          nVidFrmPTS;
    VIDEO_FRM_MV_INFO nMvInfo;
    int              bDropPreFrame;
}VIDEO_FRM_STATUS_INFO;

typedef struct VIDEOPICTURE
{
    int     nID;
    int     nStreamIndex;
    int     ePixelFormat;
    int     nWidth;
    int     nHeight;
    int     nLineStride;
    int     nTopOffset;
    int     nLeftOffset;
    int     nBottomOffset;
    int     nRightOffset;
    int     nFrameRate;
    int     nAspectRatio;
    int     bIsProgressive;
    int     bTopFieldFirst;
    int     bRepeatTopField;
    int64_t nPts;
    int64_t nPcr;
    char*   pData0;
    char*   pData1;
    char*   pData2;
    char*   pData3;
    int     bMafValid;
    char*   pMafData;
    int     nMafFlagStride;
    int     bPreFrmValid;
    int     nBufId;
    size_addr phyYBufAddr;
    size_addr phyCBufAddr;
    void*   pPrivate;
    int     nBufFd;
    int     nBufStatus;
    int     bTopFieldError;
    int        bBottomFieldError;
    int     nColorPrimary;  // default value is 0xffffffff, valid value id 0x0000xxyy
                            // xx: is video full range code
                            // yy: is matrix coefficient
    int     bFrameErrorFlag;

    //* to save hdr info and afbc header info
    void*   pMetaData;

    //*display related parameter
    VIDEO_FULL_RANGE_FLAG   video_full_range_flag;
    VIDEO_TRANSFER          transfer_characteristics;
    VIDEO_MATRIX_COEFFS     matrix_coeffs;
    u8      colour_primaries;
    //*end of display related parameter defined
    //size_addr    nLower2BitPhyAddr;
    int          nLower2BitBufSize;
    int          nLower2BitBufOffset;
    int          nLower2BitBufStride;
    int          b10BitPicFlag;
    int          bEnableAfbcFlag;

    int     nBufSize;
    int     nAfbcSize;
    int     nDebugCount;
    VIDEO_FRM_STATUS_INFO nCurFrameInfo;
}VideoPicture;

typedef struct VIDEOFBMINFO
{
    unsigned int         nValidBufNum;
    void*                pFbmFirst;
    void*                pFbmSecond;
    FbmBufInfo           pFbmBufInfo;
    unsigned int         bIs3DStream;
    unsigned int         bTwoStreamShareOneFbm;
    VideoPicture*        pMajorDispFrame;
    VideoPicture*        pMajorDecoderFrame;
    unsigned int         nMinorYBufOffset;
    unsigned int         nMinorCBufOffset;
    int                  bIsFrameCtsTestFlag;
    int                  nExtraFbmBufferNum;
    int                  nDecoderNeededMiniFbmNum;
    int                  nDecoderNeededMiniFbmNumSD;
    int                  bIsSoftDecoderFlag;
}VideoFbmInfo;

typedef struct JPEGSKIACONFIG
{
    int mode_selection;
    int filed_alpha;
    int imcu_int_minus1;
    int region_top;
    int region_bot;
    int region_left;
    int region_right;
    int nScaleDownRatio;
    void* pFrameBuffer;
    void* pInputIndexBuffer;
    int   nInputIndexSize;
    void* pTileVbvBuffer;
    int nTileVbvVBufferSize;
}JpegSkiaConfig;


//added by xyliu for set and get the decoder debug command
enum EVDECODERSETPERFORMCMD
{
    VDECODE_SETCMD_DEFAULT                  = 0,
    VDECODE_SETCMD_START_CALDROPFRAME       = 1,
    VDECODE_SETCMD_STOP_CALDROPFRAME        = 2,
};

enum EVDECODERGETPERFORMCMD
{
    VDECODE_GETCMD_DEFAULT                  = 0,
    VDECODE_GETCMD_DROPFRAME_INFO           = 1,
};

typedef struct VID_PERFORMANCE
{
    unsigned int nDropFrameNum;
    // this variable is valid for VDECODE_GETCMD_DROPFRAME_INFO command
    int nFrameDuration;
}VDecodePerformaceInfo;

typedef void* VideoDecoder;

//* NO.1
extern void AddVDPlugin(void);

//* NO.2
VideoDecoder* CreateVideoDecoder(void);

//* NO.3
void DestroyVideoDecoder(VideoDecoder* pDecoder);

//* NO.4
int InitializeVideoDecoder(VideoDecoder* pDecoder,
                     VideoStreamInfo* pVideoInfo,
                     VConfig* pVconfig);

//* NO.5
void ResetVideoDecoder(VideoDecoder* pDecoder);

//* NO.6
int ReopenVideoEngine(VideoDecoder* pDecoder,
                    VConfig* pVConfig,
                    VideoStreamInfo* pStreamInfo);

//* NO.7
int DecodeVideoStream(VideoDecoder* pDecoder,
                      int           bEndOfStream,
                      int           bDecodeKeyFrameOnly,
                      int           bDropBFrameIfDelay,
                      int64_t       nCurrentTimeUs);

//* NO.8 about sbm
int RequestVideoStreamBuffer(VideoDecoder* pDecoder,
                             int           nRequireSize,
                             char**        ppBuf,
                             int*          pBufSize,
                             char**        ppRingBuf,
                             int*          pRingBufSize,
                             int           nStreamBufIndex);
//* NO.9 about sbm
int SubmitVideoStreamData(VideoDecoder*        pDecoder,
                          VideoStreamDataInfo* pDataInfo,
                          int                  nStreamBufIndex);

//* NO.10 about fbm
VideoPicture* NextPictureInfo(VideoDecoder* pDecoder, int nStreamIndex);

//* NO.11 about fbm
VideoPicture* RequestPicture(VideoDecoder* pDecoder, int nStreamIndex);

//* NO.12 about fbm
int ReturnPicture(VideoDecoder* pDecoder, VideoPicture* pPicture);

//* NO.13
int RotatePicture(struct ScMemOpsS* memOps,
                  VideoPicture* pPictureIn,
                  VideoPicture* pPictureOut,
                  int           nRotateDegree,
                  int           nGpuYAlign,
                  int           nGpuCAlign);

//* NO.14
int RotatePictureHw(VideoDecoder* pDecoder,
                    VideoPicture* pPictureIn,
                    VideoPicture* pPictureOut,
                    int           nRotateDegree);

//* NO.15
int GetVideoStreamInfo(VideoDecoder* pDecoder,
                    VideoStreamInfo* pVideoInfo);

//* NO.16 about sbm
int VideoStreamBufferSize(VideoDecoder* pDecoder, int nStreamBufIndex);

//* NO.17 about sbm
int VideoStreamDataSize(VideoDecoder* pDecoder, int nStreamBufIndex);

//* NO.18 about sbm
int VideoStreamFrameNum(VideoDecoder* pDecoder, int nStreamBufIndex);

//* NO.19 about sbm
void* VideoStreamDataInfoPointer(VideoDecoder* pDecoder, int nStreamBufIndex);

//* NO.20 about fbm
int TotalPictureBufferNum(VideoDecoder* pDecoder, int nStreamIndex);

//* NO.21 about fbm
int EmptyPictureBufferNum(VideoDecoder* pDecoder, int nStreamIndex);

//* NO.22 about fbm
int ValidPictureNum(VideoDecoder* pDecoder, int nStreamIndex);

//* NO.23 about malloc buffer
VideoPicture* AllocatePictureBuffer(struct ScMemOpsS* memOps,
                              int nWidth,
                              int nHeight,
                              int nLineStride,
                              int ePixelFormat);

//* NO.24 about malloc buffer
int FreePictureBuffer(struct ScMemOpsS* memOps,
                  VideoPicture* pPicture);

//* NO.25 about malloc buffer
void *VideoDecoderPallocIonBuf(VideoDecoder* pDecoder, int nSize);

//* NO.26 about malloc buffer
void VideoDecoderFreeIonBuf(VideoDecoder* pDecoder, void *pIonBuf);


//* NO.27 about new display
FbmBufInfo* GetVideoFbmBufInfo(VideoDecoder* pDecoder);

//* NO.28 about new display
VideoPicture* SetVideoFbmBufAddress(VideoDecoder* pDecoder,
                                 VideoPicture* pVideoPicture,
                                 int bForbidUseFlag);

//* NO.29 about new display
int SetVideoFbmBufRelease(VideoDecoder* pDecoder);


//* NO.30 about new display
VideoPicture* RequestReleasePicture(VideoDecoder* pDecoder);


//* NO.31 about new display
VideoPicture*  ReturnRelasePicture(VideoDecoder* pDecoder,
                              VideoPicture* pVpicture,
                              int bForbidUseFlag);

//* NO.32
int ConfigExtraScaleInfo(VideoDecoder* pDecoder,
                     int nWidthTh,
                     int nHeightTh,
                     int nHorizonScaleRatio,
                     int nVerticalScaleRatio);

//* NO.33
int DecoderSetSpecialData(VideoDecoder* pDecoder, void *pArg);

//* NO.34
int SetDecodePerformCmd(VideoDecoder* pDecoder,
                      enum EVDECODERSETPERFORMCMD performCmd);

//* NO.35
int GetDecodePerformInfo(VideoDecoder* pDecoder,
                      enum EVDECODERGETPERFORMCMD performCmd,
                      VDecodePerformaceInfo** performInfo);

int VideoDecoderSetFreq(VideoDecoder* pDecoder, int nVeFreq);
#ifdef __cplusplus
}
#endif

#endif

