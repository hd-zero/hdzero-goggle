/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mm_comm_video.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/03/15
  Last Modified :
  Description   : multimedia common video structure and definition.
  Function List :
  History       :
******************************************************************************/

#ifndef __IPCLINUX_MM_COMM_VIDEO_H__
#define __IPCLINUX_MM_COMM_VIDEO_H__

#include <stdint.h>
#include <plat_type.h>
#include <plat_defines.h>
//#include "mm_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum PIC_SIZE_E
{
    PIC_QCIF = 0,
    PIC_CIF,
    PIC_2CIF,
    PIC_HD1,
    PIC_D1,
    PIC_960H,

    PIC_QVGA,    /* 320 * 240 */
    PIC_VGA,     /* 640 * 480 */
    PIC_XGA,     /* 1024 * 768 */
    PIC_SXGA,    /* 1400 * 1050 */
    PIC_UXGA,    /* 1600 * 1200 */
    PIC_QXGA,    /* 2048 * 1536 */

    PIC_WVGA,    /* 854 * 480 */
    PIC_WSXGA,   /* 1680 * 1050 */
    PIC_WUXGA,   /* 1920 * 1200 */
    PIC_WQXGA,   /* 2560 * 1600 */

    PIC_HD720,   /* 1280 * 720 */
    PIC_HD1080,  /* 1920 * 1080 */
    PIC_2304x1296, /* 3M:2304 * 1296 */
    PIC_2592x1520, /* 4M:2592 * 1520 */
    PIC_5M,        /* 2592 * 1944 */
    PIC_UHD4K,     /* 3840 * 2160 */

    PIC_BUTT
} PIC_SIZE_E;

typedef enum VIDEO_NORM_E
{
    VIDEO_ENCODING_MODE_PAL = 0,
    VIDEO_ENCODING_MODE_NTSC,
    VIDEO_ENCODING_MODE_AUTO,
    VIDEO_ENCODING_MODE_BUTT
} VIDEO_NORM_E;

typedef enum VIDEO_CONTROL_MODE_E
{
    VIDEO_CONTROL_MODE_SLAVER = 0,
    VIDEO_CONTROL_MODE_MASTER,
    VIDEO_CONTROL_MODE_BUTT
} VIDEO_CONTROL_MODE_E;

/* we ONLY define picture format used, all unused will be deleted!*/
typedef enum PIXEL_FORMAT_E
{
    MM_PIXEL_FORMAT_RGB_1BPP = 0,
    MM_PIXEL_FORMAT_RGB_2BPP,
    MM_PIXEL_FORMAT_RGB_4BPP,
    MM_PIXEL_FORMAT_RGB_8BPP,
    MM_PIXEL_FORMAT_RGB_444,

    MM_PIXEL_FORMAT_RGB_4444,
    MM_PIXEL_FORMAT_RGB_555,
    MM_PIXEL_FORMAT_RGB_565,
    MM_PIXEL_FORMAT_RGB_1555,

    /*  9 reserved */
    MM_PIXEL_FORMAT_RGB_888,
    MM_PIXEL_FORMAT_RGB_8888,

    MM_PIXEL_FORMAT_RGB_PLANAR_888,
    MM_PIXEL_FORMAT_RGB_BAYER_8BPP,
    MM_PIXEL_FORMAT_RGB_BAYER_10BPP,
    MM_PIXEL_FORMAT_RGB_BAYER_12BPP,
    MM_PIXEL_FORMAT_RGB_BAYER_14BPP,

    MM_PIXEL_FORMAT_RGB_BAYER,         /* 16 bpp */

    MM_PIXEL_FORMAT_YUV_A422,
    MM_PIXEL_FORMAT_YUV_A444,

    MM_PIXEL_FORMAT_YUV_PLANAR_422,
    MM_PIXEL_FORMAT_YUV_PLANAR_420,    //YU12

    MM_PIXEL_FORMAT_YUV_PLANAR_444,

    MM_PIXEL_FORMAT_YUV_SEMIPLANAR_422,    //NV16
    MM_PIXEL_FORMAT_YUV_SEMIPLANAR_420,    //NV12
    MM_PIXEL_FORMAT_YUV_SEMIPLANAR_444,

    MM_PIXEL_FORMAT_UYVY_PACKAGE_422,
    MM_PIXEL_FORMAT_YUYV_PACKAGE_422,
    MM_PIXEL_FORMAT_VYUY_PACKAGE_422,
    MM_PIXEL_FORMAT_YCbCr_PLANAR,

    MM_PIXEL_FORMAT_SINGLE,

    MM_PIXEL_FORMAT_YVU_PLANAR_420,    //YV12
    MM_PIXEL_FORMAT_YVU_SEMIPLANAR_422,    //NV61
    MM_PIXEL_FORMAT_YVU_SEMIPLANAR_420,    //NV21

    MM_PIXEL_FORMAT_YUV_AW_AFBC,
    MM_PIXEL_FORMAT_YUV_AW_LBC_2_0X,
    MM_PIXEL_FORMAT_YUV_AW_LBC_2_5X,
    MM_PIXEL_FORMAT_YUV_AW_LBC_1_0X,

    MM_PIXEL_FORMAT_RAW_SBGGR8,
    MM_PIXEL_FORMAT_RAW_SGBRG8,
    MM_PIXEL_FORMAT_RAW_SGRBG8,
    MM_PIXEL_FORMAT_RAW_SRGGB8,
    MM_PIXEL_FORMAT_RAW_SBGGR10,
    MM_PIXEL_FORMAT_RAW_SGBRG10,
    MM_PIXEL_FORMAT_RAW_SGRBG10,
    MM_PIXEL_FORMAT_RAW_SRGGB10,
    MM_PIXEL_FORMAT_RAW_SBGGR12,
    MM_PIXEL_FORMAT_RAW_SGBRG12,
    MM_PIXEL_FORMAT_RAW_SGRBG12,
    MM_PIXEL_FORMAT_RAW_SRGGB12,

    MM_PIXEL_FORMAT_BUTT
} PIXEL_FORMAT_E;

