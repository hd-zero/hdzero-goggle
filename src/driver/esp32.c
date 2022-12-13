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

/////////////////////////////////////////////////////////////////////////////////
pthread_mutex_t esp32_mutex;
int             fd_esp32 = 0;
uint8_t         uart3_buffer[256];
uint8_t         uart3_rptr,uart3_wptr;

static void *pthread_recv_esp32(void *arg)
{
	int     i,len=0;
	uint8_t buffer[128];
	fd_set  rd;

	//Init UART buffers
	uart3_rptr=0;
	uart3_wptr=0;

	for(;;)
	{
		FD_ZERO(&rd);
		FD_SET(fd_esp32,&rd);
		while(FD_ISSET(fd_esp32,&rd))
		{
		  if(select(fd_esp32+1,&rd,NULL,NULL,NULL) < 0)
			Printf("UART3:select error!\n");
		  else
		  {
			len = uart_read(fd_esp32, buffer, 128);
			//if(len) Printf("(UART3-%d)\n",len);
			for(i=0;i<len;i++)
			{
				uart3_buffer[uart3_wptr] = buffer[i];
				uart3_wptr++; 

				if(uart3_wptr == uart3_rptr) 
					Printf("UART3 fifo full!\n");
			}
			if(len) 
				esp32_rx();
		  }
		}
	}
	return NULL;
}

void init_esp32()
{
    pthread_t tid;
    set_gpio(GPIO_ESP32_BOOT0,1);
    set_gpio(GPIO_ESP32_EN,0);
    usleep(50000);
    set_gpio(GPIO_ESP32_EN,1);
    fd_esp32 = uart_open(3);
    if(fd_esp32 != -1)
        pthread_create(&tid, NULL, pthread_recv_esp32, NULL);
    pthread_mutex_init(&esp32_mutex, NULL);
}

void esp32_tx(uint8_t* cmd, uint8_t cmd_len)
{
	uint8_t i;
    if(fd_esp32 == -1) return;

	//pthread_mutex_lock(&esp32_mutex);
	for(i=0;i<cmd_len;i++) {
		uart_write_byte(fd_esp32,cmd[i]);
	}
	//pthread_mutex_unlock(&esp32_mutex);
}

void esp32_rx()
{
    uint8_t i=0;
    char hello[6];
    static bool bFirst  = true;

    while(uart3_rptr != uart3_wptr) {
        //Putchar(uart3_buffer[uart3_rptr]); 
        if(g_test_en && bFirst) {   //<---- Keep this for production test
            hello[i++] = uart3_buffer[uart3_rptr];
            if(i==5) {
                hello[5] = 0;
                if(strcmp(hello,"hello") == 0) {
                    bFirst = false;
                    Printf("[Pass] ESP32\n");
                    beep();
                }
				set_gpio(GPIO_ESP32_EN,0); //disable ESP32
            }
        }
        uart3_rptr++;
    }
}


