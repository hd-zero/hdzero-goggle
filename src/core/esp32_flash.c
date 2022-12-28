#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <stdlib.h>

#include "../core/common.hh"
#include "uart.h"

#include "serial_io.h"

static int fd_esp32 = -1;
static int64_t s_time_end;

void loader_port_enter_bootloader(void)
{
	set_gpio(GPIO_ESP32_EN, 0);
	set_gpio(GPIO_ESP32_BOOT0, 0);
    loader_port_delay_ms(100);
	set_gpio(GPIO_ESP32_BOOT0, 0);
	set_gpio(GPIO_ESP32_EN, 1);
    loader_port_delay_ms(100);
}

void loader_port_reset_target(void)
{
	set_gpio(GPIO_ESP32_EN, 0);
	set_gpio(GPIO_ESP32_BOOT0, 1);
    loader_port_delay_ms(100);
	set_gpio(GPIO_ESP32_EN, 1);
}

void loader_port_delay_ms(uint32_t ms)
{
    usleep(ms * 1000);
}

void loader_port_start_timer(uint32_t ms)
{
    s_time_end = clock() + (ms * (CLOCKS_PER_SEC / 1000));
}

uint32_t loader_port_remaining_time(void)
{
    int64_t remaining = (s_time_end - clock()) / 1000;
    return (remaining > 0) ? (uint32_t)remaining : 0;
}

esp_loader_error_t loader_port_serial_write(const uint8_t *data, uint16_t size, uint32_t timeout)
{
    int written = write(fd_esp32, data, size);

    if (written < 0) {
        return ESP_LOADER_ERROR_FAIL;
    } else if (written < size) {
        return ESP_LOADER_ERROR_TIMEOUT;
    } else {
        return ESP_LOADER_SUCCESS;
    }
}

static esp_loader_error_t read_char(uint8_t *c, uint32_t timeout)
{
    struct timeval tv = {timeout / 1000, (timeout % 1000) * 1000};
	fd_set  rd;
	FD_ZERO(&rd);
	FD_SET(fd_esp32,&rd);
	int count = select(fd_esp32+1,&rd,NULL,NULL,&tv);
	if (count == -1) return ESP_LOADER_ERROR_FAIL;
	if (count == 0) return ESP_LOADER_ERROR_TIMEOUT;

    int read_bytes = read(fd_esp32, c, 1);

    if (read_bytes == 1) {
        return ESP_LOADER_SUCCESS;
    } else if (read_bytes == 0) {
        return ESP_LOADER_ERROR_TIMEOUT;
    } else {
        return ESP_LOADER_ERROR_FAIL;
    }
}

static esp_loader_error_t read_data(uint8_t *buffer, uint32_t size)
{
    for (int i = 0; i < size; i++) {
        uint32_t remaining_time = loader_port_remaining_time();
        RETURN_ON_ERROR( read_char(&buffer[i], remaining_time) );
    }

    return ESP_LOADER_SUCCESS;
}

esp_loader_error_t loader_port_serial_read(uint8_t *data, uint16_t size, uint32_t timeout)
{
	RETURN_ON_ERROR( read_data(data, size) );
    return ESP_LOADER_SUCCESS;
}

esp_loader_error_t loader_port_init()
{
	fd_esp32 = uart_open(3);
	uart_set_opt(fd_esp32, 230400, 8, 'N', 1);
	return ESP_LOADER_SUCCESS;
}

void loader_port_close()
{
	if(fd_esp32 != -1) {
		uart_close(fd_esp32);
		fd_esp32 = -1;
	}
}

esp_loader_error_t loader_port_change_baudrate(uint32_t baudrate)
{
	if (!uart_set_opt(fd_esp32, baudrate, 8, 'N', 1))
		return ESP_LOADER_SUCCESS;
	return ESP_LOADER_ERROR_FAIL;
}
