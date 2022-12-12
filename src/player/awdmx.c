//#define LOG_NDEBUG 0
#define LOG_TAG "awdmx"
#include <plat_log.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include <mpi_demux.h>
#include <mpi_vdec.h>
#include <mpi_clock.h>
#include <mpi_sys.h>
#include "awdmx.h"

static ERRORTYPE MPPCallbackWrapper(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData)
{
    AwdmxContext_t *dmxCtx = (AwdmxContext_t *)cookie;

    if (pChn->mModId == MOD_ID_DEMUX) {
        switch (event) {
        case MPP_EVENT_NOTIFY_EOF:
            printf("\n--------------demux to end of file\n");
            if (dmxCtx->vdecChn >= 0) {
                AW_MPI_VDEC_SetStreamEof(dmxCtx->vdecChn, 1);
            }
            dmxCtx->bEof = true;
            break;

        default:
            break;
        }
    }

    return SUCCESS;
}

static void awdmx_configDmxChnAttr(AwdmxContext_t *dmxCtx, DEMUX_CHN_ATTR_S* dmxChnAttr)
{
    dmxChnAttr->mStreamType = STREAMTYPE_LOCALFILE;
    dmxChnAttr->mSourceType = SOURCETYPE_FD;
    dmxChnAttr->mSourceUrl = NULL;
    dmxChnAttr->mFd = dmxCtx->srcFd;
    dmxChnAttr->mDemuxDisableTrack = DEMUX_DISABLE_SUBTITLE_TRACK|DEMUX_DISABLE_AUDIO_TRACK;
}

static ERRORTYPE awdmx_createDemuxChn(AwdmxContext_t *dmxCtx)
{
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
            alogd("create demux channel[%d] success!", dmxCtx->dmxChn);
            break;
        } else if (ERR_DEMUX_EXIST == ret) {
            alogd("demux channel[%d] is exist, find next!", dmxCtx->dmxChn);
            dmxCtx->dmxChn++;
        } else {
            alogd("create demux channel[%d] ret[0x%x]!", dmxCtx->dmxChn, ret);
            break;
        }
    }

    if (FALSE == nSuccessFlag) {
        dmxCtx->dmxChn = MM_INVALID_CHN;
        aloge("fatal error! create demux channel fail!");
        return ret;
    } else {
        MPPCallbackInfo cbInfo;
        cbInfo.cookie = (void*)dmxCtx;
        cbInfo.callback = (MPPCallbackFuncType)&MPPCallbackWrapper;
        AW_MPI_DEMUX_RegisterCallback(dmxCtx->dmxChn, &cbInfo);
        return SUCCESS;
    }
}

AwdmxContext_t* awdmx_open(char* sFile)
{
    AwdmxContext_t *dmxCtx = (AwdmxContext_t*)malloc(sizeof(AwdmxContext_t));
    ERRORTYPE ret;
    DEMUX_MEDIA_INFO_S DemuxMediaInfo = {0};

    if( dmxCtx == NULL ) {
        aloge("out of memory");
        return NULL;
    }

    memset(dmxCtx, 0, sizeof(AwdmxContext_t));
    dmxCtx->dmxChn = MM_INVALID_CHN;
    dmxCtx->vdecChn = MM_INVALID_CHN;
    dmxCtx->bEof = false;
    dmxCtx->videoNum = 0;

    alogd("open file");
    strcpy(dmxCtx->srcFile, sFile);
    dmxCtx->srcFd = open(sFile, O_RDONLY);
    if(dmxCtx->srcFd < 0) {
        aloge("open file failed: %s", strerror(errno));
        goto failed;
    }

    ret = awdmx_createDemuxChn(dmxCtx);
    if ( ret != SUCCESS ) {
        aloge("create demux chn fail");
        goto failed;
    }

    ret = AW_MPI_DEMUX_GetMediaInfo(dmxCtx->dmxChn, &DemuxMediaInfo);
    if ( ret != SUCCESS ) {
        aloge("fatal error! get media info fail!");
        goto failed;
    }

    if ((DemuxMediaInfo.mVideoNum >0 && DemuxMediaInfo.mVideoIndex >= DemuxMediaInfo.mVideoNum)
        || (DemuxMediaInfo.mAudioNum >0 && DemuxMediaInfo.mAudioIndex >= DemuxMediaInfo.mAudioNum)
        || (DemuxMediaInfo.mSubtitleNum >0 && DemuxMediaInfo.mSubtitleIndex >= DemuxMediaInfo.mSubtitleNum)) {
        alogd("fatal error, trackIndex wrong! [%d][%d],[%d][%d],[%d][%d]",
              DemuxMediaInfo.mVideoNum, DemuxMediaInfo.mVideoIndex, DemuxMediaInfo.mAudioNum, DemuxMediaInfo.mAudioIndex, DemuxMediaInfo.mSubtitleNum, DemuxMediaInfo.mSubtitleIndex);
        goto failed;
    }

    dmxCtx->videoNum = DemuxMediaInfo.mVideoNum;
    dmxCtx->width = DemuxMediaInfo.mVideoStreamInfo[0].mWidth;
    dmxCtx->height = DemuxMediaInfo.mVideoStreamInfo[0].mHeight;
    dmxCtx->codecType = DemuxMediaInfo.mVideoStreamInfo[0].mCodecType;
    dmxCtx->msDuration = DemuxMediaInfo.mDuration;
    alogd("stream info %dx%d", DemuxMediaInfo.mVideoStreamInfo[0].mWidth, DemuxMediaInfo.mVideoStreamInfo[0].mHeight);

    return dmxCtx;

failed:
    awdmx_close(dmxCtx);
    return NULL;
}

