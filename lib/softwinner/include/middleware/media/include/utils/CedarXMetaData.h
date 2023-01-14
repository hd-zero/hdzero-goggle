/*
********************************************************************************
*                           Android multimedia module
*
*          (c) Copyright 2010-2015, Allwinner Microelectronic Co., Ltd.
*                              All Rights Reserved
*
* File   : CedarXMetaData.h
* Version: V1.0
* By     : eric_wang
* Date   : 2015-9-14
* Description:
    extend meta data key.
********************************************************************************
*/
#ifndef _CEDARXMETADATA_H_
#define _CEDARXMETADATA_H_

#include <MetaData.h>

namespace EyeseeLinux {

enum CedarXMetaDataKey {
    kCedarXKeyFrameCount = 'fcnt',  // int32_t, gpu frame count
    kCedarXKeyColorSpace = 'ColS',   //color space
    kCedarXKeyVideoLayer = 'VLay',   //video layer id.
    kCedarXKeyDisplayTopX   = 'topX',  // int32_t, displayTopX
    kCedarXKeyDisplayTopY   = 'topY',  // int32_t, displayTopY
};

}  // namespace android

#endif  /* _CEDARXMETADATA_H_ */

