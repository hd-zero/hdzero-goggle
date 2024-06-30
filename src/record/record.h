#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "vi2venc.h"
#include "ai2aenc.h"
#include "ai2ao.h"
#include "ffpack.h"
#include "disk.h"
#include "record_definitions.h"

typedef enum
{
    REC_statSave = -1,
    REC_statIdle,
    REC_statRun,
    REC_statStop,

    REC_statCreating,   //internal used
    REC_statCreated,
    REC_statRecording,

    REC_statBut,
} RecordState_e;

typedef enum
{
    REC_statusSave = -1,
    REC_statusIdle,
    REC_statusRun,
    REC_statusStop,

    REC_statusDiskUmounted,   //no sdcard
    REC_statusDiskPathFailed, //record path check failed
    REC_statusDiskFull,       //disk full
    REC_statusFramesTimeout,  //get frames timeout
    REC_statusFileError,      //open record file failed

    REC_statusBut,
} RecordStatus_e;

typedef enum {
    NAMING_CONTIGUOUS,
    NAMING_DATE
} FileNaming_t;

typedef struct
{
    char        diskPath[MAX_pathLEN/2];
    char        packPath[MAX_pathLEN];
    char        packType[4];
    uint32_t    packDuration;
    uint32_t    minDiskSize;
    uint64_t    packSize;
    bool        enableAudio;
    FileNaming_t fileNaming;
} RecordParams_t;

typedef struct
{
    uint32_t tickFps;
    uint32_t nbFrames;
} RecordFps_t;

typedef struct
{
    uint32_t tick;
    uint32_t num;
} CountWithTick_t;

typedef struct
{
    Vi2Venc_t* vv;
    Vi2Venc_t* vvLive;
    Ai2Aenc_t* aa;
    Ai2Ao_t*   ao;
    FFPack_t*  ff;
    uint32_t   tickBegin;
    uint32_t   nbFramesTotal;
    uint32_t   nbAudioFrames;
    int        nbVideoStreamIndex;
    int        nbAudioStreamIndex;
    int        nbFileIndex;
    pthread_mutex_t mutex;
    RecordState_e   stateGo;
    RecordState_e   stateGoing;
    RecordStatus_e  status;
    RecordStatus_e  statusSaved;
    uint64_t   ptsBase;
    uint64_t   ptsBaseAudio;
    uint64_t   ptsBaseDeltaA;
    uint64_t   ptsBaseDeltaV;

    SdcardStatus_t sdstat;
    RecordParams_t params;
    RecordFps_t    fpsStatus;
    char confFile[MAX_pathLEN];

    bool     enableLive;
    uint32_t nbFramesLive;
    uint64_t ptsBaseLive;
    VencSpspps_t spspps;
} RecordContext_t;

#ifdef __cplusplus
}
#endif