typedef struct VIDEO_VBI_INFO_S
{
    unsigned int mData[VIU_MAX_VBI_LEN];
    unsigned int mLen;
} VIDEO_VBI_INFO_S;

typedef enum VIDEO_FIELD_E
{
    VIDEO_FIELD_TOP         = 0x1,    /* even field */
    VIDEO_FIELD_BOTTOM      = 0x2,    /* odd field */
    VIDEO_FIELD_INTERLACED  = 0x3,    /* two interlaced fields */
    VIDEO_FIELD_FRAME       = 0x4,    /* frame */

    VIDEO_FIELD_BUTT
} VIDEO_FIELD_E;

typedef enum VIDEO_FORMAT_E
{
    VIDEO_FORMAT_LINEAR		= 0x0,		/* nature video line */
    VIDEO_FORMAT_TILE		= 0x1,		/* tile cell: 256pixel x 16line, default tile mode */
    VIDEO_FORMAT_TILE64		= 0x2,		/* tile cell: 64pixel x 16line */

    VIDEO_FORMAT_BUTT
} VIDEO_FORMAT_E;

typedef enum COMPRESS_MODE_E
{
    COMPRESS_MODE_NONE		= 0x0,		/* no compress */
    COMPRESS_MODE_SEG		= 0x1,		/* compress unit is 256 bytes as a segment, default seg mode */
    COMPRESS_MODE_SEG128	= 0x2,		/* compress unit is 128 bytes as a segment */
    COMPRESS_MODE_LINE		= 0x3,		/* compress unit is the whole line */
    COMPRESS_MODE_FRAME		= 0x4,		/* compress unit is the whole frame */

    COMPRESS_MODE_BUTT
} COMPRESS_MODE_E;

typedef enum VIDEO_DISPLAY_MODE_E
{
    VIDEO_DISPLAY_MODE_PREVIEW		= 0x0,
    VIDEO_DISPLAY_MODE_PLAYBACK		= 0x1,

    VIDEO_DISPLAY_MODE_BUTT
} VIDEO_DISPLAY_MODE_E;


#define DCF_DRSCRIPTION_LENGTH      32
#define DCF_CAPTURE_TIME_LENGTH     20

