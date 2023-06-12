#include "beep.h"

#include <pthread.h>
#include <unistd.h>

#include "core/defines.h"
#include "gpio.h"

static pthread_t beep_thread_handle;
static pthread_mutex_t beep_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t beep_cond = PTHREAD_COND_INITIALIZER;
static int beep_dur_ms = 0;

static void *beep_thread(void *);

void beep_init() {
    pthread_create(&beep_thread_handle, NULL, beep_thread, NULL);
}

void beep_dur(int ms) {
    pthread_mutex_lock(&beep_mutex);
    beep_dur_ms = ms;
    pthread_cond_signal(&beep_cond);
    pthread_mutex_unlock(&beep_mutex);
}

static void *beep_thread(void *arg) {
    while (1) {
        pthread_mutex_lock(&beep_mutex);
        pthread_cond_wait(&beep_cond, &beep_mutex);
        int this_beep_ms = beep_dur_ms;
        beep_dur_ms = 0;
        pthread_mutex_unlock(&beep_mutex);

        if (this_beep_ms > 0) {
            gpio_set(GPIO_BEEP, 1);
            usleep(this_beep_ms * 1000);
            gpio_set(GPIO_BEEP, 0);
        }
    }
}
