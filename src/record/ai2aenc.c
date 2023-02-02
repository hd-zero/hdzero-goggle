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
#define LOG_TAG "ai2aenc"
#include <log/log.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "ai2aenc.h"

static void ai2aenc_configAiAttr(Ai2Aenc_t* aa, AiParams_t* param)
{
    memset(&aa->aiAttr, 0, sizeof(AIO_ATTR_S));

    aa->aiAttr.u32ChnCnt = param->channels;
    aa->aiAttr.enSamplerate = (AUDIO_SAMPLE_RATE_E)param->sampleRate;
    aa->aiAttr.enBitwidth = (AUDIO_BIT_WIDTH_E)(param->bitsPerSample/8-1);
}

static void ai2aenc_configAencAttr(Ai2Aenc_t* aa, AiParams_t* aiParam, AencParams_t* aeParam)
{
    memset(&aa->aeAttr, 0, sizeof(AENC_ATTR_S));

    aa->aeAttr.sampleRate = aiParam->sampleRate;
    aa->aeAttr.channels = aiParam->channels;
    aa->aeAttr.bitRate = aeParam->bitRate;             // usful when codec G726
    aa->aeAttr.bitsPerSample = aiParam->bitsPerSample;
    aa->aeAttr.attachAACHeader = 1;     // ADTS
    aa->aeAttr.Type = aeParam->codecType;
}

static ERRORTYPE MPPCallbackWrapper(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData)
{
    //Ai2Aenc_t *aa = (Ai2Aenc_t *)cookie;

    if (pChn->mModId == MOD_ID_AI)
    {
        switch (event)
        {
        default:
            break;
        }
    }
    else if (pChn->mModId == MOD_ID_AENC)
    {
        switch (event)
        {
        default:
            break;
        }
    }

    return SUCCESS;
}

static ERRORTYPE ai2aenc_createAencChn(Ai2Aenc_t* aa, AiParams_t* aiParam, AencParams_t* aeParam)
{
    ERRORTYPE ret;
    BOOL nSuccessFlag = FALSE;
    AENC_CHN aeChn = 0;

    ai2aenc_configAencAttr(aa, aiParam, aeParam);
    aa->aeChn = 0;
    while (aeChn < AENC_MAX_CHN_NUM)
    {
        ret = AW_MPI_AENC_CreateChn(aeChn, (AENC_CHN_ATTR_S*)&aa->aeAttr);
        if (SUCCESS == ret)
        {
            nSuccessFlag = TRUE;
            LOGD("create aenc channel[%d] success!", aeChn);
            break;
        }
        else if(ERR_AENC_EXIST == ret)
        {
            LOGD("aenc channel[%d] exist, find next!", aeChn);
            aeChn++;
        }
        else
        {
            LOGD("create aenc channel[%d] ret[0x%x], find next!", aeChn, ret);
            aeChn++;
        }
    }

    if(FALSE == nSuccessFlag)
    {
        aa->aeChn = MM_INVALID_CHN;
        LOGE("fatal error! create aenc channel fail!");
        return FAILURE;
    }
    aa->aeChn = aeChn;

    MPPCallbackInfo cbInfo;
    cbInfo.cookie = (void*)aa;
    cbInfo.callback = MPPCallbackWrapper;//NULL;

    AW_MPI_AENC_RegisterCallback(aa->aeChn, &cbInfo);
    return SUCCESS;
}

