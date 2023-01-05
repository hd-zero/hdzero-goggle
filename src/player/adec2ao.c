/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     :
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/11/4
  Last Modified :
  Description   :
  Function List :
  History       :
******************************************************************************/

#include "adec2ao.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cdx_list.h>

// #define LOG_NDEBUG 0
#define LOG_TAG "adec2ao"
#include <log/log.h>

static ERRORTYPE MPPCallbackWrapper(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData) {
    Adec2AoContext_t *aa = (Adec2AoContext_t *)cookie;

    if (pChn->mModId == MOD_ID_ADEC) {
        switch (event) {
        case MPP_EVENT_NOTIFY_EOF:
            LOGD("adec get EOF flag");
            if (aa->aoChn >= 0) {
                AW_MPI_AO_SetStreamEof(aa->aoDev, aa->aoChn, 1, 0);
            }
            aa->flagEOF |= ADEC2AO_adecEOF;
            break;
        default:
            break;
        }
    } else if (pChn->mModId == MOD_ID_AO) {
        switch (event) {
        case MPP_EVENT_NOTIFY_EOF:
            LOGD("ao get EOF flag");
            aa->flagEOF |= ADEC2AO_aoEOF;
            break;
        default:
            break;
        }
    }

    return SUCCESS;
}

static ERRORTYPE adec2ao_createClockChn(Adec2AoContext_t *aa) {
    ERRORTYPE ret;
    BOOL bSuccessFlag = FALSE;
    CLOCK_CHN_ATTR_S clkChnAttr;
    CLOCK_CHN clkChn = 0;

    aa->clkChn = 0;
    clkChnAttr.nWaitMask = 0;
    clkChnAttr.nWaitMask |= 1 << CLOCK_PORT_INDEX_AUDIO;
    while (clkChn < CLOCK_MAX_CHN_NUM) {
        ret = AW_MPI_CLOCK_CreateChn(clkChn, &clkChnAttr);
        if (SUCCESS == ret) {
            bSuccessFlag = TRUE;
            LOGD("create clock channel[%d] success!", clkChn);
            break;
        } else if (ERR_CLOCK_EXIST == ret) {
            LOGD("clock channel[%d] is exist, find next!", clkChn);
            clkChn++;
        } else {
            LOGD("create clock channel[%d] ret[0x%x]!", clkChn, ret);
            break;
        }
    }

    if (FALSE == bSuccessFlag) {
        aa->clkChn = MM_INVALID_CHN;
        LOGE("fatal error! create clock channel fail!");
        return FAILURE;
    }
    aa->clkChn = clkChn;

    return SUCCESS;
}

static ERRORTYPE adec2ao_configAdecChnAttr(ADEC_CHN_ATTR_S *adecChnAttr, AdecParams_t *params) {
    memset(adecChnAttr, 0, sizeof(ADEC_CHN_ATTR_S));
    adecChnAttr->mType = params->codecType;
    adecChnAttr->sampleRate = params->sampleRate;
    adecChnAttr->channels = params->channels;
    adecChnAttr->bitsPerSample = params->bitsPerSample;

    return SUCCESS;
}

static ERRORTYPE adec2ao_createAdecChn(Adec2AoContext_t *aa, AdecParams_t *params) {
    ERRORTYPE ret;
    BOOL nSuccessFlag = FALSE;
    ADEC_CHN_ATTR_S adecChnAttr;
    ADEC_CHN adecChn = 0;

    LOGD("try to get adec chn");
    adec2ao_configAdecChnAttr(&adecChnAttr, params);
    aa->adecChn = 0;

    while (adecChn < ADEC_MAX_CHN_NUM) {
        ret = AW_MPI_ADEC_CreateChn(adecChn, &adecChnAttr);
        if (SUCCESS == ret) {
            nSuccessFlag = TRUE;
            LOGD("create adec channel[%d] success!", adecChn);
            break;
        } else if (ERR_ADEC_EXIST == ret) {
            LOGD("adec channel[%d] is exist, find next!", adecChn);
            adecChn++;
        } else {
            LOGD("create adec channel[%d] ret[0x%x]!", adecChn, ret);
            break;
        }
    }

    if (FALSE == nSuccessFlag) {
        aa->adecChn = MM_INVALID_CHN;
        LOGE("fatal error! create adec channel fail!");
        return FAILURE;
    }
    aa->adecChn = adecChn;

    LOGD("add call back");
    MPPCallbackInfo cbInfo;
    cbInfo.cookie = (void *)aa;
    cbInfo.callback = (MPPCallbackFuncType)&MPPCallbackWrapper;
    AW_MPI_ADEC_RegisterCallback(aa->adecChn, &cbInfo);

    return SUCCESS;
}

