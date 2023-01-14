/******************************************************************************
 Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
 File Name     : VideoISECompStream.h
 Version       : Initial Draft
 Author        : Allwinner BU3-PD2 Team
 Created       : 2017/01/13
 Last Modified :
 Description   :
 Function List :
 History       :
 ******************************************************************************/
#ifndef _VIDEOISECOMPSTREAM_H_
#define _VIDEOISECOMPSTREAM_H_

// ref platform headers
#include "plat_defines.h"
#include "plat_errno.h"
#include "plat_math.h"
#include "plat_type.h"

typedef struct VIseCompOutputBuffer {
    // VencOutputBuffer mOutBuf;   //unit:us
    int mUsedRefCnt;  // enc component use it to record used ref count.
    struct list_head mList;
} VIseCompOutputBuffer;

typedef struct IseChnAttr {
    ISE_GRP mGrpId;
    ISE_CHN mChnId;
    ISE_CHN_ATTR_S *pChnAttr;
} IseChnAttr;

#endif  /* _VIDEOISECOMPSTREAM_H_ */

