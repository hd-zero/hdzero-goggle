//#define LOG_NDEBUG 0
#define LOG_TAG "record"
#include <log/log.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include "gogglemsg.h"
#include "record.h"
#include "jpegenc.h"
#include "ffpack.h"
#include "disk.h"
#include "confparser.h"
#include "version.h"
#include "log.h"

void show_version(int argc, char* argv[])
{
    if( (argc > 1)  && (strcmp(argv[1], "-v") == 0) ) {
        printf("version: %s(%s, %s)\n", VERSION_FULL, __DATE__, __TIME__);
        exit(0);
    }
}

uint32_t get_tickCount(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

/******************************************************************************
* funciton : signal handler
******************************************************************************/
void signal_pipe( int sig )
{
	LOGE( "signal sigpipe\n" );

	return ;
}

void signal_segv(int sig)
{
	LOGE( "signal sigsegv\n" );

	abort();
}

void dump_bytes(uint8_t* data, int len)
{
#define BYTES_perLINE 16
    int nLines = (len / BYTES_perLINE) + ((len & ~BYTES_perLINE) > 0);
    int i;

    LOGD("addr=%p len=%d lines=%d", data, len, nLines);
    for(i=0; i<nLines; i++) {
        if( len >= BYTES_perLINE ) {
            LOGD("%04x: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                  i*BYTES_perLINE,
                  data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
                  data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]
                  );
            data += BYTES_perLINE;
            len -= BYTES_perLINE;
        }
        else {
            uint8_t dataLine[BYTES_perLINE];
            memset(dataLine, 0xFF, BYTES_perLINE);
            memcpy(dataLine, data, len);
            data = dataLine;
            LOGD("%04x: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                  i*BYTES_perLINE,
                  data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
                  data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]
                  );
            break;
        }
    }
}
/******************************************************************************
* funciton : lock file
******************************************************************************/
int lockfile(int fd)
{
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return(fcntl(fd, F_SETLK, &fl));
}

/******************************************************************************
* funciton : check if application is already running
******************************************************************************/
int already_running(void)
{
    int     fd;
    char    buf[16];

    fd = open(LOCKFILE, O_RDWR|O_CREAT, LOCKMODE);
    if (fd < 0) {
        LOGE( "can't open %s: %s", LOCKFILE, strerror(errno));
        exit(1);
    }
    if (lockfile(fd) < 0) {
        if (errno == EACCES || errno == EAGAIN) {

            close(fd);
            return(1);
        }
        LOGE( "can't lock %s: %s", LOCKFILE, strerror(errno));
        exit(1);
    }
    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf)+1);
    return(0);
}

void record_dumpParams(RecordParams_t* params)
{
    LOGD("record params");
    LOGD("disk: %s", params->diskPath);
    LOGD("path: %s", params->packPath);
    LOGD("type: %s", params->packType);
    LOGD("size: %llu MB", params->packSize/(1024*1024));
    LOGD("full: %d MB", params->minDiskSize);
    LOGD("duration: %d minutes", params->packDuration/(60*1000));
    LOGD("audio   : %s", params->enableAudio ? "yes" : "no");
}

void record_dumpVeParams(VencParams_t* params)
{
    LOGD("venc params");
    LOGD("codec : %s", (params->codecType==PT_H264) ? "H.264" : "H.265" );
    LOGD("width : %d", params->width);
    LOGD("height: %d", params->height);
    LOGD("fps   : %d", params->fps);
    LOGD("bps   : %d", params->bps);
    LOGD("rc    : %s", vi2venc_getRcModeName(params->rcMode) );
    LOGD("profile: %s", vi2venc_getProfileName(params->veAttr.mAttrH264.mProfile, (params->codecType==PT_H265)));
}

void record_dumpViParams(ViParams_t* params)
{
    LOGD("vi params");
    LOGD("width : %d", params->width);
    LOGD("height: %d", params->height);
    LOGD("fps   : %d", params->fps);
}

