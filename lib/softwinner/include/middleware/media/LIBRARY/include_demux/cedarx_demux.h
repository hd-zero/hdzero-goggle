/*******************************************************************************
--                                                                            --
--                    CedarX Multimedia Framework                             --
--                                                                            --
--          the Multimedia Framework for Linux/Android System                 --
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                         Softwinner Products.                               --
--                                                                            --
--                   (C) COPYRIGHT 2011 SOFTWINNER PRODUCTS                   --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
*******************************************************************************/

#ifndef CEDARX_DEMUX_DWX5_H_
#define CEDARX_DEMUX_DWX5_H_

#include <errno.h>
#include <cedarx_stream.h>

__BEGIN_DECLS
#if 0
typedef enum CDX_MEDIA_STATUS
{
    CDX_MEDIA_STATUS_UNKOWN,
    CDX_MEDIA_STATUS_IDLE,
    CDX_MEDIA_STATUS_PAUSE,
    CDX_MEDIA_STATUS_STEP,
    CDX_MEDIA_STATUS_PLAY,
    CDX_MEDIA_STATUS_FORWARD,
    CDX_MEDIA_STATUS_BACKWARD,
    CDX_MEDIA_STATUS_STOP,
    CDX_MEDIA_STATUS_ABORT,
    CDX_MEDIA_STATUS_JUMP,
}CDX_MEDIA_STATUS;

typedef enum CDX_PACKETTYPE
{
    CDX_PacketUnkown = 0,
    CDX_PacketVideo,
    CDX_PacketAudio,
    CDX_PacketSubtitle,
    CDX_PacketImage
} CDX_PACKETTYPE;

typedef enum SUBTITLE_SOURCE{
    SUBTITLE_POS_NONE   = 0,    //no subtitle exist
    SUBTITLE_POS_EMBED  = 1,    //embeded subtilte in video file
    SUBTITLE_POS_EXT    = 2,    //extern subtitle in subtitle file
    SUBTITLE_POS_,
} SUBTITLE_SOURCE;

typedef enum CDX_DEMUX_MODE
{
    CDX_DEMUX_MODE_PULL,
    CDX_DEMUX_MODE_PUSH         //* some parsers like ts parser may work in push mode.
}CDX_DEMUX_MODE_E;
#endif

typedef enum CDX_DEMUX_COMMANDS
{
    CDX_DMX_CMD_NOP = 0,
    CDX_DMX_CMD_RESET,
    CDX_DMX_CMD_MEDIAMODE_CONTRL,
    CDX_DMX_CMD_SWITCH_AUDIO,
    CDX_DMX_CMD_SWITCH_SUBTITLE,

    CDX_DMX_CMD_DATA_READY,
    CDX_DMX_CMD_IS_REQUESTING_DATA,
    //* the following 3 commands is added for TS parser which is work in push mode.
    //CDX_DMX_CMD_GET_DEMUX_MODE,                 //* return whether this demux is work in PULL mode or PUSH mode.
    CDX_DMX_CMD_SET_REQUEST_BUFFER_CALLBACK,    //* the cmd_sub param should be a handle to the demux component.
    CDX_DMX_CMD_SET_UPDATE_DATA_CALLBACK,       //*

    CDX_DMX_CMD_GET_CURRENT_MEDIATIME_CALLBACK,
    CDX_DMX_CMD_GET_CACHE_STATE,                //* state control for net real time stream
    CDX_DMX_CMD_SET_DURATION,
    CDX_DMX_CMD_SET_DEMUX_EVENT_CALLBACK,
    CDX_DMX_CMD_DISABLE_AUDIO_TRACK,
    CDX_DMX_CMD_DISABLE_SUBTITLE_TRACK,
    CDX_DMX_CMD_SUPPORT_AUDIO_SEAMELESS_SWITCH,
    CDX_DMX_CMD_SET_QUERY_BUFFER_STATUS_CALLBACK,
    CDX_DMX_CMD_SET_PLAY_BD_FILE,

    CDX_DMX_CMD_SNIFF_SFT_MIME,
    CDX_STREAM_CMD_FORCE_EXIT,

    CDX_DMX_CMD_SET_DEFAULT_LOW_WATER_THRESHOLD,
    CDX_DMX_CMD_SET_DEFAULT_HIGH_WATER_THRESHOLD,

    CDX_DMX_CMD_SET_TRACK_INFO,
    //only usefull for ts.
    CDX_DMX_CMD_SET_AV_SYNC,
    CDX_DMX_CMD_CLEAR_BUFFER,

    CDX_DMX_CMD_GET_STATUS,

    CDX_DMX_CMD_SKIP_CHUNK_DATA = 0x100,
    CDX_DMX_CMD_GET_PARSER_TYPE,    //ret = enum CdxParserTypeE
} CDX_DEMUX_COMMANDS;

