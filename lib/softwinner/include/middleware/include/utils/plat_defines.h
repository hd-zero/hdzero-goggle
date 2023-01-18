/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : plat_defines.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/03/15
  Last Modified :
  Description   : The common configuration defination, always hardware constraint.
  Function List :
  History       :
******************************************************************************/

#ifndef __IPCLINUX_PLAT_DEFINES_H__
#define __IPCLINUX_PLAT_DEFINES_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

//these definitions must keep sync with mpp_config.mk
#define AW_V40 0x1701
#define AW_V5  0x1721
#define AW_V316 0x1816

//AWCHIP will be defined in mpp_config.mk.
#ifndef AWCHIP
    #define AWCHIP AW_V5
    #error AWCHIP define may be error
#endif

#if (AWCHIP==AW_V40)
    #define CHIP_NAME    "AWV40"
    #define MPP_VER_PRIX "_MPP_V"
#elif (AWCHIP==AW_V5)
    #define CHIP_NAME    "AWV5"
    #define MPP_VER_PRIX "_MPP_V"
#elif (AWCHIP==AW_V316)
    #define CHIP_NAME    "AWV316"
    #define MPP_VER_PRIX "_MPP_V"
#else
    #error AWCHIP define may be error
#endif

#define DEFAULT_ALIGN           16

#define MAX_NODE_NUM            16

/* For VDA */
#define VDA_MAX_NODE_NUM        32
#define VDA_MAX_INTERNAL        256
#define VDA_CHN_NUM_MAX         32
#define VDA_MAX_WIDTH           960
#define VDA_MAX_HEIGHT          960
#define VDA_MIN_WIDTH           32
#define VDA_MIN_HEIGHT          32

/* For VENC */
#define VENC_MAX_NAME_LEN       16
#define VENC_MAX_CHN_NUM        16
#define VENC_MAX_GRP_NUM        16
#define H264E_MAX_WIDTH         2592
#define H264E_MAX_HEIGHT        2592
#define H264E_MIN_WIDTH         160
#define H264E_MIN_HEIGHT        64
#define H265E_MAX_WIDTH         2592
#define H265E_MAX_HEIGHT        2592
#define H265E_MIN_WIDTH         128
#define H265E_MIN_HEIGHT        128
#define JPEGE_MAX_WIDTH         8192
#define JPEGE_MAX_HEIGHT        8192
#define JPEGE_MIN_WIDTH         32
#define JPEGE_MIN_HEIGHT        32
#define VENC_MAX_ROI_NUM        8               /* The max numbers of ROI region support */
#define H264E_MIN_HW_INDEX      0
#define H264E_MAX_HW_INDEX      11
#define H264E_MIN_VW_INDEX      0
#define H264E_MAX_VW_INDEX      3




/* For RC */
#define RC_TEXTURE_THR_SIZE     12
#define RC_RQRATIO_SIZE         8


/* For VDEC */
#define VDEC_MAX_CHN_NUM        16

/* For Region */
#define RGN_MIN_WIDTH             2
#define RGN_MIN_HEIGHT            2
#define RGN_MAX_WIDTH             2592
#define RGN_MAX_HEIGHT            2592
#define RGN_ALIGN                 2

#define RGN_HANDLE_MAX            1024

#define COVER_MAX_NUM_VI          0
#define COVEREX_MAX_NUM_VI        16
#define OVERLAY_MAX_NUM_VI        0
#define OVERLAYEX_MAX_NUM_VI      16

#define OVERLAY_MAX_NUM_VENC      8

#define COVER_MAX_NUM_VPSS        8
#define COVEREX_MAX_NUM_VPSS      8
#define OVERLAY_MAX_NUM_VPSS      1
#define OVERLAYEX_MAX_NUM_VPSS    8

#define OVERLAYEX_MAX_NUM_VGS     1
#define COVER_MAX_NUM_VGS         1


#define COVEREX_MAX_NUM_VO         1
#define OVERLAYEX_MAX_NUM_VO       1

#define OVERLAYEX_MAX_NUM_PCIV    0

