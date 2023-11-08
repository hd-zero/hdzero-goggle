#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <plat_type.h>
#include <tsemaphore.h>

#include "mm_comm_vi.h"
#include "mpi_vi.h"
#include "mpi_isp.h"
#include "mpi_venc.h"
#include "mpi_sys.h"
#include "mm_common.h"
#include "mm_comm_venc.h"
#include "mm_comm_rc.h"

#define VENC_spsppsPATCH    1
#define VENC_spsppsLEN      256

typedef enum
{
    VENC_rcCBR = 0,
    VENC_rcVBR = 1,
    VENC_rcFIXQP = 2,
    VENC_rcABR = 3,
    VENC_rcQPMAP = 4,
} VencRateControlMode_e;
#define VE_rcModeNAME { \
    "CBR", \
    "VBR", \
    "FixedQP", \
    "ABR", \
    "QPMap", \
}

typedef enum
{
    VENC_profileBASELINE = 0,
    VENC_profileMP = 1,
    VENC_profileHP = 2,
    VENC_profileSVC = 3,
} VencProfile_e;
#define VE_h264profileNAME { \
    "baseline", \
    "main", \
    "high", \
    "svc" \
}
#define VE_h265profileNAME { \
    "main", \
    "main 10", \
    "still", \
    "unknown" \
}

typedef struct
{
    unsigned int      mBitRate;                             /* average bitrate */
    unsigned int      mMaxQp;                               /* the max qp */
    unsigned int      mMinQp;                               /* the min qp */
} VencAttrH264Cbr_t;

typedef struct
{
    unsigned int      mMaxBitRate;                          /* the max bitrate */
    unsigned int      mMaxQp;                               /* the max qp */
    unsigned int      mMinQp;                               /* the min qp */
    unsigned int      mRatioChangeQp;                       //range[50,100], default:85
    int               mQuality;                             /* range[1,13], 1:worst quality, 13:bestquality */
} VencAttrH264Vbr_t;

typedef struct
{
    unsigned int      mIQp;                                 /* qp of the i frame */
    unsigned int      mPQp;                                 /* qp of the p frame */
} VencAttrH264FixQp_t;

typedef struct
{
    unsigned int      mMaxBitRate;                          /* the max bitrate */
    unsigned int      mRatioChangeQp;                       /* range[50,100], default:85 */
    int               mQuality;                             //range[1,13], 1:worst quality, 13:best quality, recommend:8.
    unsigned int      mMinIQp;                              /* I frame qp lower limit */
    unsigned int      mMaxIQp;      //no support upper limit, I frame qp only use lower_limit(mMinIQp)
    unsigned int      mMaxQp;                               /* the max qp */
    unsigned int      mMinQp;                               /* the min qp */
} VencAttrH264Abr_t;

typedef VencAttrH264Cbr_t   VencAttrH265Cbr_t;
typedef VencAttrH264Vbr_t   VencAttrH265Vbr_t;
typedef VencAttrH264FixQp_t VencAttrH265FixQp_t;
typedef VencAttrH264Abr_t   VencAttrH265Abr_t;

typedef struct
{
    unsigned int      mBitRate;                             /* average bitrate */
} VencAttrMjpegCbr_t;

typedef struct
{
    unsigned int      mQfactor;                             /* image quality :[1,99]*/
} VencAttrMjpegFixQp_t;

typedef struct
{
    PAYLOAD_TYPE_E           mVEncType;
    VencRateControlMode_e    mRcMode;                       /*the type of rc*/
    union
    {
        VencAttrH264Cbr_t    mAttrH264Cbr;
        VencAttrH264Vbr_t    mAttrH264Vbr;
        VencAttrH264FixQp_t  mAttrH264FixQp;
        VencAttrH264Abr_t    mAttrH264Abr;
        VencAttrMjpegCbr_t   mAttrMjpegCbr;
        VencAttrMjpegFixQp_t mAttrMjpegFixQp;
        VencAttrH265Cbr_t    mAttrH265Cbr;
        VencAttrH265Vbr_t    mAttrH265Vbr;
        VencAttrH265FixQp_t  mAttrH265FixQp;
        VencAttrH265Abr_t    mAttrH265Abr;
    };
} VencRateControlAttr_t;

