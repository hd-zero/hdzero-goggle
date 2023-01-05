#include "awdmx.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// #define LOG_NDEBUG 0
#define LOG_TAG "awdmx"
#include <log/log.h>

#include <ClockCompPortIndex.h>
#include <mpi_clock.h>
#include <mpi_demux.h>
#include <mpi_sys.h>
#include <mpi_vdec.h>

static ERRORTYPE MPPCallbackWrapper(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData) {
    AwdmxContext_t *dmxCtx = (AwdmxContext_t *)cookie;

    if (pChn->mModId == MOD_ID_DEMUX) {
        switch (event) {
        case MPP_EVENT_NOTIFY_EOF:
            LOGD("demux to end of file");
            dmxCtx->bEof = true;
            if (dmxCtx->cbOnEof != NULL) {
                dmxCtx->cbOnEof(dmxCtx->cbOnEofContext);
            }
            break;

        default:
            break;
        }
    }

    return SUCCESS;
}

static void awdmx_configDmxChnAttr(AwdmxContext_t *dmxCtx, DEMUX_CHN_ATTR_S *dmxChnAttr) {
    dmxChnAttr->mStreamType = STREAMTYPE_LOCALFILE;
    dmxChnAttr->mSourceType = SOURCETYPE_FD;
    dmxChnAttr->mSourceUrl = NULL;
    dmxChnAttr->mFd = dmxCtx->srcFd;
    dmxChnAttr->mDemuxDisableTrack = DEMUX_DISABLE_SUBTITLE_TRACK;
}

static ERRORTYPE awdmx_createClockChn(AwdmxContext_t *dmxCtx) {
    ERRORTYPE ret;
    BOOL bSuccessFlag = FALSE;

    CLOCK_CHN_ATTR_S clkChnAttr;
    CLOCK_CHN clkChn = 0;

    dmxCtx->clkChn = 0;
    clkChnAttr.nWaitMask = 0;

    if (dmxCtx->videoNum > 0) {
        clkChnAttr.nWaitMask |= 1 << CLOCK_PORT_INDEX_VIDEO;
    }

    if (dmxCtx->audioNum > 0) {
        clkChnAttr.nWaitMask |= 1 << CLOCK_PORT_INDEX_AUDIO;
    }

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
        dmxCtx->clkChn = MM_INVALID_CHN;
        LOGE("fatal error! create clock channel fail!");
        return FAILURE;
    }
    dmxCtx->clkChn = clkChn;

    return SUCCESS;
}

static ERRORTYPE awdmx_createDemuxChn(AwdmxContext_t *dmxCtx) {
    ERRORTYPE ret;
    BOOL nSuccessFlag = FALSE;
    DEMUX_CHN_ATTR_S dmxChnAttr;

    memset(&dmxChnAttr, 0, sizeof(dmxChnAttr));
    awdmx_configDmxChnAttr(dmxCtx, &dmxChnAttr);

    dmxCtx->dmxChn = 0;
    while (dmxCtx->dmxChn < DEMUX_MAX_CHN_NUM) {
        ret = AW_MPI_DEMUX_CreateChn(dmxCtx->dmxChn, &dmxChnAttr);
        if (SUCCESS == ret) {
            nSuccessFlag = TRUE;
            LOGD("create demux channel[%d] success!", dmxCtx->dmxChn);
            break;
        } else if (ERR_DEMUX_EXIST == ret) {
            LOGD("demux channel[%d] is exist, find next!", dmxCtx->dmxChn);
            dmxCtx->dmxChn++;
        } else {
            LOGD("create demux channel[%d] ret[0x%x]!", dmxCtx->dmxChn, ret);
            break;
        }
    }

    if (FALSE == nSuccessFlag) {
        dmxCtx->dmxChn = MM_INVALID_CHN;
        LOGE("fatal error! create demux channel fail!");
        return ret;
    } else {
        MPPCallbackInfo cbInfo;
        cbInfo.cookie = (void *)dmxCtx;
        cbInfo.callback = (MPPCallbackFuncType)&MPPCallbackWrapper;
        AW_MPI_DEMUX_RegisterCallback(dmxCtx->dmxChn, &cbInfo);
        return SUCCESS;
    }
}