void record_dumpAeParams(AencParams_t* params)
{
    LOGD("aenc params");
    LOGD("codec : %s", (params->codecType==PT_AAC) ? "AAC" : "MP3" );
    LOGD("bps   : %d", params->bitRate);
}

void record_dumpAiParams(AiParams_t* params)
{
    LOGD("ai params");
    LOGD("audio device: %d", params->aiDev);
    LOGD("sample rate : %d", params->sampleRate);
    LOGD("channels    : %d", params->channels);
    LOGD("sample bits : %d", params->bitsPerSample);
}

void record_saveStatus(RecordContext_t* recCtx, RecordStatus_e recStatus)
{
    int     fd;
    char    buf[16];

    if( recStatus == REC_statusSave ) {
        if( recCtx->status == recCtx->statusSaved ) {
            return;
        }
    }
    else if( recStatus == recCtx->status ) {
        return;
    }
    recCtx->status = recStatus;

    fd = open(REC_dataFILE, O_RDWR|O_CREAT, LOCKMODE);
    if (fd < 0) {
        LOGE( "can't open %s: %s", REC_dataFILE, strerror(errno));
    }
    if (lockfile(fd) < 0) {
        if (errno == EACCES || errno == EAGAIN) {

            close(fd);
            return;
        }
        LOGE( "can't lock %s: %s", REC_dataFILE, strerror(errno));
    }
    ftruncate(fd, 0);
    sprintf(buf, "%d", recCtx->status);
    write(fd, buf, strlen(buf)+1);
    close(fd);

    recCtx->statusSaved = recCtx->status;
}

int record_takePicture(RecordContext_t* recCtx, char* sFile)
{
    int ret = SUCCESS;

    LOGD("start");

#if(0)
    VIDEO_FRAME_INFO_S frameBuffer;
    memset(&frameBuffer, 0, sizeof(frameBuffer));

    ret = vi2venc_requireRawFrame(recCtx->vv, &frameBuffer);
    if( ret == SUCCESS ) {
        JpegEncConfig_t jencCfg;
        jencCfg.width = VI_WIDTH;
        jencCfg.height= VI_HEIGHT;
        jencCfg.quality= 60;
        jencCfg.thumbnailWidth = VE_thumbWIDTH;
        jencCfg.thumbnailHeight= VE_thumbHEIGHT;
        jencCfg.thunbnailQuality = VE_thumbQUALITY;

        ret = jpegenc_encodeFrame(&frameBuffer, &jencCfg, NULL, NULL);
        vi2venc_releaseRawFrame(recCtx->vv, &frameBuffer);
    }
    LOGD("done: %x", ret);
    return ret;
#else
    JpegEncConfig_t jencCfg;
    JpegEncIO_t     jencIO;

    memset(&jencCfg, 0, sizeof(jencCfg));
    memset(&jencIO, 0, sizeof(jencIO));

    jencIO.cbOnFrame = NULL;
    jencIO.ctxOnFrame= NULL;
    jencIO.viDev = recCtx->vv->viDev;
    jencIO.viChn = recCtx->vv->viChn + 1;
    jencIO.sFile = sFile;
    jencIO.bThumb= true;

    jencCfg.width = VI_WIDTH;
    jencCfg.height= VI_HEIGHT;
    jencCfg.quality= 60;
    jencCfg.thumbnailWidth = VE_thumbWIDTH;
    jencCfg.thumbnailHeight= VE_thumbHEIGHT;
    jencCfg.thunbnailQuality = VE_thumbQUALITY;

    ret = jpegenc_takePicture(&jencIO, &jencCfg, 500);
#endif

    LOGD("done: %x", ret);
    return ret;
}

inline static bool record_shouldStop(RecordContext_t* recCtx)
{
    return (recCtx->sdstat.full || !recCtx->sdstat.mounted);
}

