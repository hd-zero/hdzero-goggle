/*
* Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : vencoder.h
* Description :
* History :
*   Author  : fangning <fangning@allwinnertech.com>
*   Date    : 2016/04/13
*   Comment :
*
*
*/

/*
 *this software is based in part on the work
 * of the Independent JPEG Group
 */
#include "sc_interface.h"
#include "veInterface.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef _VENCODER_H_
#define _VENCODER_H_

#define  DATA_TIME_LENGTH           24
#define  INFO_LENGTH                64
#define  GPS_PROCESS_METHOD_LENGTH  100
#define  DESCRIPTOR_INFO            128
#define  PROC_BUF_LEN               1024
#define  MAX_CHANNEL_NUM            16

#define MAX_FRM_NUM                       5
#define MAX_GOP_SIZE                      63
#define MAX_OVERLAY_SIZE                  64


#define VENC_BUFFERFLAG_KEYFRAME 0x00000001
#define VENC_BUFFERFLAG_EOS 0x00000002
#define VENC_BUFFERFLAG_THUMB 0x00000004

#define H264_VERSION2_USE64 0

typedef enum {
    PSKIP               = 0,
    BSKIP_DIRECT        = 0,
    P16x16              = 1,
    P16x8               = 2,
    P8x16               = 3,
    SMB8x8              = 4,
    SMB8x4              = 5,
    SMB4x8              = 6,
    SMB4x4              = 7,
    P8x8                = 8,
    I4MB                = 9,
    I16MB               = 10,
    IBLOCK              = 11,
    SI4MB               = 12,
    I8MB                = 13,
    IPCM                = 14,
    MAXMODE             = 15
}MB_TYPE;

typedef struct {
    short         mv_x;
    short         mv_y;
    int           mode;
    unsigned char lt_flag;
    unsigned short depth;
} VencMotionVector;

typedef struct {
    MB_TYPE mb_type;
    VencMotionVector mb_mv;
}MbMvList;

typedef struct rational_t {
    unsigned int num;
    unsigned int den;
}rational_t;

typedef struct srational_t {
    int num;
    int den;
}srational_t;

typedef enum ExifMeteringModeType {
    UNKNOWN_AW_EXIF,
    AVERAGE_AW_EXIF,
    CENTER_AW_EXIF,
    SPOT_AW_EXIF,
    MULTISPOT_AW_EXIF,
    PATTERN_AW_EXIF,
    PARTIAL_AW_EXIF,
    OTHER_AW_EXIF     = 255,
} ExifMeteringModeType;

typedef enum ExifExposureModeType {
    EXPOSURE_AUTO_AW_EXIF,
    EXPOSURE_MANUAL_AW_EXIF,
    EXPOSURE_AUTO_BRACKET_AW_EXIF,
}ExifExposureModeType;

typedef enum {
    UNKNOWN = 0,
    SUNLIGHT = 1,
    TUNGSTEN_LAMP = 2,
    FILAMENT_LAMP = 3,
    FLASH_LAMP = 4,
    OVERCAST = 9,
    CLOUDY = 10,
    SHADOW = 11,
    INCANDESCENT_LAMP = 12,
    WHITE_DAY_FLUORESCENT_LAMP = 13,
    COOL_COLOUR_FLUORESCENT_LAMP = 14,
    WHITE_FLUORESCENT_LAMP = 15,
    STANDARD_LAMP_A = 17,
    STANDARD_LAMP_B = 18,
    STANDARD_lAMP_C = 19,
    D55 = 20,
    D65 = 21,
    D75 = 22,
    D50= 23,
    PROJECTION_ROOM_LAMP = 24,
    OTHERS = 255
}ExifLightSource;

typedef struct EXIFInfo {
    unsigned char  CameraMake[INFO_LENGTH];
    unsigned char  CameraModel[INFO_LENGTH];
    unsigned char  DateTime[DATA_TIME_LENGTH];

    unsigned int   ThumbWidth;
    unsigned int   ThumbHeight;
    unsigned char* ThumbAddrVir;
    unsigned int   ThumbLen;

    int              Orientation;  //value can be 0,90,180,270 degree
    rational_t       ExposureTime; //tag 0x829A
    rational_t       FNumber; //tag 0x829D
    short           ISOSpeed;//tag 0x8827

    srational_t    ShutterSpeedValue; //tag 0x9201
    rational_t       Aperture; //tag 0x9202
    //srational_t    BrightnessValue;   //tag 0x9203
    srational_t    ExposureBiasValue; //tag 0x9204

    rational_t       MaxAperture; //tag 0x9205

    short           MeteringMode; //tag 0x9207
    short           LightSource; //tag 0x9208
    short           FlashUsed;     //tag 0x9209
    rational_t       FocalLength; //tag 0x920A

    rational_t       DigitalZoomRatio; // tag 0xA404
    short           WhiteBalance; //tag 0xA403
    short           ExposureMode; //tag 0xA402

    // gps info
    int            enableGpsInfo;
    double         gps_latitude;
    double           gps_longitude;
    double         gps_altitude;
    long           gps_timestamp;
    unsigned char  gpsProcessingMethod[GPS_PROCESS_METHOD_LENGTH];

    unsigned char  CameraSerialNum[128];     //tag 0xA431 (exif 2.3 version)
    short              FocalLengthIn35mmFilm;     // tag 0xA405

    unsigned char  ImageName[128];             //tag 0x010D
    unsigned char  ImageDescription[128];     //tag 0x010E
    short            ImageWidth;                 //tag 0xA002
    short            ImageHeight;             //tag 0xA003

    int             thumb_quality;
}EXIFInfo;

typedef struct VencRect {
    int nLeft;
    int nTop;
    int nWidth;
    int nHeight;
}VencRect;

typedef enum VENC_YUV2YUV {
    VENC_YCCToBT601,
    VENC_BT601ToYCC,
}VENC_YUV2YUV;

