/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mm_comm_aenc.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/12
  Last Modified :
  Description   : common struct definition for AENC
  Function List :
  History       :
******************************************************************************/

#ifndef __IPCLINUX_COMM_AENC_H__
#define __IPCLINUX_COMM_AENC_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "plat_type.h"
#include "plat_errno.h"

#include "mm_common.h"
//#include "mm_comm_audio.h"


/* invlalid channel ID */
#define ERR_AENC_INVALID_CHNID     DEF_ERR(MOD_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define ERR_AENC_ILLEGAL_PARAM     DEF_ERR(MOD_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define ERR_AENC_EXIST             DEF_ERR(MOD_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* channel exists */
#define ERR_AENC_UNEXIST           DEF_ERR(MOD_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define ERR_AENC_NULL_PTR          DEF_ERR(MOD_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define ERR_AENC_NOT_CONFIG        DEF_ERR(MOD_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define ERR_AENC_NOT_SUPPORT       DEF_ERR(MOD_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define ERR_AENC_NOT_PERM          DEF_ERR(MOD_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define ERR_AENC_NOMEM             DEF_ERR(MOD_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define ERR_AENC_NOBUF             DEF_ERR(MOD_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define ERR_AENC_BUF_EMPTY         DEF_ERR(MOD_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define ERR_AENC_BUF_FULL          DEF_ERR(MOD_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define ERR_AENC_SYS_NOTREADY      DEF_ERR(MOD_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* system is busy*/
#define ERR_AENC_BUSY              DEF_ERR(MOD_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/* component state is same as user wanted */
#define ERR_AENC_SAMESTATE         DEF_ERR(MOD_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_SAMESTATE)
/* component state is transit to invalid state */
#define ERR_AENC_INVALIDSTATE      DEF_ERR(MOD_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALIDSTATE)
/* component current state can't transit to destination state */
#define ERR_AENC_INCORRECT_STATE_TRANSITION DEF_ERR(MOD_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_TRANSITION)
/* Attempting a command that is not allowed during the present state. */
#define ERR_AENC_INCORRECT_STATE_OPERATION DEF_ERR(MOD_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_OPERATION)


//typedef struct PCM_ATTR_S
//{
//    int sampleRate;
//    int bitsPerSample;
//    int channelCnt;
//    int frameSize;  // pcm data size in period_time
//}PCM_ATTR_S;

typedef struct AENC_CHN_STAT_S
{
    int mValidPcmSize;     /*left to be encoded pcm size: bytes*/
    int mTotalPcmBufSize;  /*total pcm buf size: bytes*/
    int mLeftBsNodes;      /*unused out_buf bs nodes number*/
    int mTotalBsNodes;     /*total out_buf bs node number*/
}AENC_CHN_STAT_S;

typedef struct AENC_ATTR_S
{
    PAYLOAD_TYPE_E Type;                         /*the type of payload*/
    int sampleRate;
    int channels;
    int bitRate;
    int bitsPerSample;
    int attachAACHeader; // attach header for every frame, effect only when audio codec is AAC
}AENC_ATTR_S;

typedef struct AENC_CHN_ATTR_S
{
    AENC_ATTR_S AeAttr;                           /*the attribution of audio encoder*/
}AENC_CHN_ATTR_S;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __IPCLINUX_COMM_AENC_H__ */
