#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */
#include <errno.h>   /* ERROR Number Definitions           */

#include "../core/common.hh"
#include "../page/page_common.h"
#include "dm5680.h"
#include "uart.h"
#include "esp32.h"
#include "msp.h"
#include "../esp32/serial_io.h"

/////////////////////////////////////////////////////////////////////////////////
static int       fd_esp32 = -1;
static uint8_t   uart3_buffer[256];
static uint8_t   uart3_rptr,uart3_wptr;
static pthread_t tid;
static volatile int stopping = false;

static int64_t s_time_end;

static void *pthread_recv_esp32(void *arg)
{
	int     i,len=0;
	uint8_t buffer[128];
	fd_set  rd;
	struct timeval tv = {0, 100000};

	//Init UART buffers
	uart3_rptr=0;
	uart3_wptr=0;

	stopping = false;
	Printf("UART3:starting reader thread\n");

	for(;;)
	{
		FD_ZERO(&rd);
		FD_SET(fd_esp32,&rd);
		if (stopping) {
			Printf("UART3:stopping, exiting thread.\n");
			return NULL;
		}
		if(select(fd_esp32+1,&rd,NULL,NULL,&tv) < 0) {
			Printf("UART3:select error!\n");
			return NULL;
		}
		if (FD_ISSET(fd_esp32,&rd))
		{
			len = uart_read(fd_esp32, buffer, 128);
			if (len < 0) {
				Printf("UART3:read error, exiting thread.\n");
				return NULL;
			}
			for(i=0;i<len;i++)
			{
				uart3_buffer[uart3_wptr] = buffer[i];
				uart3_wptr++;

				if(uart3_wptr == uart3_rptr)
					Printf("UART3:fifo full!\n");
			}
			if(len)
				esp32_rx();
		}
	}
	return NULL;
}

void enable_esp32()
{
    fd_esp32 = uart_open(3);
    if(fd_esp32 != -1) {
		Printf("[ESP32] Powering on\n");
		set_gpio(GPIO_ESP32_EN,0);
		set_gpio(GPIO_ESP32_BOOT0,1);
        pthread_create(&tid, NULL, pthread_recv_esp32, NULL);
		usleep(50000);
		set_gpio(GPIO_ESP32_EN,1);
	}
}

void disable_esp32()
{
	if (fd_esp32 != -1) {
		Printf("[ESP32] Powering off\n");
		set_gpio(GPIO_ESP32_EN, 0);
		stopping = true;
		pthread_join(tid, NULL);
		uart_close(fd_esp32);
		fd_esp32 = -1;
	}
}

void esp32_tx(uint8_t* cmd, uint8_t cmd_len)
{
	uint8_t i;
    if(fd_esp32 == -1) return;

	for(i=0;i<cmd_len;i++) {
		uart_write_byte(fd_esp32,cmd[i]);
	}
}

void esp32_rx()
{
    static uint8_t i=0;
    static char buffer[80];

    while(uart3_rptr != uart3_wptr) {
        if(g_test_en) {
			bool processed = msp_process_byte(uart3_buffer[uart3_rptr]);
			if (!processed) {
	            buffer[i++] = uart3_buffer[uart3_rptr];
				if(buffer[i-1]=='\n' || i==80) {
					buffer[i-1] = 0;
					Printf("[ESP] %s\n", buffer);
					i = 0;
				}
			}
			else if (i>0) {
				buffer[i-1] = 0;
				Printf("[ESP] %s\n", buffer);
				i = 0;
			}
        }
        uart3_rptr++;
    }
}

void msp_process_packet(mspPacket_t *packet)
{
	// TODO process packets
}


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
