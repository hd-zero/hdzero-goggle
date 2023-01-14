/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : EyeseeBDII.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/05
  Last Modified :
  Description   : EyeseeBDII module.
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_EYESEEBDIICOMMON_H__
#define __IPCLINUX_EYESEEBDIICOMMON_H__

#include<EyeseeCamera.h>
#include "aw_ai_cve_bdii_config.h"
#include "aw_ai_cve_bdii_interface.h"


namespace EyeseeLinux {
typedef enum {
    BDII_CAMERA_LEFT = 0,
    BDII_CAMERA_RIGHT = 1,
}BDII_CAMERA_STYLE;

class BDIICameraChnInfo
{
public:
    EyeseeCamera *mpCamera;
    int mnCameraChannel;
    BDII_CAMERA_STYLE mCamStyle;
};

typedef struct CVE_BDII_RES_OUT_S
{
    AW_AI_CVE_BDII_RULT_S result;
    unsigned int deepImgSize;
    unsigned int costImgSize;
    SIZE_S mImgSize;
    uint64_t mPts;  //unit:us
}CVE_BDII_RES_OUT_S;


class BDIIDebugFileOutString
{
public:
    std::string fileOutStr;
    enum DEBUG_OUT_FILE_STYLE{
        FILE_STR_LEFT_YUV   = 0,
        FILE_STR_RIGHT_YUV  = 1,
        FILE_STR_RESULT_DEEP_YUV = 2,
        FILE_STR_RESULT_COST_YUV = 3,
    };
    DEBUG_OUT_FILE_STYLE fileOutStyle;
};


}; /* namespace EyeseeLinux */

#endif /* __IPCLINUX_EYESEEBDIICOMMON_H__ */