inline static int record_stopStatus(RecordContext_t* recCtx)
{
    if( !recCtx->sdstat.mounted ) {
        return REC_statusDiskUmounted;
    }
    else if( recCtx->sdstat.full ) {
        return REC_statusDiskFull;
    }

    return REC_statusIdle;
}

void record_stop(RecordContext_t* recCtx)
{
    if( recCtx->params.enableAudio ) {
        bool aoplay = ai2ao_playing(recCtx->ao);
        ai2aenc_stop(recCtx->aa, !aoplay);
    }

    vi2venc_stop(recCtx->vv);

    pthread_mutex_lock(&recCtx->mutex);
    if( recCtx->ff != NULL ) {
        ffpack_close(recCtx->ff);
        recCtx->ff = NULL;
    }
    recCtx->stateGoing = REC_statStop;
    pthread_mutex_unlock(&recCtx->mutex);

    record_saveStatus(recCtx, record_stopStatus(recCtx));
}

int record_start(RecordContext_t* recCtx)
{
    if( recCtx->ff != NULL ) {
        LOGD("recording ...");
        return 0;
    }

    if( record_shouldStop(recCtx) ) {
        //LOGD("sdcard error ...");
        return 0;
    }

    int ret;
    VencSpspps_t veHeader = { NULL, 0 };
    int nbFileIndex = recCtx->nbFileIndex;

    ViParams_t   viParams;
    VencParams_t veParams;
    AiParams_t   aiParams;
    AencParams_t aeParams;

    ZeroMemory(&viParams, sizeof(viParams));
    ZeroMemory(&veParams, sizeof(veParams));
    ZeroMemory(&aiParams, sizeof(aiParams));
    ZeroMemory(&aeParams, sizeof(aeParams));

    conf_loadViParams(recCtx->confFile, &viParams);
    conf_loadVencParams(recCtx->confFile, &veParams);

    if( recCtx->params.enableAudio ) {
        conf_loadAiParams(recCtx->confFile, &aiParams);
        conf_loadAencParams(recCtx->confFile, &aeParams);
    }

    if( veParams.maxKeyItl==0 ) {
        veParams.maxKeyItl = veParams.fps;
    }
    veParams.picFormat = MM_PIXEL_FORMAT_YVU_SEMIPLANAR_420;

    ret = vi2venc_prepare(recCtx->vv, &viParams, &veParams, NULL);
    if( ret != 0) {
        goto failed;
    }

    if( recCtx->params.enableAudio ) {
        ret = ai2aenc_prepare(recCtx->aa, &aiParams, &aeParams);
        if( ret != 0) {
            goto failed;
        }
    }

    ret = vi2venc_getSpsPpsInfo(recCtx->vv, &veHeader, true);
    if( ret == SUCCESS ) {
        //LOGD("dump spspps");
        //dump_bytes(veHeader.pBuffer, veHeader.nLength);
    }
    else {
        LOGE("get sps failed: %x", ret);
    }

    char sFile[256];
    ZeroMemory(sFile, sizeof(sFile));
    sprintf(sFile, "%s%s%03d.%s", recCtx->params.packPath, REC_packPREFIX, nbFileIndex, recCtx->params.packType);

    FFPack_t* ff = ffpack_openFile(sFile, NULL);
    if( ff == NULL ) {
        LOGE("open failed");
        ret = -1;
        goto failed;
    }
    recCtx->ff = ff;

    FFStreamParameters_t streamParam;
    ZeroMemory(&streamParam, sizeof(streamParam));
    streamParam.mediaType = AVMEDIA_TYPE_VIDEO;
    streamParam.codecId = (PT_H265 == veParams.codecType) ? AV_CODEC_ID_HEVC : AV_CODEC_ID_H264;
    streamParam.video.width = veParams.width;//VE_WIDTH;
    streamParam.video.height= veParams.height;//VE_HEIGHT;
    streamParam.video.fps = veParams.fps;//VE_FPS;
    streamParam.spsData = veHeader.pBuffer;
    streamParam.spsLen = veHeader.nLength;
    int streamIndex = ffpack_newVideoStream(ff, -1, &streamParam);
    if( streamIndex < 0 ) {
        LOGE("create video stream failed");
        ret = -2;
        goto failed;
    }
    recCtx->nbVideoStreamIndex = streamIndex;

    if( recCtx->params.enableAudio ) {
        ZeroMemory(&streamParam, sizeof(streamParam));
        streamParam.mediaType = AVMEDIA_TYPE_AUDIO;
        streamParam.codecId = (PT_AAC == aeParams.codecType) ? AV_CODEC_ID_AAC : AV_CODEC_ID_MP3;
        streamParam.audio.sample_rate = aiParams.sampleRate;
        streamParam.audio.channels = aiParams.channels;
        streamParam.audio.bits_per_sample = aiParams.bitsPerSample;
        streamIndex = ffpack_newAudioStream(ff, -1, &streamParam);
        if( streamIndex < 0 ) {
            LOGE("create audio stream failed");
            ret = -3;
            goto failed;
        }
        recCtx->nbAudioStreamIndex = streamIndex;
    }

    recCtx->tickBegin = get_tickCount();
    recCtx->nbFramesTotal = 0;
    recCtx->nbAudioFrames = 0;
    recCtx->ptsBase = 0;
    recCtx->ptsBaseAudio = 0;
    recCtx->ptsBaseDeltaA = 0;
    recCtx->ptsBaseDeltaV = 0;
    recCtx->fpsStatus.tickFps = recCtx->tickBegin;
    recCtx->fpsStatus.nbFrames= 0;

    ret = ffpack_start(ff);
    if( ret != SUCCESS ) {
        goto failed;
    }

    ret = vi2venc_start(recCtx->vv);
    if( ret != SUCCESS) {
        goto failed;
    }

    if( recCtx->params.enableAudio ) {
        ret = ai2aenc_start(recCtx->aa);
        if( ret != SUCCESS) {
            goto failed;
        }
    }

    recCtx->stateGoing = REC_statRun;
    record_saveStatus(recCtx, REC_statusRun);
    recCtx->nbFileIndex++;

    sprintf(sFile, "%s%s%03d.%s", recCtx->params.packPath, REC_packPREFIX, nbFileIndex, REC_packSnapTYPE);
    ret = record_takePicture(recCtx, sFile);

    record_dumpViParams(&viParams);
    record_dumpVeParams(&veParams);

    if( recCtx->params.enableAudio ) {
        record_dumpAiParams(&aiParams);
        record_dumpAeParams(&aeParams);
    }

    return SUCCESS;

failed:
    LOGE("start failed: %d", ret);
    record_stop(recCtx);
    return ret;
}

