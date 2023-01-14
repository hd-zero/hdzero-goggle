/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : plat_type.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/03/15
  Last Modified :
  Description   : The common data type defination, used in platform scope.
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_PLAT_TYPE_H__
#define __IPCLINUX_PLAT_TYPE_H__

#include <stdint.h>
#include <stddef.h>
#include <cdx_list_type.h>
#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

/*----------------------------------------------*
 * The common data type, will be used in the whole project.*
 *----------------------------------------------*/
//typedef int                 BOOL;
typedef int                 ERRORTYPE;
//typedef long long           int64_t;  //stdint.h
//typedef unsigned long long  uint64_t;

/*----------------------------------------------*
 * const defination                             *
 *----------------------------------------------*/
#ifndef BOOL
    typedef int BOOL;
#endif

#ifndef FALSE
   #define FALSE       0
#endif

#ifndef TRUE
   #define TRUE        1
#endif

// typedef enum {
    // FALSE = 0,
    // TRUE  = 1,
// } BOOL;

//#ifndef NULL
//#   ifdef __cplusplus
//#       define NULL            (0)
//#   else
//#       define NULL            ((void *)0)
//#   endif
//#endif

#define SUCCESS  0
#define FAILURE  (-1)

/** OMX_IN is used to identify inputs to an OMX function.  This designation 
    will also be used in the case of a pointer that points to a parameter 
    that is used as an output. */
#ifndef PARAM_IN
#define PARAM_IN
#endif

/** OMX_OUT is used to identify outputs from an OMX function.  This 
    designation will also be used in the case of a pointer that points 
    to a parameter that is used as an input. */
#ifndef PARAM_OUT
#define PARAM_OUT
#endif


/** OMX_INOUT is used to identify parameters that may be either inputs or
    outputs from an OMX function at the same time.  This designation will 
    also be used in the case of a pointer that  points to a parameter that 
    is used both as an input and an output. */
#ifndef PARAM_INOUT
#define PARAM_INOUT
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* __IPCLINUX_PLAT_TYPE_H__ */

