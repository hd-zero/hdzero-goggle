#pragma once

#ifdef __cplusplus
extern "C" {
#endif

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
#define VE_KBPS             (16*1024) //4096
#define VE_KBPS_LIVE        (4*1024) //4096
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
#define REC_hotPREFIX       "hot_"
#define REC_packHotPREFIX   REC_hotPREFIX REC_packPREFIX
#define REC_packMP4         "mp4"
#define REC_packTS          "ts"
#define REC_packJPG         "jpg"
#define REC_packBMP         "bmp"
#define REC_packPGN         "png"
#define REC_packTYPE        REC_packTS
#define REC_packSnapTYPE    REC_packJPG
#define REC_packIndexLEN    3
#define REC_packTYPES       {REC_packMP4,REC_packTS}
#define DOT                 "."
#define REC_packEXTS        {DOT REC_packMP4, DOT REC_packTS}
#define REC_packTypesNUM    2

#define REC_filePathGet(BUFF, PATH, PREFIX, INDEX, FILEFMT) \
    sprintf((BUFF), "%s%s%03d.%s", (PATH), (PREFIX), (INDEX), (FILEFMT));

#define ZeroMemory(p, size) memset(p, 0, size)

#ifdef __cplusplus
}
#endif
