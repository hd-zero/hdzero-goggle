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

//#define LOG_NDEBUG 0
#define LOG_TAG "ai2ao"
#include <log/log.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <cdx_list.h>
#include "ai2ao.h"

static void ai2ao_configAioAttr(Ai2Ao_t* aa, AiParams_t* param)
{
    memset(&aa->aioAttr, 0, sizeof(AIO_ATTR_S));

    aa->aioAttr.u32ChnCnt = param->channels;
    aa->aioAttr.enSamplerate = (AUDIO_SAMPLE_RATE_E)param->sampleRate;
    aa->aioAttr.enBitwidth = (AUDIO_BIT_WIDTH_E)(param->bitsPerSample/8-1);
}

static ERRORTYPE MPPCallbackAoDataListener(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData)
{
    Ai2Ao_t *aa = (Ai2Ao_t *)cookie;

    if (pChn->mModId == MOD_ID_AI)
    {
        switch (event)
        {
        default:
            break;
        }
    }
    else if (pChn->mModId == MOD_ID_AO)
    {
        int ret;
        pcmFrameNode *pNode;

        switch (event)
        {
        case MPP_EVENT_RELEASE_AUDIO_BUFFER:
            pthread_mutex_lock(&aa->pcmlock);
            pNode = list_first_entry(&aa->pcmUsingList, pcmFrameNode, mList);
            pthread_mutex_unlock(&aa->pcmlock);

            ret = AW_MPI_AI_ReleaseFrame(aa->aiDev, aa->aiChn, &pNode->frame, NULL);
            if (SUCCESS != ret) {
                LOGE("release frame to ai fail! ret: %#x", ret);
            } else {
                pthread_mutex_lock(&aa->pcmlock);
                list_move_tail(&pNode->mList, &aa->pcmIdleList);
                pthread_mutex_unlock(&aa->pcmlock);
            }
            break;
        default:
            break;
        }
    }

    return SUCCESS;
}

static ERRORTYPE ai2ao_createAioChn(Ai2Ao_t* aa, AiParams_t* aiParam)
{
    ERRORTYPE ret;
    BOOL bSuccessFlag = FALSE;

    aa->aoDev = 0;
    aa->aiDev = aiParam->aiDev;
    ai2ao_configAioAttr(aa, aiParam);
    AW_MPI_AI_SetPubAttr(aa->aiDev, &aa->aioAttr);
    AW_MPI_AO_SetPubAttr(aa->aoDev, &aa->aioAttr);
    AW_MPI_AI_Enable(aa->aiDev);
    AW_MPI_AO_Enable(aa->aoDev);

    AI_CHN aiChn = 0;
    aa->aiChn = 0;
    while(aiChn < AIO_MAX_CHN_NUM) {
        ret = AW_MPI_AI_CreateChn(aa->aiDev, aiChn);
        if(SUCCESS == ret) {
            bSuccessFlag = TRUE;
            LOGD("create ai channel[%d] success!", aiChn);
            break;
        } else if (ERR_AI_EXIST == ret) {
            LOGD("ai channel[%d] exist, find next!", aiChn);
            aiChn++;
        } else if(ERR_AI_NOT_ENABLED == ret) {
            LOGE("audio_hw_ai not started!");
            break;
        } else {
            LOGE("create ai channel[%d] fail! ret[0x%x]!", aiChn, ret);
            break;
        }
    }
    if (FALSE == bSuccessFlag)
    {
        aa->aiChn = MM_INVALID_CHN;
        LOGE("fatal error! create ai channel fail!");
        return FAILURE;
    }
    aa->aiChn = aiChn;

    AO_CHN aoChn = 0;
    bSuccessFlag = FALSE;
    aa->aoChn = 0;
    while(aoChn < AIO_MAX_CHN_NUM) {
        ret = AW_MPI_AO_EnableChn(aa->aoDev, aoChn);
        if(SUCCESS == ret) {
            bSuccessFlag = TRUE;
            LOGD("create ao channel[%d] success!", aoChn);
            break;
        } else if (ERR_AO_EXIST == ret) {
            LOGD("ao channel[%d] exist, find next!", aoChn);
            aoChn++;
        } else if(ERR_AO_NOT_ENABLED == ret) {
            LOGE("audio_hw_ao not started!");
            break;
        } else {
            LOGE("create ao channel[%d] fail! ret[0x%x]!", aoChn, ret);
            break;
        }
    }
    if (FALSE == bSuccessFlag)
    {
        aa->aoChn = MM_INVALID_CHN;
        LOGE("fatal error! create ao channel fail!");
        return FAILURE;
    }
    aa->aoChn = aoChn;

    CLOCK_CHN  clkChn = 0;
    bSuccessFlag = FALSE;
    aa->clkChnAttr.nWaitMask = 0;
    aa->clkChnAttr.nWaitMask |= 1<<CLOCK_PORT_INDEX_AUDIO;
    aa->clkChn = 0;
    while(clkChn < CLOCK_MAX_CHN_NUM) {
        ret = AW_MPI_CLOCK_CreateChn(clkChn, &aa->clkChnAttr);
        if(SUCCESS == ret) {
            bSuccessFlag = TRUE;
            LOGD("create clock channel[%d] success!", clkChn);
            break;
        } else if(ERR_CLOCK_EXIST == ret) {
            LOGD("clock channel[%d] is exist, find next!", clkChn);
            clkChn++;
        } else {
            LOGD("create clock channel[%d] ret[0x%x]!", clkChn, ret);
            break;
        }
    }
    if(FALSE == bSuccessFlag) {
        aa->clkChn = MM_INVALID_CHN;
        LOGE("fatal error! create clock channel fail!");
        return FAILURE;
    }
    aa->clkChn = clkChn;

    //register vo callback
    MPPCallbackInfo voCallback = {aa, MPPCallbackAoDataListener};
    AW_MPI_AO_RegisterCallback(aa->aoDev, aa->aoChn, &voCallback);

    return SUCCESS;
}