typedef enum VENC_CODING_MODE {
  VENC_FRAME_CODING         = 0,
  VENC_FIELD_CODING         = 1,
}VENC_CODING_MODE;

//* The Amount of Temporal SVC Layers
typedef enum {
    NO_T_SVC = 0,
    T_LAYER_2 = 2,
    T_LAYER_3 = 3,
    T_LAYER_4 = 4
}T_LAYER;

//* The Multiple of Skip_Frame
typedef enum {
    NO_SKIP = 0,
    SKIP_2 = 2,
    SKIP_4 = 4,
    SKIP_8 = 8
}SKIP_FRAME;

typedef enum VENC_CODEC_TYPE {
    VENC_CODEC_H264,
    VENC_CODEC_JPEG,
    VENC_CODEC_H264_VER2,
    VENC_CODEC_H265,
    VENC_CODEC_VP8,
}VENC_CODEC_TYPE;

typedef enum VENC_PIXEL_FMT {
    VENC_PIXEL_YUV420SP,
    VENC_PIXEL_YVU420SP,
    VENC_PIXEL_YUV420P,
    VENC_PIXEL_YVU420P,
    VENC_PIXEL_YUV422SP,
    VENC_PIXEL_YVU422SP,
    VENC_PIXEL_YUV422P,
    VENC_PIXEL_YVU422P,
    VENC_PIXEL_YUYV422,
    VENC_PIXEL_UYVY422,
    VENC_PIXEL_YVYU422,
    VENC_PIXEL_VYUY422,
    VENC_PIXEL_ARGB,
    VENC_PIXEL_RGBA,
    VENC_PIXEL_ABGR,
    VENC_PIXEL_BGRA,
    VENC_PIXEL_TILE_32X32,
    VENC_PIXEL_TILE_128X32,
    VENC_PIXEL_AFBC_AW,
    VENC_PIXEL_LBC_AW, //* for v5v200 and newer ic
}VENC_PIXEL_FMT;

typedef enum E_ISP_SCALER_RATIO {
    VENC_ISP_SCALER_0       = 0, //no write back
    VENC_ISP_SCALER_EIGHTH  = 1, //scaler 1/8 write back
    VENC_ISP_SCALER_HALF    = 2, //scaler 1/2 write back
    VENC_ISP_SCALER_QUARTER = 3, //scaler 1/4 write back
}E_ISP_SCALER_RATIO;

typedef struct VencThumbInfo {
    unsigned int        nThumbSize;
    unsigned char*      pThumbBuf;
}VencThumbInfo;

typedef struct VencBaseConfig {
    unsigned char       bEncH264Nalu;
    unsigned int        nInputWidth;
    unsigned int        nInputHeight;
    unsigned int        nDstWidth;
    unsigned int        nDstHeight;
    unsigned int        nStride;
    VENC_PIXEL_FMT      eInputFormat;
    struct ScMemOpsS *memops;
    VeOpsS*           veOpsS;
    void*             pVeOpsSelf;

    unsigned char     bOnlyWbFlag;

    //* for v5v200 and newer ic
    unsigned char     bLbcLossyComEnFlag2x;
    unsigned char     bLbcLossyComEnFlag2_5x;
    unsigned char     bIsVbvNoCache;
	//* end
}VencBaseConfig;

/**
 * H264 profile types
 */
typedef enum VENC_H264PROFILETYPE {
    VENC_H264ProfileBaseline  = 66,         /**< Baseline profile */
    VENC_H264ProfileMain      = 77,         /**< Main profile */
    VENC_H264ProfileHigh      = 100,           /**< High profile */
}VENC_H264PROFILETYPE;

/**
 * H264 level types
 */
typedef enum VENC_H264LEVELTYPE {
    VENC_H264Level1   = 10,     /**< Level 1 */
    VENC_H264Level11  = 11,     /**< Level 1.1 */
    VENC_H264Level12  = 12,     /**< Level 1.2 */
    VENC_H264Level13  = 13,     /**< Level 1.3 */
    VENC_H264Level2   = 20,     /**< Level 2 */
    VENC_H264Level21  = 21,     /**< Level 2.1 */
    VENC_H264Level22  = 22,     /**< Level 2.2 */
    VENC_H264Level3   = 30,     /**< Level 3 */
    VENC_H264Level31  = 31,     /**< Level 3.1 */
    VENC_H264Level32  = 32,     /**< Level 3.2 */
    VENC_H264Level4   = 40,     /**< Level 4 */
    VENC_H264Level41  = 41,     /**< Level 4.1 */
    VENC_H264Level42  = 42,     /**< Level 4.2 */
    VENC_H264Level5   = 50,     /**< Level 5 */
    VENC_H264Level51  = 51,     /**< Level 5.1 */
}VENC_H264LEVELTYPE;

typedef struct VencH264ProfileLevel {
    VENC_H264PROFILETYPE    nProfile;
    VENC_H264LEVELTYPE        nLevel;
}VencH264ProfileLevel;

typedef struct VencQPRange {
    int    nMaxqp;
    int    nMinqp;
}VencQPRange;

typedef struct MotionParam {
    int nMotionDetectEnable;
    int nMotionDetectRatio; /* 0~12, advise set 0 */
    int nStaticDetectRatio; /* 0~12, should be larger than  nMotionDetectRatio, advise set 2 */
    int nMaxNumStaticFrame; /* advise set 4 */
    double nStaticBitsRatio; /* advise set 0.2~0.3 at daytime, set 0.1 at night */
    double nMV64x64Ratio; /* advise set 0.01 */
    short nMVXTh; /* advise set 6 */
    short nMVYTh; /* advise set 6 */
}MotionParam;

typedef struct VencHeaderData {
    unsigned char*  pBuffer;
    unsigned int    nLength;
}VencHeaderData;

/* support 4 ROI region */
typedef struct VencROIConfig {
    int                     bEnable;
    int                        index; /* (0~3) */
    int                     nQPoffset;
    unsigned char           roi_abs_flag;
    VencRect                sRect;
}VencROIConfig;

