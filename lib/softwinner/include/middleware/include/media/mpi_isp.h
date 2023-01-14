
/*
 ******************************************************************************
 *
 * MPI_ISP.h
 *
 * Hawkview ISP - mpi_isp.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 *   1.0		  yuanxianfeng   2016/04/01	    ISP
 *
 *****************************************************************************
 */

#ifndef _AW_ISP_API_H_
#define _AW_ISP_API_H_

// #include "common_vi.h"
// #include "plat_error.h"
#include "isp_type.h"
#include "mm_comm_vi.h"
#include "isp_comm.h"
#include "isp_tuning_priv.h"
#include "isp_tuning.h"

#ifdef __cplusplus
//#if __cplusplus
extern "C" {
//#endif /* __cplusplus */
#endif  /* __cplusplus */

/****************************************************\
	ISP Common
\****************************************************/
typedef struct
{
	HW_S32		test_mode;
	HW_S32		gain;
	HW_S32		exp_line;
	HW_S32		color_temp;
	HW_S32		log_param;
}ISP_TEST_PUB_S;

typedef struct
{
	HW_S32		manual;
	HW_S32		afs;
	HW_S32		sharp;
	HW_S32		pri_contrast;
	HW_S32		contrast;
	HW_S32		denoise;
	HW_S32		drc;
	HW_S32		lut_dpc;
	HW_S32		lsc;
	HW_S32		gamma;
	HW_S32		cm;
	HW_S32		ae;
	HW_S32		af;
	HW_S32		awb;
	HW_S32		hist;
	HW_S32		gain_offset;
	HW_S32		wb;
	HW_S32		otf_dpc;
	HW_S32		cfa;
	HW_S32		sprite;
	HW_S32		tdf;
	HW_S32		cnr;
	HW_S32		saturation;
	HW_S32		defog;
	HW_S32		linearity;
	HW_S32		distortion;
	HW_S32		hdr_gamma;
	HW_S32		gtm;
	HW_S32		dig_gain;
	HW_S32		brightness;
}ISP_TEST_ENABLE_S;

typedef enum __ISP_ERR_CODE_E
{
	ERR_ISP_NOT_INIT				= 1,
	ERR_ISP_MEM_NOT_INIT			= 2,
	ERR_ISP_ATTR_NOT_CFG			= 3,
	ERR_ISP_SNS_UNREGISTER			= 4,
	ERR_ISP_INVALID_ADDR			= 5,
	ERR_ISP_NOMEM					= 6,
} ISP_ERR_CODE_E;

#define ERR_ISP_NULL_PTR             DEF_ERR(MOD_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define ERR_ISP_ILLEGAL_PARAM        DEF_ERR(MOD_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define ERR_ISP_NOT_SUPPORT          DEF_ERR(MOD_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* efuse check fail */
#define ERR_ISP_EFUSE_ERR            DEF_ERR(MOD_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_EFUSE_ERROR)
#define ERR_ISP_NOT_INIT             DEF_ERR(MOD_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_NOT_INIT)
#define ERR_ISP_MEM_NOT_INIT         DEF_ERR(MOD_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_MEM_NOT_INIT)
#define ERR_ISP_ATTR_NOT_CFG         DEF_ERR(MOD_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_ATTR_NOT_CFG)
#define ERR_ISP_SNS_UNREGISTER       DEF_ERR(MOD_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_SNS_UNREGISTER)
#define ERR_ISP_INVALID_ADDR         DEF_ERR(MOD_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_INVALID_ADDR)
#define ERR_ISP_NOMEM                DEF_ERR(MOD_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_NOMEM)

typedef enum __ISP_OP_TYPE_E
{
	OP_TYPE_AUTO	= 0,
	OP_TYPE_MANUAL	= 1,
	OP_TYPE_BUTT	= 2,
} ISP_OP_TYPE_E;

typedef enum __ISP_STATE_E
{
    ISP_STATE_INIT     = 0,
    ISP_STATE_SUCCESS  = 1,
    ISP_STATE_TIMEOUT  = 2,
    ISP_STATE_BUTT
} ISP_STATUS_E;

typedef enum __ISP_BAYER_FORMAT_E
{
    BAYER_RGGB    = 0,
    BAYER_GRBG    = 1,
    BAYER_GBRG    = 2,
    BAYER_BGGR    = 3,
    BAYER_BUTT
} ISP_BAYER_FORMAT_E;

typedef struct
{
	HW_S32		gain;
	HW_S32		delay_frame;
} ISP_FLASH_S;

typedef struct
{
	HW_S32		type;
	HW_S32		ratio;
} ISP_FLICKER_S;

typedef struct
{
	HW_S32		strength;
} ISP_DEFOG_S;

typedef struct
{
	HW_S32		horizontal;/*0~1024*/
	HW_S32		vertical;
	HW_S32		focus_length;
} ISP_VISUAL_ANGLE_S;

typedef struct
{
	HW_S32		gain[4];
	HW_S32		offset[4];
} ISP_GAIN_OFFSET_S;

typedef struct
{
	HW_U16				temperature;
	struct isp_rgb2rgb_gain_offset	value;
} ISP_CCM_S;
#define ISP_DYNAMIC_GROUP_COUNT 14

struct isp_dynamic_gtm_item_t {
	HW_S32		value[9];
};
typedef struct
{
	HW_S32		trigger;
	struct isp_dynamic_gtm_item_t	tuning_cfg[14];
} ISP_DYNAMIC_GTM_S;

struct isp_dynamic_pltm_item_t {
	HW_S32		value[16];
};
typedef struct 
{
	HW_S32		trigger;
	struct isp_tuning_pltm_cfg pltm_cfg;//HW_ISP_CFG_TUNING_PLTM
	// struct isp_tuning_pltm_table_cfg pltm_table_cfg;// HW_ISP_CFG_TUNING_PLTM_TBL
} ISP_DYNAMIC_PLTM_S;

/* global tone mapping */
typedef struct
{
	HW_S32		type;
	HW_S32		gamma_type;
	HW_S32		auto_alpha_en;
} ISP_GTM_S;

typedef struct
{
	HW_S32		thres_slop;
	HW_S32		min_thres;
	HW_S32		max_thres;
	HW_S32		cfa_dir_thres;
} ISP_OTF;


typedef struct
{
	HW_S32		number;
	HW_U16		value[5][256];
	HW_U16		lv_triggers[5];
} ISP_GAMMA_S;

typedef struct
{
	HW_S32		ff_mod;
	HW_S32		center_x;
	HW_S32		center_y;
	HW_U16		value[12][768];
	HW_U16		color_temp_triggers[6];
}  ISP_LSC_S;

typedef struct
{
	HW_U16		value[768];
} ISP_LINEARITY_S;

typedef struct
{
	HW_U16		value[512];
} ISP_DISTORTION_S;

typedef struct
{
	enum isp_triger_type sharp_triger;
	enum isp_triger_type contrast_triger;
	enum isp_triger_type denoise_triger;
	enum isp_triger_type brightness_triger;
	enum isp_triger_type gcontrast_triger;
	enum isp_triger_type saturation_triger;
	enum isp_triger_type tdf_triger;
	enum isp_triger_type color_denoise_triger;
	enum isp_triger_type ae_cfg_triger;
	enum isp_triger_type gtm_cfg_triger;
} ISP_TRIGER_S;

typedef struct
{
	HW_S32		lower;
	HW_S32		upper;
	HW_S32		strength;
} isp_dynamic_range_s;

typedef struct
{
	HW_S32		value[14]; // ISP_DYNAMIC_GROUP_COUNT
} ISP_SINGLE_S;

typedef struct
{
	HW_S32		trigger;
	HW_S32		value[ISP_DYNAMIC_GROUP_COUNT];
} ISP_BRIGHTNESS_S;

typedef struct
{
	HW_S32		lower;
	HW_S32		upper;
	HW_S32		strength;
} ISP_RANGE_S;

typedef struct
{
	isp_dynamic_range_s	tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
} ISP_SHARP_S;

struct isp_dynamic_contrast_item_t {
   HW_S32      value[ISP_CONTRAST_MAX];
   HW_S32      global;
};
typedef struct
{
    HW_S32      trigger;
    HW_S32      global_trigger;
    struct isp_dynamic_contrast_item_t    tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
} ISP_CONTRAST_S;

typedef struct
{
	isp_dynamic_range_s	tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
	HW_S32				color_denoise[ISP_DYNAMIC_GROUP_COUNT];
} ISP_DENOISE_S;

struct isp_dynamic_saturation_item_t {
	HW_S32		cb;
	HW_S32		cr;
	HW_S32		value[ISP_SATURATION_MAX];
};
typedef struct
{
	HW_S32		trigger;
	struct isp_dynamic_saturation_item_t	tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
} ISP_SATURATION_S;

typedef struct
{
	int xxx;
} ISP_HUE_S;

typedef struct
{
	HW_S32		value[ISP_DYNAMIC_GROUP_COUNT][4];
} ISP_TDF_S;

/*
typedef struct
{
	HW_S32		value[ISP_DYNAMIC_GROUP_COUNT][9];
} ISP_DYNAMIC_GTM_S;
*/

typedef struct
{
	HW_S32		anti_win_over;
	HW_S32		anti_win_under;
	HW_S32		anti_hist_over;
	HW_S32		anti_hist_under;
	HW_S32		preview_speed;
	HW_S32		capture_speed;
	HW_S32		video_speed;
	HW_S32		touch_speed;
	HW_S32		tolerance;
	HW_S32		target;
	HW_S32		hist_cfg[4];
} ISP_AE_TUNE_S;

typedef struct
{
	struct isp_dynamic_ae_item tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
} ISP_AE_S;

typedef struct
{
	HW_S32		value[ISP_DYNAMIC_GROUP_COUNT][19];
} ISP_RESERVED_S;



typedef struct __ISP_PUB_ATTR_S
{
    RECT_S          stWndRect;      /* RW. */
    float        f32FrameRate;   /* RW, Range: [0.00, 65535.00]. */
    ISP_BAYER_FORMAT_E  enBayer;    /* RW. */
} ISP_PUB_ATTR_S;

typedef enum __ISP_FMW_STATE_E
{
    ISP_FMW_STATE_RUN = 0,
    ISP_FMW_STATE_FREEZE,
    ISP_FMW_STATE_BUTT
} ISP_FMW_STATE_E;

typedef struct __ISP_WDR_MODE_S
{
    WDR_MODE_E  enWDRMode;
} ISP_WDR_MODE_S;

typedef union __ISP_MODULE_CTRL_U
{
    AW_U32  u32Key;
    struct
    {
        AW_U32  bitBypassVideoTest  : 1 ;   /* [0] */
        AW_U32  bitBypassBalanceFe  : 1 ;   /* [1] */
        AW_U32  bitBypassISPDGain   : 1 ;   /* [2] */
        AW_U32  bitBypassGammaFe    : 1 ;   /* [3] */
        AW_U32  bitBypassCrosstalkR : 1 ;   /* [4] */
        AW_U32  bitBypassDPC        : 1 ;   /* [5] */
        AW_U32  bitBypassNR         : 1 ;   /* [6] */
        AW_U32  bitRsv1             : 1 ;   /* [7] */
        AW_U32  bitBypassWBGain     : 1 ;   /* [8] */
        AW_U32  bitBypassShading    : 1 ;   /* [9] */
        AW_U32  bitRsv2             : 1 ;   /* [10] */
        AW_U32  bitBypassDRC        : 1 ;   /* [11] */
        AW_U32  bitBypassDemosaic   : 1 ;   /* [12] */
        AW_U32  bitBypassColorMatrix: 1 ;   /* [13] */
        AW_U32  bitBypassGamma      : 1 ;   /* [14] */
        AW_U32  bitBypassFSWDR      : 1 ;   /* [15] */
        AW_U32  bitGammaFePosition  : 1 ;   /* [16] */
        AW_U32  bit2Rsv3            : 2 ;   /* [17:18] */
        AW_U32  bitBypassCsConv     : 1 ;   /* [19] */
        AW_U32  bit2Rsv4            : 2 ;   /* [20:21] */
        AW_U32  bitBypassSharpen    : 1 ;   /* [22] */
        AW_U32  bitChnSwitch        : 1 ;   /* [23] */
        AW_U32  bit2BypassMode      : 2 ;   /* [24:25] */
        AW_U32  bitBypassAll        : 1 ;   /* [26] */
        AW_U32  bit5Rsv5            : 5 ;   /* [27:31] */
    };
}ISP_MODULE_CTRL_U;


typedef enum __ISP_COMBINE_MODE_E
{
    FS_WDR_COMBINE_SHORT_FIRST = 0, /* Short exposure data is used when combining */
    FS_WDR_COMBINE_LONG_FIRST = 1,  /* Long exposure data is used when combining */
    FS_WDR_COMBINE_BUTT
} ISP_COMBINE_MODE_E;
typedef struct __ISP_WDR_FS_ATTR_S
{
    AW_BOOL bMotionComp;            /* RW, AW_TRUE: enable motion compensation; AW_FLASE: disable motion compensation*/
    AW_U16  u16ShortThresh;         /* RW, Range: [0x0, 0xFFF]. Data above this threshold will be taken from short exposure only.
                                            This value is normally not changed and should be left at the default value */
    AW_U16  u16LongThresh;          /* RW, Range: [0x0, u16ShortThresh]. Data below this threshold will be taken from long exposure only.
                                            This value is normally not changed and should be left at the default value */
    ISP_COMBINE_MODE_E enFSWDRComMode;
} ISP_WDR_FS_ATTR_S;


typedef struct __ISP_DRC_MANUAL_ATTR_S
{
    AW_U32  u32Strength;          /* RW, Range: [0, 0xFF]. Strength of dynamic range compression.
                                          Higher values lead to higher differential gain between shadows and highlights. */
} ISP_DRC_MANUAL_ATTR_S;
typedef struct __ISP_DRC_AUTO_ATTR_S
{
    AW_U32  u32Strength;          /* RW, Range: [0, 0xFF]. It is the base strength. The strength used in ISP is generated by firmware.
                                        In linear mode, strength = f1(u32Strength, histogram, ISO)
                                        In sensor WDR mode: strength = f2(u32Strength, histogram, ISO)
                                        In 2to1 WDR mode: strength = f3(ExpRatio, ISO) */
} ISP_DRC_AUTO_ATTR_S;

typedef struct __ISP_DRC_ATTR_S
{
    AW_BOOL bEnable;
    AW_U32  u32SlopeMax;          /* RW, Range: [0, 0xFF].  Restricts the maximum slope (gain). */
    AW_U32  u32SlopeMin;          /* RW, Range: [0, 0xFF].  Restricts the minimum slope (gain). */
    AW_U32  u32WhiteLevel;        /* RW, Range: [0, 0xFFF]. Values above this are not processed and remain linear. */
    AW_U32  u32BlackLevel;        /* RW, Range: [0, 0xFFF]. Values below this are not processed and remain linear. */
    AW_U32  u32VarianceSpace;     /* RW, Range: [0, 0xF].   Degree of spatial sensitivity of the DRC algorithm. */
    AW_U32  u32VarianceIntensity; /* RW, Range: [0, 0xF].   Degree of intensity sensitivity of the DRC algorithm. */

    AW_U32  u32Asymmetry;         /* RW, Range: [0, 0xFF].  The parameter0 of DRC tone curve. Default value: 0x14. */
    AW_U32  u32BrightEnhance;     /* RW, Range: [0, 0xFF].  The parameter1 of DRC tone curve. Default value: 0xC8. */

    ISP_OP_TYPE_E enOpType;
    ISP_DRC_MANUAL_ATTR_S stManual;
    ISP_DRC_AUTO_ATTR_S   stAuto;
} ISP_DRC_ATTR_S;

typedef struct __ISP_DP_DYNAMIC_ATTR_S
{
    AW_BOOL bEnable;

    AW_U16  u16Slope;             /* RW, Range: [0x0, 0xFFF] */
    AW_U16  u16Thresh;            /* RW, Range: [0x0, 0xFFF] */
    AW_U16  u16LineThresh;        /* RW, Range: [0x0, 0xFFF] */
    AW_U16  u16HpThresh;          /* RW, Range: [0x0, 0xFFF] */
    AW_U8   u8BlendRatio;         /* RW, Range: [0x0, 0xFF] */
} ISP_DP_DYNAMIC_ATTR_S;

typedef struct __ISP_DP_STATIC_ATTR_S
{
    AW_BOOL bEnable;
    AW_U16  u16BrightCount;       /* RW, When used as input(W), indicate the number of static bright defect pixels; As output(R),indicate the number of static bright and dark defect pixels */
    AW_U16  u16DarkCount;         /* RW, When used as input(W), indicate the number of static dark defect pixels; As output(R), invalid value 0 */
    AW_BOOL bShow;

	AW_S32 dpc_th_slop;
	AW_S32 dpc_otf_min_th;
	AW_S32 dpc_otf_max_th;
	AW_S32 cfa_dir_th;
} ISP_DP_STATIC_ATTR_S;

typedef struct __ISP_DP_ATTR_S
{
    ISP_DP_DYNAMIC_ATTR_S stDynamicAttr;
    ISP_DP_STATIC_ATTR_S  stStaticAttr;
} ISP_DP_ATTR_S;

typedef struct __ISP_DIS_ATTR_S
{
    AW_BOOL bEnable;
} ISP_DIS_ATTR_S;

typedef struct __ISP_SHADING_RADIAL_S
{
    AW_U16      u16OffCenter;                   /* RW, Range: [0x0, 0xFFFF] */
    POINT_S     stCenter;                       /* RW, Range: [0x0, 0xFFFF] */

} ISP_SHADING_RADIAL_S;
typedef struct __ISP_SHADING_ATTR_S
{
	HW_S32		ff_mod;
	HW_S32		center_x;
	HW_S32		center_y;
	HW_S32		rolloff_ratio;
	HW_U16		value[12][768];
	HW_U16		color_temp_triggers[6];
} ISP_SHADING_ATTR_S;

typedef struct __ISP_NR_MANUAL_ATTR_S
{
    AW_U8 u8Thresh;                            /* RW, Range: [0x0, 0xFF]. Noise reduction effect for high spacial frequencies. */
    AW_U8 u8ThreshLong;                        /* RW, Range: [0x0, 0xFF]. Noise reduction effect for high spacial frequencies. */
} ISP_NR_MANUAL_ATTR_S;
typedef struct __ISP_NR_AUTO_ATTR_S
{

} ISP_NR_AUTO_ATTR_S;

struct isp_dynamic_denoise_item_t {
	HW_S32		value[ISP_DENOISE_MAX];
	HW_S32		color_denoise;
};
typedef struct __ISP_NR_ATTR_S
{
	HW_S32		trigger;
	HW_S32		color_trigger;
	struct isp_dynamic_denoise_item_t tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
} ISP_NR_ATTR_S;
struct isp_dynamic_tdf_item_t {
	HW_S32		value[ISP_TDF_MAX];
};
typedef struct __ISP_3NR_ATTR_S
{
	HW_S32		trigger;
	struct isp_dynamic_tdf_item_t tuning_cfg[ISP_DYNAMIC_GROUP_COUNT];
} ISP_3NR_ATTR_S;

/* Noise profile table for ISP 2DNR */
typedef struct __ISP_NP_TABLE_S
{

} ISP_NP_TABLE_S;

typedef enum __ISP_GAMMA_CURVE_TYPE_E
{
    ISP_GAMMA_CURVE_DEFAULT = 0x0,  /* default Gamma curve */
    ISP_GAMMA_CURVE_SRGB,
    ISP_GAMMA_CURVE_USER_DEFINE,    /* user defined Gamma curve, Gamma Table must be correct */
    ISP_GAMMA_CURVE_BUTT
} ISP_GAMMA_CURVE_TYPE_E;
typedef struct __ISP_GAMMA_ATTR_S
{
	HW_S32		number;
	HW_U16		value[5][ISP_GAMMA_TBL_LENGTH];
	HW_U16		lv_triggers[5];
} ISP_GAMMA_ATTR_S;

typedef struct __ISP_GAMMAFE_ATTR_S
{
    AW_BOOL bEnable;


} ISP_GAMMAFE_ATTR_S;

typedef struct __ISP_SHARPEN_MANUAL_ATTR_S
{
    AW_U8 u8SharpenD;       /* RW,  Range:[0, 0xFF]. */
    AW_U8 u8SharpenUd;      /* RW,  Range:[0, 0xFF]. */
    AW_U8 u8SharpenRGB;     /* RW,  Range:[0, 0xFF]. */
} ISP_SHARPEN_MANUAL_ATTR_S;
typedef struct __ISP_SHARPEN_AUTO_ATTR_S
{

} ISP_SHARPEN_AUTO_ATTR_S;

typedef struct __ISP_RGBSHARPEN_ATTR_S
{
    AW_U8   u8LutCore;                          /* RW, Range: [0-255]*/
    AW_U8   u8LutStrength;                      /* RW, Range: [0-127]*/
    AW_U8   u8LutMagnitude;                     /* RW, Range: [0-31]*/

	AW_S32 sLower;
	AW_S32 sUpper;
	AW_S32 sStrength;
} ISP_RGBSHARPEN_ATTR_S;

struct isp_dynamic_sharp_item_t {
		HW_S32		value[10];
};
typedef struct __ISP_SHARPEN_ATTR_S
{
	HW_S32		trigger;
	struct isp_dynamic_sharp_item_t	tuning_cfg[14];
} ISP_SHARPEN_ATTR_S;

typedef struct __ISP_ANTI_FALSECOLOR_S
{
    AW_U8  u8Strength;      /* RW, Range: [0x0, 0xFF]. The recommended range is [0x0, 0x95], the normal
                                    color will gradually be eroded when this register is larger than 0x95. */
} ISP_ANTI_FALSECOLOR_S;

typedef struct __ISP_BLACK_LEVEL_S
{
    AW_U16 au16BlackLevel[4]; /* RW, Range: [0x0, 0xFFF]*/
} ISP_BLACK_LEVEL_S;

/* FPN */
typedef enum __ISP_FPN_TYPE_E
{
    ISP_FPN_TYPE_FRAME = 0,
    ISP_FPN_TYPE_LINE = 1,
    ISP_FPN_TYPE_BUTT
}ISP_FPN_TYPE_E;

typedef struct __ISP_FPN_FRAME_INFO_S
{
    AW_U32              u32Iso;             /* FPN CALIBRATE ISO */
    AW_U32              u32Offset;          /* FPN frame u32Offset (agv pixel value) */
    AW_U32              u32FrmSize;         /* FPN frame size (exactly frm size or compress len) */
    VIDEO_FRAME_INFO_S  stFpnFrame;         /* FPN frame info, 8bpp,10bpp,12bpp,16bpp. Compression or not */
}ISP_FPN_FRAME_INFO_S;

typedef struct __ISP_FPN_CALIBRATE_ATTR_S
{
    AW_U32                          u32Threshold;        /* pix value > threshold means defective pixel, [1, 0xFFF] */
    AW_U32                          u32FrameNum;         /* value is 2^N, range: [1, 16] */
    ISP_FPN_TYPE_E                  enFpnType;           /* frame mode or line mode */
    ISP_FPN_FRAME_INFO_S            stFpnCaliFrame;
}ISP_FPN_CALIBRATE_ATTR_S;

typedef struct __ISP_FPN_MANUAL_ATTR_S
{
    AW_U32               u32Strength;         /* gain, RW fp 4.8 */
}ISP_FPN_MANUAL_ATTR_S;

typedef struct __ISP_FPN_AUTO_ATTR_S
{
    AW_U32               u32Strength;         /* gain, RO fp 4.8 */
}ISP_FPN_AUTO_ATTR_S;

typedef struct __ISP_FPN_ATTR_S
{
    AW_BOOL               bEnable;
    ISP_OP_TYPE_E         enOpType;           /* manual or auto mode */
    ISP_FPN_TYPE_E        enFpnType;          /* frame mode or line mode */
    ISP_FPN_FRAME_INFO_S  stFpnFrmInfo;       /* input in correction mode. */
    ISP_FPN_MANUAL_ATTR_S stManual;
    ISP_FPN_AUTO_ATTR_S   stAuto;
}ISP_FPN_ATTR_S;

/* DeFog */
typedef struct __ISP_DEFOG_MANUAL_ATTR_S
{
    AW_U8               u8strength;
}ISP_DEFOG_MANUAL_ATTR_S;

typedef struct __ISP_DEFOG_AUTO_ATTR_S
{
    AW_U8               u8strength;
}ISP_DEFOG_AUTO_ATTR_S;

typedef struct __ISP_DEFOG_ATTR_S
{
	AW_S32		strength;
}ISP_DEFOG_ATTR_S;

typedef struct __ISP_MODULE_ONOFF {
	HW_S32		manual;     /*手动模块开关使能(pipeline ip 开关)*/
	HW_S32		afs;        /*自动工频检测使能(50Hz 60Hz)*/
	HW_S32		sharp;      /*锐化使能*/
	HW_S32		contrast;   /*对比度使能*/
	HW_S32		denoise;    /*2D降噪使能*/
	HW_S32		drc;        /*动态范围压缩使能*/
	HW_S32		cem;        /*颜色增强使能*/
	HW_S32		lsc;        /*镜头阴影矫正使能*/
	HW_S32		gamma;      /*Gamma矫正使能*/
	HW_S32		cm;         /*Color Matrix使能*/
	HW_S32		ae;         /*自动曝光使能*/
	HW_S32		af;         /*自动对焦使能*/
	HW_S32		awb;        /*自动白平衡使能*/
	HW_S32		hist;       /*自动直方图统计使能*/
	HW_S32		blc;        /*黑电平矫正使能*/
	HW_S32		so;         /*Sensor Offset使能*/
	HW_S32		wb;         /*白平衡矫正使能*/
	HW_S32		otf_dpc;    /*坏点补偿使能*/
	HW_S32		cfa;        /*CFA差值使能*/
	HW_S32		tdf;        /* 3dnr - 0:关闭; 1:打开*/
	HW_S32		cnr;        /*色度降噪使能*/
	HW_S32		saturation; /*饱和度调整使能*/
	HW_S32		defog;      /*去雾使能*/
	HW_S32		linearity;  /*线性矫正使能*/
	HW_S32		gtm;        /*全局色调映射使能*/
	HW_S32		dig_gain;   /*数字增益使能*/
	HW_S32		pltm;       /*局部色调映射使能*/
	HW_S32		wdr;        /* wdr - 0:关闭; 1:打开 */
	HW_S32		ctc;        /*crosstalk correction*/
} ISP_MODULE_ONOFF;

/****************************************************\
	IRIS
\****************************************************/
/****************************************************\
	AE
\****************************************************/
typedef enum __ISP_AE_MODE_E
{
	AE_MODE_SLOW_SHUTTER = 0,
	AE_MODE_FIX_FRAME_RATE	= 1,
	AE_MODE_BUTT
} ISP_AE_MODE_E;

typedef enum __ISP_AE_STRATEGY_E
{
	AE_EXP_HIGHLIGHT_PRIOR = 0,
	AE_EXP_LOWLIGHT_PRIOR  = 1,
	AE_STRATEGY_MODE_BUTT
} ISP_AE_STRATEGY_E;

typedef struct __ISP_AE_RANGE_S
{
	AW_U32 u32Max;
	AW_U32 u32Min;
} ISP_AE_RANGE_S;

typedef enum __ISP_ANTIFLICKER_MODE_E
{
	/* The epxosure time is fixed to be the multiplies of 1/(2*frequency) sec,
	 * it may lead to over exposure in the high-luminance environments. */
	ISP_ANTIFLICKER_NORMAL_MODE = 0x0,

	/* The anti flicker may be closed to avoid over exposure in the high-luminance environments. */
	ISP_ANTIFLICKER_AUTO_MODE = 0x1,

	ISP_ANTIFLICKER_MODE_BUTT
}ISP_ANTIFLICKER_MODE_E;

typedef struct __ISP_ANTIFLICKER_S
{
	AW_BOOL bEnable;
	AW_U8	u8Frequency;		  /*RW, Range: usually this value is 50 or 60  which is the frequency of the AC power supply*/
	ISP_ANTIFLICKER_MODE_E	enMode;
} ISP_ANTIFLICKER_S;

typedef struct __ISP_SUBFLICKER_S
{
	AW_BOOL bEnable;

	/* RW, Range: [0x0, 0x64], if subflicker mode enable, current luma is less than AE compensation plus LumaDiff,
	  AE will keep min antiflicker shutter time(for example: 1/100s or 1/120s) to avoid flicker. while current luma is
	  larger than AE compensation plus the LumaDiff, AE will reduce shutter time to avoid over-exposure and introduce
	  flicker in the pircture */
	AW_U8	u8LumaDiff;
} ISP_SUBFLICKER_S;

typedef struct __ISP_AE_ATTR_S
{
	HW_S32		define_table;
	HW_S32		max_lv;
	HW_S32		hist_mode_en;
	HW_S32		compensation_step;
	HW_S32		touch_dist_index;
	HW_S32		iso2gain_ratio;
	HW_S32		fno_table[16];
	HW_S32		ki;
	HW_S32		conv_data_index;
	HW_S32		wdr_cfg[3];
} ISP_AE_ATTR_PUB_S;

typedef struct
{
	HW_S32		length;
	struct ae_table	value[7];
} ISP_AE_TABLE_S;

typedef struct
{
	HW_S32		weight[64];
} ISP_AE_WEIGHT_S;

typedef struct
{
	HW_S32		ae_frame;
	HW_S32		exp_frame;
	HW_S32		gain_frame;
} ISP_AE_DELAY_S;

typedef struct __ISP_EXP_INFO_S
{
	AW_U32 u32ExpTime;				/* RO, Range: (0x0, 0xFFFFFFFF] */
	AW_U32 u32AGain;				/* RO, Range: [0x400, 0xFFFFFFFF] */
	AW_U32 u32DGain;				/* RO, Range: [0x400, 0xFFFFFFFF] */
	AW_U32 u32ISPDGain; 			/* RO, Range: [0x400, 0xFFFFFFFF] */

	AW_U32 u32SensorTotalGain; // AGain * DGain
	AW_U32 u32TotalGain; // AGain * DGain * ISPDGain

	AW_U32 u32Exposure; 			/* RO, Range: [0x40, 0xFFFFFFFF] */
	AW_BOOL bExposureIsMAX; 		/* RO, Range: [0x0, 0x1]*/
	AW_S16 s16HistError;			/* RO, Range: [-0x8000, 0x7FFF]*/
	AW_U32 u32AE_Hist256Value[256]; /* RO, 256 bins histogram */
	AW_U16 u16AE_Hist5Value[5]; 	/* RO, 5 bins histogram */
	AW_U8  u8AveLum;				/* RO, Range: [0x0, 0xFF]*/
	AW_U32 u32LinesPer500ms;		/* RO, Range: [0x0, 0xFFFFFFFF], exposure lines per 500ms */
	AW_U32 u32PirisFNO; 			/* RO, Range: [0x0, 0x400] */
	AW_U32 u32Fps;					/* RO, actual fps */
}ISP_EXP_INFO_S;

/****************************************************\
	AWB
\****************************************************/
typedef struct
{
	HW_S32		low;
	HW_S32		high;
	HW_S32		base;
} ISP_AWB_TEMP_RANGE_S;

typedef struct __awb_speed
{
	HW_S32 interval;
	HW_S32 speed;
	HW_S32 stat_sel;
} ISP_AWB_SPEED_S;

typedef struct
{
	HW_S32		number;
	HW_S32		value[320];
} ISP_AWB_TEMP_INFO_S;

typedef struct
{
	HW_S32		value[22];
} ISP_AWB_PRESET_GAIN_S;

typedef struct
{
	HW_S32		rgain;
	HW_S32		bgain;
} ISP_AWB_FAVOR_S;


typedef enum hiISP_AWB_ALG_TYPE_E
{
	AWB_ALG_LOWCOST = 0,
	AWB_ALG_ADVANCE = 1,
	AWB_ALG_BUTT
} ISP_AWB_ALG_TYPE_E;

typedef struct hiISP_AWB_CT_LIMIT_ATTR_S
{
	AW_BOOL bEnable;
	ISP_OP_TYPE_E	enOpType;

	AW_U16 u16HighRgLimit;	   /*RW, Range:[0x0, 0xFFF], in Manual Mode, user define the Max Rgain of High Color Temperature*/
	AW_U16 u16HighBgLimit;	   /*RW, Range:[0x0, 0xFFF], in Manual Mode, user define the Min Bgain of High Color Temperature*/
	AW_U16 u16LowRgLimit;	   /*RW, Range:[0x0, 0xFFF], in Manual Mode, user define the Min Rgain of Low Color Temperature*/
	AW_U16 u16LowBgLimit;	   /*RW, Range:[0x0, 0xFFF], in Manual Mode, user define the Max Bgain of Low Color Temperature*/
} ISP_AWB_CT_LIMIT_ATTR_S;

typedef struct hiISP_AWB_IN_OUT_ATTR_S
{
	AW_BOOL bEnable;
	ISP_OP_TYPE_E	enOpType;
	AW_BOOL bOutdoorStatus; 			 /*in Auto mode, this is RO, in Manual mode, this is WO*/
	AW_U32 u32OutThresh;				 /*shutter time(in us) to judge indoor or outdoor */
	AW_U16 u16LowStart; 				 /*5000K is recommend*/
	AW_U16 u16LowStop;					 /*4500K is recommend, should be smaller than u8LowStart*/
	AW_U16 u16HighStart;				 /*6500K is recommend, shoule be larger than u8LowStart*/
	AW_U16 u16HighStop; 				 /*8000K is recommend, should be larger than u8HighStart*/
	AW_BOOL bGreenEnhanceEn;			 /*If this is enabled, Green channel will be enhanced based on the ratio of green plant*/
} ISP_AWB_IN_OUT_ATTR_S;

typedef struct hiISP_AWB_CBCR_TRACK_ATTR_S
{
	AW_BOOL bEnable;						  /*If enabled, statistic parameter cr, cb will change according to luminance*/

	//AW_U16	au16CrMax[ISP_AUTO_STENGTH_NUM];  /*only take effect for lowlight & low color temperature*/
	//AW_U16	au16CrMin[ISP_AUTO_STENGTH_NUM];
	//AW_U16	au16CbMax[ISP_AUTO_STENGTH_NUM];
	//AW_U16	au16CbMin[ISP_AUTO_STENGTH_NUM];
} ISP_AWB_CBCR_TRACK_ATTR_S;

typedef struct hiISP_AWB_LUM_HISTGRAM_ATTR_S
{
	AW_BOOL bEnable;						  /*If enabled, zone weight to awb is combined with zone luma*/
	ISP_OP_TYPE_E	enOpType;				  /*In auto mode, the weight distribution follows Gaussian distribution*/
	AW_U8	 au8HistThresh[6];				  /*In manual mode, user define luma thresh, thresh[0] is fixed to 0, thresh[5] is fixed to 0xFF*/
	AW_U16	au16HistWt[6];					  /*In manual mode, user define luma weight*/
} ISP_AWB_LUM_HISTGRAM_ATTR_S;

typedef struct hiISP_AWB_LIGHTSOURCE_INFO_S
{
	AW_U16 u16WhiteRgain;		  /*G/R of White points at this light source*/
	AW_U16 u16WhiteBgain;		  /*G/B of White points at this light source*/
	AW_U16 u16ExpQuant; 		  /*shutter time * again * dgain >> 4, Not support Now*/
	AW_U8  u8LightStatus;			/*RW, 0: idle  1:add light source  2:delete sensitive color */
	AW_U8  u8Radius;				  /*Radius of light source*/
} ISP_AWB_EXTRA_LIGHTSOURCE_INFO_S;

typedef struct hiISP_AWB_ATTR_EX_S
{
	AW_U8  u8Tolerance; 		  /*RW, Range:[0x0, 0xFF], AWB adjust tolerance*/
	AW_U8  u8ZoneRadius;		  /*RW, Range:[0x0, 0xFF], radius of AWB blocks*/
	AW_U16 u16CurveLLimit;		  /*RW, Range:[0x0, 0x100],   Left limit of AWB Curve, recomend for indoor 0xE0, outdoor 0xE0*/
	AW_U16 u16CurveRLimit;		  /*RW, Range:[0x100, 0xFFF], Right Limit of AWB Curve,recomend for indoor 0x130, outdoor 0x120*/

	AW_BOOL  bExtraLightEn; 	  /*Enable special light source function*/
	ISP_AWB_EXTRA_LIGHTSOURCE_INFO_S	stLightInfo[4];
	ISP_AWB_IN_OUT_ATTR_S							stInOrOut;

	AW_BOOL bMultiLightSourceEn;   /*If enabled, active saturation will be decreased in multi light source enviroment*/

} ISP_AWB_ATTR_EX_S;

typedef struct hiISP_AWB_ATTR_S
{
	AW_BOOL bEnable;			   /*If AWB is disabled, static wb gain will be used, otherwise awb gain will be used*/

	AW_U16 u16RefColorTemp; 	   /*Calibration Information*/
	AW_U16 au16StaticWB[4]; 	   /*Calibration Information*/
	AW_S32 as32CurvePara[6];	   /*Calibration Information*/

	ISP_AWB_ALG_TYPE_E			enAlgType;

	AW_U8  u8RGStrength;							/*RW, AWB Strength of R Channel, Range: [0x0, 0xFF]*/
	AW_U8  u8BGStrength;							/*RW, AWB Strength of B Channel, Range: [0x0, 0xFF]*/
	AW_U16 u16Speed;								/*RW, Convergence speed of AWB, Range:[0x0, 0xFFF] */
	AW_U16 u16ZoneSel;								/*RW,  A value of 0 or 0xFF means global AWB, A value between 0 and 0xFF means zoned AWB */
	AW_U16 u16HighColorTemp;						/*RW, AWB max temperature, Recommended: [8500, 10000] */
	AW_U16 u16LowColorTemp; 						/*RW, AWB min temperature, Range: [0x0, u8HighColorTemp), Recommended: [2000, 2500] */
	ISP_AWB_CT_LIMIT_ATTR_S   stCTLimit;
	AW_BOOL bShiftLimitEn;							/*If the statistic information is out of range, it should be project*/
	AW_U8  u8ShiftLimit;
	AW_BOOL bGainNormEn;

	ISP_AWB_CBCR_TRACK_ATTR_S stCbCrTrack;
	ISP_AWB_LUM_HISTGRAM_ATTR_S stLumaHist;
} ISP_AWB_ATTR_S;

typedef struct hiISP_MWB_ATTR_S
{
	AW_U16 u16Rgain;			/*RW, Multiplier for R	color channel, Range: [0x0, 0xFFF]*/
	AW_U16 u16Grgain;			/*RW, Multiplier for Gr color channel, Range: [0x0, 0xFFF]*/
	AW_U16 u16Gbgain;			/*RW, Multiplier for Gb color channel, Range: [0x0, 0xFFF]*/
	AW_U16 u16Bgain;			/*RW, Multiplier for B	color channel, Range: [0x0, 0xFFF]*/
} ISP_MWB_ATTR_S;

typedef struct __ISP_WB_ATTR_S
{
	// AW_BOOL bByPass;
	ISP_OP_TYPE_E	enOpType;
	ISP_MWB_ATTR_S	stManual;
	ISP_AWB_ATTR_S	stAuto;
} ISP_WB_ATTR_S;

typedef struct hiISP_COLORMATRIX_MANUAL_S
{
	AW_BOOL bSatEn; 			/*If bSatEn=1, the active CCM = SatMatrix * ManualMatrix */
	AW_U16 au16CCM[9];			/*RW,  Range: [0x0,  0xFFFF]*/
} ISP_COLORMATRIX_MANUAL_S;

typedef struct hiISP_COLORMATRIX_AUTO_S
{
	AW_BOOL bISOActEn;			 /*if enabled, CCM will bypass in low light*/
	AW_BOOL bTempActEn; 		 /*if enabled, CCM will bypass when color temperate is larger than 10K or less than 2500K*/

	AW_U16 u16HighColorTemp;	/*RW,  Range: <=10000*/
	AW_U16 au16HighCCM[9];		/*RW,  Range: [0x0,  0xFFFF]*/
	AW_U16 u16MidColorTemp; 	/*RW,  the MidColorTemp should be at least 400 smaller than HighColorTemp*/
	AW_U16 au16MidCCM[9];		/*RW,  Range: [0x0,  0xFFFF]*/
	AW_U16 u16LowColorTemp; 	/*RW,  the LowColorTemp should be at least 400 smaller than u16MidColorTemp, >= 2000*/
	AW_U16 au16LowCCM[9];		/*RW,  Range: [0x0,  0xFFFF]*/
} ISP_COLORMATRIX_AUTO_S;

typedef struct hiISP_COLORMATRIX_ATTR_S
{
	HW_U16		temperature;
	struct isp_rgb2rgb_gain_offset	value;
}ISP_COLORMATRIX_ATTR_S;

typedef struct hiISP_SATURATION_MANUAL_S
{
	AW_U8	u8Saturation;		 /*RW,	Range: [0, 0xFF] */
} ISP_SATURATION_MANUAL_S;

typedef struct hiISP_SATURATION_AUTO_S
{
	AW_S32		cb;
	AW_S32		cr;
	AW_S32		value[4];

	// AW_U8	au8Sat[ISP_AUTO_STENGTH_NUM];			/*RW,  Range: [0, 0xFF], should be decreased based on ISO increase */
} ISP_SATURATION_AUTO_S;

typedef struct hiISP_SATURATION_ATTR_S
{
	ISP_OP_TYPE_E enOpType;
	ISP_SATURATION_MANUAL_S stManual;
	ISP_SATURATION_AUTO_S stAuto[ISP_DYNAMIC_GROUP_COUNT];
}ISP_SATURATION_ATTR_S;

typedef struct hiISP_COLOR_TONE_ATTR_S
{
	AW_U16 u16RedCastGain;		  /*RW, R channel gain after CCM*/
	AW_U16 u16GreenCastGain;	  /*RW, G channel gain after CCM*/
	AW_U16 u16BlueCastGain; 	  /*RW, B channel gain after CCM*/
} ISP_COLOR_TONE_ATTR_S;

typedef struct hiISP_WB_INFO_S
{
	AW_U16 u16Rgain;			 /*RO, AWB result of R color channel, Range: [0x0, 0xFFF]*/
	AW_U16 u16Grgain;			 /*RO, AWB result of Gr color channel, Range: [0x0, 0xFFF]*/
	AW_U16 u16Gbgain;			 /*RO, AWB result of Gb color channel, Range: [0x0, 0xFFF]*/
	AW_U16 u16Bgain;			 /*RO, AWB result of B color channel, Range: [0x0, 0xFFF]*/
	AW_U16 u16Saturation;		 /*RO, Current saturation, Range:[0x0, 0xFF]*/
	AW_U16 u16ColorTemp;		 /*RO, Detect color temperature, maybe out of color cemeprature range*/
	AW_U16 au16CCM[9];			 /*RO, Current color correction matrix*/
}ISP_WB_INFO_S;

/****************************************************\
	AF
\****************************************************/
typedef struct
{
	HW_S32		min;
	HW_S32		max;
} ISP_AF_VCM_CODE_S;

typedef struct
{
	HW_S32		use_otp;
} ISP_AF_OTP_S;

typedef struct
{
	HW_S32		interval_time;
	HW_S32		index;
} ISP_AF_SPEED_S;

typedef struct
{
	HW_S32		auto_en;
	HW_S32		single_en;
	HW_S32		step;
} ISP_AF_FINE_SEARCH_S;

typedef struct
{
	HW_S32		move_cnt;
	HW_S32		still_cnt;
	HW_S32		move_monitor_cnt;
	HW_S32		still_monitor_cnt;
} ISP_AF_REFOCUS_S;

typedef struct
{
	HW_S32		near_distance;
	HW_S32		far_distance;
	HW_S32		offset;
	HW_S32		table_length;
	HW_S32		std_code_table[20];
	HW_S32		value[20];
} ISP_AF_TOLERANCE_S;

typedef struct
{
	HW_S32		stable_min;
	HW_S32		stable_max;
	HW_S32		low_light_lv;
	HW_S32		peak_thres;
	HW_S32		direction_thres;
	HW_S32		change_ratio;
	HW_S32		move_minus;
	HW_S32		still_minus;
	HW_S32		scene_motion_thres;
} ISP_AF_SCENE_S;

typedef struct __ISP_AF_ATTR_S
{
	AW_S32 s32DistanceMax; /* the focuse range */
	AW_S32 s32DistanceMin;

	/* weighting table */
	// AW_U8 u8Weight[AF_ZONE_ROW][AF_ZONE_COLUMN];
} ISP_AF_ATTR_S;
typedef struct __ISP_MF_ATTR_S
{
	AW_S32 s32DefaultSpeed; 	/* 1,default speed(unit:m/s).(onvif)*/
} ISP_MF_ATTR_S;
typedef struct __ISP_FOCUS_ATTR_S
{
	ISP_OP_TYPE_E  enOpType;
	ISP_MF_ATTR_S  stManual;
	ISP_AF_ATTR_S  stAuto;
} ISP_FOCUS_ATTR_S;

/****************************************************\
	ISP
\****************************************************/
AW_S32 AW_MPI_ISP_Init();
AW_S32 AW_MPI_ISP_Run(ISP_DEV IspDev); // [0, 1]
AW_S32 AW_MPI_ISP_Stop(ISP_DEV IspDev);
AW_S32 AW_MPI_ISP_Exit();

// CFG ISP IP On or Off
AW_S32 AW_MPI_ISP_SetModuleOnOff(ISP_DEV IspDev, ISP_MODULE_ONOFF *pstIspModuleOnOff);
AW_S32 AW_MPI_ISP_GetModuleOnOff(ISP_DEV IspDev, ISP_MODULE_ONOFF *pstIspModuleOnOff);

//set cfg
AW_S32 AW_MPI_ISP_GetAe(ISP_DEV IspDev, ISP_AE_S *pAe);
AW_S32 AW_MPI_ISP_SetAe(ISP_DEV IspDev, ISP_AE_S *pAe);

// ======================
// IOCTL ISP
AW_S32 AW_MPI_ISP_AE_SetMode(ISP_DEV IspDev, int Value);			// [0:auto, 1:manual]
AW_S32 AW_MPI_ISP_AE_SetExposureBias(ISP_DEV IspDev, int Value);	// [1, 8]
AW_S32 AW_MPI_ISP_AE_SetExposure(ISP_DEV IspDev, int Value);		// [0, 65535*16]
AW_S32 AW_MPI_ISP_AE_SetISOSensitiveMode(ISP_DEV IspDev, int Mode); // [0:manual, 1:auto]
AW_S32 AW_MPI_ISP_AE_SetISOSensitive(ISP_DEV IspDev, int Value); // [0~7]->[auto,100,200,400,800,1600,3200,6400]
AW_S32 AW_MPI_ISP_AE_SetMetering(ISP_DEV IspDev, int Value);        // [0:average,1:center,2:spot,3:matrix]
AW_S32 AW_MPI_ISP_AE_SetGain(ISP_DEV IspDev, int Value);			// [0, 65535]
AW_S32 AW_MPI_ISP_AWB_SetMode(ISP_DEV IspDev, int Value);		// [0:auto, 1:manual]
AW_S32 AW_MPI_ISP_AWB_SetColorTemp(ISP_DEV IspDev, int Value); // [2, 9]
AW_S32 AW_MPI_ISP_AWB_SetRGain(ISP_DEV IspDev, int Value);		// [256, 256 * 64]
AW_S32 AW_MPI_ISP_AWB_SetBGain(ISP_DEV IspDev, int Value);		// [256, 256 * 64]
AW_S32 AW_MPI_ISP_AWB_SetGrGain(ISP_DEV IspDev, int Value);		// [256, 256 * 64]
AW_S32 AW_MPI_ISP_AWB_SetGbGain(ISP_DEV IspDev, int Value);		// [256, 256 * 64]

AW_S32 AW_MPI_ISP_SetFlicker(ISP_DEV IspDev, int Value); 				// [0:disable,1:50,2:60,3:auto]
AW_S32 AW_MPI_ISP_SetBrightness(ISP_DEV IspDev, int Value); 		//[-126, 126]
AW_S32 AW_MPI_ISP_SetContrast(ISP_DEV IspDev, int Value);   			// [-126, 126]
AW_S32 AW_MPI_ISP_SetSaturation(ISP_DEV IspDev, int Value); 			// [-256, 512]
AW_S32 AW_MPI_ISP_SetSharpness(ISP_DEV IspDev, int Value); 		// [0, 10]
AW_S32 AW_MPI_ISP_SetScene(ISP_DEV IspDev, int Value);

AW_S32 AW_MPI_ISP_AE_GetMode(ISP_DEV IspDev, int *Value);
AW_S32 AW_MPI_ISP_AE_GetExposureBias(ISP_DEV IspDev, int *Value);
AW_S32 AW_MPI_ISP_AE_GetExposure(ISP_DEV IspDev, int *Value);
//add by jaosn
AW_S32 AW_MPI_ISP_AE_GetExposureLine(ISP_DEV IspDev, int *Value);
AW_S32 AW_MPI_ISP_AE_GetEvIdx(ISP_DEV IspDev, int *Value);
AW_S32 AW_MPI_ISP_AWB_GetCurColorT(ISP_DEV IspDev, int *Value);


AW_S32 AW_MPI_ISP_AE_GetISOSensitiveMode(ISP_DEV IspDev, int *Mode); // [0:manual, 1:auto]
AW_S32 AW_MPI_ISP_AE_GetISOSensitive(ISP_DEV IspDev, int *Value); // [0~7]->[auto,100,200,400,800,1600,3200,6400]
AW_S32 AW_MPI_ISP_AE_GetMetering(ISP_DEV IspDev, int *Value); // [0:average,1:center,2:spot,3:matrix]
AW_S32 AW_MPI_ISP_AE_GetGain(ISP_DEV IspDev, int *Value);
AW_S32 AW_MPI_ISP_AWB_GetMode(ISP_DEV IspDev, int *Value);
AW_S32 AW_MPI_ISP_AWB_GetColorTemp(ISP_DEV IspDev, int *Value);
AW_S32 AW_MPI_ISP_AWB_GetRGain(ISP_DEV IspDev, int *Value);
AW_S32 AW_MPI_ISP_AWB_GetBGain(ISP_DEV IspDev, int *Value);
AW_S32 AW_MPI_ISP_AWB_GetGrGain(ISP_DEV IspDev, int *Value);
AW_S32 AW_MPI_ISP_AWB_GetGbGain(ISP_DEV IspDev, int *Value);

AW_S32 AW_MPI_ISP_GetFlicker(ISP_DEV IspDev, int *Value);
AW_S32 AW_MPI_ISP_GetBrightness(ISP_DEV IspDev, int *Value);
AW_S32 AW_MPI_ISP_GetContrast(ISP_DEV IspDev, int *Value);
AW_S32 AW_MPI_ISP_GetSaturation(ISP_DEV IspDev, int *Value);
AW_S32 AW_MPI_ISP_GetSharpness(ISP_DEV IspDev, int *Value);
AW_S32 AW_MPI_ISP_GetScene(ISP_DEV IspDev, int *Value);

AW_S32 AW_MPI_ISP_SetPltmWDR(ISP_DEV IspDev, int Value); 	// [0, 255]
AW_S32 AW_MPI_ISP_GetPltmWDR(ISP_DEV IspDev, int *Value); // [0, 255]
AW_S32 AW_MPI_ISP_SetNRAttr(ISP_DEV IspDev,  int Value);		// [0, 100]
AW_S32 AW_MPI_ISP_GetNRAttr(ISP_DEV IspDev, int *Value);		// [0, 100]
AW_S32 AW_MPI_ISP_Set3NRAttr(ISP_DEV IspDev,int Value);		// [0, 100]
AW_S32 AW_MPI_ISP_Get3NRAttr(ISP_DEV IspDev, int *Value);	// [0, 100]

AW_S32 AW_MPI_ISP_SetSaveCTX(ISP_DEV IspDev);

/**
 * get environment luminance value.
 */
int AW_MPI_ISP_GetEnvLV(ISP_DEV IspDev);

#ifdef __cplusplus
//#if __cplusplus
}
//#endif /* __cplusplus */
#endif  /* __cplusplus */

#endif /*_AW_ISP_API_H_*/