void awdmx_close(AwdmxContext_t* dmxCtx)
{
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

    close(dmxCtx->srcFd);
    dmxCtx->srcFd = -1;
    dmxCtx->videoNum = 0;
    dmxCtx->bEof = false;
    free(dmxCtx);
}

ERRORTYPE awdmx_bindVdecAndClock(AwdmxContext_t* dmxCtx, VDEC_CHN vdecChn, CLOCK_CHN clkChn)
{
    ERRORTYPE ret = SUCCESS;

    if (dmxCtx->videoNum >0) {
        dmxCtx->vdecChn = vdecChn;
        dmxCtx->clkChn = clkChn;

        MPP_CHN_S DmxChn = {MOD_ID_DEMUX, 0, dmxCtx->dmxChn};
        MPP_CHN_S VdecChn = {MOD_ID_VDEC, 0, dmxCtx->vdecChn};
        MPP_CHN_S ClockChn = {MOD_ID_CLOCK, 0, dmxCtx->clkChn};

        ret = AW_MPI_SYS_Bind(&DmxChn, &VdecChn);
        alogd("bind demux %d & vdec %d: %x", dmxCtx->dmxChn, dmxCtx->vdecChn, ret);

        ret = AW_MPI_SYS_Bind(&ClockChn, &DmxChn);
        alogd("bind demux %d & clock %d: %x", dmxCtx->dmxChn, dmxCtx->clkChn, ret);
    }

    return ret;
}

ERRORTYPE awdmx_unbindVdecAndClock(AwdmxContext_t* dmxCtx, VDEC_CHN vdecChn, CLOCK_CHN clkChn)
{
    ERRORTYPE ret = SUCCESS;

    if (dmxCtx->videoNum >0) {
        dmxCtx->vdecChn = vdecChn;
        dmxCtx->clkChn = clkChn;

        MPP_CHN_S DmxChn = {MOD_ID_DEMUX, 0, dmxCtx->dmxChn};
        MPP_CHN_S VdecChn = {MOD_ID_VDEC, 0, dmxCtx->vdecChn};
        MPP_CHN_S ClockChn = {MOD_ID_CLOCK, 0, dmxCtx->clkChn};

        ret = AW_MPI_SYS_UnBind(&DmxChn, &VdecChn);
        alogd("unbind demux %d & vdec %d: %x", dmxCtx->dmxChn, dmxCtx->vdecChn, ret);

        ret = AW_MPI_SYS_UnBind(&ClockChn, &DmxChn);
        alogd("unbind demux %d & clock %d: %x", dmxCtx->dmxChn, dmxCtx->clkChn, ret);
    }

    return ret;
}

ERRORTYPE awdmx_start(AwdmxContext_t *dmxCtx)
{
    ERRORTYPE ret = SUCCESS;

    if (dmxCtx->dmxChn >= 0) {
        ret = AW_MPI_DEMUX_Start(dmxCtx->dmxChn);
        alogd("start: %x", ret);
    }

    dmxCtx->bEof = false;

    return ret;
}

ERRORTYPE awdmx_pause(AwdmxContext_t *dmxCtx)
{
    ERRORTYPE ret = SUCCESS;

    if (dmxCtx->dmxChn >= 0) {
        ret = AW_MPI_DEMUX_Pause(dmxCtx->dmxChn);
        alogd("pause: %x", ret);
    }

    return ret;
}

ERRORTYPE awdmx_stop(AwdmxContext_t *dmxCtx)
{
    ERRORTYPE ret = SUCCESS;

    if (dmxCtx->dmxChn >= 0) {
        ret = AW_MPI_DEMUX_Stop(dmxCtx->dmxChn);
        alogd("stop: %x", ret);
    }

    return ret;
}

ERRORTYPE awdmx_seekTo(AwdmxContext_t *dmxCtx, int seekTime)
{
    ERRORTYPE ret = SUCCESS;

    if (dmxCtx->dmxChn >= 0) {
        ret = AW_MPI_DEMUX_Seek(dmxCtx->dmxChn, seekTime);
        alogd("seek to %d: %x", seekTime, ret);
        if( ret == SUCCESS ) {
            dmxCtx->seekTime = seekTime;
        }
    }

    return ret;
}

bool awdmx_isEOF(AwdmxContext_t *dmxCtx)
{
    return dmxCtx->bEof;
}
