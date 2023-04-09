#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "confparser.h"
#include "minIni.h"

#define SEC_LIVE        "live"
#define SEC_H264_LIVE   "h264_live"
#define SEC_H265_LIVE   "h265_live"
#define SEC_VENC_LIVE   "venc_live"

#define KEY_H265        "h265"
#define KEY_NAME        "name"
#define KEY_PORT        "port"
#define KEY_MULITCAST   "multicast"

#define sizearray(a)  (sizeof(a) / sizeof((a)[0]))
#define check_set(v, min, max)  ( ((v)<(min)) ? (min) : (((v)>(max)) ? (max) : (v)) )

void conf_loadLiveParams(LiveContext_t* liveCtx)
{
    char* confFile = liveCtx->confFile;
    long lValue = 0;
    char* sSection = SEC_VENC_LIVE;

    lValue = ini_getbool(sSection, KEY_H265, LIVE_vencH265, confFile);
    liveCtx->h265 = (lValue > 0);

    char sTemp[MAX_streamNameLEN+1];
    sSection = SEC_LIVE;

    lValue = ini_gets(sSection, KEY_NAME, LIVE_streamNAME, sTemp, sizearray(sTemp), confFile);
    if( lValue > 0 ) {
        strncpy(liveCtx->streamName, sTemp, MAX_streamNameLEN);
    }

    lValue = ini_getl(sSection, KEY_PORT, LIVE_rtspPORT, confFile);
    liveCtx->port = (uint16_t)lValue;

    lValue = ini_getbool(sSection, KEY_MULITCAST, LIVE_rtspMULTICAST, confFile);
    liveCtx->IsMulticast = (lValue > 0);
}