/* number of channle and device on video input unit of chip
 * Note! VIU_MAX_CHN_NUM is NOT equal to VIU_MAX_DEV_NUM
 * multiplied by VIU_MAX_CHN_NUM, because all VI devices
 * can't work at mode of 4 channles at the same time.
 */
#define VIU_MAX_DEV_NUM              1
#define VIU_MAX_WAY_NUM_PER_DEV      1
#define VIU_MAX_CHN_NUM_PER_DEV      1
#define VIU_MAX_PHYCHN_NUM           1
#define VIU_MAX_RAWCHN_NUM           1      /* raw data chn, DVR/NVR: 0 */
#define VIU_EXT_CHN_START            VIU_MAX_PHYCHN_NUM
#define VIU_MAX_EXT_CHN_NUM          16
#define VIU_MAX_EXTCHN_BIND_PER_CHN  8

#define VIU_MAX_CHN_NUM              (VIU_MAX_PHYCHN_NUM + VIU_MAX_EXT_CHN_NUM)
#define VIU_MAX_UFLIST_NUM           (VIU_MAX_CHN_NUM + VIU_MAX_RAWCHN_NUM)

#define VIU_CHNID_DEV_FACTOR    2

/* Hi3516A do not support cascade. The definition of Macro is just for compiling */
#define VIU_MAX_CAS_CHN_NUM     2
#define VIU_SUB_CHN_START       16    /* The starting number of sub channel*/
#define VIU_CAS_CHN_START       32    /* The starting number of cascade channel */

/* max number of VBI region*/
#define VIU_MAX_VBI_NUM         2
/* max length of one VBI region (by word) */
#define VIU_MAX_VBI_LEN         8

#define VO_MIN_CHN_WIDTH        32      /* channel minimal width */
#define VO_MIN_CHN_HEIGHT       32      /* channel minimal height */

#define VO_MAX_ZOOM_RATIO       1000    /* max zoom ratio, 1000 means 100% scale */

#define VO_MAX_DEV_NUM          1       /* max dev num */
#define VO_MAX_LAYER_NUM        16 //1       /* max layer num */
#define PIP_MAX_CHN_NUM         8
#define VHD_MAX_CHN_NUM         32       /* max VHD chn num */
#define VO_MAX_CHN_NUM          VHD_MAX_CHN_NUM      /* max chn num */
#define VO_MAX_LAYER_IN_DEV     1      /* max layer num of each dev */
#define VO_MIN_CHN_LINE         3


#define VO_MAX_GFX_LAYER_PER_DEV    1
#define VO_MAX_GRAPHICS_LAYER_NUM   1
#define MDDRC_ZONE_MAX_NUM          32

#define VO_MAX_PRIORITY         2
#define VO_MIN_TOLERATE         1       /* min play toleration 1ms */
#define VO_MAX_TOLERATE         100000  /* max play toleration 100s */

#define AI_DEV_MAX_NUM          1
#define AO_DEV_MIN_NUM          0
#define AO_DEV_MAX_NUM          1
#define AIO_MAX_NUM             1
#define AIO_MAX_CHN_NUM         16
#define AENC_MAX_CHN_NUM        32
#define ADEC_MAX_CHN_NUM        32

/* For VPSS */
#define VPSS_MAX_GRP_NUM   		128

#define VPSS_MAX_PHY_CHN_NUM	4
#define VPSS_MAX_EXT_CHN_NUM  	8
#define VPSS_MAX_CHN_NUM   		(VPSS_MAX_PHY_CHN_NUM + VPSS_MAX_EXT_CHN_NUM)

#define VPSS_BSTR_CHN     		0
#define VPSS_LSTR_CHN     		1
#define VPSS_PRE0_CHN			2

#define VPSS_COVER_POINT_NUM    4

/* For DEMUX */
#define DEMUX_MAX_CHN_NUM       16

/* For MUX */
#define MUX_MAX_GRP_NUM         8
#define MUX_MAX_CHN_NUM         16

/* For CLOCK */
#define CLOCK_MAX_CHN_NUM        16

/*For TextEnc*/
#define TENC_MAX_CHN_NUM        16


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __IPCLINUX_PLAT_DEFINES_H__ */