bool record_pack(RecordContext_t* recCtx)
{
    uint32_t tickNow = get_tickCount();
    uint32_t tickDuration = tickNow - recCtx->tickBegin;

    if( recCtx->stateGo != REC_statRun ) {
        return false;
    }
    if( recCtx->stateGoing != REC_statRun ) {
        return false;
    }

    bool bPack = ( tickDuration >= recCtx->params.packDuration );
    bPack |= ( recCtx->ff->nbTotalSize >= recCtx->params.packSize );
    if( !bPack ) {
        return false;
    }

    pthread_mutex_lock(&recCtx->mutex);
    ffpack_close(recCtx->ff);
    recCtx->ff = NULL;
    recCtx->nbFramesTotal = 0;
    recCtx->nbAudioFrames = 0;
    recCtx->ptsBase = 0;
    recCtx->ptsBaseAudio = 0;
    recCtx->ptsBaseDeltaA= 0;
    recCtx->ptsBaseDeltaV= 0;
    recCtx->fpsStatus.tickFps = get_tickCount();
    recCtx->fpsStatus.nbFrames= 0;

    VencSpspps_t veHeader = { NULL, 0 };
    int  nbFileIndex = recCtx->nbFileIndex;
    char sFile[256];
    memset(sFile, 0, sizeof(sFile));
    sprintf(sFile, "%s%s%03d.%s", recCtx->params.packPath, REC_packPREFIX, nbFileIndex, recCtx->params.packType);

    FFPack_t* ff = ffpack_openFile(sFile, NULL);
    if( ff == NULL ) {
        LOGE("open failed");
        goto failed;
    }
    recCtx->ff = ff;

    vi2venc_getSpsPpsInfo(recCtx->vv, &veHeader, true);
    //LOGD("dump spspps");
    //dump_bytes(veHeader.pBuffer, veHeader.nLength);

    VencParams_t* veParams = &recCtx->vv->veParams;

    FFStreamParameters_t streamParam;
    ZeroMemory(&streamParam, sizeof(streamParam));
    streamParam.mediaType = AVMEDIA_TYPE_VIDEO;
    streamParam.codecId = (PT_H265 == veParams->codecType) ? AV_CODEC_ID_HEVC : AV_CODEC_ID_H264;
    streamParam.video.width = veParams->width;//VE_WIDTH;
    streamParam.video.height= veParams->height;//VE_HEIGHT;
    streamParam.video.fps = veParams->fps;//VE_FPS;
    streamParam.spsData = veHeader.pBuffer;
    streamParam.spsLen = veHeader.nLength;
    int streamIndex = ffpack_newVideoStream(ff, -1, &streamParam);
    if( streamIndex < 0 ) {
        LOGE("create video stream failed");
        goto failed;
    }
    recCtx->nbVideoStreamIndex = streamIndex;

    if( recCtx->params.enableAudio ) {
        AencAttr_t*   aeParams = &recCtx->aa->aeAttr;

        ZeroMemory(&streamParam, sizeof(streamParam));
        streamParam.mediaType = AVMEDIA_TYPE_AUDIO;
        streamParam.codecId = (PT_AAC == aeParams->Type) ? AV_CODEC_ID_AAC : AV_CODEC_ID_MP3;
        streamParam.audio.sample_rate = aeParams->sampleRate;
        streamParam.audio.channels = aeParams->channels;
        streamParam.audio.bits_per_sample = aeParams->bitsPerSample;
        streamIndex = ffpack_newAudioStream(ff, -1, &streamParam);
        if( streamIndex < 0 ) {
            LOGE("create audio stream failed");
            goto failed;
        }
        recCtx->nbAudioStreamIndex = streamIndex;
    }

    if( ffpack_start(ff) != 0 ) {
        goto failed;
    }

    recCtx->tickBegin = get_tickCount();
    recCtx->nbFileIndex++;
    vi2venc_requestIFrame(recCtx->vv);

    pthread_mutex_unlock(&recCtx->mutex);

    sprintf(sFile, "%s%s%03d.%s", recCtx->params.packPath, REC_packPREFIX, nbFileIndex, REC_packSnapTYPE);
    record_takePicture(recCtx, sFile);

    return true;

failed:
    LOGE("create %s failed", sFile);
    remove(sFile);
    if( recCtx->ff != NULL ) {
        ffpack_close(recCtx->ff);
        recCtx->ff = NULL;
    }
    pthread_mutex_unlock(&recCtx->mutex);
    return true;
}

