/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mm_comm_sys.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/04/21
  Last Modified :
  Description   : chip specific configure data structure
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_COMM_SYS_H__
#define __IPCLINUX_COMM_SYS_H__

#include "plat_type.h"
#include "plat_errno.h"

#include <sys/select.h>
#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

//#define HI_TRACE_SYS(level, fmt...) HI_TRACE(level, HI_ID_SYS,##fmt)
typedef struct MPP_SYS_CONF_S
{
    /* stride of picture buffer must be aligned with this value.
     * you can choose a value from 1 to 1024, and it must be multiple of 16.
     */
    unsigned int nAlignWidth;
    char mkfcTmpDir[256];
} MPP_SYS_CONF_S;

typedef struct SYS_VIRMEM_INFO_S
{
    unsigned int  u32PhyAddr;
    BOOL bCached;
} SYS_VIRMEM_INFO_S;

typedef struct MppBindControl {
    int eDomain;
}MppBindControl;

typedef fd_set handle_set;

#define ERR_SYS_NULL_PTR         DEF_ERR(MOD_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define ERR_SYS_NOTREADY         DEF_ERR(MOD_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define ERR_SYS_NOT_PERM         DEF_ERR(MOD_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define ERR_SYS_NOMEM            DEF_ERR(MOD_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define ERR_SYS_ILLEGAL_PARAM    DEF_ERR(MOD_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define ERR_SYS_BUSY             DEF_ERR(MOD_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define ERR_SYS_NOT_SUPPORT      DEF_ERR(MOD_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* __IPCLINUX_COMM_SYS_H__ */

