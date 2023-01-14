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
#ifndef _RECORDERMODE_H_
#define _RECORDERMODE_H_

#include <record_writer.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
    RECORDER_MODE_NONE      = 0,
	RECORDER_MODE_AUDIO		= 1<<0	,		// only audio recorder
	RECORDER_MODE_VIDEO		= 1<<1	,		// only video recorder
	RECORDER_MODE_TEXT		= 1<<2	,		// only text recorder
	RECORDER_MODE_CAMERA	= (RECORDER_MODE_AUDIO | RECORDER_MODE_VIDEO),		// audio and video recorder
}RECORDER_MODE;

typedef enum {
    RecordFileDurationPolicy_MinDuration        = 0,    //file duration must large than minDuration. guarantee next frame is I frame.
    RecordFileDurationPolicy_AverageDuration,           //take user value as average file duration. guarantee next frame is I frame.
    RecordFileDurationPolicy_AccurateDuration,          //immediately finish current file. not guarantee next frame is I frame.
}RecordFileDurationPolicy;

typedef struct CdxOutputSinkInfo
{
    int                 mMuxerId;
    MUXERMODES          nMuxerMode;
    int                 nOutputFd;
    //char                *mPath;
    int                 nFallocateLen;
    int                 nCallbackOutFlag;
    BOOL                bBufFromCacheFlag;
    struct list_head    mList;
}CdxOutputSinkInfo;

#ifdef __cplusplus
}
#endif

#endif  /* _RECORDERMODE_H_ */
/* File EOF */
