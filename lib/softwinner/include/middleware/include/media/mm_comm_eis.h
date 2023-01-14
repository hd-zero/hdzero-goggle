/*
 ******************************************************************************
 *
 * common_eis.h
 *
 * common_eis.h
 *
 * Copyright (c) 2018 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version        Author         Date           Description
 *
 *   1.0          huangbohan     2018/05/08     VIDEO STABLIZATION
 *
 *****************************************************************************
 */

#ifndef _AW_COMMON_EIS_H_
#define _AW_COMMON_EIS_H_

#include <stdbool.h>

/* platform types */
#include "mm_common.h"
#include <mm_comm_video.h>
#include "plat_errno.h"
#include <aw_type.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define MAX_EIS_CHN_NUM 2

#define EIS_ERR_PRIV_TIMEOUT (EN_ERR_BUTT + 1)
#define EIS_ERR_PRIV_NOTENABLE (EN_ERR_BUTT + 2)
#define EIS_ERR_PRIV_NOTDISABLE (EN_ERR_BUTT + 3)
#define EIS_ERR_PRIV_NORM_UNMATCH (EN_ERR_BUTT + 4)
#define EIS_ERR_PRIV_INVALID_PHYCHNID (EN_ERR_BUTT + 5)
#define EIS_ERR_PRIV_FAILED_NOTBIND (EN_ERR_BUTT + 6)
#define EIS_ERR_PRIV_FAILED_BINDED (EN_ERR_BUTT + 7)
#define EIS_ERR_PRIV_UNEXIST (EN_ERR_BUTT + 8)
#define EIS_ERR_PRIV_EXIST (EN_ERR_BUTT + 9)

#define ERR_EIS_INVALID_CHNID DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define ERR_EIS_INVALID_PARA DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define ERR_EIS_INVALID_NULL_PTR DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define ERR_EIS_FAILED_NOTCONFIG DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
#define ERR_EIS_NOT_SUPPORT DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define ERR_EIS_NOT_PERM DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define ERR_EIS_INVALID_DEVID DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define ERR_EIS_NOMEM DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define ERR_EIS_BUF_EMPTY DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
#define ERR_EIS_BUF_FULL DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
#define ERR_EIS_SYS_NOTREADY DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define ERR_EIS_BUSY DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

#define ERR_EIS_FAILED_NOTENABLE DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EIS_ERR_PRIV_NOTENABLE)
#define ERR_EIS_FAILED_NOTDISABLE DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EIS_ERR_PRIV_NOTDISABLE)
#define ERR_EIS_CFG_TIMEOUT DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EIS_ERR_PRIV_TIMEOUT)
#define ERR_EIS_NORM_UNMATCH DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EIS_ERR_PRIV_NORM_UNMATCH)
#define ERR_EIS_INVALID_PHYCHNID DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EIS_ERR_PRIV_INVALID_PHYCHNID)
#define ERR_EIS_FAILED_NOTBIND DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EIS_ERR_PRIV_FAILED_NOTBIND)
#define ERR_EIS_FAILED_BINDED DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EIS_ERR_PRIV_FAILED_BINDED)
#define ERR_EIS_UNEXIST DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EIS_ERR_PRIV_UNEXIST)
#define ERR_EIS_EXIST DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EIS_ERR_PRIV_EXIST)
#define ERR_EIS_SAMESTATE DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EN_ERR_SAMESTATE)
#define ERR_EIS_INVALIDSTATE DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALIDSTATE)
#define ERR_EIS_INCORRECT_STATE_TRANSITION DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_TRANSITION)
#define ERR_EIS_INCORRECT_STATE_OPERATION DEF_ERR(MOD_ID_EIS, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_OPERATION)

