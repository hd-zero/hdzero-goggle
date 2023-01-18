#ifndef __DD_AGPS_H__
#define __DD_AGPS_H__
#include <tsemaphore.h>
int create_GET_AGPS_thread();
extern cdx_sem_t mSemGetAgpsStart;
extern int agps_readback_data_size;
#endif
