#include "gpio.h"

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "core/defines.h"
#include "util/file.h"

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