static ERRORTYPE ai2aenc_createAiChn(Ai2Aenc_t* aa, AiParams_t* aiParam)
{
    ERRORTYPE ret;
    BOOL bSuccessFlag = FALSE;

    aa->aiDev = aiParam->aiDev;
    ai2aenc_configAiAttr(aa, aiParam);
    AW_MPI_AI_SetPubAttr(aa->aiDev, &aa->aiAttr);
    //AW_MPI_AI_Enable(aa->aiDev);

    AI_CHN aiChn = 0;
    aa->aiChn = 0;
    while (aiChn < AIO_MAX_CHN_NUM)
    {
        ret = AW_MPI_AI_CreateChn(aa->aiDev, aiChn);
        if (SUCCESS == ret)
        {
            bSuccessFlag = TRUE;
            LOGD("create ai channel[%d] success!", aiChn);
            break;
        }
        else if (ERR_AI_EXIST == ret)
        {
            LOGD("ai channel[%d] exist, find next!", aiChn);
            aiChn++;
        }
        else
        {
            LOGD("create ai channel[%d] ret[0x%x], find next!", aiChn, ret);
            aiChn++;
        }
    }

    if (FALSE == bSuccessFlag)
    {
        aa->aiChn = MM_INVALID_CHN;
        LOGE("fatal error! create ai channel fail!");
        return FAILURE;
    }
    aa->aiChn = aiChn;

    MPPCallbackInfo cbInfo;
    cbInfo.cookie = (void*)aa;
    cbInfo.callback = NULL;
    AW_MPI_AI_RegisterCallback(aa->aiDev, aa->aiChn, &cbInfo);
    return SUCCESS;
}

static void *ai2aenc_frameProc(void *pThreadData)
{
    AUDIO_STREAM_S stream;
    Ai2Aenc_t *aa = (Ai2Aenc_t *)pThreadData;

    LOGD("aenc thread: dev[%d] chn[%d] ae[%d]", aa->aiDev, aa->aiChn, aa->aeChn);

    while (!aa->bExit)
    {
        if (SUCCESS == AW_MPI_AENC_GetStream(aa->aeChn, &stream, 100/*0*/))
        {
            //LOGD("get one stream with size: [%d]", stream.mLen);
            if( aa->cbOnFrame != NULL)
            {
                aa->cbOnFrame(aa, stream.pStream, stream.mLen, stream.mTimeStamp, aa->contextOfOnFrame);
            }
            AW_MPI_AENC_ReleaseStream(aa->aeChn, &stream);
        }
        else {
            usleep(1000);
        }
    }

    LOGD("aenc thread exit");

    return NULL;
}

Ai2Aenc_t* ai2aenc_initSys(CB_onAencFrame cbOnFrame, void* context)
{
    Ai2Aenc_t* aa = (Ai2Aenc_t*)malloc(sizeof(Ai2Aenc_t));
    if( aa == NULL )
    {
        return NULL;
    }

    memset(aa, 0, sizeof(Ai2Aenc_t));
    aa->aiDev = 0;
    aa->aiChn = MM_INVALID_CHN;
    aa->aeChn = MM_INVALID_CHN;
    aa->cbOnFrame = cbOnFrame;
    aa->contextOfOnFrame = context;

    return aa;
}

void ai2aenc_deinitSys(Ai2Aenc_t* aa)
{
    if (aa->aiChn >= 0)
    {
        AW_MPI_AI_ResetChn(aa->aiDev, aa->aiChn);
        AW_MPI_AI_DestroyChn(aa->aiDev, aa->aiChn);
        AW_MPI_AI_Disable(aa->aiDev);
    }

    if (aa->aeChn >= 0)
    {
        AW_MPI_AENC_ResetChn(aa->aeChn);
        AW_MPI_AENC_DestroyChn(aa->aeChn);
    }

    free(aa);

    LOGD("done");
}

ERRORTYPE ai2aenc_prepare(Ai2Aenc_t* aa, AiParams_t* aiParams, AencParams_t* aeParams)
{
    ERRORTYPE ret = ai2aenc_createAiChn(aa, aiParams);
    if (ret < 0)
    {
        LOGE("create ai chn fail");
        return ret;
    }

    ret = ai2aenc_createAencChn(aa, aiParams, aeParams);
    if (ret < 0)
    {
        LOGE("create ae chn fail");
        return ret;
    }

    if (aa->aiChn >= 0)
    {
        LOGD("bind ai & aenc");
        MPP_CHN_S AiChn = {MOD_ID_AI, aa->aiDev, aa->aiChn};
        MPP_CHN_S AencChn = {MOD_ID_AENC, 0, aa->aeChn};
        ret = AW_MPI_SYS_Bind(&AiChn, &AencChn);
    }

    return ret;
}

