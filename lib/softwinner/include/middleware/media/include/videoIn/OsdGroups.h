/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : OsdGroups.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2017/07/31
  Last Modified :
  Description   :
  Function List :
  History       :
******************************************************************************/
#ifndef _OSDGROUPS_H_
#define _OSDGROUPS_H_

#include <plat_type.h>

#include <mm_common.h>
#include <mm_comm_region.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    RGN_TYPE_E mType;
    RECT_S mRect; /* overlay or cover win */
    union
    {
        struct
        {
            void *mBitmap;
            BOOL mbInvColEn;    //overlay color invert enable
            int mPriority;  //overlay:[0,VI_MAX_OVERLAY_NUM-1]
        } mOverlay;
        struct
        {
            int mChromaKey; //color value in mode cover. ARGB
            int mPriority;  //cover:[0,VI_MAX_COVER_NUM-1]
        } mCover;
    } mInfo;
    struct list_head mList;
}OsdRegion;

typedef struct
{
    struct list_head mOsdList;  //OsdRegion
    struct list_head mRedrawOsdList;  //OsdRegion
    struct list_head mList;
}OsdGroup;

typedef struct
{
    int mPixFmt;  //V4L2_PIX_FMT_RGB32, for overlay
    int mGlobalAlpha;// for overlay, [0,MAX_GLOBAL_ALPHA]
    struct list_head mGroupList;  //OsdGroup
}OsdGroups;

OsdGroup* OsdGroupConstruct();
void OsdGroupDestruct(OsdGroup *pGroup);
OsdGroups* OsdGroupsConstruct();
void OsdGroupsDestruct(OsdGroups* pThiz);

#ifdef __cplusplus
}
#endif

#endif  /* _OSDGROUPS_H_ */

