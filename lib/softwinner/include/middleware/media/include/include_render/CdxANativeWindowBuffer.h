/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _CDXANATIVEWINDOWBUFFER_H_
#define _CDXANATIVEWINDOWBUFFER_H_

#include <mm_common.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum tag_VideoRenderMode
{
    VideoRender_HW      = 0,    //direct set to hardware composer
    VideoRender_SIMUHW  = 1,    //copy to gui_buffer, the method is like set hwlayer, so name as simulate hw.
    VideoRender_SW      = 2,    //dequeue, convert to gui_buf, enqueue
    VideoRender_GUI     = 3,    //display frame is malloc from GUI. 
}VideoRenderMode;

typedef struct
{
    void *frame_info;   //VideoPicture*
    int frame_id;
}CdxVRRenderDataPara;

typedef struct
{
    //void *frame_info;   //VideoPicture*
    //int frame_id;
    void *pANativeWindowBuffer; //ANativeWindowBufferCedarXWrapper*
}CdxVRDequeueFramePara;

typedef struct
{
    int mnBufWidth;
	int mnBufHeight;
    int mnDisplayTopX;
    int mnDisplayTopY;
	int mnDisplayWidth;
	int mnDisplayHeight;
	int mePixelFormat;  //PIXEL_FORMAT_YV12
	int mColorSpace;  //VENC_BT601
	VO_LAYER mVoLayer;
    int mVideoScalingMode;  //tmp not use it, NATIVE_WINDOW_SCALING_MODE_SCALE_TO_WINDOW
}CdxVRHwcLayerInitPara; //for VideoRender_HW

typedef struct
{
	int mnBufWidth;
	int mnBufHeight;
	int mnDisplayWidth;
	int mnDisplayHeight;
	int mePixelFormat;  //PIXEL_FORMAT_YV12
}CdxVRSWANWInitPara;    //for VideoRender_SW

typedef struct
{
	int mnBufNum;
	int mnBufWidth;
	int mnBufHeight;
	int mePixelFormat;  //PIXEL_FORMAT_YV12
	int mnAlignValue;   //16, 32
    int mbProgressiveFlag;
    //int mbIsSoftDecoderFlag;
}CdxVRANWInitPara;  //for VideoRender_GUI

typedef struct
{
    int mnDisplayTopX;
    int mnDisplayTopY;
	int mnDisplayWidth;
	int mnDisplayHeight;
    int mnBufWidth;
    int mnBufHeight;
}CdxVRFrameInfo;    //for VideoRender_GUI

typedef struct tag_ANativeWindowBufferCedarXWrapper //ANativeWindowBuffer
{
    int width;
    int height;
    int stride;
    int format; //CDX_PIXEL_FORMAT_AW_MB422
    int usage;

    void*   dst;    //memory address
    void*   dstPhy; //Physical memory address for vdec, (CPUPhyAddress - PHY_OFFSET)
    
    void*   pObjANativeWindowBuffer;   //ANativeWindowBuffer
    int mIonUserHandle;   //ion_user_handle_t

    int mbOccupyFlag;   //1:owned by us, 0:owned by Android Native Window
} ANativeWindowBufferCedarXWrapper; //for VideoRender_GUI

#define ANW_BUFFER_NUM 32
typedef struct
{
	int mnBufNum;
    ANativeWindowBufferCedarXWrapper mANWBuffers[ANW_BUFFER_NUM];
}CdxANWBuffersInfo; //for VideoRender_GUI

typedef struct
{
    int nRenderMode;    //VideoRender_HW
    void *pIn;
    void *pOut;
}CdxEventVideoRenderInitPara;   //for callback transmit param.

#ifdef __cplusplus
}
#endif

#endif  /* _CDXANATIVEWINDOWBUFFER_H_ */

