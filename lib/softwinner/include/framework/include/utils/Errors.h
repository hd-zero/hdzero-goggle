/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : Errors.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/05/03
  Last Modified :
  Description   :
  Function List :
  History       :
******************************************************************************/

#ifndef __IPCLINUX_ERRORS_H__
#define __IPCLINUX_ERRORS_H__

#include <sys/types.h>
#include <errno.h>

namespace EyeseeLinux {

// use this type to return error codes
typedef int status_t;

/*
 * Error codes. 
 * All error codes are negative values.
 */
enum {
    OK                = 0,    // Everything's swell.
    NO_ERROR          = 0,    // No errors.
    
    UNKNOWN_ERROR       = 0x80000000,

    NO_MEMORY           = -ENOMEM,
    INVALID_OPERATION   = -ENOSYS,
    BAD_VALUE           = -EINVAL,
    BAD_TYPE            = 0x80000001,
    BAD_FILE            = 0x80000002,
    NAME_NOT_FOUND      = -ENOENT,
    PERMISSION_DENIED   = -EPERM,
    NO_INIT             = -ENODEV,
    ALREADY_EXISTS      = -EEXIST,
    DEAD_OBJECT         = -EPIPE,
    FAILED_TRANSACTION  = 0x80000002,
    JPARKS_BROKE_IT     = -EPIPE,
    BAD_INDEX           = -EOVERFLOW,
    NOT_ENOUGH_DATA     = -ENODATA,
    WOULD_BLOCK         = -EWOULDBLOCK,
    TIMED_OUT           = -ETIMEDOUT,
    UNKNOWN_TRANSACTION = -EBADMSG,
    FDS_NOT_ALLOWED     = 0x80000007,
};

enum ModuleIdPrefixMark
{
    RecorderIdPrefixMark    = 0x00<<16,
    ISEIdPrefixMark         = 0x01<<16,
//by andy
    BDIIIdPrefixMark        = 0x02<<16,
    EISIdPrefixMark        =  0x03<<16,
};

}; // namespace EyeseeLinux

// ---------------------------------------------------------------------------

#endif // __IPCLINUX_ERRORS_H__