Ai2Ao_t* ai2ao_initSys(void)
{
    Ai2Ao_t* aa = (Ai2Ao_t*)malloc(sizeof(Ai2Ao_t));
    if( aa == NULL )
    {
        return NULL;
    }

    memset(aa, 0, sizeof(Ai2Ao_t));
    aa->aiDev = MM_INVALID_DEV;
    aa->aoDev = MM_INVALID_DEV;
    aa->aiChn = MM_INVALID_CHN;
    aa->aoChn = MM_INVALID_DEV;
    aa->clkChn= MM_INVALID_DEV;
    aa->tunnelMode = true;

    INIT_LIST_HEAD(&aa->pcmIdleList);
    INIT_LIST_HEAD(&aa->pcmUsingList);

    int audioFrameNum = 5;
    int i = 0;
    for (; i<audioFrameNum; i++) {
        pcmFrameNode *pPcmNode = malloc(sizeof(pcmFrameNode));
        //LOGD("malloc new pcmNode >>> addr:%p", pPcmNode);
        list_add_tail(&pPcmNode->mList, &aa->pcmIdleList);
    }

    pthread_mutex_init(&aa->pcmlock, NULL);

    return aa;
}

void ai2ao_deinitSys(Ai2Ao_t* aa)
{
    pcmFrameNode *pEntry, *pTmp;
    pthread_mutex_lock(&aa->pcmlock);
    list_for_each_entry_safe(pEntry, pTmp, &aa->pcmIdleList, mList) {
        //LOGD("delete PcmNode(%p) in IdleList!", pEntry);
        list_del(&pEntry->mList);
        free(pEntry);
    }
    list_for_each_entry_safe(pEntry, pTmp, &aa->pcmUsingList, mList) {
        //LOGD("delete PcmNode(%p) in UsingList!", pEntry);
        list_del(&pEntry->mList);
        free(pEntry);
    }
    pthread_mutex_unlock(&aa->pcmlock);
    pthread_mutex_destroy(&aa->pcmlock);

    free(aa);

    LOGD("done");
}

static void *ai2ao_frameProc(void *pThreadData)
{
    pcmFrameNode *pPcmNode = NULL;
    Ai2Ao_t *aa = (Ai2Ao_t *)pThreadData;
    int ret = 0;

    LOGD("aio thread: dev[%d] chn[%d]", aa->aoDev, aa->aoChn);

    while (!aa->bExit)
    {
        pthread_mutex_lock(&aa->pcmlock);

        if (list_empty(&aa->pcmIdleList)) {
            LOGE("why no pcm node?");
        } else {
            pPcmNode = list_first_entry(&aa->pcmIdleList, pcmFrameNode, mList);
            list_move_tail(&pPcmNode->mList, &aa->pcmUsingList);
        }
        pthread_mutex_unlock(&aa->pcmlock);

        ret = AW_MPI_AI_GetFrame(aa->aiDev, aa->aiChn, &pPcmNode->frame, NULL, -1);
        if (SUCCESS == ret) {
            AW_MPI_AO_SendFrame(aa->aoDev, aa->aoChn, &pPcmNode->frame, 0);
        } else {
            LOGE("get pcm from ai in block mode fail! ret: %#x", ret);
            break;
        }
    }

    LOGD("aio thread exit");

    return NULL;
}