typedef struct ISP_DCF_INFO_S
{
    unsigned char       mImageDescription[DCF_DRSCRIPTION_LENGTH];        /*Describes image*/
    unsigned char       mMake[DCF_DRSCRIPTION_LENGTH];                    /*Shows manufacturer of digital cameras*/
    unsigned char       mModel[DCF_DRSCRIPTION_LENGTH];                   /*Shows model number of digital cameras*/
    unsigned char       mSoftware[DCF_DRSCRIPTION_LENGTH];                /*Shows firmware (internal software of digital cameras) version number*/

    unsigned short      mISOSpeedRatings;                                 /*CCD sensitivity equivalent to Ag-Hr film speedrate*/
    unsigned int      mFNumber;                                         /*The actual F-number (F-stop) of lens when the image was taken*/
    unsigned int      mMaxApertureValue;                                /*Maximum aperture value of lens.*/
    unsigned int      mExposureTime;                                    /*Exposure time (reciprocal of shutter speed).*/
    unsigned int      mExposureBiasValue;                               /*Exposure bias (compensation) value of taking picture*/
    unsigned char       mExposureProgram;                                  /*Exposure program that the camera used when image was taken. '1' means manual control, '2'
                                                                              program normal, '3' aperture priority, '4' shutter priority, '5' program creative (slow program),
                                                                              '6' program action(high-speed program), '7' portrait mode, '8' landscape mode*/
    unsigned char       mMeteringMode;                                     /*Exposure metering method. '0' means unknown, '1' average, '2' center weighted average, '3'
                                                                              spot, '4' multi-spot, '5' multi-segment, '6' partial, '255' other*/
    unsigned char       mLightSource;                                      /*Light source, actually this means white balance setting. '0' means unknown, '1' daylight, '2'
                                                                               fluorescent, '3' tungsten, '10' flash, '17' standard light A, '18' standard light B, '19' standard light
                                                                               C, '20' D55, '21' D65, '22' D75, '255' other*/
    unsigned int      mFocalLength;                                     /*Focal length of lens used to take image. Unit is millimeter*/
    unsigned char       mSceneType;                                        /*Indicates the type of scene. Value '0x01' means that the image was directly photographed.*/
    unsigned char       mCustomRendered;                                   /*Indicates the use of special processing on image data, such as rendering geared to output.
                                                                               0 = Normal process  1 = Custom process   */
    unsigned char       mExposureMode;                                     /*Indicates the exposure mode set when the image was shot.
                                                                              0 = Auto exposure,1 = Manual exposure, 2 = Auto bracket*/
    unsigned char       mWhiteBalance;                                     /* Indicates the white balance mode set when the image was shot.
                                                                                0 = Auto white balance ,1 = Manual white balance */
    unsigned char       mFocalLengthIn35mmFilm;                            /*Indicates the equivalent focal length assuming a 35mm film camera, in mm*/
    unsigned char       mSceneCaptureType;                                 /*Indicates the type of scene that was shot. 0 = Standard,1 = Landscape,2 = Portrait,3 = Night scene. */
    unsigned char       mGainControl;                                      /*Indicates the degree of overall image gain adjustment. 0 = None,1 = Low gain up,2 = High gain up,3 = Low gain down,4 = High gain down. */
    unsigned char       mContrast;                                         /*Indicates the direction of contrast processing applied by the camera when the image was shot.
                                                                               0 = Normal,1 = Soft,2 = Hard */
    unsigned char       mSaturation;                                       /*Indicates the direction of saturation processing applied by the camera when the image was shot.
                                                                              0 = Normal,1 = Low saturation,2 = High saturation*/
    unsigned char       mSharpness;                                        /*Indicates the direction of sharpness processing applied by the camera when the image was shot.
                                                                              0 = Normal,1 = Soft,2 = Hard .*/
} ISP_DCF_INFO_S;

typedef struct JPEG_DCF_S
{
    unsigned char           mCaptureTime[DCF_CAPTURE_TIME_LENGTH];            /*The date and time when the picture data was generated*/
    unsigned char           mbFlash;                                             /*whether the picture is captured when a flash lamp is on*/
    unsigned int          mDigitalZoomRatio;                                /*Indicates the digital zoom ratio when the image was shot.
                                                                                   If the numerator of the recorded value is 0, this indicates that digital zoom was not used.*/
    ISP_DCF_INFO_S  mIspDCFInfo;

} JPEG_DCF_S;

typedef enum FRAME_FLASH_TYPE_E
{
    FRAME_FLASH_OFF  = 0,
    FRAME_FLASH_ON   = 1,
    FRAME_FLASH_BUTT,
} FRAME_FLASH_TYPE_E;

typedef struct VIDEO_SUPPLEMENT_S
{
    FRAME_FLASH_TYPE_E mFlashType;
    unsigned int   mJpegDcfPhyAddr;
    void* mpJpegDcfVirAddr;

} VIDEO_SUPPLEMENT_S;

/* you can add your own specific frame information type here */
typedef enum FRAME_FLAG_E {
    FF_INVALID = -1,
    FF_NONE,
    /* larger than 1S interval is regarded as long-exposure frame
    * <mFrmFlag = exp_time|FF_LONGEXP> ===> exp_time(unit:ms)
    */
    FF_LONGEXP,
    FF_VIDEO_EIS,
    FF_VIDEO_ISE,
    FF_VIDEO_WDR,
    /* lost frames, lost_num = lost frames between cur-frame and last frame
    * <mFrmFlag = lost_num|FF_FRAME_LOST>
    */
    FF_FRAME_LOST,
} FRAME_FLAG_E;

