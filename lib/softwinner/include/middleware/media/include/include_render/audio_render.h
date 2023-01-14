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

#ifndef AUDIO_RENDER_H_
#define AUDIO_RENDER_H_

typedef struct CDX_AudioRenderHAL{
	const char *info;
	void *callback_info;

	int (*init)(struct CDX_AudioRenderHAL *handle, int nSampleRate, int nChannelNum, int format);
	void (*exit)(struct CDX_AudioRenderHAL *handle, int immed);
	int (*render)(struct CDX_AudioRenderHAL *handle, void* data, int len);
	int (*get_delay)(struct CDX_AudioRenderHAL *handle);
	int (*get_space)(struct CDX_AudioRenderHAL *handle);    //get free space, unit:byte
	int (*flush_cache)(struct CDX_AudioRenderHAL *handle);
	int (*pause)(struct CDX_AudioRenderHAL *handle);
}CDX_AudioRenderHAL;

CDX_AudioRenderHAL *cedarx_audio_render_create(void *callback_info);
void cedarx_audio_render_destroy(void *handle);

#endif
