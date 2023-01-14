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

#ifndef RECORD_WRITER_H_
#define RECORD_WRITER_H_

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum MUXERMODES {
	MUXER_MODE_MP4 = 0,
    MUXER_MODE_MP3,
    MUXER_MODE_AAC,
	MUXER_MODE_AWTS,
	MUXER_MODE_RAW,
	MUXER_MODE_TS,

	MUXER_MODE_USER0,
	MUXER_MODE_USER1,
	MUXER_MODE_USER2,
	MUXER_MODE_USER3,
}MUXERMODES;

typedef enum RECODERWRITECOMMANDS
{
	UNKOWNCMD = 0,
    SETAVPARA,  //_media_file_inf_t*
	SETTOTALTIME,
	SETFALLOCATELEN,    //set fallocate size for fd.
	SETCACHEFD,
	SETCACHEFD2,
	SETOUTURL,
	REGISTER_WRITE_CALLBACK,
	SETSDCARDSTATE,
	SETCACHEMEM,
	SET_FS_WRITE_MODE,
	SET_FS_SIMPLE_CACHE_SIZE,
	SET_STREAM_CALLBACK,

	/* gushiming compressed source */
	//SET_VIDEO_CODEC_ID,
	//SET_AUDIO_CODEC_ID,
} RECODERWRITECOMMANDS;

enum CodecType {
    CODEC_TYPE_UNKNOWN = -1,
    CODEC_TYPE_VIDEO = 0,//don't change it!
    CODEC_TYPE_AUDIO = 1,//don't change it!
    CODEC_TYPE_TEXT = 2,	//add by zjz
    CODEC_TYPE_DATA
};

enum CodecID {
    CODEC_ID_MPEG4 = 32,
    CODEC_ID_H264 = 33,
    CODEC_ID_H265 = 34,
    CODEC_ID_MJPEG = 108,
    CODEC_ID_AAC = 64,
    CODEC_ID_MP3 = 105,
    CODEC_ID_PCM,   // 16-bit little-endian, twos-complement
    CODEC_ID_ADPCM, // DVI/Intel IMAADPCM-ACM code 17
    CODEC_ID_GGAD = 150,	// add by zjz for text track, Gps, Gsensor, Adas, Driver_id.
};

enum AVPACKET_FLAGS {
	AVPACKET_FLAG_KEYFRAME = 1,
};

typedef struct AVPacket {
    int64_t pts;    //unit: different muxer has different unit, ts muxer is ms, others is us.
    int64_t dts;
    char  *data0;
    int size0;
    char  *data1;
    int size1;
    int stream_index;
    int flags;  //AVPACKET_FLAGS
    int duration;   ///< presentation duration in time_base units (0 if not available), unit:ms
    int64_t pos;        ///< byte position in stream, -1 if unknown

    //for video frame.
    int CurrQp;         //qp of current frame
    int avQp;
    int nGopIndex;      //index of gop
    int nFrameIndex;    //index of current frame in gop.
    int nTotalIndex;    //index of current frame in whole encoded frames
} AVPacket;

typedef enum RawPacketType
{
	RawPacketTypeVideo = 0,
	RawPacketTypeAudio,

	RawPacketTypeVideoExtra = 128,
	RawPacketTypeAudioExtra
}RawPacketType;

typedef struct RawPacketHeader
{
	int  stream_type;   //RawPacketType
	int  size;
	long long pts;  //unit:us
    //for video frame.
    int CurrQp;         //qp of current frame
    int avQp;
	int nGopIndex;      //index of gop
	int nFrameIndex;    //index of current frame in gop
	int nTotalIndex;    //index of current frame in whole encoded frames
}__attribute__((packed)) RawPacketHeader;

typedef struct TSPacketHeader
{
	int  stream_type;   //RawPacketType
	int  size;
	long long pts;  //unit:ms
}__attribute__((packed)) TSPacketHeader;

typedef struct _media_file_inf_t
{
	// video 
    int nHeight;
	int nWidth;
	int uVideoFrmRate;  // *1000
	int create_time;
	int maxKeyInterval;
    int mVideoEncodeType;   //VENC_CODEC_H264

	// audio
	int channels;			
	int bits_per_sample;	
	int frame_size; //sample_cnt_per_frame
	int sample_rate;	
	int audio_encode_type;  //AUDIO_ENCODER_AAC_TYPE

	// text
	int text_encode_type;	//TEXT_ENCODER_GGAD
	int geo_available;
	int latitudex10000;
	int longitudex10000;	

	int rotate_degree;
}_media_file_inf_t;

typedef struct CDX_RecordWriter{
	const char *info;

	void*   (*MuxerOpen)(int *ret);
	int (*MuxerClose)(void *handle);
	int (*MuxerWriteExtraData)(void *handle, unsigned char *vosData, unsigned int vosLen, unsigned int idx);
	int (*MuxerWriteHeader)(void *handle);
	int (*MuxerWriteTrailer)(void *handle);
	int (*MuxerWritePacket)(void *handle, void *pkt);
	int (*MuxerIoctrl)(void *handle, unsigned int uCmd, unsigned int uParam, void* pParam2);
}CDX_RecordWriter;

CDX_RecordWriter *cedarx_record_writer_create(int mode);
void cedarx_record_writer_destroy(void *handle);

#if defined(__cplusplus)
}
#endif

#endif
