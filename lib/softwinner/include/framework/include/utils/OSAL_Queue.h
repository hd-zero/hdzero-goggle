#ifndef __OSAL_QUEUE__
#define __OSAL_QUEUE__

#include <plat_type.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _OSAL_QElem
{
    void *data;
    //struct _OSAL_QElem *qNext;
    struct list_head mList;
} OSAL_QElem;

typedef struct _OSAL_QUEUE
{
    //OSAL_QElem *first;
    //OSAL_QElem *last;
    //int numElem;
    struct list_head mIdleElemList;  //OSAL_QElem
    struct list_head mValidElemList;
    void *qMutex;
    int maxElem;
} OSAL_QUEUE;

int OSAL_QueueCreate(OSAL_QUEUE *queueHandle, int maxQueueElem);
int OSAL_QueueTerminate(OSAL_QUEUE *queueHandle);
int OSAL_Queue(OSAL_QUEUE *queueHandle, void *data);
void *OSAL_Dequeue(OSAL_QUEUE *queueHandle);
int OSAL_GetElemNum(OSAL_QUEUE *queueHandle);
int OSAL_QueueSetElem(OSAL_QUEUE *queueHandle, void *data);

#ifdef __cplusplus
}
#endif

#endif	// __OSAL_QUEUE__