ERRORTYPE ai2aenc_start(Ai2Aenc_t* aa)
{
    ERRORTYPE ret = SUCCESS;

    LOGD("start");

    //start transe
    ret = AW_MPI_AI_EnableChn(aa->aiDev, aa->aiChn);
    if (ret != SUCCESS)
    {
        LOGE("AI enable error: %d", ret);
        return ret;
    }

    if (aa->aeChn >= 0)
    {
        ret = AW_MPI_AENC_StartRecvPcm(aa->aeChn);
        if( ret != SUCCESS ) {
            LOGE("start recv pic error: %d", ret);
            return ret;
        }

        aa->bExit = false;
        pthread_create(&aa->threadId, NULL, ai2aenc_frameProc, aa);
        LOGD("aenc pthread: dev[%d] chn[%d] aeChn[%d]", aa->aiDev, aa->aiChn, aa->aeChn);
    }

    return ret;
}

ERRORTYPE ai2aenc_stop(Ai2Aenc_t* aa, bool disableDev)
{
    LOGD("stop");

    aa->bExit = true;
    if( aa->threadId > 0 ) {
        pthread_join(aa->threadId, NULL);
        aa->threadId = 0;
    }

    //stop
    if (aa->aiChn >= 0)
    {
        AW_MPI_AI_DisableChn(aa->aiDev, aa->aiChn);
    }

    if (aa->aeChn >= 0)
    {
        LOGD("stop aenc");
        AW_MPI_AENC_StopRecvPcm(aa->aeChn);
    }

    if (aa->aiChn >= 0)
    {
        AW_MPI_AI_ResetChn(aa->aiDev, aa->aiChn);
        AW_MPI_AI_DestroyChn(aa->aiDev, aa->aiChn);
        if(disableDev)
        {
            AW_MPI_AI_Disable(aa->aiDev);
        }
        aa->aiDev = MM_INVALID_DEV;
        aa->aiChn = MM_INVALID_CHN;
    }

    if (aa->aeChn >= 0)
    {
        AW_MPI_AENC_ResetChn(aa->aeChn);
        AW_MPI_AENC_DestroyChn(aa->aeChn);
        aa->aeChn = MM_INVALID_CHN;
    }

    LOGD("done");

    return SUCCESS;
}

#define  ADTS_HEADER_SIZE (7)

typedef struct
{
    int write_adts;
    int objecttype;
    int sample_rate_index;
    int channel_conf;
} ADTSContext;

int aac_decode_extradata(ADTSContext *adts, unsigned char *pbuf, int bufsize)
{
    int aot, aotext, samfreindex;
    int channelconfig;
    unsigned char *p = pbuf;
    if (!adts || !pbuf || bufsize<2)
    {
        return -1;
    }
    aot = (p[0]>>3)&0x1f;
    if (aot == 31)
    {
        aotext = (p[0]<<3 | (p[1]>>5)) & 0x3f;
        aot = 32 + aotext;
        samfreindex = (p[1]>>1) & 0x0f;
        if (samfreindex == 0x0f)
        {
            channelconfig = ((p[4]<<3) | (p[5]>>5)) & 0x0f;
        }
        else
        {
            channelconfig = ((p[1]<<3)|(p[2]>>5)) & 0x0f;
        }
    }
    else
    {
        samfreindex = ((p[0]<<1)|p[1]>>7) & 0x0f;
        if (samfreindex == 0x0f)
        {
            channelconfig = (p[4]>>3) & 0x0f;
        }
        else
        {
            channelconfig = (p[1]>>3) & 0x0f;
        }
    }
#ifdef AOT_PROFILE_CTRL
    if (aot < 2) aot = 2;
#endif
    adts->objecttype = aot-1;
    adts->sample_rate_index = samfreindex;
    adts->channel_conf = channelconfig;
    adts->write_adts = 1;
    return 0;
}