typedef struct
{
    unsigned int mProfile;          /*0: baseline; 1:MP; 2:HP; 3: SVC-T [0,3]; */
    H264_LEVEL_E mLevel;            //VENC_H264LEVELTYPE
} VencAttrH264_t;

typedef struct
{
    unsigned int mProfile;          /*0: MP; 1: main10; 2: still */
    H265_LEVEL_E mLevel;            //VENC_H264LEVELTYPE
} VencAttrH265_t;

typedef struct
{
    PAYLOAD_TYPE_E  mType;          /*the type of payload*/
    unsigned int mBufSize;          /*stream buffer size, 0: use default */
    unsigned int mThreshSize;       /* threshSize in stream buffer, must < stream buffer size, 0 means auto config*/
    union
    {
        VencAttrH264_t mAttrH264;
        VencAttrH265_t mAttrH265;
    };
} VencAttr_t;

typedef struct
{
    int devNum;
    int width;
    int height;
    int fps;
} ViParams_t;

typedef struct
{
    PAYLOAD_TYPE_E codecType;

    //rc_mode for H264/H265 0:CBR  1:VBR  2:FIXQP  3:QPMAP
    VencRateControlMode_e rcMode;

    int timeLapseEnable;
    int timeBetweenFrameCapture;
    int width;
    int height;
    int fps;
    int bps;
    int fastEnc;
    PIXEL_FORMAT_E picFormat; //MM_PIXEL_FORMAT_YUV_PLANAR_420

    uint32_t intraRefreshBlockNum;
    int  level3DNR;
    bool enableSmartP;
    bool enableAdvanceRef;
    bool enableHorizonfilp;
    bool enableAdaptiveintrainp;
    bool enableColor2Grey;
    bool enableNullSkip;
    bool enablePSkip;
    int  maxKeyItl;

    VencAttr_t  veAttr;
    VencRateControlAttr_t rcAttr;
} VencParams_t;

typedef enum {
    FRAME_typeI = 1,
    FRAME_typeP,
    FRAME_typeB,
    FRAME_typeO,    //others
} VencFrameType_e;

typedef void* (*vi2venc_threadProc)(void *arg);
typedef int (*CB_onFrame)(void* vvCtxPtr, uint8_t* frameData, int frameLen, VencFrameType_e frameType, uint64_t pts, void* context);

typedef struct Vi2Venc
{
    VI_DEV      viDev;
    VI_CHN      viChn;
    ISP_DEV     ispDev;
    ViParams_t  viParams;

    pthread_t   threadId;
    VENC_CHN    veChn;
    bool        bExit;
    VencParams_t veParams;
    uint8_t*    spsppsBuff;

    CB_onFrame  cbOnFrame;
    void*       contextOfOnFrame;

    int64_t     startTimestamp;
} Vi2Venc_t;

typedef VencHeaderData  VencSpspps_t;

Vi2Venc_t* vi2venc_initSys(CB_onFrame cbOnFrame, void* context);
void vi2venc_deinitSys(Vi2Venc_t* vv);

ERRORTYPE vi2venc_prepare(Vi2Venc_t* vv, ViParams_t* viParams, VencParams_t* veParams, VencParams_t* jpgParams);
ERRORTYPE vi2venc_start(Vi2Venc_t* vv, vi2venc_threadProc proc);
ERRORTYPE vi2venc_stop(Vi2Venc_t* vv, bool all);
ERRORTYPE vi2venc_getSpsPpsInfo(Vi2Venc_t* vv, VencSpspps_t* spsppsInfo, bool patch);
ERRORTYPE vi2venc_requestIFrame(Vi2Venc_t* vv);
char*     vi2venc_getRcModeName(VencRateControlMode_e rcMode);
char*     vi2venc_getProfileName(VencProfile_e profile, bool h265);

#ifdef __cplusplus
}
#endif