static void adec2ao_config_AioAttr(AIO_ATTR_S *pAioAttr, AdecParams_t *params) {
    memset(pAioAttr, 0, sizeof(AIO_ATTR_S));
    pAioAttr->u32ChnCnt = params->channels;
    pAioAttr->enBitwidth = (AUDIO_BIT_WIDTH_E)(params->bitsPerSample / 8 - 1);
    pAioAttr->enSamplerate = (AUDIO_SAMPLE_RATE_E)params->sampleRate;
}

static ERRORTYPE adec2ao_createAOChn(Adec2AoContext_t *aa, AdecParams_t *params) {
    ERRORTYPE ret;
    AIO_ATTR_S aioAttr;

    aa->aoDev = 0;

    adec2ao_config_AioAttr(&aioAttr, params);
    AW_MPI_AO_SetPubAttr(aa->aoDev, &aioAttr);

    // enable audio_hw_ao
    ret = AW_MPI_AO_Enable(aa->aoDev);

    // create ao channel
    BOOL bSuccessFlag = FALSE;
    AO_CHN aoChn = 0;
    aa->aoChn = 0;

    while (aoChn < AIO_MAX_CHN_NUM) {
        ret = AW_MPI_AO_EnableChn(aa->aoDev, aoChn);
        if (SUCCESS == ret) {
            bSuccessFlag = TRUE;
            LOGD("create ao channel[%d] success!", aoChn);
            break;
        } else if (ERR_AO_EXIST == ret) {
            LOGD("ao channel[%d] exist, find next!", aoChn);
            aoChn++;
        } else if (ERR_AO_NOT_ENABLED == ret) {
            LOGE("audio_hw_ao not started!");
            break;
        } else {
            LOGE("create ao channel[%d] fail! ret[0x%x]!", aoChn, ret);
            break;
        }
    }
    if (FALSE == bSuccessFlag) {
        aa->aoChn = MM_INVALID_CHN;
        LOGE("fatal error! create ao channel fail!");
        ret = FAILURE;
    }
    aa->aoChn = aoChn;

    LOGD("add call back");
    MPPCallbackInfo cbInfo;
    cbInfo.cookie = (void *)aa;
    cbInfo.callback = (MPPCallbackFuncType)&MPPCallbackWrapper;
    AW_MPI_AO_RegisterCallback(aa->aoDev, aa->aoChn, &cbInfo);

    return SUCCESS;
}

static void adec2ao_destroy(Adec2AoContext_t *aa) {
    LOGD("begin");

    if (aa->aoDev >= 0) {
        if (aa->aoChn >= 0) {
            AW_MPI_AO_DisableChn(aa->aoDev, aa->aoChn);
            aa->aoChn = MM_INVALID_CHN;
        }

        AW_MPI_AO_Disable(aa->aoDev);
        aa->aoDev = MM_INVALID_DEV;
    }

    if (aa->adecChn >= 0) {
        AW_MPI_ADEC_DestroyChn(aa->adecChn);
        aa->adecChn = MM_INVALID_CHN;
    }

    LOGD("done");
}

Adec2AoContext_t *adec2ao_initSys(void) {
    Adec2AoContext_t *aa = (Adec2AoContext_t *)malloc(sizeof(Adec2AoContext_t));
    if (aa == NULL) {
        return NULL;
    }

    memset(aa, 0, sizeof(Adec2AoContext_t));
    aa->aoDev = MM_INVALID_DEV;
    aa->aoChn = MM_INVALID_CHN;
    aa->adecChn = MM_INVALID_DEV;
    aa->clkChn = MM_INVALID_DEV;

    return aa;
}

void adec2ao_deinitSys(Adec2AoContext_t *aa) {
    adec2ao_destroy(aa);
    free(aa);

    LOGD("done");
}

