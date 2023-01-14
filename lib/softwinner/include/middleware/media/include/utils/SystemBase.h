/*******************************************************************************
--                                                                            --
--                    CedarX Multimedia Framework                             --
--                                                                            --
--          the Multimedia Framework for Linux/Android System                 --
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                         Softwinner Products.                               --
--                                                                            --
--                   (C) COPYRIGHT 2011 SOFTWINNER PRODUCTS                   --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
*******************************************************************************/

#ifndef __IPCLINUX_SYSTEMBASE_H__
#define __IPCLINUX_SYSTEMBASE_H__

//ref platform headers
#include <stdint.h>
#include "plat_type.h"
#include <pthread.h>

#ifdef  __cplusplus
extern "C" {
#endif
/**
 * condition must set condAttr clock to CLOCK_MONOTONIC
 */
int pthread_cond_wait_timeout(pthread_cond_t* const condition, pthread_mutex_t* const mutex, unsigned int msecs);
int64_t CDX_GetSysTimeUsMonotonic();
int CDX_SetTimeUs(int64_t timeUs);
int64_t CDX_GetTimeUs(void);
void dumpCallStack(const char* pTag);

#ifdef  __cplusplus
}
#endif
#endif /* __IPCLINUX_SYSTEMBASE_H__ */

