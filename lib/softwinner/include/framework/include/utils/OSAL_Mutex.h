#ifndef __OSAL_MUTEX__
#define __OSAL_MUTEX__

#ifdef __cplusplus
extern "C" {
#endif

int OSAL_MutexCreate(void **mutexHandle);
int OSAL_MutexTerminate(void *mutexHandle);
int OSAL_MutexLock(void *mutexHandle);
int OSAL_MutexUnlock(void *mutexHandle);

#ifdef __cplusplus
}
#endif

#endif

