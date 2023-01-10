#include "esp32.h"

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

#include <log/log.h>

#include "uart.h"
#include "../core/common.hh"
#include "ui/page_common.h"


/////////////////////////////////////////////////////////////////////////////////
static int       fd_esp32 = -1;
static uint8_t   uart3_buffer[256];
static uint8_t   uart3_rptr,uart3_wptr;
static pthread_t tid;
static volatile int stopping = false;

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
	LOGI("UART3:starting reader thread");

	for(;;)
	{
		FD_ZERO(&rd);
		FD_SET(fd_esp32,&rd);
		if (stopping) {
			LOGI("UART3:stopping, exiting thread.");
			return NULL;
		}
		if(select(fd_esp32+1,&rd,NULL,NULL,&tv) < 0) {
			LOGE("UART3:select error!");
			return NULL;
		}
		if (FD_ISSET(fd_esp32,&rd))
		{
			len = uart_read(fd_esp32, buffer, 128);
			if (len < 0) {
				LOGE("UART3:read error, exiting thread.");
				return NULL;
			}
			for(i=0;i<len;i++)
			{
				uart3_buffer[uart3_wptr] = buffer[i];
				uart3_wptr++;

				if(uart3_wptr == uart3_rptr)
					LOGW("UART3:fifo full!");
			}
			if(len)
				esp32_rx();
		}
		else esp32_handler_timeout();
	}
	return NULL;
}

void esp32_init()
{
}

void enable_esp32()
{
	fd_esp32 = uart_open(3);
	if(fd_esp32 != -1) {
		LOGI("[ESP32] Powering on");
		set_gpio(GPIO_ESP32_EN,0);
		set_gpio(GPIO_ESP32_BOOT0,1);
		pthread_create(&tid, NULL, pthread_recv_esp32, NULL);
		usleep(50000);
		set_gpio(GPIO_ESP32_EN,1);
		esp32_handler_set_uart(fd_esp32);
	}
}

void disable_esp32()
{
	if (fd_esp32 != -1) {
		LOGI("[ESP32] Powering off");
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
		uint8_t ch = uart3_buffer[uart3_rptr++];
		bool processed = esp32_handler_process_byte(ch);
		if (!processed)
			buffer[i++] = ch;
		if ((processed && i>0) || buffer[i-1]=='\n' || i==80) {
			buffer[i-1] = 0;
			LOGD("[ESP] %s", buffer);
			i = 0;
		}
    }
}