int aac_set_adts_head(ADTSContext *acfg, uint8_t *buf, int size)
{
    uint8_t byte;
    if (size < ADTS_HEADER_SIZE)
    {
        return -1;
    }
    buf[0] = 0xff;
    buf[1] = 0xf1;
    byte = 0;
    byte |= (acfg->objecttype & 0x03) << 6;
    byte |= (acfg->sample_rate_index & 0x0f) << 2;
    byte |= (acfg->channel_conf & 0x07) >> 2;
    buf[2] = byte;
    byte = 0;
    byte |= (acfg->channel_conf & 0x07) << 6;
    byte |= (ADTS_HEADER_SIZE + size) >> 11;
    buf[3] = byte;
    byte = 0;
    byte |= (ADTS_HEADER_SIZE + size) >> 3;
    buf[4] = byte;
    byte = 0;
    byte |= ((ADTS_HEADER_SIZE + size) & 0x7) << 5;
    byte |= (0x7ff >> 6) & 0x1f;
    buf[5] = byte;
    byte = 0;
    byte |= (0x7ff & 0x3f) << 2;
    buf[6] = byte;
    return 0;
}

#define ADTS_HEADER__LEN 7

const int sampling_frequencies[] = {
    96000,  // 0x0
    88200,  // 0x1
    64000,  // 0x2
    48000,  // 0x3
    44100,  // 0x4
    32000,  // 0x5
    24000,  // 0x6
    22050,  // 0x7
    16000,  // 0x8
    12000,  // 0x9
    11025,  // 0xa
    8000   // 0xb
    // 0xc d e f是保留的
};

int adts_header(char * const p_adts_header, const int data_length,
                const int profile, const int samplerate,
                const int channels)
{
    int sampling_frequency_index = 3; // 默认使用48000hz
    int adtsLen = data_length + 7;

    int frequencies_size = sizeof(sampling_frequencies) / sizeof(sampling_frequencies[0]);
    int i = 0;
    for(i = 0; i < frequencies_size; i++)
    {
        if(sampling_frequencies[i] == samplerate)
        {
            sampling_frequency_index = i;
            break;
        }
    }
    if(i >= frequencies_size)
    {
        printf("unsupport samplerate:%d\n", samplerate);
        return -1;
    }

    p_adts_header[0] = 0xff;         //syncword:0xfff                          高8bits
    p_adts_header[1] = 0xf0;         //syncword:0xfff                          低4bits
    p_adts_header[1] |= (0 << 3);    //MPEG Version:0 for MPEG-4,1 for MPEG-2  1bit
    p_adts_header[1] |= (0 << 1);    //Layer:0                                 2bits
    p_adts_header[1] |= 1;           //protection absent:1                     1bit

    p_adts_header[2] = (profile) << 6;            //profile:profile               2bits
    p_adts_header[2] |= (sampling_frequency_index & 0x0f) << 2; //sampling frequency index:sampling_frequency_index  4bits
    p_adts_header[2] |= (0 << 1);             //private bit:0                   1bit
    p_adts_header[2] |= (channels & 0x04) >> 2; //channel configuration:channels  高1bit

    p_adts_header[3] = (channels & 0x03) << 6; //channel configuration:channels 低2bits
    p_adts_header[3] |= (0 << 5);               //original：0                1bit
    p_adts_header[3] |= (0 << 4);               //home：0                    1bit
    p_adts_header[3] |= (0 << 3);               //copyright id bit：0        1bit
    p_adts_header[3] |= (0 << 2);               //copyright id start：0      1bit
    p_adts_header[3] |= ((adtsLen & 0x1800) >> 11);           //frame length：value   高2bits

    p_adts_header[4] = (uint8_t)((adtsLen & 0x7f8) >> 3);     //frame length:value    中间8bits
    p_adts_header[5] = (uint8_t)((adtsLen & 0x7) << 5);       //frame length:value    低3bits
    p_adts_header[5] |= 0x1f;                                 //buffer fullness:0x7ff 高5bits
    p_adts_header[6] = 0xfc;      //11111100      //buffer fullness:0x7ff 低6bits
    // number_of_raw_data_blocks_in_frame：
    //    表示ADTS帧中有number_of_raw_data_blocks_in_frame + 1个AAC原始帧。

    return 0;
}

void ai2aenc_getAacADTS(uint8_t* aacData, int len)
{

}
