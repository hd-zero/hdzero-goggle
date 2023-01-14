/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : PIXEL_FORMAT_E_g2d_format_convert.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2017/04/10
  Last Modified :
  Description   : PIXEL_FORMAT_E g2d_data_fmt,g2d_pixel_seq conversion.
  Function List :
  History       :
******************************************************************************/
#ifndef _EPIXELFORMAT_G2D_FORMAT_CONVERT_H_
#define _EPIXELFORMAT_G2D_FORMAT_CONVERT_H_

#include <stdbool.h>
#include <plat_type.h>
#include <vdecoder.h>
#include <asm-generic/int-ll64.h>
#include <linux/g2d_driver.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

ERRORTYPE convert_EPIXELFORMAT_to_G2dFormat(
    PARAM_IN enum EPIXELFORMAT ePixelFormat, 
    PARAM_OUT g2d_data_fmt *pG2dFormat, 
    PARAM_OUT g2d_pixel_seq *pG2dPixelSeq);

ERRORTYPE convert_EPIXELFORMAT_to_g2d_fmt_enh(
    PARAM_IN enum EPIXELFORMAT ePixelFormat, 
    PARAM_OUT g2d_fmt_enh *pG2dFormat);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* _EPIXELFORMAT_G2D_FORMAT_CONVERT_H_ */