ERRORTYPE ai2ao_prepare(Ai2Ao_t* aa, AiParams_t* aiParams)
{
    ERRORTYPE ret = SUCCESS;

    ret = ai2ao_createAioChn(aa, aiParams);
    if (ret < 0)
    {
        LOGE("create aio chn fail");
        return ret;
    }

    MPP_CHN_S AiChn = {MOD_ID_AI, aa->aiDev, aa->aiChn};
    MPP_CHN_S AoChn = {MOD_ID_AO, aa->aoDev, aa->aoChn};
    if(aa->tunnelMode)
    {
        LOGD("bind ai & ao");
        ret = AW_MPI_SYS_Bind(&AiChn, &AoChn);
    }

    //bind clock and ao
    MPP_CHN_S ClockChn = {MOD_ID_CLOCK, 0, aa->clkChn};
    ret = AW_MPI_SYS_Bind(&ClockChn, &AoChn);

    return ret;
}

ERRORTYPE ai2ao_start(Ai2Ao_t* aa, AiParams_t* aiParams)
{
    ERRORTYPE ret = SUCCESS;

    LOGD("start");

    if(aa->aoChn != MM_INVALID_CHN)
    {
        LOGD("playing");
        return ret;
    }

    ret = ai2ao_prepare(aa, aiParams);
    if( ret != SUCCESS )
    {
        return ret;
    }

    //start clk chn.
    ret = AW_MPI_CLOCK_Start(aa->clkChn);

    //start ai chn.
    ret = AW_MPI_AI_EnableChn(aa->aiDev, aa->aiChn);
    if (ret != SUCCESS)
    {
        LOGE("AI enable error: %d", ret);
        return ret;
    }

    //start ao chn.
    ret = AW_MPI_AO_StartChn(aa->aoDev, aa->aoChn);
    if((ret == SUCCESS) && !aa->tunnelMode)
    {
        aa->bExit = false;
        pthread_create(&aa->threadId, NULL, ai2ao_frameProc, aa);
        LOGD("aio pthread: dev[%d] chn[%d] aoChn[%d]", aa->aiDev, aa->aiChn, aa->aoChn);
    }

    return ret;
}

ERRORTYPE ai2ao_stop(Ai2Ao_t* aa, bool disableDev)
{
    LOGD("stop");

    if( aa->threadId > 0 ) {
        aa->bExit = true;
        pthread_join(aa->threadId, NULL);
        aa->threadId = 0;
    }

    //stop
    if (aa->aiChn >= 0)
    {
        AW_MPI_AI_DisableChn(aa->aiDev, aa->aiChn);
    }
    if (aa->clkChn >= 0)
    {
        AW_MPI_CLOCK_Stop(aa->clkChn);
    }
    if (aa->aoChn >= 0)
    {
        AW_MPI_AO_StopChn(aa->aoDev, aa->aoChn);
    }

     //stop and reset ao chn, destroy ao dev
    if (aa->aoChn >= 0)
    {
        //AW_MPI_AO_StopChn(aa->aoDev, aa->aoChn);
        AW_MPI_AO_DisableChn(aa->aoDev, aa->aoChn);
        aa->aoChn = MM_INVALID_CHN;
        aa->aoDev = MM_INVALID_DEV;
    }

    //stop, reset ai chn and destroy ai dev
    if (aa->aiChn >= 0)
    {
        //AW_MPI_AI_DisableChn(aa->aiDev, aa->aiChn);
        AW_MPI_AI_ResetChn(aa->aiDev, aa->aiChn);
        AW_MPI_AI_DestroyChn(aa->aiDev, aa->aiChn);
        if(disableDev)
        {
            AW_MPI_AI_Disable(aa->aiDev);
        }
        aa->aiDev = MM_INVALID_DEV;
        aa->aiChn = MM_INVALID_CHN;
    }

    // stop and destroy clock chn
    if (aa->clkChn >= 0)
    {
        //AW_MPI_CLOCK_Stop(aa->clkChn);
        AW_MPI_CLOCK_DestroyChn(aa->clkChn);
        aa->clkChn = MM_INVALID_CHN;
    }

    LOGD("done");

    return SUCCESS;
}

bool ai2ao_playing(Ai2Ao_t* aa)
{
    return (aa->clkChn > MM_INVALID_CHN);
}
