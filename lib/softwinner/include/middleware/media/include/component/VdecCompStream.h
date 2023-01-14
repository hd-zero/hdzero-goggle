/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : VdecCompStream.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/12
  Last Modified :
  Description   : common struct definition in VdecComp and MPIVdec.
  Function List :
  History       :
******************************************************************************/

#ifndef _VDECCOMPSTREAM_H_
#define _VDECCOMPSTREAM_H_

#include <plat_type.h>
#include <vdecoder.h>
#include <CdxANativeWindowBuffer.h>

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */


typedef struct VDecCompOutputFrame
{
    VideoPicture    *mpPicture; //unit:us
    VideoPicture    *mpSubPicture;
    struct list_head mList;
}VDecCompOutputFrame;

typedef struct 
{
    CdxANWBuffersInfo *mpANWBuffersInfo;  //[in]
    struct list_head mFramesOwnedByANW;   //[out], VDecCompOutputFrame
}VDecCompFrameBuffersParam;


#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif  /* _VDECCOMPSTREAM_H_ */

