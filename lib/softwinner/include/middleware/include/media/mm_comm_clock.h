/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mm_comm_clock.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/04/20
  Last Modified :
  Description   : common struct definition for VDEC
  Function List :
  History       :
******************************************************************************/


#ifndef  __IPCLINUX_COMM_CLOCK_H__
#define  __IPCLINUX_COMM_CLOCK_H__

#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "mm_common.h"

#ifdef __cplusplus
extern "C"{
#endif /* End of #ifdef __cplusplus */

typedef struct CLOCK_CHN_ATTR_S
{
    int nWaitMask;          /**< Mask of CLOCK_COMP_PORT_INDEX values. */
}CLOCK_CHN_ATTR_S;

/* invlalid channel ID */
#define ERR_CLOCK_INVALID_CHNID     DEF_ERR(MOD_ID_CLOCK, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define ERR_CLOCK_ILLEGAL_PARAM     DEF_ERR(MOD_ID_CLOCK, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define ERR_CLOCK_EXIST             DEF_ERR(MOD_ID_CLOCK, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* using a NULL point */
#define ERR_CLOCK_NULL_PTR          DEF_ERR(MOD_ID_CLOCK, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define ERR_CLOCK_NOT_CONFIG        DEF_ERR(MOD_ID_CLOCK, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define ERR_CLOCK_NOT_SURPPORT      DEF_ERR(MOD_ID_CLOCK, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define ERR_CLOCK_NOT_PERM          DEF_ERR(MOD_ID_CLOCK, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* the channle is not existed  */
#define ERR_CLOCK_UNEXIST           DEF_ERR(MOD_ID_CLOCK, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* failure caused by malloc memory */
#define ERR_CLOCK_NOMEM             DEF_ERR(MOD_ID_CLOCK, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define ERR_CLOCK_NOBUF             DEF_ERR(MOD_ID_CLOCK, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define ERR_CLOCK_BUF_EMPTY         DEF_ERR(MOD_ID_CLOCK, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define ERR_CLOCK_BUF_FULL          DEF_ERR(MOD_ID_CLOCK, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define ERR_CLOCK_SYS_NOTREADY      DEF_ERR(MOD_ID_CLOCK, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/*system busy*/
#define ERR_CLOCK_BUSY			  DEF_ERR(MOD_ID_CLOCK, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/* component state is same as user wanted */
#define ERR_CLOCK_SAMESTATE         DEF_ERR(MOD_ID_CLOCK, EN_ERR_LEVEL_ERROR, EN_ERR_SAMESTATE)
/* component state is transit to invalid state */
#define ERR_CLOCK_INVALIDSTATE      DEF_ERR(MOD_ID_CLOCK, EN_ERR_LEVEL_ERROR, EN_ERR_INVALIDSTATE)
/* component current state can't transit to destination state */
#define ERR_CLOCK_INCORRECT_STATE_TRANSITION DEF_ERR(MOD_ID_CLOCK, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_TRANSITION)
/* Attempting a command that is not allowed during the present state. */
#define ERR_CLOCK_INCORRECT_STATE_OPERATION DEF_ERR(MOD_ID_CLOCK, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_OPERATION)

/* bad address,  eg. used for copy_from_user & copy_to_user   */
#define ERR_CLOCK_BADADDR			  DEF_ERR(MOD_ID_CLOCK, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef  __IPCLINUX_COMM_CLOCK_H__ */