typedef struct VencFixQP {
    int                     bEnable;
    int                     nIQp;
    int                     nPQp;
}VencFixQP;

typedef struct VencCopyROIConfig {
    int                     bEnable;
    int                     num; /* (0~16) */
    VencRect                sRect[16];
    unsigned char         *pRoiYAddrVir;
    unsigned char         *pRoiCAddrVir;
    unsigned long         pRoiYAddrPhy;
    unsigned long         pRoiCAddrPhy;
}VencCopyROIConfig;


typedef enum {
    AW_CBR = 0,
    AW_VBR = 1,
    AW_AVBR = 2,
    AW_QPMAP = 3,
    AW_FIXQP = 4,
}VENC_RC_MODE;

typedef struct VencInputBuffer {
    unsigned long  nID;
    long long         nPts;
    unsigned int   nFlag;
    unsigned char* pAddrPhyY;
    unsigned char* pAddrPhyC;
    unsigned char* pAddrVirY;
    unsigned char* pAddrVirC;
    int            bEnableCorp;
    VencRect       sCropInfo;

    int            ispPicVar;
    int            ispPicVarChroma;     //chroma  filter  coef[0-63],  from isp
    int			   bUseInputBufferRoi;
    VencROIConfig  roi_param[8];
    int            bAllocMemSelf;
    int            nShareBufFd;
    unsigned char  bUseCsiColorFormat;
    VENC_PIXEL_FMT eCsiColorFormat;

    int             envLV;
}VencInputBuffer;

typedef struct FrameInfo {
    int             CurrQp;
    int             avQp;
    int             nGopIndex;
    int             nFrameIndex;
    int             nTotalIndex;
}FrameInfo;

typedef struct VeProcSet {
    unsigned char               bProcEnable;
    unsigned int                nProcFreq;
	unsigned int				nStatisBitRateTime;
	unsigned int				nStatisFrRateTime;
}VeProcSet;

typedef struct VeProcEncInfo {
    unsigned int                nChannelNum;
    unsigned int                nProfileIdc;
    int                         nIDRItl;
    int                         nBitRate;
    int                         nFrameRate;

    unsigned int                nInputWidth;
    unsigned int                nInputHeight;
    unsigned int                nDstWidth;
    unsigned int                nDstHeight;
    unsigned int                nStride;

    int                         eSliceType;
    int                         nCurrQp;
    int                         nGopIndex;
    int                         nFrameIndex;
    int                         nTotalIndex;

	unsigned int				uMaxBitRate;
	int 						nQuality;

	VENC_RC_MODE				eRcMode;
	VencFixQP                   fix_qp;
	int 						i_qp_offset;
    int                         qp_max;
    int                         qp_min;

	unsigned int 				AFBC_Enable;
	unsigned int				rot_angle;
	unsigned char				filter3d_level;

	int 						crop_left;
	int 						crop_top;
	int 						crop_width;
	int 						crop_height;

	unsigned int				qp_offset1;
	unsigned int				qp_offset2;
	unsigned int				qp_offset3;
	unsigned int				qp_offset4;
	unsigned int				qp_offset5;
	unsigned int				qp_offset6;
	unsigned int				qp_offset7;
	unsigned int				qp_offset8;

	unsigned int				avr_bit_rate;
	unsigned int				real_bit_rate;
	unsigned int				avr_frame_rate;
	unsigned int				real_frame_rate;

	int							vbv_size;
	int							UnusedBufferSize;
	int							nValidFrameNum;
	int							nValidDataSize;
}VeProcEncInfo;

typedef struct VencOutputBuffer {
    int               nID;
    long long         nPts;
    unsigned int   nFlag;
    unsigned int   nSize0;
    unsigned int   nSize1;
    unsigned char* pData0;
    unsigned char* pData1;

    FrameInfo       frame_info;
    unsigned int   nSize2;
    unsigned char* pData2;
}VencOutputBuffer;

typedef struct VencAllocateBufferParam {
    unsigned int   nBufferNum;
    unsigned int   nSizeY;
    unsigned int   nSizeC;
}VencAllocateBufferParam;

#define EXTENDED_SAR 255
typedef struct VencH264AspectRatio {
    unsigned char aspect_ratio_idc;
    unsigned short  sar_width;
    unsigned short  sar_height;
}VencH264AspectRatio;

typedef struct VencSaveBSFile {
    char filename[256];
	unsigned char save_bsfile_flag;
    unsigned int save_start_time;
    unsigned int save_end_time;
}VencSaveBSFile;

typedef enum VENC_COLOR_SPACE {
    RESERVED0    = 0,
    VENC_BT709  = 1,                 /* bt709 */
    RESERVED1    = 2,
    RESERVED2    = 3,
    RESERVED3    = 4,
    VENC_BT601  = 5,                 /* bt601-625 default use this colorspace */
    BT601_525   = 6,                 /* bt601-525 */
    RESERVED4    = 7,
    VENC_YCC    = 8,                /* YCC: full range BT.601  */
}VENC_COLOR_SPACE;

typedef enum VENC_VIDEO_FORMAT {
    COMPONENT       = 0,                 /* component */
    PAL               = 1,                 /* pal*/
    NTSC            = 2,                 /* ntsc */
    SECAM            = 3,                /* secam  */
    MAC                = 4,                /* mac  */
    DEFAULT            = 5,                /* Unspecified video format  */
}VENC_VIDEO_FORMAT;

typedef struct VencJpegVideoSignal {
    VENC_COLOR_SPACE src_colour_primaries;
    VENC_COLOR_SPACE dst_colour_primaries;
}VencJpegVideoSignal;

typedef struct VencH264VideoSignal {
    VENC_VIDEO_FORMAT video_format;

    unsigned char full_range_flag;

    VENC_COLOR_SPACE src_colour_primaries;
    VENC_COLOR_SPACE dst_colour_primaries;
}VencH264VideoSignal;

