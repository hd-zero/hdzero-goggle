/******************************************************************************
  Copyright (C), 2001-2017, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mm_comm_ise.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2017/1/5
  Last Modified :
  Description   : mpp component implement
  Function List :
  History       :
******************************************************************************/

#ifndef _MM_COMMON_ISE_H_
#define _MM_COMMON_ISE_H_

#include "mm_common.h"
#include "plat_errno.h"
#include "sunxi_camera_v2.h"
#include "mm_comm_vi.h"
// #include "ISE_common.h"
#include "ISE_lib_bi.h"
#include "ISE_lib_mo.h"
#include "ISE_lib_sti.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef void AW_VOID;

#define ISE_MAX_GRP_NUM 8
#define ISE_MAX_CHN_NUM 4

#define ISEMODE_ONE_FISHEYE 0x01
#define ISEMODE_TWO_FISHEYE 0x02
#define ISEMODE_TWO_ISE     0x03

typedef enum {
    HANDLE_BY_HARDWARE = 0x0001,
    HANDLE_BY_SOFT     = 0x0002
}DFISH_HANDLE_MODE;

typedef struct {
    int bgfg_intvl;         // 每隔多少个SLEEP_MS进行一次model
    int getbgd_intvl;       // 每隔多少帧获取背景图片
    int bgfg_sleep_ms;      // SC16队列非空时，sleep多少MS
}ISE_BGFG_PARA_STI;

typedef struct {
    ISE_CFG_PARA_BI			ise_cfg;
    DFISH_HANDLE_MODE       handle_mode;
} DFISH;

typedef struct {
    ISE_CFG_PARA_MO         ise_cfg;
} FISH;

typedef struct {
    ISE_CFG_PARA_STI        ise_cfg;
	ISE_PROCCFG_PARA_STI 	ise_proccfg;
	ISE_BGFG_PARA_STI       ise_bgfg;
} ISE;

typedef union {
    FISH    mFish;
    DFISH   mDFish;
    ISE     mIse;
} MODE_ATTR;

typedef struct {
    AW_U32 iseMode;  // one or two camera:ISEMODE_ONE_FISHEYE,ISEMODE_TWO_FISHEYE,ISEMODE_TWO_ISE
    PIXEL_FORMAT_E  mPixelFormat;
} ISE_GROUP_ATTR_S;

typedef struct {
    MODE_ATTR mode_attr;
    int buffer_num;
} ISE_CHN_ATTR_S;

typedef struct {
    ISE_GRP mIseGrp;
    ISE_CHN mIseChn;
    int nMilliSec;
    VIDEO_FRAME_INFO_S *frame;
} ISE_DATA_S;

typedef struct
{
    VIDEO_FRAME_INFO_S *pbuf0;
    VIDEO_FRAME_INFO_S *pbuf1;
} VI_FRAME_BUF_INFOS;

/* invlalid channel ID */
#define ERR_ISE_INVALID_CHNID DEF_ERR(MOD_ID_ISE, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define ERR_ISE_ILLEGAL_PARAM DEF_ERR(MOD_ID_ISE, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define ERR_ISE_EXIST DEF_ERR(MOD_ID_ISE, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* using a NULL point */
#define ERR_ISE_NULL_PTR DEF_ERR(MOD_ID_ISE, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing
 * attribute */
#define ERR_ISE_NOT_CONFIG DEF_ERR(MOD_ID_ISE, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define ERR_ISE_NOT_SUPPORT DEF_ERR(MOD_ID_ISE, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define ERR_ISE_NOT_PERM DEF_ERR(MOD_ID_ISE, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* the channle is not existed  */
#define ERR_ISE_UNEXIST DEF_ERR(MOD_ID_ISE, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* failure caused by malloc memory */
#define ERR_ISE_NOMEM DEF_ERR(MOD_ID_ISE, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define ERR_ISE_NOBUF DEF_ERR(MOD_ID_ISE, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* system is not ready,had not initialed or loaded*/
#define ERR_ISE_SYS_NOTREADY DEF_ERR(MOD_ID_ISE, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/*system busy*/
#define ERR_ISE_BUSY DEF_ERR(MOD_ID_ISE, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/* timeout */
#define ERR_ISE_TIMEOUT DEF_ERR(MOD_ID_ISE, EN_ERR_LEVEL_ERROR, EN_ERR_TIMEOUT)
/* component state is same as user wanted */
#define ERR_ISE_SAMESTATE DEF_ERR(MOD_ID_ISE, EN_ERR_LEVEL_ERROR, EN_ERR_SAMESTATE)
/* component state is transit to invalid state */
#define ERR_ISE_INVALIDSTATE DEF_ERR(MOD_ID_ISE, EN_ERR_LEVEL_ERROR, EN_ERR_INVALIDSTATE)
/* component current state can't transit to destination state */
#define ERR_ISE_INCORRECT_STATE_TRANSITION DEF_ERR(MOD_ID_ISE, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_TRANSITION)
/* Attempting a command that is not allowed during the present state. */
#define ERR_ISE_INCORRECT_STATE_OPERATION DEF_ERR(MOD_ID_ISE, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_OPERATION)
#define ERR_ISE_BUF_EMPTY DEF_ERR(MOD_ID_ISE, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* efuse check fail */
#define ERR_ISE_EFUSE_ERROR DEF_ERR(MOD_ID_ISE, EN_ERR_LEVEL_ERROR, EN_ERR_EFUSE_ERROR)
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*_MM_COMMON_ISE_H_*/
