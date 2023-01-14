/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : VdecStream.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/12
  Last Modified :
  Description   : common struct definition in VdecComp and MPIVdec.
  Function List :
  History       :
******************************************************************************/
#ifndef _VIDEOFRAMEINFONODE_H_
#define _VIDEOFRAMEINFONODE_H_

#include <plat_type.h>

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

typedef struct VideoFrameInfoNode
{
    VIDEO_FRAME_INFO_S VFrame;
    struct list_head mList;
}VideoFrameInfoNode;

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif  /* _VIDEOFRAMEINFONODE_H_ */

