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

#ifndef VIDEO_RENDER_H_
#define VIDEO_RENDER_H_

#include <CdxANativeWindowBuffer.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct CDX_VideoRenderHAL{
	const char *info;
	void *callback_info;
    void *mPriv;
	//int  vr4sft_first_frame;

	int (*init)(struct CDX_VideoRenderHAL *handle, /*in*/int nRenderMode, /*in*/void *pIn, /*out*/void *pOut); //nRenderMode = VideoRender_SW,...
	void (*exit)(struct CDX_VideoRenderHAL *handle);
	int (*render)(struct CDX_VideoRenderHAL *handle, void *frame_info, int frame_id);
	int (*get_disp_frame_id)(struct CDX_VideoRenderHAL *handle);
    int (*set_showflag)(struct CDX_VideoRenderHAL *handle, int bShowFlag);

    int (*dequeue_frame)(struct CDX_VideoRenderHAL *handle, void *pANativeWindowBuffer);    //request frame from gpu, ANativeWindowBufferCedarXWrapper
    int (*enqueue_frame)(struct CDX_VideoRenderHAL *handle, void *pANativeWindowBuffer);    //release frame to gpu, ANativeWindowBufferCedarXWrapper
    int (*cancel_frame)(struct CDX_VideoRenderHAL *handle, void *pANativeWindowBuffer);    //cancel frame to gpu, ANativeWindowBufferCedarXWrapper
    int (*update_display_size)(struct CDX_VideoRenderHAL *handle, void *pInfo);    //update frame display size.
}CDX_VideoRenderHAL;

CDX_VideoRenderHAL *cedarx_video_render_create(void *callback_info);
void cedarx_video_render_destroy(void *handle);

#ifdef __cplusplus
}
#endif

#endif