typedef struct VencH264VideoTiming {
      unsigned long num_units_in_tick;
      unsigned long time_scale;
      unsigned int fixed_frame_rate_flag;

}VencH264VideoTiming;

// Add for setting SVC and Skip_Frame
typedef struct VencH264SVCSkip {
    T_LAYER        nTemporalSVC;
    SKIP_FRAME     nSkipFrame;
    int            bEnableLayerRatio;
    unsigned int   nLayerRatio[4];
}VencH264SVCSkip;

typedef struct VencCyclicIntraRefresh {
    int                     bEnable;
    int                     nBlockNumber;
}VencCyclicIntraRefresh;

typedef struct VencSize {
    int                     nWidth;
    int                     nHeight;
}VencSize;

typedef enum  VENC_VIDEO_GOP_MODE {
    AW_NORMALP                    = 1,   //one p ref frame
    AW_ADVANCE_SINGLE             = 2,
    AW_DOUBLEP                    = 3,
    AW_SPECIAL_DOUBLE            = 4,
    AW_SPECIAL_SMARTP            = 5,   //double p ref frames and use virtual i frame,but virtual i ref virtual i
    AW_SMARTP                    = 6,   //double p ref frames and use virtual i frame
}VENC_VIDEO_GOP_MODE;

typedef struct VencAdvancedRefParam {
    unsigned char              bAdvancedRefEn;   //advanced ref frame mode, 0:not use , 1:use
    unsigned int               nBase;       //base frame num
    unsigned int               nEnhance;    //enhance frame num
    unsigned char              bRefBaseEn;  //ctrl base frame ref base frame, 0:enable, 1:disable
}VencAdvancedRefParam;

typedef struct VencGopParam {
    unsigned char              bUseGopCtrlEn;   //use user set gop mode
    VENC_VIDEO_GOP_MODE        eGopMode;        //gop mode
    unsigned int               nVirtualIFrameInterval;
    unsigned int               nSpInterval;     //user set special p frame ref interval
    VencAdvancedRefParam       sRefParam;       //user set advanced ref frame mode
}VencGopParam;

typedef struct VencCheckColorFormat {
    int                        index;
    VENC_PIXEL_FMT          eColorFormat;
}VencCheckColorFormat;

typedef struct VencVP8Param {
    int                     nFramerate; /* fps*/
    int                     nBitrate;   /* bps*/
    int                     nMaxKeyInterval;
}VencVP8Param;

typedef enum VENC_SUPERFRAME_MODE {
    VENC_SUPERFRAME_NONE,
    VENC_SUPERFRAME_DISCARD,
    VENC_SUPERFRAME_REENCODE,
}VENC_SUPERFRAME_MODE;

typedef struct VencSuperFrameConfig {
    VENC_SUPERFRAME_MODE    eSuperFrameMode;
    unsigned int            nMaxIFrameBits;
    unsigned int            nMaxPFrameBits;
}VencSuperFrameConfig;

typedef struct VencBitRateRange {
    int            bitRateMax;
    int            bitRateMin;
}VencBitRateRange;

typedef struct VencRoiBgFrameRate {
    int            nSrcFrameRate;
    int            nDstFrameRate;
}VencRoiBgFrameRate;

typedef struct VencAlterFrameRateInfo {
    unsigned char       bEnable;
    unsigned char       bUseUserSetRoiInfo;   //0:use csi roi info; 1:use user set roi info
    VencRoiBgFrameRate  sRoiBgFrameRate;
    VencROIConfig       roi_param[8];
}VencAlterFrameRateInfo;

typedef struct VencH265TranS {
    /*** unsigned char       transquant_bypass_enabled_flag; not support ***/
    //0:disable transform skip; 1:enable transform skip
    unsigned char       transform_skip_enabled_flag;
    //chroma_qp= sliece_qp+chroma_qp_offset
    char                chroma_qp_offset;
}VencH265TranS;

typedef struct VencH265SaoS {
    //0:disable luma sao filter; 1:enable luma sao filter
    unsigned char       slice_sao_luma_flag;
    //0:disable chroma sao filter; 1:enable chroma sao filter
    unsigned char       slice_sao_chroma_flag;
}VencH265SaoS;

typedef struct VencH265DblkS {
    //0:enable deblock filter; 1:disable deblock filter
    unsigned char       slice_deblocking_filter_disabled_flag;
    char                slice_beta_offset_div2; //range: [-6,6]
    char                slice_tc_offset_div2; //range: [-6,6]
}VencH265DblkS;

typedef struct VencH265TimingS {
    //0:stream without timing info; 1:stream with timing info
    unsigned char       timing_info_present_flag;
    unsigned int        num_units_in_tick;       //time_scale/frameRate
    unsigned int        time_scale;             //1second is  average divided by time_scale
    unsigned int        num_ticks_poc_diff_one; //num ticks of diff frame
}VencH265TimingS;

typedef enum VENC_OVERLAY_ARGB_TYPE {
    VENC_OVERLAY_ARGB_MIN    = -1,
    VENC_OVERLAY_ARGB8888    = 0,
    VENC_OVERLAY_ARGB4444   = 1,
    VENC_OVERLAY_ARGB1555   = 2,
    VENC_OVERLAY_ARGB_MAX    = 3,
}VENC_OVERLAY_ARGB_TYPE;

typedef enum VENC_OVERLAY_TYPE {
    NORMAL_OVERLAY          = 0,    //normal overlay
    COVER_OVERLAY           = 1,    //use the setting yuv to cover region
    LUMA_REVERSE_OVERLAY    = 2,    //normal overlay and luma reverse
}VENC_OVERLAY_TYPE;

typedef struct VencOverlayCoverYuvS {
     unsigned char       use_cover_yuv_flag; //1:use the cover yuv; 0:transform the argb data to yuv ,then cover
     unsigned char       cover_y; //the value of cover y
     unsigned char       cover_u; //the value of cover u
     unsigned char       cover_v; //the value of cover v
}VencOverlayCoverYuvS;

