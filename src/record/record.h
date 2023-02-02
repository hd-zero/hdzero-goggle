
#ifndef __RECORD_H_
#define __RECORD_H_

#include "vi2venc.h"
#include "ai2aenc.h"
#include "ai2ao.h"
#include "ffpack.h"
#include "disk.h"

#define MAX_pathLEN         128

#define LOCKFILE "/tmp/record.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

#define VI_HDZERO           1
#if(VI_HDZERO)
#define VI_FPS              60
#define VI_WIDTH            1280
#define VI_HEIGHT           720
#define VE_FPS              60
#define VE_WIDTH            1280
#define VE_HEIGHT           720
#else
#define VI_FPS              30
#define VI_WIDTH            1280
#define VI_HEIGHT           720
#define VE_FPS              30
#define VE_WIDTH            1280
#define VE_HEIGHT           720
#endif
#define VI_PIXELFORMAT      V4L2_PIX_FMT_NV21M
#define VE_thumbWIDTH       320
#define VE_thumbHEIGHT      180
#define VE_thumbQUALITY     60
#define VE_BPS              8388608 //4194304
#define VE_KBPS             (8*1024) //4096
#define VE_ENCODER          PT_H265
#define VE_RC               VENC_rcCBR
#define VE_GOP              0
#define VE_profileH264      VENC_profileHP          //high
#define VE_profileH265      VENC_profileBASELINE    //main
#define VE_PROFILE          ((VE_ENCODER==PT_H265) ? VE_profileH265 : VE_profileH264)
#define VE_levelH264        H264_LEVEL_51
#define VE_levelH265        H265_LEVEL_62
#define VE_LEVEL            ((VE_ENCODER==PT_H265) ? VE_levelH265 : VE_levelH264)
#define VE_minQP            10
#define VE_maxQP            52
#define VE_iQP              28
#define VE_pQP              28
#define VE_minIQP           0
#define VE_qpMAX            100

#define AI_devNO            0
#define AI_sampleRATE       48000
#define AI_sampleBITS       16
#define AI_CHANNELS         2
#define AE_CODEC            PT_AAC
#define AE_BPS              64000

#define REC_diskPATH        "/mnt/extsd"
//#define REC_diskPATH        "/home"
#define REC_diskFULL        50  //MB
#define REC_diskDURATION    5000//ms
#define REC_dataFILE        "/tmp/record.dat"
//#define REC_confFILE        "/app/record.conf"
#define REC_confPATH        "/data/confs"
#define REC_confFILE        "record.conf"
#define REC_confPathFILE    "/confs/record.conf"
#define REC_confDEFAULT     REC_confPATH"/record.conf"
#define REC_packDURATION    10                  //minutes
#define REC_minDURATION     1                   //minutes
#define REC_maxDURATION     60                  //ms
#define REC_packSIZE        1024                //MB
#define REC_minSIZE         50                  //MB
#define REC_maxSIZE         (2 * 1024)          //MB
#define REC_packPATH        "/movies/" //REC_diskPATH "/movies/"
#define REC_packPREFIX      "hdz_"
#define REC_packMP4         "mp4"
#define REC_packTS          "ts"
#define REC_packJPG         "jpg"
#define REC_packBMP         "bmp"
#define REC_packPGN         "png"
#define REC_packTYPE        REC_packMP4
#define REC_packSnapTYPE    REC_packJPG
#define REC_packIndexLEN    3
#define REC_packTYPES       {REC_packMP4,REC_packTS}
#define DOT                 "."
#define REC_packEXTS        {DOT REC_packMP4, DOT REC_packTS}
#define REC_packTypesNUM    2

#define ZeroMemory(p, size) memset(p, 0, size)

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

    REC_statusBut,
} RecordStatus_e;

typedef struct
{
    char        diskPath[MAX_pathLEN/2];
    char        packPath[MAX_pathLEN];
    char        packType[4];
    uint32_t    packDuration;
    uint32_t    minDiskSize;
    uint64_t    packSize;
    bool        enableAudio;
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
} RecordContext_t;

#endif  /* __RECORD_H_ */
