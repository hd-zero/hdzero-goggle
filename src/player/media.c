#include "media.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// #define LOG_NDEBUG 0
#define LOG_TAG "play"
#include <log/log.h>

#include "adec2ao.h"
#include "awdmx.h"
#include "gogglemsg.h"
#include "vdec2vo.h"
#include "version.h"

#ifdef EVB_DEBUG
#define PLAY_HDZERO 0
#else
#define PLAY_HDZERO 1
#endif

#if (PLAY_HDZERO)
#define VO_intfTYPE VO_INTF_BT1120
#define VO_intfSYNC VO_OUTPUT_NTSC
#define VO_WIDTH    1920
#define VO_HEIGHT   1080
#define VO_uiCHN    2
#else
#define VO_intfTYPE VO_INTF_LCD
#define VO_intfSYNC VO_OUTPUT_NTSC
#define VO_WIDTH    720
#define VO_HEIGHT   1280
#define VO_uiCHN    2
#endif

#define PLAY_statIDLE      0
#define PLAY_statOPENED    BIT(0)
#define PLAY_statSTARTED   BIT(1)
#define PLAY_statPAUSED    BIT(2)
#define PLAY_statCOMPLETED BIT(3)

#define PLAY_statSEEKING BIT(16)

typedef struct
{
    Vdec2VoContext_t *vv;
    Adec2AoContext_t *aa;
    AwdmxContext_t *dmx;
    uint32_t state;
    pthread_mutex_t mutex;

    int playingTime; // ms
} PlayContext_t;

static int play_start(PlayContext_t *playCtx) {
    int ret = vdec2vo_start(playCtx->vv);

    if (ret == SUCCESS) {
        ret = adec2ao_start(playCtx->aa);
        ret = awdmx_start(playCtx->dmx);
    }

    playCtx->state &= ~(PLAY_statPAUSED | PLAY_statCOMPLETED);
    playCtx->state |= PLAY_statSTARTED;

    return ret;
}

static int play_pause(PlayContext_t *playCtx) {
    int ret = awdmx_pause(playCtx->dmx);

    if (ret == SUCCESS) {
        ret = adec2ao_pause(playCtx->aa);
        ret = vdec2vo_pause(playCtx->vv);
    }

    playCtx->state |= PLAY_statPAUSED;

    return ret;
}

static int play_stop(PlayContext_t *playCtx) {
    int ret = awdmx_stop(playCtx->dmx);

    if (ret == SUCCESS) {
        ret = adec2ao_stop(playCtx->aa);
        ret = vdec2vo_stop(playCtx->vv);
    }

    return ret;
}

static int play_seekto(PlayContext_t *playCtx, int seekTime) {
    int ret;

    ret = awdmx_seekTo(playCtx->dmx, seekTime);

    if (ret == SUCCESS) {
        ret = adec2ao_seekTo(playCtx->aa);
        ret = vdec2vo_seekTo(playCtx->vv);
    }

    return ret;
}

static void play_moveStatus(PlayContext_t *playCtx) {
    if (!(playCtx->state & PLAY_statSTARTED)) {
        return;
    }

    if (!(playCtx->state & PLAY_statCOMPLETED)) {
        if (awdmx_isEOF(playCtx->dmx)) {
            vdec2vo_checkEof(playCtx->vv);
            adec2ao_checkEof(playCtx->aa);
        }

        if (vdec2vo_isEOF(playCtx->vv)) {
            play_stop(playCtx);
            playCtx->state |= PLAY_statCOMPLETED;
        }
    }
}

static void play_onDemuxEof(void *context) {
    PlayContext_t *playCtx = (PlayContext_t *)context;
    vdec2vo_setVdecEof(playCtx->vv);
    adec2ao_setAdecEof(playCtx->aa);
}

void *thread_media(void *params) {
    media_info_t info;
    media_t *media = (media_t *)params;
    PlayContext_t *playCtx = (PlayContext_t *)media->context;
    notify_cb_t notify = media->notify;
    for (;;) {
        if (!media)
            break;
        // if(media->is_media_thread_exit)
        //     LOGI("is_media_thread_exit = 1");

        if (media->is_media_thread_exit)
            break;

        pthread_mutex_lock(&playCtx->mutex);
        play_moveStatus(playCtx);
        if (!vdec2vo_isEOF(playCtx->vv)) {
            vdec2vo_currentMediaTime(playCtx->vv, &playCtx->playingTime);
        } else {
            playCtx->playingTime = -2;
        }
        pthread_mutex_unlock(&playCtx->mutex);

        // if(media->is_media_thread_exit)
        //     LOGI("is_media_thread_exit = 2");

        info.playing_time = playCtx->playingTime;
        info.duration = playCtx->dmx->msDuration;
        if (!media->is_media_thread_exit)
            notify(&info);
        usleep(100000);
    }
    return NULL;
}