typedef struct VencOverlayHeaderS {
    unsigned short      start_mb_x;         //horizonal value of  start points divided by 16
    unsigned short      end_mb_x;           //horizonal value of  end points divided by 16
    unsigned short      start_mb_y;         //vertical value of  start points divided by 16
    unsigned short      end_mb_y;           //vertical value of  end points divided by 16
    unsigned char       extra_alpha_flag;   //0:no use extra_alpha; 1:use extra_alpha
    unsigned char       extra_alpha;        //use user set extra_alpha, range is [0, 15]
    VencOverlayCoverYuvS cover_yuv;         //when use COVER_OVERLAY should set the cover yuv
    VENC_OVERLAY_TYPE   overlay_type;       //reference define of VENC_OVERLAY_TYPE
    unsigned char*      overlay_blk_addr;   //the vir addr of overlay block
    unsigned int        bitmap_size;        //the size of bitmap

    //* for v5v200 and newer ic
    unsigned int        bforce_reverse_flag;
    unsigned int        reverse_unit_mb_w_minus1;
    unsigned int        reverse_unit_mb_h_minus1;
    //* end

}VencOverlayHeaderS;

typedef struct VencOverlayInfoS {
    unsigned char               blk_num; //num of overlay region
    VENC_OVERLAY_ARGB_TYPE      argb_type;//reference define of VENC_ARGB_TYPE
    VencOverlayHeaderS          overlayHeaderList[MAX_OVERLAY_SIZE];

    //* for v5v200 and newer ic
    unsigned int                invert_mode;
    unsigned int                invert_threshold;
    //* end

}VencOverlayInfoS;

typedef struct VencBrightnessS {
    unsigned int               dark_th; //dark threshold, default 60, range[0, 255]
    unsigned int               bright_th; //bright threshold, default 200, range[0, 255]
}VencBrightnessS;

typedef struct VencEncodeTimeS {
    unsigned int                  frame_num; //current frame num
    unsigned int                  curr_enc_time; //current frame encoder time
    unsigned int                  curr_empty_time; //the time between current frame and last frame
    unsigned int                  avr_enc_time; //average encoder time
    unsigned int                  avr_empty_time; //average empty time
    unsigned int                  max_enc_time;
    unsigned int                  max_enc_time_frame_num;
    unsigned int                  max_empty_time;
    unsigned int                  max_empty_time_frame_num;
}VencEncodeTimeS;