#if 0
typedef enum CDX_DEMUX_FLAGS{
    CDX_DEMUX_CAN_SEEK = 1, //don't touch it
    CDX_DEMUX_CAN_FFRR = 2,

    CDX_DEMUX_PTS_CORRECT = 65536,
}CDX_DEMUX_FLAGS;

typedef struct CDX_CACHE_STATE
{
    int buffering_percent;
    int filled_percent;
    int filled_size;
    int eof_flag;
    int bandwidth_kbps;
    long long buffering_time; //us
}CDX_CACHE_STATE;

//--------------------------------------------------------------------------------

//the information for cedarx demuxer infomation
#define MAX_AUDIO_STREAM_NUM    (16)
#define MAX_VIDEO_STREAM_NUM    (1)
#define MAX_SUBTITLE_STREAM_NUM (32)
#define ID3_INFO_BUF_SIZE       (64)
#define MAX_LANG_CHAR_SIZE  (32)
#define MAX_SUBTITLE_LANG_SIZE     (32)

enum CEDARXAUDIOFLAGSENUM{
    ADEC_DISABLE_AAC_PACKING = (1<<0),
};

typedef struct  CedarXAudioStreamInfo
{
    int       codec_type;     //CDX_AUDIO_MP3, enum __CEDARLIB_AUDIO_CODEC_TYPE,AUDIO_MP3??   <==>__cedar_audio_fmt_t
    int       sub_codec_type;
    int       channels;
    int       bits_per_sample;
    int       sample_per_second;
    int       avg_bit_rate;
    int       max_bit_rate;

    unsigned int file_length;
    unsigned short audio_bs_src; // audio bitstream source, __cedarlib_file_fmt_t , CEDARLIB_FILE_FMT_AVI, <==>CEDAR_MEDIA_FILE_FMT_AVI��
    int       is_raw_data_output; //0:pcm output, 1:raw data

    int       data_encode_type;
    unsigned char        lang[MAX_LANG_CHAR_SIZE];

    int       extra_data_len;
    char      *extra_data;
    int       flags;
} CedarXAudioStreamInfo;

typedef struct CedarXPaletteInfo{
    char    valid_flg;
    int   palette_num;
    int   palette_ary[32];
}CedarXPaletteInfo;

typedef struct CedarXSubtitleStreamInfo{
    int       subtitle_coming_type;
    int       subtitle_type;
    CedarXPaletteInfo palette_info;
    int       stream_idx;
    int       data_encode_type;
    unsigned char        lang[MAX_LANG_CHAR_SIZE];
    int       org_sub_width;
    int       org_sub_height;
} CedarXSubtitleStreamInfo;

typedef struct CedarXExtSubtitleStreamInfo {
    char *lang[32];
    int nExtSubStrmNum;
}CedarXExtSubtitleStreamInfo;;
#endif

