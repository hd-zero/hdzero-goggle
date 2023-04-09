#ifndef _THREAD_H
#define _THREAD_H

#include <pthread.h>
#include <stdint.h>

#define THREAD_COUNT_MAX (10)
#define THREAD_COUNT (3)

typedef void *(*fun_thread_instance_t)(void *params);

typedef struct {
    pthread_t pid[THREAD_COUNT_MAX];
	fun_thread_instance_t instance[THREAD_COUNT_MAX];
} threads_obj_t;

int create_threads();
#endif
