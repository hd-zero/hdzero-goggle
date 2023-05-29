#include "gpio.h"

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "core/defines.h"
#include "util/file.h"

static pthread_t beep_thread_handle;
static pthread_mutex_t beep_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t beep_cond = PTHREAD_COND_INITIALIZER;
static int beep_dur_ms = 0;

static void *beep_thread(void *);

void gpio_init() {
    gpio_open(GPIO_BEEP);
    gpio_set(GPIO_BEEP, 0);

    gpio_open(GPIO_HDZ_TX_RESET);
    gpio_set(GPIO_HDZ_TX_RESET, 0);

    gpio_open(GPIO_HDZ_RX_RESET);
    gpio_set(GPIO_HDZ_RX_RESET, 1);

    gpio_open(GPIO_FPGA_RESET);
    gpio_set(GPIO_FPGA_RESET, 0);

    gpio_open(GPIO_TP2825_RSTB);
    gpio_set(GPIO_TP2825_RSTB, 0);

    gpio_open(GPIO_ESP32_EN);
    gpio_set(GPIO_ESP32_EN, 0);

    gpio_open(GPIO_ESP32_BOOT0);
    gpio_set(GPIO_ESP32_BOOT0, 0);

    pthread_create(&beep_thread_handle, NULL, beep_thread, NULL);
}

void gpio_open(int port_num) {
    if (!file_printf("/sys/class/gpio/export", "%d", port_num)) {
        return;
    }

    char buf[64];
    sprintf(buf, "/sys/class/gpio/gpio%d/direction", port_num);

    if (!file_printf(buf, "out")) {
        return;
    }
}

void gpio_set(int port_num, bool val) {
    char buf[64];
    sprintf(buf, "/sys/class/gpio/gpio%d/value", port_num);
    file_printf(buf, "%d", val ? 1 : 0);
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