typedef enum VENC_INDEXTYPE {
    VENC_IndexParamBitrate                = 0x0,
    /**< reference type: int */
    VENC_IndexParamFramerate,
    /**< reference type: int */
    VENC_IndexParamMaxKeyInterval,
    /**< reference type: int */
    VENC_IndexParamIfilter,
    /**< reference type: int */
    VENC_IndexParamRotation,
    /**< reference type: int */
    VENC_IndexParamSliceHeight,
    /**< reference type: int */
    VENC_IndexParamForceKeyFrame,
    /**< reference type: int (write only)*/
    VENC_IndexParamMotionDetectEnable,
    /**< reference type: MotionParam(write only) */
    VENC_IndexParamMotionDetectStatus,
    /**< reference type: int(read only) */
    VENC_IndexParamRgb2Yuv,
    /**< reference type: VENC_COLOR_SPACE */
    VENC_IndexParamYuv2Yuv,
    /**< reference type: VENC_YUV2YUV */
    VENC_IndexParamROIConfig,
    /**< reference type: VencROIConfig */
    VENC_IndexParamStride,
    /**< reference type: int */
    VENC_IndexParamColorFormat,
    /**< reference type: VENC_PIXEL_FMT */
    VENC_IndexParamSize,
    /**< reference type: VencSize(read only) */
    VENC_IndexParamSetVbvSize,
    /**< reference type: setVbvSize(write only) */
    VENC_IndexParamVbvInfo,
    /**< reference type: getVbvInfo(read only) */
    VENC_IndexParamSuperFrameConfig,
    /**< reference type: VencSuperFrameConfig */
    VENC_IndexParamSetPSkip,
    /**< reference type: unsigned int */
    VENC_IndexParamResetEnc,
    /**< reference type: */
	VENC_IndexParamSaveBSFile,
	/**< reference type: VencSaveBSFile */
	VENC_IndexParamHorizonFlip,
	/**< reference type: unsigned int */

    /* check capabiliy */
    VENC_IndexParamMAXSupportSize,
    /**< reference type: VencSize(read only) */
    VENC_IndexParamCheckColorFormat,
    /**< reference type: VencCheckFormat(read only) */

    /* H264 param */
    VENC_IndexParamH264Param  = 0x100,
    /**< reference type: VencH264Param */
    VENC_IndexParamH264SPSPPS,
    /**< reference type: VencHeaderData (read only)*/
    VENC_IndexParamH264QPRange,
    /**< reference type: VencQPRange */
    VENC_IndexParamH264ProfileLevel,
    /**< reference type: VencProfileLevel */
    VENC_IndexParamH264EntropyCodingCABAC,
    /**< reference type: int(0:CAVLC 1:CABAC) */
    VENC_IndexParamH264CyclicIntraRefresh,
    /**< reference type: VencCyclicIntraRefresh */
    VENC_IndexParamH264FixQP,
    /**< reference type: VencFixQP */
    VENC_IndexParamH264SVCSkip,
    /**< reference type: VencH264SVCSkip */
    VENC_IndexParamH264AspectRatio,
    /**< reference type: VencH264AspectRatio */
    VENC_IndexParamFastEnc,
    /**< reference type: int */
    VENC_IndexParamH264VideoSignal,
    /**< reference type: VencH264VideoSignal */
    VENC_IndexParamH264VideoTiming,
    /**< reference type: VencH264VideoTiming */
    VENC_IndexParamChmoraGray,
    /**< reference type: unsigned char */
    VENC_IndexParamIQpOffset,
    /**< reference type: int */
    /* jpeg param */
    VENC_IndexParamJpegQuality            = 0x200,
    /**< reference type: int (1~100) */
    VENC_IndexParamJpegExifInfo,
    /**< reference type: EXIFInfo */
    VENC_IndexParamJpegEncMode,
    /**< reference type: 0:jpeg; 1:motion_jepg */
    VENC_IndexParamJpegVideoSignal,
    /**< reference type: VencJpegVideoSignal */

    /* VP8 param */
    VENC_IndexParamVP8Param,
    /* max one frame length */
    VENC_IndexParamSetFrameLenThreshold,
    /**< reference type: int */
    /* decrease the a20 dram bands */
    VENC_IndexParamSetA20LowBands,
    /**< reference type: 0:disable; 1:enable */
    VENC_IndexParamSetBitRateRange,
    /**< reference type: VencBitRateRange */
    VENC_IndexParamLongTermReference,
    /**< reference type: 0:disable; 1:enable, default:enable */

    /* h265 param */
    VENC_IndexParamH265Param = 0x300,
    VENC_IndexParamH265Gop,
    VENC_IndexParamH265ToalFramesNum,
    VENC_IndexParamH26xUpdateLTRef,
    VENC_IndexParamH265Header,
    VENC_IndexParamH265TendRatioCoef,
	VENC_IndexParamH265Trans,
    /**< reference type: VencH265TranS */
    VENC_IndexParamH265Sao,
    /**< reference type: VencH265SaoS */
    VENC_IndexParamH265Dblk,
    /**< reference type: VencH265DblkS */
    VENC_IndexParamH265Timing,
    /**< reference type: VencH265TimingS */
    VENC_IndexParamIntraPeriod,
    VENC_IndexParamMBModeCtrl,
    VENC_IndexParamMBSumInfoOutput,
    VENC_IndexParamMBInfoOutput,
    VENC_IndexParamVUIAspectRatio,
    VENC_IndexParamVUIVideoSignal,
    VENC_IndexParamVUIChromaLoc,
    VENC_IndexParamVUIDisplayWindow,
    VENC_IndexParamVUIBitstreamRestriction,

    VENC_IndexParamAlterFrame = 0x400,
    /**< reference type: unsigned int */
    VENC_IndexParamVirtualIFrame,
    VENC_IndexParamChannelNum,
    VENC_IndexParamProcSet,
    /**< reference type: VencOverlayInfoS */
    VENC_IndexParamSetOverlay,
    /**< reference type: unsigned char */
    VENC_IndexParamAllParams,
    /**< reference type:VencBrightnessS */
    VENC_IndexParamBright,
    /**< reference type:VencSmartFun */
    VENC_IndexParamSmartFuntion,
    /**< reference type: VencHVS */
    VENC_IndexParamHVS,
    /**< reference type: unsigned char */
    VENC_IndexParamSkipTend,
    /**< reference type: unsigned char */
    VENC_IndexParamHighPassFilter,
    /**< reference type: unsigned char */
    VENC_IndexParamPFrameIntraEn,
    /**< reference type: unsigned char */
    VENC_IndexParamEncodeTimeEn,
    /**< reference type: VencEncodeTimeS */
    VENC_IndexParamGetEncodeTime,
    /**< reference type: unsigned char */
    VENC_IndexParam3DFilter,
    /**< reference type: unsigned char */
    VENC_IndexParamIntra4x4En,

    /**< reference type: unsigned int */
    VENC_IndexParamSetNullFrame = 0x500,
    /**< reference type: VencThumbInfo */
    VENC_IndexParamGetThumbYUV,
    /**< reference type: E_ISP_SCALER_RATIO */
    VENC_IndexParamSetThumbScaler,
    /**< reference type: unsigned char */
    VENC_IndexParamAdaptiveIntraInP,
    /**< reference type: VencBaseConfig */
    VENC_IndexParamUpdateBaseInfo,

    /**< reference type: unsigned char */
    VENC_IndexParamFillingCbr,

    /**< reference type: unsigned char */
    VENC_IndexParamRoi,
}VENC_INDEXTYPE;

typedef enum VENC_RESULT_TYPE {
    VENC_RESULT_ERROR             = -1,
    VENC_RESULT_OK                   = 0,
    VENC_RESULT_NO_FRAME_BUFFER   = 1,
    VENC_RESULT_BITSTREAM_IS_FULL = 2,
    VENC_RESULT_ILLEGAL_PARAM     = 3,
    VENC_RESULT_NOT_SUPPORT     = 4,
    VENC_RESULT_BITSTREAM_IS_EMPTY = 5,
    VENC_RESULT_NO_MEMORY     = 6,
    VENC_RESULT_NO_RESOURCE     = 7,
    VENC_RESULT_NULL_PTR     = 8,
    VENC_RESULT_DROP_FRAME   = 9,

    VENC_RESULT_EFUSE_ERROR  = 25,
}VENC_RESULT_TYPE;

typedef enum {
    H265_B_SLICE             = 0x0,
    H265_P_SLICE             = 0x1,
    H265_I_SLICE             = 0x2,
    H265_IDR_SLICE           = 0x12
}VENC_H265_CODE_TYPE;

typedef struct JpegEncInfo {
    VencBaseConfig  sBaseInfo;
    int             bNoUseAddrPhy;
    unsigned char*  pAddrPhyY;
    unsigned char*  pAddrPhyC;
    unsigned char*  pAddrVirY;
    unsigned char*  pAddrVirC;
    int             bEnableCorp;
    VencRect        sCropInfo;
    int                quality;
    int             nShareBufFd;
}JpegEncInfo;

typedef struct VbvInfo {
    unsigned int vbv_size;
    unsigned int coded_frame_num;
    unsigned int coded_size;
    unsigned int maxFrameLen;
}VbvInfo;