inline static bool record_isGoing(RecordContext_t* recCtx)
{
    return (recCtx->stateGo == REC_statRun) && (recCtx->stateGo == recCtx->stateGoing);
}

inline static bool record_isArecording(RecordContext_t* recCtx)
{
    return record_isGoing(recCtx) && recCtx->params.enableAudio;
}

int vv_onFrame(void* vvPtr, uint8_t* frameData, int frameLen, VencFrameType_e frameType, uint64_t pts, void* context)
{
    int ret = 0;
    RecordContext_t* recCtx = (RecordContext_t*)context;
    uint64_t   pts2 = 0;//nbFrames * 90000;

    pthread_mutex_lock(&recCtx->mutex);

    FFPack_t*  ff = (FFPack_t*)recCtx->ff;
    int streamIndex = recCtx->nbVideoStreamIndex;

    if( recCtx->nbFramesTotal == 0 ) {
        LOGD("first frame (type=%d, len=%d, pts=%llu)", frameType, frameLen, pts);

        if( frameType != FRAME_typeI ) {
            pthread_mutex_unlock(&recCtx->mutex);
            vi2venc_requestIFrame(recCtx->vv);
            return 0;
        }
        recCtx->ptsBase = pts;

        #if(0)
        VencSpspps_t veHeader = { NULL, 0, false };
        ret = vi2venc_getSpsPpsInfo(recCtx->vv, &veHeader, true);
        if( ret == SUCCESS ) {
            LOGD("dump spspps on first frame");
            dump_bytes(veHeader.pBuffer, veHeader.nLength);
            vi2venc_freeSpsPpsInfo(&veHeader);
        }
        #endif
    }

    if( recCtx->nbAudioFrames == 0 ) {
        recCtx->ptsBaseDeltaA = pts - recCtx->ptsBase;
    }

    pts2 = (pts - recCtx->ptsBase + recCtx->ptsBaseDeltaV) / 10;    //unit us to 10us
    //LOGD("%llu, %llu", pts2, recCtx->ptsBase);

    ret = ffpack_input(ff, streamIndex, frameData, frameLen, frameType==FRAME_typeI, pts2);
    if( ret == -5 ) {
        /* I/O error, need to check disk immediately */
        //record_checkDiskImmediately(recCtx);
    }
    recCtx->nbFramesTotal++;
    recCtx->fpsStatus.nbFrames++;

    pthread_mutex_unlock(&recCtx->mutex);

    return 0;
}

