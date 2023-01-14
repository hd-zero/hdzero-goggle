#ifndef __SEMAPHORE_H_X6E3__
#define __SEMAPHORE_H_X6E3__

#include <pthread.h>

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

typedef struct cdx_sem_t
{
	pthread_cond_t 	condition;
	pthread_mutex_t mutex;
	unsigned int 	semval;
}cdx_sem_t;

int cdx_sem_init(cdx_sem_t* tsem, unsigned int val);
void cdx_sem_deinit(cdx_sem_t* tsem);
void cdx_sem_down(cdx_sem_t* tsem);
int cdx_sem_down_timedwait(cdx_sem_t* tsem, unsigned int timeout);
void cdx_sem_up(cdx_sem_t* tsem);
void cdx_sem_up_unique(cdx_sem_t* tsem);
void cdx_sem_reset(cdx_sem_t* tsem);
void cdx_sem_wait(cdx_sem_t* tsem);
void cdx_sem_timewait(cdx_sem_t* tsem, unsigned int msec);
void cdx_sem_signal(cdx_sem_t* tsem);
unsigned int cdx_sem_get_val(cdx_sem_t* tsem);


#define cdx_mutex_lock(x)	pthread_mutex_lock(x)
#define cdx_mutex_unlock(x)  pthread_mutex_unlock(x)

#if 0
#define cdx_cond_wait_while_exp(tsem, expression) \
	pthread_mutex_lock(&tsem.mutex); \
	while (expression) { \
		LOGV("sem_wait:%d",__LINE__); \
		pthread_cond_wait(&tsem.condition, &tsem.mutex); \
		LOGV("sem_wait end:%d",__LINE__); \
	} \
	pthread_mutex_unlock(&tsem.mutex);

#define cdx_cond_wait_if_exp(tsem, expression) \
	pthread_mutex_lock(&tsem.mutex); \
	if (expression) { \
		LOGV("sem_wait:%d",__LINE__); \
		pthread_cond_wait(&tsem.condition, &tsem.mutex); \
		LOGV("sem_wait end:%d",__LINE__); \
	} \
	pthread_mutex_unlock(&tsem.mutex);
#else

#define cdx_cond_wait_while_exp(tsem, expression) \
	pthread_mutex_lock(&tsem.mutex); \
	while (expression) { \
		pthread_cond_wait(&tsem.condition, &tsem.mutex); \
	} \
	pthread_mutex_unlock(&tsem.mutex);

#define cdx_cond_wait_if_exp(tsem, expression) \
	pthread_mutex_lock(&tsem.mutex); \
	if (expression) { \
		pthread_cond_wait(&tsem.condition, &tsem.mutex); \
	} \
	pthread_mutex_unlock(&tsem.mutex);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