AwdmxContext_t *awdmx_open(char *sFile, CB_onDmxEof cbOnEof, void *context) {
    AwdmxContext_t *dmxCtx = (AwdmxContext_t *)malloc(sizeof(AwdmxContext_t));
    ERRORTYPE ret;
    int nIndex = 0;
    DEMUX_MEDIA_INFO_S DemuxMediaInfo = {0};

    if (dmxCtx == NULL) {
        LOGE("out of memory");
        return NULL;
    }

    memset(dmxCtx, 0, sizeof(AwdmxContext_t));
    dmxCtx->dmxChn = MM_INVALID_CHN;
    dmxCtx->clkChn = MM_INVALID_CHN;
    dmxCtx->bEof = false;
    dmxCtx->videoNum = 0;
    dmxCtx->audioNum = 0;
    dmxCtx->cbOnEof = cbOnEof;
    dmxCtx->cbOnEofContext = context;

    LOGD("open file");
    strcpy(dmxCtx->srcFile, sFile);
    dmxCtx->srcFd = open(sFile, O_RDONLY);
    if (dmxCtx->srcFd < 0) {
        LOGE("open file failed: %s", strerror(errno));
        goto failed;
    }

    ret = awdmx_createDemuxChn(dmxCtx);
    if (ret != SUCCESS) {
        LOGE("create demux chn fail");
        goto failed;
    }

    ret = AW_MPI_DEMUX_GetMediaInfo(dmxCtx->dmxChn, &DemuxMediaInfo);
    if (ret != SUCCESS) {
        LOGE("fatal error! get media info fail!");
        goto failed;
    }

    if ((DemuxMediaInfo.mVideoNum > 0 && DemuxMediaInfo.mVideoIndex >= DemuxMediaInfo.mVideoNum) ||
        (DemuxMediaInfo.mAudioNum > 0 && DemuxMediaInfo.mAudioIndex >= DemuxMediaInfo.mAudioNum) ||
        (DemuxMediaInfo.mSubtitleNum > 0 && DemuxMediaInfo.mSubtitleIndex >= DemuxMediaInfo.mSubtitleNum)) {
        LOGD("fatal error, trackIndex wrong! [%d][%d],[%d][%d],[%d][%d]",
             DemuxMediaInfo.mVideoNum, DemuxMediaInfo.mVideoIndex, DemuxMediaInfo.mAudioNum, DemuxMediaInfo.mAudioIndex, DemuxMediaInfo.mSubtitleNum, DemuxMediaInfo.mSubtitleIndex);
        goto failed;
    }

    nIndex = DemuxMediaInfo.mVideoIndex;
    dmxCtx->videoNum = DemuxMediaInfo.mVideoNum;
    dmxCtx->width = DemuxMediaInfo.mVideoStreamInfo[nIndex].mWidth;
    dmxCtx->height = DemuxMediaInfo.mVideoStreamInfo[nIndex].mHeight;
    dmxCtx->codecType = DemuxMediaInfo.mVideoStreamInfo[nIndex].mCodecType;
    dmxCtx->msDuration = DemuxMediaInfo.mDuration;
    LOGD("stream info %dx%d", DemuxMediaInfo.mVideoStreamInfo[nIndex].mWidth, DemuxMediaInfo.mVideoStreamInfo[nIndex].mHeight);

    if (DemuxMediaInfo.mAudioNum > 0) {
        nIndex = DemuxMediaInfo.mAudioIndex;
        dmxCtx->audioNum = DemuxMediaInfo.mAudioNum;
        dmxCtx->aCodecType = DemuxMediaInfo.mAudioStreamInfo[nIndex].mCodecType;
        dmxCtx->channels = DemuxMediaInfo.mAudioStreamInfo[nIndex].mChannelNum;
        dmxCtx->bitsPerSample = DemuxMediaInfo.mAudioStreamInfo[nIndex].mBitsPerSample;
        dmxCtx->sampleRate = DemuxMediaInfo.mAudioStreamInfo[nIndex].mSampleRate;
        LOGD("stream info %dHz,%dch,%dbits,codec=%d", dmxCtx->sampleRate,
             dmxCtx->channels,
             dmxCtx->bitsPerSample,
             dmxCtx->aCodecType);

        if (dmxCtx->sampleRate == 0) {
            dmxCtx->channels = AUDIO_defChannels;
            dmxCtx->bitsPerSample = AUDIO_defSampleBits;
            dmxCtx->sampleRate = AUDIO_defSampleRate;
            LOGD("stream info filled: %dHz,%dch,%dbits,codec=%d", dmxCtx->sampleRate,
                 dmxCtx->channels,
                 dmxCtx->bitsPerSample,
                 dmxCtx->aCodecType);
        }
    }

    ret = awdmx_createClockChn(dmxCtx);
    if (ret != SUCCESS) {
        LOGE("create clock chn fail");
        goto failed;
    }

    MPP_CHN_S DmxChn = {MOD_ID_DEMUX, 0, dmxCtx->dmxChn};
    MPP_CHN_S ClockChn = {MOD_ID_CLOCK, 0, dmxCtx->clkChn};

    ret = AW_MPI_SYS_Bind(&ClockChn, &DmxChn);
    LOGD("bind demux %d & clock %d: %x", dmxCtx->dmxChn, dmxCtx->clkChn, ret);

    return dmxCtx;

failed:
    awdmx_close(dmxCtx);
    return NULL;
}

void awdmx_close(AwdmxContext_t *dmxCtx) {
    if (dmxCtx == NULL) {
        return;
    }

    if (dmxCtx->srcFd < 0) {
        return;
    }

    if (dmxCtx->dmxChn >= 0) {
        AW_MPI_DEMUX_DestroyChn(dmxCtx->dmxChn);
        dmxCtx->dmxChn = MM_INVALID_CHN;
    }

    if (dmxCtx->clkChn >= 0) {
        AW_MPI_CLOCK_DestroyChn(dmxCtx->clkChn);
        dmxCtx->clkChn = MM_INVALID_CHN;
    }

    close(dmxCtx->srcFd);
    dmxCtx->srcFd = -1;
    dmxCtx->videoNum = 0;
    dmxCtx->bEof = false;
    free(dmxCtx);
}

