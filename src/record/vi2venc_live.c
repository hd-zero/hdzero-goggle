/******************************************************************************
  Copyright (C), 2001-2017, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : sample_virvi2venc.c
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2017/1/5
  Last Modified :
  Description   : mpp component implement
  Function List :
  History       :
******************************************************************************/

//#define LOG_NDEBUG 0
#define LOG_TAG "vi2enc_live"
#include <log/log.h>
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vi2venc.h"
#include "spspps_patch.h"

Vi2Venc_t* vi2live_initSys(CB_onFrame onFrame, void* context)
{
    Vi2Venc_t* vv = (Vi2Venc_t*)malloc( sizeof(Vi2Venc_t) );

    if( vv == NULL ) {
        LOGE("sys init: out of memory");
        return NULL;
    }

    memset( vv, 0, sizeof(Vi2Venc_t) );

    vv->cbOnFrame = onFrame;
    vv->contextOfOnFrame = context;
    vv->bExit = false;
    vv->viDev = MM_INVALID_DEV;
    vv->viChn = MM_INVALID_CHN;
    vv->veChn = MM_INVALID_CHN;
    vv->ispDev = MM_INVALID_DEV;

#if(VENC_spsppsPATCH)
    vv->spsppsBuff = malloc(VENC_spsppsLEN);
#endif

    return vv;
}

void vi2live_deinitSys(Vi2Venc_t* vv)
{
    LOGD("begin");

    if( vv->spsppsBuff != NULL ) {
        free(vv->spsppsBuff);
    }

    free(vv);

    LOGD("done");
}

ERRORTYPE vi2live_started(Vi2Venc_t* vv)
{
    return ( vv->threadId > 0 );
}