void media_control(media_t *media, player_cmd_t *cmd) {
    PlayContext_t *playCtx = (PlayContext_t *)media->context;

    pthread_mutex_lock(&playCtx->mutex);

    switch (cmd->opt) {
    case PLAYER_START:
        play_start(playCtx);
        break;
    case PLAYER_STOP:
        play_stop(playCtx);
        break;
    case PLAYER_PAUSE:
        play_pause(playCtx);
        break;
    case PLAYER_SEEK:
        play_seekto(playCtx, cmd->params);
        break;

    default:
        break;
    }

    pthread_mutex_unlock(&playCtx->mutex);
}

media_t *media_instantiate(char *filename, notify_cb_t notify) {
    int ret = 0;
    pthread_t pid;

    PlayContext_t *playCtx = calloc(1, sizeof(PlayContext_t));
    if (!playCtx)
        return NULL;

    media_t *media = calloc(1, sizeof(media_t));
    if (!media)
        return NULL;
    media->context = playCtx;
    media->notify = notify;

    pthread_mutex_init(&playCtx->mutex, NULL);

    playCtx->vv = vdec2vo_initSys();
    if (playCtx->vv == NULL) {
        LOGE("create vdec2vo failed");
        goto failed;
    }

    playCtx->aa = adec2ao_initSys();
    if (playCtx->aa == NULL) {
        LOGE("create adec2ao failed");
        goto failed;
    }

    playCtx->dmx = awdmx_open(filename, play_onDemuxEof, playCtx);
    if (playCtx->dmx == NULL) {
        LOGE("open demux failed");
        goto failed;
    } else {
        Vdec2VoParams_t vvParams;
        memset(&vvParams, 0, sizeof(vvParams));

        vvParams.initRotation = 0;
        vvParams.pixelFormat = MM_PIXEL_FORMAT_YVU_PLANAR_420;

        vvParams.vdec.codecType = playCtx->dmx->codecType;
        vvParams.vdec.width = playCtx->dmx->width;
        vvParams.vdec.height = playCtx->dmx->height;

        vvParams.vo.width = VO_WIDTH;
        vvParams.vo.height = VO_HEIGHT;
        vvParams.vo.intfType = VO_intfTYPE;
        vvParams.vo.intfSync = VO_intfSYNC;
        vvParams.vo.uiChn = VO_uiCHN;

        vvParams.clkChn = playCtx->dmx->clkChn;
        vvParams.dmxChn = playCtx->dmx->dmxChn;

        ret = vdec2vo_prepare(playCtx->vv, &vvParams);

        if (playCtx->dmx->audioNum > 0) {
            AdecParams_t aaParams;
            memset(&aaParams, 0, sizeof(aaParams));
            aaParams.clkChn = playCtx->dmx->clkChn;
            aaParams.dmxChn = playCtx->dmx->dmxChn;
            aaParams.codecType = playCtx->dmx->aCodecType;
            aaParams.bitsPerSample = playCtx->dmx->bitsPerSample;
            aaParams.channels = playCtx->dmx->channels;
            aaParams.sampleRate = playCtx->dmx->sampleRate;
            ret = adec2ao_prepare(playCtx->aa, &aaParams);
        }
    }
    if (ret != 0) {
        LOGE("prepare vdec2vo failed");
        goto failed;
    }

    LOGE("ready to play");
    media->is_media_thread_exit = false;
    ret = pthread_create(&pid, NULL, thread_media, (void *)media);
    if (ret) {
        LOGE("create thread media failed, exit");
        return NULL;
    }
    media->pid = pid;
    return media;
failed:
    adec2ao_deinitSys(playCtx->aa);
    vdec2vo_deinitSys(playCtx->vv);
    awdmx_close(playCtx->dmx);
    pthread_mutex_destroy(&playCtx->mutex);
    free(playCtx);
    LOGD("exit done");

    return NULL;
}

void media_exit(media_t *media) {
    assert(media);
    media->is_media_thread_exit = true;
    pthread_join(media->pid, NULL);

    PlayContext_t *playCtx = (PlayContext_t *)media->context;
    play_stop(playCtx);
    adec2ao_deinitSys(playCtx->aa);
    vdec2vo_deinitSys(playCtx->vv);
    awdmx_close(playCtx->dmx);
    pthread_mutex_destroy(&playCtx->mutex);
    free(media->context);
    free(media);
}
