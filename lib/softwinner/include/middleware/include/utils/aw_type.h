/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : aw_type.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2017/03/04
  Last Modified :
  Description   : The common data type defination for some aw internal libs, used in platform scope.
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_AW_TYPE_H__
#define __IPCLINUX_AW_TYPE_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

//----------------------------------------------
// The common data type, will be used in the whole project.
//----------------------------------------------
typedef unsigned char           AW_U8;
typedef unsigned short          AW_U16;
typedef unsigned int            AW_U32;
typedef char                    AW_S8;
typedef short                   AW_S16;
typedef int                     AW_S32;
typedef float 					AW_FLOAT;
typedef char                    AW_CHAR;
typedef void*                   AW_HANDLE;
typedef void*                   AW_PVOID;
typedef double                  AW_DOUBLE;

#ifndef _M_IX86
    typedef unsigned long long  AW_U64;
    typedef long long           AW_S64;
#else
    typedef __int64             AW_U64;
    typedef __int64             AW_S64;
#endif

typedef enum {
    AW_FALSE = 0,
    AW_TRUE = 1,
} AW_BOOL;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* __IPCLINUX_AW_TYPE_H__ */