int aa_onFrame(void* aaPtr, uint8_t* frameData, int frameLen, uint64_t pts, void* context)
{
    int ret = 0;
    RecordContext_t* recCtx = (RecordContext_t*)context;
    uint64_t   pts2 = 0;//nbFrames * 90000;

    pthread_mutex_lock(&recCtx->mutex);

    FFPack_t*  ff = (FFPack_t*)recCtx->ff;
    int streamIndex = recCtx->nbAudioStreamIndex;

    if( recCtx->nbAudioFrames == 0 ) {
        recCtx->ptsBaseAudio = pts;
        LOGD("first frame (len=%d, pts=%llu)", frameLen, pts);

        #if(0)
        /* no need */
        if( recCtx->aa->aeAttr.Type == PT_AAC ) {
            ffpack_setExtradata(ff, streamIndex, frameData, ADTS_HEADER__LEN);
        }
        #endif
    }

    if( recCtx->nbFramesTotal == 0 ) {
        recCtx->ptsBaseDeltaV = pts - recCtx->ptsBaseAudio;
    }

    pts2 = (pts - recCtx->ptsBaseAudio + recCtx->ptsBaseDeltaA) / 10;    //unit us to 10us
    //LOGD("len=%d, %llu, %llu", frameLen, pts2, recCtx->ptsBaseAudio);

    ret = ffpack_input(ff, streamIndex, frameData, frameLen, FRAME_typeI, pts2);
    if( ret == -5 ) {
        /* I/O error, need to check disk immediately */
        //record_checkDiskImmediately(recCtx);
    }

    recCtx->nbAudioFrames++;

    pthread_mutex_unlock(&recCtx->mutex);

    return 0;
}