ERRORTYPE adec2ao_prepare(Adec2AoContext_t *aa, AdecParams_t *params) {
    ERRORTYPE ret;

    aa->dmxChn = params->dmxChn;
    aa->clkChn = params->clkChn;

    ret = adec2ao_createAdecChn(aa, params);
    if (ret == SUCCESS) {
        LOGD("bind demux & adec");
        MPP_CHN_S DmxChn = {MOD_ID_DEMUX, 0, params->dmxChn};
        MPP_CHN_S AdecChn = {MOD_ID_ADEC, 0, aa->adecChn};
        AW_MPI_SYS_Bind(&DmxChn, &AdecChn);
    }

    ret = adec2ao_createAOChn(aa, params);
    if (ret == SUCCESS) {
        LOGD("bind adec & ao");
        MPP_CHN_S AdecChn = {MOD_ID_ADEC, 0, aa->adecChn};
        MPP_CHN_S AoChn = {MOD_ID_AO, 0, aa->aoChn};
        AW_MPI_SYS_Bind(&AdecChn, &AoChn);

        LOGD("bind clock & ao");
        MPP_CHN_S ClockChn = {MOD_ID_CLOCK, 0, aa->clkChn};
        AW_MPI_SYS_Bind(&ClockChn, &AoChn);
    }

    return ret;
}

ERRORTYPE adec2ao_start(Adec2AoContext_t *aa) {
    LOGD("start stream");
    // AW_MPI_CLOCK_Start(aa->clkChn);

    if (aa->aoChn >= 0) {
        AW_MPI_AO_StartChn(aa->aoDev, aa->aoChn);
    }
    if (aa->adecChn >= 0) {
        AW_MPI_ADEC_StartRecvStream(aa->adecChn);
    }

    aa->flagEOF = 0;

    return SUCCESS;
}

ERRORTYPE adec2ao_pause(Adec2AoContext_t *aa) {
    if (aa->adecChn >= 0) {
        AW_MPI_ADEC_Pause(aa->adecChn);
    }
    if (aa->aoChn >= 0) {
        AW_MPI_AO_PauseChn(aa->aoDev, aa->aoChn);
    }

    return SUCCESS;
}

ERRORTYPE adec2ao_stop(Adec2AoContext_t *aa) {
    if (aa->adecChn >= 0) {
        AW_MPI_ADEC_StopRecvStream(aa->adecChn);
    }
    if (aa->aoChn >= 0) {
        AW_MPI_AO_StopChn(aa->aoDev, aa->aoChn);
    }

    return SUCCESS;
}

ERRORTYPE adec2ao_seekTo(Adec2AoContext_t *aa) {
    ERRORTYPE ret = SUCCESS;

    LOGD("seek to");

    if (aa->adecChn >= 0) {
        ret = AW_MPI_ADEC_SetStreamEof(aa->adecChn, 0);
    }
    if (aa->aoChn >= 0) {
        AW_MPI_AO_SetStreamEof(aa->aoDev, aa->aoChn, 0, FALSE);
    }

    if (aa->adecChn >= 0) {
        ret = AW_MPI_ADEC_Seek(aa->adecChn);
    }
    if (aa->aoChn >= 0) {
        ret = AW_MPI_AO_Seek(aa->aoDev, aa->aoChn);
    }

    return ret;
}

ERRORTYPE adec2ao_setAdecEof(Adec2AoContext_t *aa) {
    ERRORTYPE ret = SUCCESS;

    if (!(aa->flagEOF & ADEC2AO_adecEOF_N)) {
        if (aa->adecChn >= 0) {
            ret = AW_MPI_ADEC_SetStreamEof(aa->adecChn, TRUE);
        }

        aa->flagEOF |= ADEC2AO_adecEOF_N;
    }

    return ret;
}

ERRORTYPE adec2ao_setAoEof(Adec2AoContext_t *aa) {
    ERRORTYPE ret = SUCCESS;

    if (!(aa->flagEOF & ADEC2AO_aoEOF_N)) {
        if (aa->aoChn >= 0) {
            ret = AW_MPI_AO_SetStreamEof(aa->aoDev, aa->aoChn, TRUE, FALSE);
        }

        aa->flagEOF |= ADEC2AO_aoEOF_N;
    }

    return ret;
}

ERRORTYPE adec2ao_checkEof(Adec2AoContext_t *aa) {
    ERRORTYPE ret = SUCCESS;

    ret = adec2ao_setAdecEof(aa);

    if (aa->flagEOF & ADEC2AO_adecEOF) {
        adec2ao_setAoEof(aa);
    }

    return ret;
}

bool adec2ao_isEOF(Adec2AoContext_t *aa) {
    return aa->flagEOF & ADEC2AO_aoEOF;
}
