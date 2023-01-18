/*
* Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : sdecoder.h
* Description :
* History :
*   Author  : xyliu <xyliu@allwinnertech.com>
*   Date    : 2016/04/13
*   Comment :
*
*
*/

#ifndef SDECODER_H
#define SDECODER_H

#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PALETTEINFO
{
    int bValid;
    int nEntryCount;
    int entry[32];
}PaletteInfo;

typedef enum SUBTITLECODEC
{
    SUBTITLE_CODEC_UNKNOWN      = 0,
    SUBTITLE_CODEC_DVDSUB       = 0x100,
    SUBTITLE_CODEC_IDXSUB,
    SUBTITLE_CODEC_PGS,
    SUBTITLE_CODEC_DIVX,
    SUBTITLE_CODEC_DVBSUB,

    SUBTITLE_CODEC_TXT          = 0x200,
    SUBTITLE_CODEC_TIMEDTEXT,
    SUBTITLE_CODEC_SSA,
    SUBTITLE_CODEC_SMI,
    SUBTITLE_CODEC_SRT,

}ESubtitleCodec;

typedef enum SUBTITLETEXTFORMAT
{
    SUBTITLE_TEXT_FORMAT_UNKNOWN = 0,
    SUBTITLE_TEXT_FORMAT_UTF8,
    SUBTITLE_TEXT_FORMAT_GB2312,
    SUBTITLE_TEXT_FORMAT_UTF16LE,
    SUBTITLE_TEXT_FORMAT_UTF16BE,
    SUBTITLE_TEXT_FORMAT_UTF32LE,
    SUBTITLE_TEXT_FORMAT_UTF32BE,
    SUBTITLE_TEXT_FORMAT_BIG5,
    SUBTITLE_TEXT_FORMAT_GBK,
    SUBTITLE_TEXT_FORMAT_ANSI,
    //* TO BE ADD.

}ESubtitleTextFormat;

typedef enum SUBTITLEPIXELFORMAT
{
    SUBTITLE_PIXEL_FORMAT_ARGB,
    SUBTITLE_PIXEL_FORMAT_YUV,

}ESubtitlePixelFormat;

//*TimedText.java, public final class TimedText will copy the following definitions,
// so don't change value, if change, must modify TimedText.java to match.
typedef enum SUBTITLE_ALIGNMENT
{
    SUBTITLE_ALIGNMENT_UNKNOWN          = 0x0,
    SUBTITLE_ALIGNMENT_TOP_LEFT         = 0x11,
    SUBTITLE_ALIGNMENT_TOP_MIDDLE       = 0x12,
    SUBTITLE_ALIGNMENT_TOP_RIGHT        = 0x13,
    SUBTITLE_ALIGNMENT_MIDDLE_LEFT      = 0x21,
    SUBTITLE_ALIGNMENT_MIDDLE_MIDDLE    = 0x22,
    SUBTITLE_ALIGNMENT_MIDDLE_RIGHT     = 0x23,
    SUBTITLE_ALIGNMENT_BOTTOM_LEFT      = 0x31,
    SUBTITLE_ALIGNMENT_BOTTOM_MIDDLE    = 0x32,
    SUBTITLE_ALIGNMENT_BOTTOM_RIGHT     = 0x33,
}ESubtitleAlignment;

#define SUBTITLE_FONT_NAME_EPILOG           "epilog"
#define SUBTITLE_FONT_NAME_VERDANA          "verdana"
#define SUBTITLE_FONT_NAME_GEORGIA          "georgia"
#define SUBTITLE_FONT_NAME_ARIAL            "arial"
#define SUBTITLE_FONT_NAME_TIMES_NEW_ROMAN  "times new roman"

//* subtitle stream information.
#define SUBBITLE_DECODER_MAX_LANG_CHAR_SIZE 64
typedef struct SUBTITLESTREAMINFO
{
    int                 nStreamIndex;
    unsigned char       strLang[SUBBITLE_DECODER_MAX_LANG_CHAR_SIZE];   //* codec in ANSI

    //* stream source.
    int                 bExternal;
    char*               pUrl;
    int                 fd;
    int                 nFdOffset;
    int                 nFdLength;
    int                 fdSub;              //* for index+sub subtitle stream's .sub file.
    int                 nLanguageIndex;     //* select a stream in a multi stream text file,

    //* CODEC format.
    ESubtitleCodec      eCodecFormat;       //* how is the subtitle stream coded.
    ESubtitleTextFormat eTextFormat;        //* how is the text in subtitle stream coded.

    int                 nReferenceVideoWidth;
    int                 nReferenceVideoHeight;
    int                 nReferenceVideoFrameRate;
    PaletteInfo         sPaletteInfo;

    int                 nCodecSpecificDataLen;
    char*               pCodecSpecificData;
}SubtitleStreamInfo;