typedef enum {
    VENC_H265ProfileMain        = 1,
    VENC_H265ProfileMain10      = 2,
    VENC_H265ProfileMainStill   = 3
}VENC_H265PROFILETYPE;

typedef enum {
    VENC_H265Level1   = 30,     /**< Level 1 */
    VENC_H265Level2  = 60,     /**< Level 2 */
    VENC_H265Level21  = 63,     /**< Level 2.1 */
    VENC_H265Level3  = 90,     /**< Level 3 */
    VENC_H265Level31   = 93,     /**< Level 3.1 */
    VENC_H265Level41  = 123,     /**< Level 4.1 */
    VENC_H265Level5  = 150,     /**< Level 5 */
    VENC_H265Level51   = 153,     /**< Level 5.1 */
    VENC_H265Level52  = 156,     /**< Level 5.2 */
    VENC_H265Level6  = 180,     /**< Level 6 */
    VENC_H265Level61   = 183,     /**< Level 6.1 */
    VENC_H265Level62  = 186     /**< Level 6.2 */
}VENC_H265LEVELTYPE;

typedef enum {
    REF_IDC_DISCARD = 0,
    REF_IDC_CURRENT_USE = 1,
    REF_IDC_FUTURE_USE = 2,
    REF_IDC_LONG_TERM = 4,
    REF_IDC_CURRENT_REF = 8,
}ReferenceIdc;

typedef struct {
    VENC_H265PROFILETYPE    nProfile;
    VENC_H265LEVELTYPE        nLevel;
}VencH265ProfileLevel;

typedef struct {
    unsigned int            uMaxBitRate;
    unsigned int            uRatioChangeQp; //range[50,100], default:85
    int                     nQuality;       //range[1,13], 1:worst quality, 13:best quality
}VencVbrParam;

typedef struct {
    unsigned char mode_ctrl_en;
    unsigned char *p_info;
}VencMBModeCtrl;

typedef struct {
    VENC_RC_MODE            eRcMode;
    unsigned char           bUseSetMadThrdFlag;
    unsigned char           uMadThrdI[12]; //range 0-255
    unsigned char           uMadThrdP[12]; //range 0-255
    unsigned char           uMadThrdB[12]; //no support
    unsigned int            uStatTime;      //range [1,10], default:1
    unsigned int            uMinIQp;
    int                     nMaxReEncodeTimes; //default use one time

    VencVbrParam            sVbrParam;      //valid only at AW_VBR/AW_AVBR
    VencFixQP               sFixQp;         //valid only at AW_FIXQP
    VencMBModeCtrl          sQpMap;         //valid only at AW_QPMAP

    unsigned int            uRowQpDelta; //no support
    unsigned int            uDirectionThrd; //no support
    unsigned int            uQpDeltaLevelI; //no support
    unsigned int            uQpDeltaLevelP; //no support
    unsigned int            uQpDeltaLevelB; //no support
    unsigned int            uInputFrmRate;  //no support
    unsigned int            uOutputFrmRate; //no support
    unsigned int            uFluctuateLevel;//no support
    unsigned int            uMinIprop;      //no support
    unsigned int            uMaxIprop;      //no support
}VencRcParam;

typedef struct VencH264Param {
    VencH264ProfileLevel    sProfileLevel;
    int                     bEntropyCodingCABAC; /* 0:CAVLC 1:CABAC*/
    VencQPRange               sQPRange;
    int                     nFramerate; /* fps*/
    int                     nSrcFramerate; /* fps*/
    int                     nBitrate;   /* bps*/
    int                     nMaxKeyInterval;
    VENC_CODING_MODE        nCodingMode;
    VencGopParam            sGopParam;
    VencRcParam             sRcParam;
}VencH264Param;

typedef struct {
    int                     idr_period;
    VencH265ProfileLevel    sProfileLevel;
    VencQPRange               sQPRange;
    int                     nFramerate; /* fps*/
    int                     nSrcFramerate; /* fps*/
    int                     nBitrate;   /* bps*/
    int                     nIntraPeriod;
    int                     nGopSize;
    int                     nQPInit; /* qp of first IDR_frame if use rate control */
    VencRcParam             sRcParam;
    VencGopParam            sGopParam;
}VencH265Param;

typedef struct {
    unsigned int    slice_type;
    int             poc; // dispaly order of the frame within a GOP, ranging from 1 to gop_size
    int             qp_offset;
#if 0
    float           qp_factor; // used for RDO weighting,the bigger value means the lower quality
                                        // and less bits, ranging from 0.3 to 1.0
#endif
    int             tc_offset_div2; // offset of LoopFilterTcOffsetDiv2, ranging from -6 to 6
    int             beta_offset_div2; // offset of LoopFilterTcOffsetDiv2, ranging from -6 to 6

    unsigned int    num_ref_pics; // number of ref_frames reserved for cur_frame and future frames
    unsigned int    num_ref_pics_active; // number of ref_frames is permited to be used in L0 or L1

    int             reference_pics[MAX_FRM_NUM-1]; // = ref_frame_poc - cur_frame_poc
    // = discard_frame_poc - cur_frame_poc, means derlta_poc of ref_frames which are discarded
    int             discard_pics[MAX_FRM_NUM-1];

    unsigned char   lt_ref_flag; // 1: enable cur_frame use long term ref_frame
    int             lt_ref_poc; // poc of lt_ref_frame of cur_frame

    // 0 means next 4 member parameters are ignored; 1 means next 3 member parameters are need
    // this parameter of the first frame of a GOP must be 0
    unsigned char   predict;

    unsigned int    delta_rps_idx; // = cur_frame_encoding_idx - predictor_frame_encoding_idx

    int             delta_rps; // = predictor_frame_poc - cur_frame_poc

    // num of ref_idcs to encoder for the current frame, the value is equal to
    // the value of num_st_ref_pics of the predictor_frame + 1 + lt_ref_flag
    unsigned int    num_ref_idcs;

    // [][0]=(ref_frame_poc or discard_frame_poc) - cur_frame_poc
    // [][1]indicating the ref_pictures reserved in ref_list_buffer:
    // [][1]=0: will not be a ref_picture anymore
    // [][1]=1: is a ref_picture used by cur_picture
    // [][1]=2: is a ref_picture used by future_picture
    // [][1]=3: is a long term ref_picture
    int             reference_idcs[MAX_FRM_NUM][2];
}RefPicSet;
typedef struct {
    int gop_size;
    int intra_period;
    int max_num_ref_pics;
    unsigned char num_ref_idx_l0_default_active;
    unsigned char num_ref_idx_l1_default_active;
    RefPicSet ref_str[MAX_GOP_SIZE + 2]; // just when custom_rps_flag is 1, it should be set
    unsigned char use_sps_rps_flag; // if it is 1, rps will not occur in slice_header
    unsigned char use_lt_ref_flag;
    unsigned char custom_rps_flag; // 0: default ref_str will be use; 1: user should set ref_str[]
}VencH265GopStruct;