typedef enum awEIS_CHANNEL_PORT_DEFINE_E {
    /* No matter bind or unbind mode, we use this port to
    * accept video frames.
    */
	EIS_CHN_PORT_INDEX_VIDEO_IN = 0,
	/* Just in offline mode, we use this port to receive
	* gyro datas. data structure: x, y, z, timestamp
	*/
	EIS_CHN_PORT_INDEX_GYRO_IN,
	EIS_CHN_PORT_INDEX_OUT,
	EIS_CHN_PORT_INDEX_OUT_M,
	EIS_CHN_PORT_INDEX_OUT_S,
	EIS_CHN_MAX_PORTS,
} EIS_CHANNEL_PORT_DEFINE_E;

typedef enum awEIS_CONF_OPR_E {
	EIS_CHN_CONF_SET = 0,
	EIS_CHN_CONF_GET,
} EIS_CONF_OPR_E;

typedef enum awEIS_ALGO_MODE {
    EIS_ALGO_MODE_HW, /* Use hardware device to process EIS. */
    EIS_ALGO_MODE_SW, /* Specific for IRIDALibs' cpu EIS. */
    EIS_ALGO_MODE_BP, /* By pass mode, no EIS process, just ouput the origin videos and gyro datas. */
} EIS_ALGO_MODE;

typedef enum awEIS_OPR_MODE_E {
    EIS_OPR_1080P30,
    EIS_OPR_1080P60,
    EIS_OPR_VGA30,
    EIS_OPR_VGA60,
    EIS_OPR_4K30,
    EIS_OPR_2P7K30,
    EIS_OPR_DEBUG_RES,
} EIS_OPR_MODE_E;

typedef struct awEIS_KMAT_S {
    float KmatK1;
    float KmatK2;
    float KmatKx;
    float KmatKy;
} EIS_KMAT_S;

/* For video format attribution segment */
typedef struct awEIS_VIDEO_FMT_ATTR_S {
    /* video input and output resolutions */
    int iVideoInWidth;
    int iVideoInHeight;
    int iVideoInWidthStride;
    int iVideoInHeightStride;

    int iVideoOutWidth;
    int iVideoOutHeight;
    int iVideoFps;
    PIXEL_FORMAT_E eVideoFmt;
} EIS_VIDEO_FMT_ATTR_S;

/* For data process attribution segment */
typedef struct awEIS_DATA_PROC_ATTR_S {
    /* In low memory device, we may want to reduce memory size,
    * then set this value, then EIS component will return input
    * video frame immediately. BBBBBBBBBBBBBBBBBBBBBBut, it is dangerious,
    * if your input buffer pool is too small and the EIS process speed
    * is too slowly, then the video maybe overlaped. */
    bool bRetInFrmFast;
    /* This is the number of filter width, unit:frame number */
    bool bVerticalFlip; /* If the pic's Y axi(horizion) be fliped */
    bool bHorizFlip; /* If the pic's X axi(vertical) be fliped. */
    int iEisFilterWidth;
    int iInputBufNum;
    int iOutputBufBum;
    /* We use timestamp to sync gyro and video frames,
    * this value indicates that we will sync with: (gyro timestamp + iDelayTimeMs) = video timestamp
    *
    *           |------iDelayTimeMs-----|
    * Timeline--------------------------------------------------------->
    *           gyro id0                video id0
    */
    int iDelayTimeMs; /* default: 0 */
    unsigned int iSyncErrTolerance; /* Unit: ms */
} EIS_DATA_PROC_ATTR_S;

typedef struct awEIS_GYRO_DATA_ATTR_S {
    /*gyro sample frequency, default 1000, unit:HZ*/
    int iGyroFreq;
    /*how many gyro elems can be cached in gyro buffer pool,
    * 3axi: 1 elem = 16 Bytes; 6axi: 1 elem = 20 Bytes;
    * default to [200elems], it means we cache 200ms datas.
    * 1000 / iGyroFreq * iGyroPoolSize = Cache time length(ms)
    */
    int iGyroPoolSize;
    /* Gyro axis count, by default: 3(Angle x,y,z)
    * if set to 6, the Accel x,y,z will be included, too.
    */
    int iGyroAxiNum;
} EIS_GYRO_DATA_ATTR_S;