//* subtitle item for render.
typedef struct SUBTITLEITEM SubtitleItem;
struct SUBTITLEITEM
{
    int                  nItemId;   //* item id for buffer management.

    //* time control.
    int64_t              nPts;
    int64_t              nDuration;

    //* position control.
    int                  nReferenceVideoWidth;
    int                  nReferenceVideoHeight;
    ESubtitleAlignment   eAlignment;
    int                  nMarginL;
    int                  nMarginR;
    int                  nMarginV;
    int                  nStartX;    //* if eAlignment == SUBTITLE_ALIGNMENT_UNKNOWN,
    int                  nStartY;    //* use nStartX/nStartY/nEndX/nEndY to control
    int                  nEndX;      //* subtitle position.
    int                  nEndY;

    //* font style.
    int                  nFontSize;
    //* 0 means don't care.
    int                  bBold;
    //* -1 means don't care.
    int                  bItalic;
    //* -1 means don't care.
    int                  bUnderlined;
    //* -1 means don't care.
    unsigned int         nPrimaryColor;
    //* ARGB, 0 means don't care.
    unsigned int         nSecondaryColor;
    //* ARGB, 0 means don't care.
    char                 strFontName[SUBBITLE_DECODER_MAX_LANG_CHAR_SIZE];
    //* SUBTITLE_FONT_NAME_X,

    //* subtitle content.
    int                  bText;
    //* 0 means bitmap, 1 means text.
    char*                pText;
    int                  nTextLength;
    //* in unit of bytes.
    ESubtitleTextFormat  eTextFormat;
    char*                pBitmapData;
    int                  nBitmapWidth;
    //* for bitmap subtitle item.
    int                  nBitmapHeight;
    //* for bitmap subtitle item.
    ESubtitlePixelFormat ePixelFormat;
    //* currently output ARGB only.
};

typedef struct SUBTITLESTREAMDATAINFO
{
    char*   pData;
    int     nLength;
    int64_t nPts;
    int64_t nPcr;
    int64_t nDuration;
}SubtitleStreamDataInfo;

typedef void* SubtitleDecoder;

//* memory for stream info is allocated inside, user should free it.
int ProbeSubtitleStream(const char* strFilePath,
                    SubtitleStreamInfo** ppStreamInfo,
                    int* pStreamCount);

int ProbeSubtitleStreamFd(int fd,int offet,
                      int len,
                      SubtitleStreamInfo** ppStreamInfo,
                      int* pStreamCount);

//* create a subtitle decoder for a subtitle stream.
SubtitleDecoder* CreateSubtitleDecoder(SubtitleStreamInfo* pStreamInfo);

//* destroy a subtitle decoder.
void DestroySubtitleDecoder(SubtitleDecoder* p);

//* reset a decoder, for seek operation.
void ResetSubtitleDecoder(SubtitleDecoder* p, int64_t nSeekTime);

typedef enum ESUBTITLERESULT
{
    SDECODE_RESULT_UNSUPPORTED     = -2,
    SDECODE_RESULT_FAIL            = -1,
    SDECODE_RESULT_OK              = 0,
    SDECODE_RESULT_FRAME_DECODED   = 1,
    SDECODE_RESULT_NO_FRAME_BUFFER = 4,
}ESubtResult;

int DecodeSubtitleStream(SubtitleDecoder* p,
                     SubtitleStreamDataInfo* pStreamData);

SubtitleItem* NextSubtitleItem(SubtitleDecoder* p);

SubtitleItem* RequestSubtitleItem(SubtitleDecoder* p);

void FlushSubtitleItem(SubtitleDecoder* p, SubtitleItem* pItem);

#ifdef __cplusplus
}
#endif

#endif
