/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : dup2SeldomUsedFd.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2017/04/12
  Last Modified :
  Description   : use dup2() to dup a fd to seldom-used number
  Function List :
  History       :
******************************************************************************/
#ifndef _DUP2SELDOMUSEDFD_H_
#define _DUP2SELDOMUSEDFD_H_

#include <plat_type.h>
#include <plat_errno.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * return type:
 *  ERR_COMMON_UNEXIST: srdFd is invalid.
 *  ERR_COMMON_SYS_NOTREADY: can't find dstFd to dup2()
 *  fd >= 0
 */
int dup2SeldomUsedFd(int nSrcFd);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* _DUP2SELDOMUSEDFD_H_ */

