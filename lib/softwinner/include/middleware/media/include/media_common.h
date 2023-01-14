/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : media_common.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/03/15
  Last Modified :
  Description   : multimedia common function for internal use.
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_MEDIA_COMMON_H__
#define __IPCLINUX_MEDIA_COMMON_H__

// ref platform headers
#include "plat_errno.h"
#include "plat_type.h"

// media api headers to app

#include <mm_comm_aio.h>
//#include <mm_comm_demux.h>
#include <mm_comm_ise.h>
//#include <mm_comm_mux.h>
//#include <mm_comm_aenc.h>
//#include <mm_comm_adec.h>
#include <mm_comm_venc.h>
#include <mm_comm_video.h>
#include <mm_comm_uvc.h>
#include <mm_common.h>
//#include "mpi_venc.h"

// media internal common headers.
#include "vdecoder.h"
#include "vencoder.h"
#include "aencoder.h"
#include "adecoder.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct DataSection {
    unsigned char *mpData;
    unsigned int mLen;
} DataSection;

/*
 grp = 0;
 chn = 0,1,2,3;
 grp = 1;
 chn = 0,1,2,3;

 grp = 0;
 chn = 0,1,2,3;
 grp = 1;
 chn = 4,5,6,7;

 default :
 grp = 0; // must = 0;
 chn = 0, 1, 2, 3;
*/

typedef struct DynamicBuffer {
    char *mpBuffer;
    int mSize;
    struct list_head mList;
} DynamicBuffer;

typedef struct CedarXSeekPara {
    int seek_dir;   // 0:forward 1:backward
    int seek_time;  // unit:ms
    int seek_relative_time;
} CedarXSeekPara;

ERRORTYPE copy_MPP_CHN_S(MPP_CHN_S *pDst, MPP_CHN_S *pSrc);
ERRORTYPE copy_VENC_CHN_ATTR_S(VENC_CHN_ATTR_S *pDst, VENC_CHN_ATTR_S *pSrc);
ERRORTYPE copy_ISE_GRP_ATTR_S(ISE_GROUP_ATTR_S *pDst, ISE_GROUP_ATTR_S *pSrc);
ERRORTYPE copy_ISE_CHN_ATTR_S(ISE_CHN_ATTR_S *pDst, ISE_CHN_ATTR_S *pSrc);
ERRORTYPE copy_UVC_CHN_ATTR_S(UVC_CHN_ATTR_S *pDst, UVC_CHN_ATTR_S *pSrc);

VENC_CODEC_TYPE map_PAYLOAD_TYPE_E_to_VENC_CODEC_TYPE(PAYLOAD_TYPE_E nPayLoadType);
AUDIO_ENCODER_TYPE map_PAYLOAD_TYPE_E_to_AUDIO_ENCODER_TYPE(PAYLOAD_TYPE_E nPayLoadType);
enum EVIDEOCODECFORMAT map_PAYLOAD_TYPE_E_to_EVIDEOCODECFORMAT(PAYLOAD_TYPE_E nPayLoadType);
PAYLOAD_TYPE_E map_EVIDEOCODECFORMAT_to_PAYLOAD_TYPE_E(enum EVIDEOCODECFORMAT eCodecFormat);
PAYLOAD_TYPE_E map_EAUDIOCODECFORMAT_to_PAYLOAD_TYPE_E(enum EAUDIOCODECFORMAT eCodecFormat);

unsigned int GetBitRateFromVENC_CHN_ATTR_S(VENC_CHN_ATTR_S *pAttr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __IPCLINUX_MEDIA_COMMON_H__ */
