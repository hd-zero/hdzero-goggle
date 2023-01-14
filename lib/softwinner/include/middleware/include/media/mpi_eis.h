/*
 ******************************************************************************
 *
 * eis_api.h
 *
 * video stablization - eis_api.h module
 *
 * Copyright (c) 2018 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version        Author         Date           Description
 *
 * 1.0            huangbohan     2018/10/19     VIDEO EIS
 *
 *****************************************************************************
 */

#ifndef _AW_EIS_API_H_
#define _AW_EIS_API_H_

#include "mm_comm_eis.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define MPI_EIS_CHECK_CHN_VALID(Chn) \
if (Chn < 0 || Chn >= MAX_EIS_CHN_NUM){\
    aloge("fatal error! invalid EISChn[%d]!", Chn);\
    return ERR_EIS_INVALID_CHNID;\
}

/*
* Create channel informations, just software abstract
*/
AW_S32 AW_MPI_EIS_CreateChn(EIS_CHN EISChn, EIS_ATTR_S *pstAttr);
AW_S32 AW_MPI_EIS_DestroyChn(EIS_CHN EISChn);

/*
* Enable channel hardware devices. such as gyro device and eis device.
*/
AW_S32 AW_MPI_EIS_EnableChn(EIS_CHN EISChn);
AW_S32 AW_MPI_EIS_DisableChn(EIS_CHN EISChn);

/*
* Start receive and process video frames. The hardware will keep running.
*/
AW_S32 AW_MPI_EIS_StartChn(EIS_CHN EISChn);
AW_S32 AW_MPI_EIS_StopChn(EIS_CHN EISChn);

/*
* Maybe the hardware device will get into sleep, when it is resumed again,
* this funciton should be invoked, and all the old datas(gyro and video) will be droped.
* And maybe, the EIS hardware will be reset, too.
*@iSync: 1 - sync operation; 0 - async operation
*/
AW_S32 AW_MPI_EIS_FlushChn(EIS_CHN EISChn, int iSync);

/*
* If the componment is started and running, you can only set <iDelayTimeMs>,
* else, you can set all the parameters.
*/
AW_S32 AW_MPI_EIS_SetChnAttr(EIS_CHN EISChn, EIS_ATTR_S *pstAttr);
AW_S32 AW_MPI_EIS_GetChnAttr(EIS_CHN EISChn, EIS_ATTR_S *pstAttr);
/* Must be setting before component run.
*/
AW_S32 AW_MPI_EIS_SetVideoFmtAttr(EIS_CHN EISChn, EIS_VIDEO_FMT_ATTR_S *pstAttr);
AW_S32 AW_MPI_EIS_SetVideoDataAttr(EIS_CHN EISChn, EIS_DATA_PROC_ATTR_S *pstAttr);
AW_S32 AW_MPI_EIS_SetGyroDataAttr(EIS_CHN EISChn, EIS_GYRO_DATA_ATTR_S *pstAttr);
AW_S32 AW_MPI_EIS_SetKmatDataAttr(EIS_CHN EISChn, EIS_KMAT_S *pstAttr);
AW_S32 AW_MPI_EIS_SetAlgoModeAttr(EIS_CHN EISChn, EIS_ALGO_MODE *pstAttr);

AW_S32 AW_MPI_EIS_GetData(EIS_CHN EISChn, VIDEO_FRAME_INFO_S *pstFrameInfo, AW_S32 s32MilliSec);
AW_S32 AW_MPI_EIS_ReleaseData(EIS_CHN EISChn, VIDEO_FRAME_INFO_S *pstFrameInfo);
AW_S32 AW_MPI_EIS_SendPic(EIS_CHN EISChn, VIDEO_FRAME_INFO_S *pstFrameInfo, AW_S32 s32MilliSec);
AW_S32 AW_MPI_EIS_SendGyroPacket(EIS_CHN EISChn, EIS_GYRO_PACKET_S *pstGyroPacket, unsigned int uPktNum);

/*
* 1. Fisrt video frame done.
* 2. Sync error(sunch as too late to send video frames).
* 3. Flush channel successful.
* 4. ... ...
*/
ERRORTYPE AW_MPI_EIS_RegisterCallback(EIS_CHN EISChn, MPPCallbackInfo *pCallback);

/*
* Other Interface, probabily no use.
*/
AW_S32 AW_MPI_EIS_SetEISFreq(EIS_CHN EISChn, int nFreq); /* nFreq: MHz */
//AW_S32 AW_MPI_EIS_GetInfo(); /* get bind config */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /*_AW_EIS_API_H_*/