#define MAX_NUM_MB (65536)
typedef struct {
    unsigned char mb_mad;
    unsigned char mb_qp;
    unsigned int mb_sse;
    double mb_psnr;
}VencMBInfoPara;

typedef struct {
    unsigned int num_mb;
    VencMBInfoPara *p_para;
}VencMBInfo;

typedef struct {
    unsigned int sum_mad;
    unsigned int sum_qp;
    unsigned long long sum_sse;
}VencMBSumInfo;

typedef struct {
    unsigned char mb_qp; // {5:0}
    unsigned char mb_skip_flag; // {6}
    unsigned char mb_en; // {7}
}VencMBModeCtrlInfo;

typedef struct {
    unsigned char hp_filter_en;
    unsigned level_hpf;
}VencHighPassFilter;

typedef struct {
    unsigned char hvs_en;
    unsigned int  th_dir;
    unsigned int  th_coef_shift;
}VencHVS;

typedef struct {
    unsigned int inter_tend;
    unsigned int skip_tend;
    unsigned int merge_tend;
}VencH265TendRatioCoef;

typedef struct {
    unsigned char smart_fun_en;
    unsigned char img_bin_en;
    unsigned int img_bin_th;
    unsigned int shift_bits;
}VencSmartFun;
typedef struct {
    unsigned int chroma_sample_top;
    unsigned int chroma_sample_bottom;
}VencVUIChromaLoc;

typedef struct {
    unsigned int win_left_offset;
    unsigned int win_right_offset;
    unsigned int win_top_offset;
    unsigned int win_bottom_offset;
}VencVUIDisplayWindow;

typedef struct {
    unsigned char tiles_fixed_structure_flag;
    unsigned char mv_over_pic_boundaries_flag;
    unsigned char restricted_ref_pic_lists_flag;
    unsigned int  min_spatial_seg_idc;
    unsigned int  max_bytes_per_pic_denom;
    unsigned int  max_bits_per_min_cu_denom;
    unsigned int  log2_max_mv_len_hor;
    unsigned int  log2_max_mv_len_ver;
}VencVUIBitstreamRestriction;

int AWJpecEnc(JpegEncInfo* pJpegInfo, EXIFInfo* pExifInfo,
    void* pOutBuffer, int* pOutBufferSize);

typedef void* VideoEncoder;

VideoEncoder* VideoEncCreate(VENC_CODEC_TYPE eCodecType);
void VideoEncDestroy(VideoEncoder* pEncoder);
int VideoEncInit(VideoEncoder* pEncoder, VencBaseConfig* pConfig);
int VideoEncUnInit(VideoEncoder* pEncoder);

int AllocInputBuffer(VideoEncoder* pEncoder, VencAllocateBufferParam *pBufferParam);
int GetOneAllocInputBuffer(VideoEncoder* pEncoder, VencInputBuffer* pInputbuffer);
int FlushCacheAllocInputBuffer(VideoEncoder* pEncoder,  VencInputBuffer *pInputbuffer);
int ReturnOneAllocInputBuffer(VideoEncoder* pEncoder,  VencInputBuffer *pInputbuffer);
int ReleaseAllocInputBuffer(VideoEncoder* pEncoder);

int AddOneInputBuffer(VideoEncoder* pEncoder, VencInputBuffer* pInputbuffer);
int VideoEncodeOneFrame(VideoEncoder* pEncoder);
int AlreadyUsedInputBuffer(VideoEncoder* pEncoder, VencInputBuffer* pBuffer);

int ValidBitstreamFrameNum(VideoEncoder* pEncoder);
int GetOneBitstreamFrame(VideoEncoder* pEncoder, VencOutputBuffer* pBuffer);
int FreeOneBitStreamFrame(VideoEncoder* pEncoder, VencOutputBuffer* pBuffer);

int VideoEncGetParameter(VideoEncoder* pEncoder, VENC_INDEXTYPE indexType, void* paramData);
int VideoEncSetParameter(VideoEncoder* pEncoder, VENC_INDEXTYPE indexType, void* paramData);

int VideoEncoderReset(VideoEncoder* pEncoder);
unsigned int VideoEncoderGetUnencodedBufferNum(VideoEncoder* pEncoder);

void VideoEncoderGetVeIommuAddr(VideoEncoder* pEncoder, struct user_iommu_param *pIommuBuf);
void VideoEncoderFreeVeIommuAddr(VideoEncoder* pEncoder, struct user_iommu_param *pIommuBuf);

int VideoEncoderSetFreq(VideoEncoder* pEncoder, int nVeFreq);
void VideoEncoderSetDdrMode(VideoEncoder* pEncoder, int nDdrType);

#endif    //_VENCODER_H_

#ifdef __cplusplus
}
#endif /* __cplusplus */