typedef struct VIDEO_FRAME_S
{
    unsigned int    mWidth;
    unsigned int    mHeight;
    VIDEO_FIELD_E   mField;
    PIXEL_FORMAT_E  mPixelFormat;

    VIDEO_FORMAT_E  mVideoFormat;
    COMPRESS_MODE_E mCompressMode;

    unsigned int    mPhyAddr[3];/* Y, U, V; Y, UV; Y, VU */
    void*           mpVirAddr[3];
    unsigned int    mStride[3];

    unsigned int    mHeaderPhyAddr[3];
    void*           mpHeaderVirAddr[3];
    unsigned int    mHeaderStride[3];

    short           mOffsetTop;		/* top offset of show area */
    short           mOffsetBottom;	/* bottom offset of show area */
    short           mOffsetLeft;	/* left offset of show area */
    short           mOffsetRight;	/* right offset of show area */

    uint64_t        mpts;          /* unit:us */
    unsigned int    mExposureTime; /* every frame exp time */
    unsigned int    mFramecnt;     /* rename mPrivateData to Framecnt_exp_start */
    int mEnvLV;                    /* environment luminance value */

    /* nobody use it until now, so just comment out it,
    * if somebody want to use it, then you can add it back.
    */
//    VIDEO_SUPPLEMENT_S mSupplement;

    /* for frame specific informations.
    *e.g. this is a Long-Exposure frame, you can set mFrmFlag = (exp_time)<<16 | FF_LONGEXP.
    *e.g. somtimes, frame lost in kernel because of return time delay, then you can set
    * mFrmFlag = (lost_num)<<16 | FF_FRAME_LOST; and maybe Venc can insert empty frames.
    */
    unsigned int    mWhoSetFlag;   /* reserve(8bit)|COMP_TYPE(8bit)|DEV_NUM(8bit)|CHN_NUM(8bit) */
    uint64_t        mFlagPts;      /* when generate this flag, unit(us) */
    /* whats this flag, data(16bit)|flag(16bit), if you want a signed data, please use short data type */
    unsigned int    mFrmFlag;
} VIDEO_FRAME_S;

typedef struct VIDEO_FRAME_INFO_S
{
    VIDEO_FRAME_S VFrame;
    //unsigned int mPoolId;
    unsigned int mId;   //id identify frame uniquely
} VIDEO_FRAME_INFO_S;

typedef struct BITMAP_S
{
    PIXEL_FORMAT_E mPixelFormat;  /* Bitmap's pixel format */
    unsigned int mWidth;               /* Bitmap's width */
    unsigned int mHeight;              /* Bitmap's height */
    void* mpData;                /* Address of Bitmap's data */
} BITMAP_S;

/* VI Mix-Capture info. */
typedef struct VI_MIXCAP_STAT_S
{
    BOOL mbMixCapMode;    /* In mix-capture mode or not. */
    BOOL mbHasDownScale;  /* VI Frame is downscaled or not. */
} VI_MIXCAP_STAT_S;

/* VI output frame info. */
typedef struct VI_FRAME_INFO_S
{
    VI_MIXCAP_STAT_S mMixCapState; /* VI Mix-Capture info. */
    VIDEO_FRAME_INFO_S mViFrmInfo; /* Video frame info. */
} VI_FRAME_INFO_S;

typedef enum LDC_VIEW_TYPE_E
{
    LDC_VIEW_TYPE_ALL = 0,  /* View scale all but x and y independtly, this will keep both x and y axis ,but corner maybe lost*/
    LDC_VIEW_TYPE_CROP,     /* Not use view scale, this will lost some side and corner */
    //LDC_VIEW_TYPE_FULL,

    LDC_VIEW_TYPE_BUTT,
} LDC_VIEW_TYPE_E;

typedef struct LDC_ATTR_S
{
    LDC_VIEW_TYPE_E mViewType;
    int mCenterXOffset;        /* Horizontal offset of the image distortion center relative to image center. [-28,28]. */
    int mCenterYOffset;        /* Vertical offset of the image distortion center relative to image center. [-14,14]. */
    int mRatio;                /* Distortion ratio. [0, 511]. */
} LDC_ATTR_S;

typedef enum WDR_MODE_E
{
    WDR_MODE_NONE = 0,
    WDR_MODE_BUILT_IN,

    WDR_MODE_2To1_LINE,
    WDR_MODE_2To1_FRAME,
    WDR_MODE_2To1_FRAME_FULL_RATE,

    WDR_MODE_3To1_LINE,
    WDR_MODE_3To1_FRAME,
    WDR_MODE_3To1_FRAME_FULL_RATE,

    WDR_MODE_4To1_LINE,
    WDR_MODE_4To1_FRAME,
    WDR_MODE_4To1_FRAME_FULL_RATE,

    WDR_MODE_BUTT,
} WDR_MODE_E;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __IPCLINUX_MM_COMM_VIDEO_H__ */