/* The whole attributions specfic for EIS channel.
* If you think it is too large, then use the samll subdivision attributions like above.
*/
typedef struct awEIS_ATTR_S {
    /**************************
    * gyro property settings
    ***************************/
    /*gyro sample frequency, default 1000, unit:HZ*/
    int iGyroFreq;
    /*how many gyro elems can be cached in gyro buffer pool,
    * 3axi: 1 elem = 16 Bytes; 6axi: 1 elem = 20 Bytes;
    * default to [200elems], it means we cache 200ms datas.
    * 1000 / iGyroFreq * iGyroPoolSize = Cache time length(ms)
    */
    int iGyroPoolSize;
    /* Gyro axis count, by default: 3(Angle x,y,z)
    * if set to 6, the Accel x,y,z will be included, too.
    */
    int iGyroAxiNum;

    /**************************
    * video property settings
    ***************************/
    /* video input and output resolutions */
    int iVideoInWidth;
    int iVideoInHeight;
    int iVideoInWidthStride;
    int iVideoInHeightStride;

    int iVideoOutWidth;
    int iVideoOutHeight;
    int iVideoFps;
    PIXEL_FORMAT_E eVideoFmt;

    /**************************
    * EIS property settings
    ***************************/
    /* In low memory device, we may want to reduce memory size,
    * then set this value, then EIS component will return input
    * video frame immediately. BBBBBBBBBBBBBBBBBBBBBBut, it is dangerious,
    * if your input buffer pool is too small and the EIS process speed
    * is too slowly, then the video maybe overlaped. */
    bool bRetInFrmFast;
    /* This is the number of filter width, unit:frame number */
    bool bVerticalFlip; /* If the pic's Y axi(horizion) be fliped */
    bool bHorizFlip; /* If the pic's X axi(vertical) be fliped. */
    int iEisFilterWidth;
    int iInputBufNum;
    int iOutputBufBum;
    /* We use timestamp to sync gyro and video frames,
    * this value indicates that we will sync with: (gyro timestamp + iDelayTimeMs) = video timestamp
    *
    *           |------iDelayTimeMs-----|
    * Timeline--------------------------------------------------------->
    *           gyro id0                video id0
    */
    int iDelayTimeMs; /* default: 0 */
    unsigned int iSyncErrTolerance; /* Unit: ms */

    /*
    * we got default k-mat values for imx317 in SDK.
    * but if you have other sensors, you should set
    * <use_custom_kmat=1>,and fill <antishake_kmat>.
    * the important thing is: you must be responsible for your own values.
    */
    bool bUseKmat;
    EIS_KMAT_S stEisKmat;
    EIS_OPR_MODE_E eOperationMode;

    /*
    * We may need simulation offline use offline datas.
    * if you need it, set this flag.
    */
    bool bSimuOffline;

    /* Use hardware EIS: then use <EIS_ALGO_MODE_HW>
    * use software EIS: then use <EIS_ALGO_MODE_SW>
    * use debug EIS: then use <EIS_ALGO_MODE_BP>
    */
    EIS_ALGO_MODE eEisAlgoMode;
    bool bSaveYUV; //If you want save the YUV file directly, then set it to 1.
    char *pBPDataSavePath; //The path you want to store your datas.
} EIS_ATTR_S;

typedef struct awEIS_GYRO_PACKET_S {
    uint64_t dTimeStamp; /* Unit: us */
    float fAccelVX;
    float fAccelVY;
    float fAccelVZ;
    float fAnglrVX;
    float fAnglrVY;
    float fAnglrVZ;
} EIS_GYRO_PACKET_S;

typedef struct awEIS_GYRO_PACKET_3AXI_S {
    uint64_t dTimeStamp;
    float fAnglrVX;
    float fAnglrVY;
    float fAnglrVZ;
} EIS_GYRO_PACKET_3AXI_S;

typedef struct awEIS_PARAMS_S {
    EIS_CHN mChn;
    EIS_ATTR_S *pstAttr;
    VIDEO_FRAME_INFO_S  *pstFrameInfo;
    AW_S32 s32MilliSec;
} EIS_PARAMS_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*_AW_COMMON_EIS_H_*/
