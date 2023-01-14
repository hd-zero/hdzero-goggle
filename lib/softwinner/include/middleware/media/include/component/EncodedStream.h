/******************************************************************************
  Copyright (C), 2001-2018, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : EncodedStream.h
  Version       : Initial Draft
  Author        : Allwinner BU3-XAPSW Team
  Created       : 2018/01/03
  Last Modified :
  Description   : common struct definition in Demux, VEnc and AEnc.
  Function List :
  History       :
******************************************************************************/

#ifndef _ENCODEDSTREAM_H_
#define _ENCODEDSTREAM_H_

#include <plat_type.h>
#include <vencoder.h>


#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

#define CEDARV_FLAG_KEYFRAME            0x00000001
#define CEDARV_FLAG_EOS                 0x00000002
#define CEDARV_FLAG_PTS_VALID           0x00000004
#define CEDARV_FLAG_FIRST_PART          0x00000008
#define CEDARV_FLAG_LAST_PART           0x00000010 
#define CEDARV_FLAG_THUMB               0x00000020  //indicate EncodedStream has thumb picture, only used for jpeg.

typedef enum CDX_PACKETTYPE
{
    CDX_PacketUnkown = 0,
    CDX_PacketVideo,
    CDX_PacketAudio,
    CDX_PacketSubtitle,
    CDX_PacketImage
} CDX_PACKETTYPE;

typedef struct _EncodedStream
{
    int media_type;              // indicate video/audio/subtitle, CDX_PACKETTYPE
    int nID;				     // BufID
    int nFilledLen;              // data size. For dmx->vdec, -1 means buffer is not enough.
    int nTobeFillLen;			 // required buffer size
    int64_t nTimeStamp;          // TimeStamp
    unsigned int nFlags;         // Cedarv Flags, ref definition above

    unsigned char* pBuffer;         //maybe vdec provide.
    unsigned char* pBufferExtra;    //maybe vdec provide.
    unsigned char* pBufferExtra2;   //jpeg encode may need three data buffer.
    unsigned int nBufferLen;        //maybe vdec provide.
    unsigned int nBufferExtraLen;   //maybe vdec provide.
    unsigned int nBufferExtraLen2;

    unsigned int video_stream_type;   // indicate VIDEO_TYPE_MAJOR, CDX_VIDEO_STREAM_MAJOR
    FrameInfo video_frame_info;       // video frame info from venc lib

    int infoVersion;                // demuxer version, default value is -1
    void *pChangedStreamsInfo;      // VideoInfo/AudioInfo/SubtitleInfo, from demuxer
    int64_t duration;               // only for subtitle, default value is -1
}EncodedStream;

typedef struct encoder_node_t
{
    EncodedStream stEncodedStream;
    int mUsedRefCnt;  // mux component use it to record used ref count.
    struct list_head mList;
}ENCODER_NODE_T;

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif  /* _ENCODEDSTREAM_H_ */