int record_checkDisk(RecordContext_t* recCtx)
{
    SdcardStatus_t* sds = &recCtx->sdstat;

    if( disk_sdstat(recCtx->params.minDiskSize, sds) ) {
        if( !sds->inserted || !sds->mounted ) {
            /* disk unmounted */
            LOGE("no sdcard");
            record_saveStatus(recCtx, REC_statusDiskUmounted);
        }
        else if ( sds->mounted ) {
            if( !disk_checkPath(recCtx->params.packPath) ) {
                /* record path check failed */
                //LOGE("no record path");
                record_saveStatus(recCtx, REC_statusDiskPathFailed);
            }
            else if ( sds->full ) {
                LOGE("sdcard full");
                record_saveStatus(recCtx, REC_statusDiskFull);
            }
            else {
                char* sTypes[] = REC_packEXTS;
                int nIndex = disk_countMovies(recCtx->params.packPath, REC_packPREFIX,
                                              sTypes, REC_packTypesNUM, REC_packIndexLEN);
                recCtx->nbFileIndex = nIndex+1;
                LOGD("movies index: %d", recCtx->nbFileIndex);
            }
        }
    }

    return 0;
}

void record_run(RecordContext_t* recCtx, RecordState_e stateGo)
{
    if( recCtx->stateGo != stateGo ) {
        recCtx->stateGo = stateGo;
    }

    if( recCtx->stateGo != recCtx->stateGoing ) {
        /* redo */
        if( recCtx->stateGo == REC_statRun ) {
            record_start(recCtx);
        }
        else if (recCtx->stateGo == REC_statStop ) {
            record_stop(recCtx);
        }
    }
    else if( recCtx->stateGoing == REC_statRun ) {
        if( record_shouldStop(recCtx) ) {
            record_stop(recCtx);
        }
        else {
            uint32_t tickNow = get_tickCount();
            if( (tickNow - recCtx->fpsStatus.tickFps) >= 1000 ) {
                if( recCtx->fpsStatus.nbFrames < 10 ) {
                    record_saveStatus(recCtx, REC_statusFramesTimeout);
                }
                recCtx->fpsStatus.tickFps = tickNow;
                recCtx->fpsStatus.nbFrames= 0;
            }
            else if( recCtx->status == REC_statusFramesTimeout ) {
                recCtx->fpsStatus.tickFps = tickNow;
                recCtx->fpsStatus.nbFrames= 0;
                record_saveStatus(recCtx, REC_statusRun);
            }
        }
    }
}

void record_checkConf(RecordContext_t* recCtx, char* confSet)
{
	if( disk_checkFile(confSet) )  {
        strcpy(recCtx->confFile, confSet);
        return;
	}

    /* path where default confs is */
	if( disk_checkFile(REC_confDEFAULT) ) {
        strcpy(recCtx->confFile, REC_confDEFAULT);
        return;
	}

    char  sTemp[MAX_pathLEN];
    char* p = sTemp;
    readlink("/proc/self/exe", sTemp, MAX_pathLEN);
    if (NULL != (p = strrchr(sTemp,'/'))){
        /* path where confs is */
        *p = '\0';
        strcat(sTemp, REC_confPathFILE);
        if( disk_checkFile(sTemp) ) {
            strcpy(recCtx->confFile, sTemp);
            return;
        }

        /* path where record is */
        memset(sTemp, 0, MAX_pathLEN);
        readlink("/proc/self/exe", sTemp, MAX_pathLEN);
        p = strrchr(sTemp,'/');
        *p = '\0';
        sprintf(recCtx->confFile, "%s/%s", sTemp, REC_confFILE);
    }
}

