/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mm_comm_adec.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/12
  Last Modified :
  Description   : common struct definition for ADEC
  Function List :
  History       :
******************************************************************************/

#ifndef __IPCLINUX_COMM_ADEC_H__
#define __IPCLINUX_COMM_ADEC_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "plat_type.h"
#include "plat_errno.h"

#include "mm_common.h"
//#include "mm_comm_audio.h"


/* invlalid channel ID */
#define ERR_ADEC_INVALID_CHNID     DEF_ERR(MOD_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define ERR_ADEC_ILLEGAL_PARAM     DEF_ERR(MOD_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define ERR_ADEC_EXIST             DEF_ERR(MOD_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* channel exists */
#define ERR_ADEC_UNEXIST           DEF_ERR(MOD_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define ERR_ADEC_NULL_PTR          DEF_ERR(MOD_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define ERR_ADEC_NOT_CONFIG        DEF_ERR(MOD_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define ERR_ADEC_NOT_SUPPORT       DEF_ERR(MOD_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define ERR_ADEC_NOT_PERM          DEF_ERR(MOD_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define ERR_ADEC_NOMEM             DEF_ERR(MOD_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define ERR_ADEC_NOBUF             DEF_ERR(MOD_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define ERR_ADEC_BUF_EMPTY         DEF_ERR(MOD_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define ERR_ADEC_BUF_FULL          DEF_ERR(MOD_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define ERR_ADEC_SYS_NOTREADY      DEF_ERR(MOD_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* system is busy*/
#define ERR_ADEC_BUSY              DEF_ERR(MOD_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/* component state is same as user wanted */
#define ERR_ADEC_SAMESTATE         DEF_ERR(MOD_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_SAMESTATE)
/* component state is transit to invalid state */
#define ERR_ADEC_INVALIDSTATE      DEF_ERR(MOD_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALIDSTATE)
/* component current state can't transit to destination state */
#define ERR_ADEC_INCORRECT_STATE_TRANSITION DEF_ERR(MOD_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_TRANSITION)
/* Attempting a command that is not allowed during the present state. */
#define ERR_ADEC_INCORRECT_STATE_OPERATION DEF_ERR(MOD_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_OPERATION)



typedef struct ADEC_CHN_ATTR_S
{
    PAYLOAD_TYPE_E mType;                         /*the type of payload*/
    int sampleRate;
    int channels;
    int bitRate;        // not use
    int bitsPerSample;
    int attachAACHeader; // attach header for every frame, effect only when audio codec is AAC
}ADEC_CHN_ATTR_S;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __IPCLINUX_COMM_ADEC_H__ */
