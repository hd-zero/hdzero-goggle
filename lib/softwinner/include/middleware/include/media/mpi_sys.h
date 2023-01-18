/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_sys.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/04/21
  Last Modified :
  Description   :
  Function List :
  History       :
******************************************************************************/

#ifndef __IPCLINUX_MPI_SYS_H__
#define __IPCLINUX_MPI_SYS_H__

#include "plat_type.h"
#include "mm_common.h"
#include "mm_comm_sys.h"

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

ERRORTYPE AW_MPI_SYS_Init_S1();
ERRORTYPE AW_MPI_SYS_Init_S2();
ERRORTYPE AW_MPI_SYS_Init_S3();
ERRORTYPE AW_MPI_SYS_Init();
ERRORTYPE AW_MPI_SYS_Exit();

ERRORTYPE AW_MPI_SYS_SetConf(const MPP_SYS_CONF_S* pSysConf);
ERRORTYPE AW_MPI_SYS_GetConf(MPP_SYS_CONF_S* pSysConf);

ERRORTYPE  AW_MPI_SYS_Bind(MPP_CHN_S* pSrcChn, MPP_CHN_S* pDestChn);
ERRORTYPE  AW_MPI_SYS_BindExt(MPP_CHN_S* pSrcChn, MPP_CHN_S* pDestChn, MppBindControl* pBindControl);
ERRORTYPE  AW_MPI_SYS_UnBind(MPP_CHN_S* pstSrcChn, MPP_CHN_S* pstDestChn);
ERRORTYPE  AW_MPI_SYS_GetBindbyDest(MPP_CHN_S* pstDestChn, MPP_CHN_S* pstSrcChn);

ERRORTYPE AW_MPI_SYS_GetVersion(MPP_VERSION_S* pstVersion);

/*
** u64Base is the global PTS of the system.
** ADVICE:
** 1. Better to call AW_MPI_SYS_GetCurPts on the host board to get u64Base.
** 2. When os start up, call AW_MPI_SYS_InitPtsBase to set the init PTS.
** 3. When media bussines is running, synchronize the PTS one time per minute
**     by calling AW_MPI_SYS_SyncPts.
*/
ERRORTYPE AW_MPI_SYS_GetCurPts(uint64_t* pu64CurPts);
ERRORTYPE AW_MPI_SYS_InitPtsBase(uint64_t u64PtsBase);
ERRORTYPE AW_MPI_SYS_SyncPts(uint64_t u64PtsBase);

/* alloc mmz memory with cache in user context                                         */
ERRORTYPE AW_MPI_SYS_MmzAlloc_Cached(unsigned int* pPhyAddr, void** ppVirtAddr, unsigned int uLen);

/* free mmz memory in user context                                          */
ERRORTYPE AW_MPI_SYS_MmzFree(unsigned int uPhyAddr, void* pVirtAddr);

/* fulsh cache */
ERRORTYPE AW_MPI_SYS_MmzFlushCache(unsigned int u32PhyAddr, void* pVitAddr, unsigned int u32Size);

/*
** Call the mmap function to map physical address to virtual address
** The system function mmap is too complicated, so we packge it.
*/
//void* AW_MPI_SYS_Mmap(unsigned int u32PhyAddr, unsigned int u32Size);
//ERRORTYPE AW_MPI_SYS_Munmap(void* pVirAddr, unsigned int u32Size);

/*
** Access the physical address.
** You can use this function to access memory address or register address.
*/
//ERRORTYPE AW_MPI_SYS_SetReg(unsigned int u32Addr, unsigned int u32Value);
//ERRORTYPE AW_MPI_SYS_GetReg(unsigned int u32Addr, unsigned int* pu32Value);

//ERRORTYPE AW_MPI_SYS_SetMemConf(MPP_CHN_S* pstMppChn, const char* pcMmzName);
//ERRORTYPE AW_MPI_SYS_GetMemConf(MPP_CHN_S* pstMppChn, char* pcMmzName);

/* Close all the FD which is used by sys module */
//ERRORTYPE AW_MPI_SYS_CloseFd(void);

/* Get VI VPSS work mode, 0: offline, >0: online */
//ERRORTYPE AW_MPI_SYS_GetViVpssMode(unsigned int* pu32Mode);

/* media profile setting, for low power */
//ERRORTYPE AW_MPI_SYS_SetProfile(PROFILE_TYPE_E enProfile);

/* Get virtual meminfo according to virtual addr, should be in one process */
ERRORTYPE AW_MPI_SYS_GetVirMemInfo(const void* pVitAddr, SYS_VIRMEM_INFO_S* pstMemInfo);

ERRORTYPE AW_MPI_SYS_HANDLE_ZERO(handle_set *pHandleSet);
ERRORTYPE AW_MPI_SYS_HANDLE_SET(int handle, handle_set *pHandleSet);
ERRORTYPE AW_MPI_SYS_HANDLE_ISSET(int handle, handle_set *pHandleSet);
ERRORTYPE AW_MPI_SYS_HANDLE_Select(handle_set *pRdFds, int nMilliSecs);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif /*__IPCLINUX_MPI_SYS_H__ */

