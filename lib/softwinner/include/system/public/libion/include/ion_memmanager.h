/*
 *  ion.c
 *
 * Memory Allocator functions for ion
 *
 *   Copyright 2011 Google, Inc
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef __SYS_ION_MEM_MANAGER_H__
#define __SYS_ION_MEM_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/ion_uapi.h>

typedef enum 
{
	IonHeapType_CARVEOUT = 0,   //carveout or cma
	//IonHeapType_CMA,
	IonHeapType_IOMMU = 3,
	IonHeapType_UNKNOWN = -1,
}IonHeapType;
typedef struct IonAllocAttr
{
    unsigned int mLen;
	unsigned int mAlign;
	IonHeapType mIonHeapType;
	int mbSupportCache; //if ion memory attach cache attribute. 1:support; 0:disable
}IonAllocAttr;

int ion_memOpen(void);
int ion_memClose(void);
unsigned char* ion_allocMem(unsigned int size);
unsigned char* ion_allocMem_extend(IonAllocAttr *pAttr);
int ion_freeMem(void *vir_ptr);
unsigned int ion_getMemPhyAddr(void *vir_ptr);
int ion_getMemMapFd(void *vir_ptr);
ion_user_handle_t ion_getMemHandle(void *vir_ptr); 
IonHeapType ion_getMemHeapType(void *vir_ptr);
int ion_flushCache(void *vir_ptr, unsigned int size);
//int ion_memGetTotalSize();

#ifdef __cplusplus
}
#endif


#endif /* __SYS_ION_MEM_MANAGER_H__ */
