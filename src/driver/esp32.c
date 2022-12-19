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
#include <semaphore.h>

#include "../core/common.hh"

#include "dm5680.h"
#include "esp32.h"
#include "mcp3021.h"
#include "msp.h"
#include "osd.h"
#include "uart.h"

#include "../page/page_common.h"
#include "../page/page_scannow.h"
#include "../page/page_version.h"
#include "../esp32/serial_io.h"

/////////////////////////////////////////////////////////////////////////////////
static int       fd_esp32 = -1;
static uint8_t   uart3_buffer[256];
static uint8_t   uart3_rptr,uart3_wptr;
static pthread_t tid;
static volatile int stopping = false;

static int64_t s_time_end;

static sem_t response_semaphore;
static mspPacket_t response_packet;

static const uint16_t freq_table[] = {
    5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917, // R
    5740, 5760 // F
};

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

void esp32_init()
{
	sem_init(&response_semaphore, 0, 0);
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
	beep();
	if (packet->type == MSP_PACKET_COMMAND) {
		switch (packet->function) {
			case MSP_GET_BAND_CHAN:
				{
					uint8_t chan;
					if (g_setting.scan.channel > 8) {
						chan = g_setting.scan.channel-9 + 3*8;	// Map F1/2
					} else {
						chan = g_setting.scan.channel-1 + 4*8;	// Map R1..8
					}
					msp_send_packet(MSP_GET_BAND_CHAN, MSP_PACKET_RESPONSE, 1, &chan);
				}
				break;
			case MSP_SET_BAND_CHAN:
				{
					uint8_t chan = packet->payload[0];
					if (chan > 4*8) {
						g_setting.scan.channel = chan - 4*8 + 1;	// Map R1..8
					} else {
						g_setting.scan.channel = chan - 3*8 + 9;	// Map F1/2
					}
					switch_to_video(true);
				}
				break;
			case MSP_GET_FREQ:
				{
					uint16_t freq = freq_table[g_setting.scan.channel - 1];
					uint8_t buf[2] = { freq & 0xFF, freq >> 8 };
					msp_send_packet(MSP_GET_FREQ, MSP_PACKET_RESPONSE, sizeof(buf), buf);
				}
				break;
			case MSP_SET_FREQ:
				{
					uint16_t freq = packet->payload[0] | (uint16_t)packet->payload[1] << 8;
					for (int i=0 ; i<10 ; i++) {
						if (freq == freq_table[i]) {
							g_setting.scan.channel = i+1;
							switch_to_video(true);
							break;
						}
					}
				}
				break;
			case MSP_GET_REC_STATE:
				{
					uint8_t buf = is_recording ? 1 : 0;
					msp_send_packet(MSP_GET_REC_STATE, MSP_PACKET_RESPONSE, 1, &buf);
				}
				break;
			case MSP_SET_REC_STATE:
				{
					// TODO delay
					rbtn_click(true, packet->payload[0] == 0 ? 1 : 2);
				}
				break;
			case MSP_GET_VRX_MODE:
				break;
			case MSP_SET_VRX_MODE:
				break;
			case MSP_GET_RSSI:
				{
					uint8_t buf[5] = { 4, rx_status[0].rx_rssi[0], rx_status[0].rx_rssi[1], rx_status[1].rx_rssi[0], rx_status[1].rx_rssi[1] };
					msp_send_packet(MSP_GET_VERSION, MSP_PACKET_RESPONSE, sizeof(buf), buf);
				}
				break;
			case MSP_GET_BAT_VOLTS:
				{
					uint8_t buf[2] = { g_battery.voltage & 0xFF, g_battery.voltage >> 8 };
					msp_send_packet(MSP_GET_BAT_VOLTS, MSP_PACKET_RESPONSE, sizeof(buf), buf);
				}
				break;
			case MSP_GET_VERSION:
				{
					sys_version_t sys_version;
					if (generate_current_version(&sys_version) == 0) {
						uint8_t buf[4] = { 3, sys_version.app, sys_version.rx, sys_version.va };
						msp_send_packet(MSP_GET_VERSION, MSP_PACKET_RESPONSE, sizeof(buf), buf);
					} else {
						msp_send_packet(MSP_GET_VERSION, MSP_PACKET_UNKNOWN, 0, NULL);
					}
				}
				break;
			case MSP_SET_BUZZER:
				beep();
				break;
			case MSP_SET_OSD_ELEM:
				break;
		}
	} else if (packet->type == MSP_PACKET_RESPONSE) {
		response_packet = *packet;
		sem_post(&response_semaphore);
	}
}

bool msp_await_resposne(uint16_t function, uint16_t payload_size, uint8_t *payload, uint32_t timeout_ms)
{
	struct timespec ts = { timeout_ms / 1000, (timeout_ms % 1000) * 1000000 };
	while(sem_timedwait(&response_semaphore, &ts) == 0) {
		if (response_packet.function == function) {
			if (response_packet.payload_size >= payload_size &&
					memcmp(response_packet.payload, payload, payload_size) == 0) {
				return true;
			}
			return false;
		}
	}
	return false;
}

void msp_send_packet(uint16_t function, mspPacketType_e type, uint16_t payload_size, uint8_t *payload)
{
	uint8_t buffer[16] = {'$', 'X', type, 0x00, function & 0xFF, function >> 8, payload_size & 0xFF, payload_size >> 8};
    memcpy(buffer + 8, payload, payload_size);
	uint8_t crc = 0;
	for (int i=3 ; i<payload_size + 8 ; i++)
		crc = msp_crc8_dvb_s2(crc, buffer[i]);
	buffer[payload_size + 8] = crc;
	uart_write(fd_esp32, buffer, payload_size + 9);
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