#include "CdxParser.h"
#if 0
typedef struct CedarXMediainfo
{
    char        nHasAudio;
    char        nHasVideo;
    char        nHasSubtitle;
    //char        nUseExtSubtitle;
    int         nFlags; //CDX_DEMUX_FLAGS .etc

    unsigned char nStreamNum;
    unsigned char nAudStrmNum;
    unsigned char nVidStrmNum;
    unsigned char nSubStrmNum;

    unsigned int  nDuration; //unit: ms
    CDX_S64       nFileSize;

    CedarXAudioStreamInfo        AudStrmList[MAX_AUDIO_STREAM_NUM];
    OMX_VIDEO_PORTDEFINITIONTYPE VidStrmList[MAX_VIDEO_STREAM_NUM];
    CedarXSubtitleStreamInfo     SubtitleStrmList[MAX_SUBTITLE_STREAM_NUM];
    int  nInitAudStrmNum;
    int  nInitVidStrmNum;
    int  nInitSubStrmNum;
    //ExternSubtitleInfo             ExtSubtitleStrmInfo;

    unsigned char        Author[ID3_INFO_BUF_SIZE];
    int       nAuthorLen;
    unsigned char        CopyRight[ID3_INFO_BUF_SIZE];
    int       nCopyRightLen;
    unsigned char        Title[ID3_INFO_BUF_SIZE];
    int       nTitleLen;
    unsigned char        Genre[ID3_INFO_BUF_SIZE];
    int       nGenreLen;

    unsigned char nGeoData[18];
    int  nGeoLen;

    void    *pPrivData;
} CedarXMediainfo;
#endif

enum VIDEO_TYPE{
    VIDEO_TYPE_MAJOR,
    VIDEO_TYPE_MINOR,
    VIDEO_TYPE_NOT_VIDEO
};

#if 0
typedef struct CedarXPacket {
    union CedarXPacketInfo {
        struct EPDK_READ_PACKET_INFO {
            unsigned char* pkt_buf0;
            int pkt_size0;
            unsigned char* pkt_buf1;
            int pkt_size1;
        } epdk_read_pkt_info;
    } pkt_info;

    struct FFMPEG_READ_PACKET_INFO {
        unsigned char* pkt_buf;
        int pkt_size;
    } ffmpeg_read_pkt_info;

    int is_dummy_packet;
    int flags;

    enum VIDEO_TYPE video_type;
    int ctrl_bits;  //CEDARV_FLAG_FIRST_PART | CEDARV_FLAG_LAST_PART | CEDARV_FLAG_PTS_VALID;
    int pkt_type;   //CDX_PacketVideo
    int pkt_length;
    CDX_S64 pkt_offset;
    CDX_S64 pkt_pts; //us
    CDX_S64 pkt_duration;
    CDX_S64 curr_offset;
    CDX_S64 file_offset;
    void    *pPrivData;
    OMX_S8   pkt_dataIndex;
} CedarXPacket;
#endif

typedef struct
{
    void* mpCbEventHandler;
    void* mpCookie;   //DEMUXDATATYPE*
}CdxDemuxEventCallbackInfo;

typedef struct CedarXDemuxerAPI {
  const char *name;
  const char *subname;
  const char *shortdesc;
  void  *use_sft_demux_handle;

  //below reserved are only used by cedarx internal
  void *reserved_0; //demux_epdk_wrapper
  void *reserved_1; //demux_epdk_wrapper
  void *reserved_2; //demux_http

  //for dynamic library
  void *dl_handle;
  //below reserved are used by customer
  void *reserved_usr_0;
  void *reserved_usr_1;

  /* base interface for demuxer plugins */
  int  (*open)(struct CedarXDemuxerAPI *handle, CdxMediaInfoT *pMediaInfo, CdxDataSourceT *datasrc_desc);
  void (*close)(struct CedarXDemuxerAPI *handle);
  int  (*prefetch)(struct CedarXDemuxerAPI *handle, CdxPacketT *cdx_pkt);
  int  (*read)(struct CedarXDemuxerAPI *handle, CdxPacketT *cdx_pkt);
  int (*seek)(struct CedarXDemuxerAPI *handle, int64_t abs_seek_secs, int flags);
  int  (*control)(struct CedarXDemuxerAPI *handle, int cmd, int cmd_sub, void *arg);
  //while demux works in a single thread, it's neccessary to have an interface to stop it.
  void  (*stop)(struct CedarXDemuxerAPI *handle, void *arg);
} CedarXDemuxerAPI;

CedarXDemuxerAPI *cedarx_demux_create(CEDARX_SOURCETYPE nSourceType);
//CedarXDemuxerAPI *cdearx_demux_create(int demux_type, CedarXDataSourceDesc datasrc_desc);
void cedarx_demux_destory(CedarXDemuxerAPI *handle);

__END_DECLS

#endif /* CEDAR_DEMUX_H_ */