void main_loop(RecordContext_t* recCtx)
{
    bool isExit = false;
    uint32_t tkIdle = 0;
    uint32_t tkNow = 0;

    gogglemsg_init(false, MTYPE_RECORD);
    gogglemsg_flush(MTYPE_RECORD);

	while( !isExit ) {
	    int nMsg = gogglemsg_recv(MTYPE_RECORD);

		switch( nMsg ) {
		case MSG_cmdQUIT:
		    isExit = true;
			break;
		case MSG_cmdSTART:
		    if( !record_isGoing(recCtx) ) {
                conf_loadRecordParams(recCtx->confFile, &recCtx->params);
		    }
		    record_run(recCtx, REC_statRun);
		    tkIdle = tkNow;
		    break;
		case MSG_cmdSTOP:
		    record_run(recCtx, REC_statStop);
		    tkIdle = tkNow;
		    break;
		case REC_cmdSAVE:
		    conf_saveRecordParams(recCtx->confFile, &recCtx->params);
		    if( recCtx->vv != NULL) {
                conf_saveVencParams(recCtx->confFile, &recCtx->vv->veParams);
                conf_saveViParams(recCtx->confFile, &recCtx->vv->viParams);
		    }
			break;
		case AIO_cmdSTART:
        {
            AiParams_t aiParams;
            ZeroMemory(&aiParams, sizeof(aiParams));
            conf_loadAiParams(recCtx->confFile, &aiParams);
            ai2ao_start(recCtx->ao, &aiParams);
		    break;
        }
		case AIO_cmdSTOP:
            ai2ao_stop(recCtx->ao, !record_isArecording(recCtx));
		    break;
		default:
            usleep(100*1000);
			break;
		}

		tkNow = get_tickCount();
		if( tkNow - tkIdle >= 500 ) {
            record_checkDisk(recCtx);
            record_saveStatus(recCtx, REC_statusSave);
            record_run(recCtx, recCtx->stateGo);
            record_pack(recCtx);
            tkIdle = tkNow;
        }
	}

    gogglemsg_uninit();
}

int main(int argc, char *argv[])
{
    show_version(argc, argv);

    LOGD("record start ...");

	signal(SIGPIPE, signal_pipe);
	signal(SIGSEGV, signal_segv);

	if( already_running() ) {
		LOGE( "already runing... exit!" );
		exit(1);
	}
    log_open();

    RecordContext_t recCtx;
    ZeroMemory(&recCtx, sizeof(recCtx));
    record_checkConf( &recCtx, (argc > 1) ? argv[1] : NULL );
    LOGD("record conf: %s", recCtx.confFile);

    recCtx.nbFileIndex = 0;
    recCtx.nbFramesTotal = 0;
	recCtx.nbVideoStreamIndex = 0;
	recCtx.ff = NULL;
	recCtx.vv = NULL;
	recCtx.aa = NULL;
	recCtx.ao = NULL;
	recCtx.stateGo = REC_statIdle;
	recCtx.stateGoing = REC_statIdle;
	recCtx.status = REC_statSave;
	recCtx.statusSaved = REC_statSave;
	pthread_mutex_init(&recCtx.mutex, NULL);
	record_saveStatus(&recCtx, REC_statusIdle);
	conf_loadRecordParams(recCtx.confFile, &recCtx.params);
	record_dumpParams(&recCtx.params);

	disk_sdstat_create(recCtx.params.diskPath, true);

    recCtx.vv = vi2venc_initSys(vv_onFrame, &recCtx);
    if( recCtx.vv == NULL ) {
        goto failed;
    }

    recCtx.aa = ai2aenc_initSys(aa_onFrame, &recCtx);
    if( recCtx.aa == NULL ) {
        goto failed;
    }

    recCtx.ao = ai2ao_initSys();

    main_loop(&recCtx);

failed:
    LOGD("exit ...");

    record_stop(&recCtx);
    ai2ao_deinitSys(recCtx.ao);
    ai2aenc_deinitSys(recCtx.aa);
    vi2venc_deinitSys(recCtx.vv);
    pthread_mutex_destroy(&recCtx.mutex);
    disk_sdstat_delete();

    LOGD("exit done");
    log_close();

    return 0;
}