ERRORTYPE awdmx_bindVdecAndClock(AwdmxContext_t *dmxCtx, VDEC_CHN vdecChn, CLOCK_CHN clkChn) {
    ERRORTYPE ret = SUCCESS;

    if (dmxCtx->videoNum > 0) {
        dmxCtx->clkChn = clkChn;

        MPP_CHN_S DmxChn = {MOD_ID_DEMUX, 0, dmxCtx->dmxChn};
        MPP_CHN_S VdecChn = {MOD_ID_VDEC, 0, vdecChn};
        MPP_CHN_S ClockChn = {MOD_ID_CLOCK, 0, dmxCtx->clkChn};

        ret = AW_MPI_SYS_Bind(&DmxChn, &VdecChn);
        LOGD("bind demux %d & vdec %d: %x", dmxCtx->dmxChn, vdecChn, ret);

        ret = AW_MPI_SYS_Bind(&ClockChn, &DmxChn);
        LOGD("bind demux %d & clock %d: %x", dmxCtx->dmxChn, dmxCtx->clkChn, ret);
    }

    return ret;
}

ERRORTYPE awdmx_unbindVdecAndClock(AwdmxContext_t *dmxCtx, VDEC_CHN vdecChn, CLOCK_CHN clkChn) {
    ERRORTYPE ret = SUCCESS;

    if (dmxCtx->videoNum > 0) {
        dmxCtx->clkChn = clkChn;

        MPP_CHN_S DmxChn = {MOD_ID_DEMUX, 0, dmxCtx->dmxChn};
        MPP_CHN_S VdecChn = {MOD_ID_VDEC, 0, vdecChn};
        MPP_CHN_S ClockChn = {MOD_ID_CLOCK, 0, dmxCtx->clkChn};

        ret = AW_MPI_SYS_UnBind(&DmxChn, &VdecChn);
        LOGD("unbind demux %d & vdec %d: %x", dmxCtx->dmxChn, vdecChn, ret);

        ret = AW_MPI_SYS_UnBind(&ClockChn, &DmxChn);
        LOGD("unbind demux %d & clock %d: %x", dmxCtx->dmxChn, dmxCtx->clkChn, ret);
    }

    return ret;
}

ERRORTYPE awdmx_start(AwdmxContext_t *dmxCtx) {
    ERRORTYPE ret = SUCCESS;

    if (dmxCtx->dmxChn >= 0) {
        ret = AW_MPI_DEMUX_Start(dmxCtx->dmxChn);
        LOGD("start: %x", ret);
    }

    if (dmxCtx->clkChn >= 0) {
        ret = AW_MPI_CLOCK_Start(dmxCtx->clkChn);
    }

    dmxCtx->bEof = false;

    return ret;
}

ERRORTYPE awdmx_pause(AwdmxContext_t *dmxCtx) {
    ERRORTYPE ret = SUCCESS;

    if (dmxCtx->dmxChn >= 0) {
        ret = AW_MPI_DEMUX_Pause(dmxCtx->dmxChn);
        LOGD("pause: %x", ret);

        if (ret == FAILURE) {
            ret = SUCCESS;
        }
    }

    if (dmxCtx->clkChn >= 0) {
        ret = AW_MPI_CLOCK_Pause(dmxCtx->clkChn);
    }
    if ((ret == ERR_CLOCK_INCORRECT_STATE_TRANSITION) && dmxCtx->bEof) {
        ret = SUCCESS;
    }

    return ret;
}

ERRORTYPE awdmx_stop(AwdmxContext_t *dmxCtx) {
    ERRORTYPE ret = SUCCESS;

    if (dmxCtx->dmxChn >= 0) {
        ret = AW_MPI_DEMUX_Stop(dmxCtx->dmxChn);
        LOGD("stop: %x", ret);

        if (ret == FAILURE) {
            ret = SUCCESS;
        }
    }

    if (dmxCtx->clkChn >= 0) {
        ret = AW_MPI_CLOCK_Stop(dmxCtx->clkChn);
    }

    return ret;
}

ERRORTYPE awdmx_seekTo(AwdmxContext_t *dmxCtx, int seekTime) {
    ERRORTYPE ret = SUCCESS;

    if (dmxCtx->dmxChn >= 0) {
        ret = AW_MPI_DEMUX_Seek(dmxCtx->dmxChn, seekTime);
        LOGD("seek to %d: %x", seekTime, ret);
        if (ret == SUCCESS) {
            dmxCtx->seekTime = seekTime;
        }
    }

    if (dmxCtx->clkChn >= 0) {
        AW_MPI_CLOCK_Seek(dmxCtx->clkChn);
    }

    return ret;
}

bool awdmx_isEOF(AwdmxContext_t *dmxCtx) {
    return dmxCtx->bEof;
}
