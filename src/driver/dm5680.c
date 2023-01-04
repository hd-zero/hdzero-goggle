#include "dm5680.h"

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
#include "common.hh"
#include "ui/page_common.h"
#include "msp_displayport.h"
#include "../core/osd.h"


/////////////////////////////////////////////////////////////////////
//global
rx_status_t     rx_status[2]; //global, 0=UART1 from Right DM5680, 1=UART2 from Left DM5680, 

//local
uint8_t         uart_buffer[2][256];
uint8_t         uart_rptr[2],uart_wptr[2];
int             fd_dm5680l =0, fd_dm5680r =0;
pthread_mutex_t cmd_to5680_mutex;

uint8_t uart_parse_core(uint8_t* buff, uint8_t* rptr,uint8_t* wptr,uint8_t* state, uint8_t* len,uint8_t* payload,uint8_t* payload_ptr)
{
	uint8_t pkt_cnt = 0;
	while(*rptr != *wptr) {
		switch(*state) {
			case 0: //
				if(buff[*rptr] == 0xCC) 
					*state = 1;
				break;

			case 1:
				if(buff[*rptr] == 0x33)
					*state = 2;
				else	
					*state = 0;
				break;

			case 2:
				*len = buff[*rptr];
				payload[*payload_ptr]= *len;
				(*payload_ptr)++;
				*state = 3;
				break;

			case 3:
				payload[*payload_ptr]= buff[*rptr];
				(*payload_ptr)++;

				(*len)--;
				if(*len ==0){
					*state = 0;
					pkt_cnt++;
				}
				break;
		}
		(*rptr)++; 
	}
	return pkt_cnt;
}

void uart_parse(uint8_t sel,uint8_t* state, uint8_t* len, uint8_t* payload,uint8_t* payload_ptr)
{
	uint8_t* uart_buf = sel? uart_buffer[1] : uart_buffer[0];
	uint8_t* uart_buf_rptr = sel? &uart_rptr[1] : &uart_rptr[0];
	uint8_t* uart_buf_wptr = sel? &uart_wptr[1] : &uart_wptr[0];

	uint8_t* ptr;
	uint8_t  i,pkt_cnt;

	pkt_cnt = uart_parse_core(uart_buf,uart_buf_rptr,uart_buf_wptr, state,len,payload,payload_ptr);
	ptr = payload;
	while(pkt_cnt--) {
		//LOGI("UART%d:Cmd=%x,len=%x,Value=%x",sel+1,ptr[1],ptr[0],ptr[2]);
		switch(ptr[1]) {
			case 0x01: //Ver
				DM5680_get_ver(sel,ptr);
				break;

			case 0x11: // Valid_channel
				DM5680_get_vldflg(sel, ptr);
				break;

			case 0x12: // rssi0 rssi1 DLQ Stat crc
				DM5680_get_rssi(sel,ptr);
				break;

			case 0x14: // vtx_type vtx_ver vtx_stat crc
				DM5680_get_vtxinfo(sel,ptr);
				break;

			case 0x15:// osd_data....... crc
				if(!sel) //Update OSD from UART1 only sel: 1=from left, 0= from right
					DM5680_OSD_parse(&ptr[2], ptr[0]-1);
				break;
					
			case 0x20:// right_btn
				if(sel) {
					g_key = RIGHT_KEY_CLICK+(ptr[2]&1);
					LOGI("btn:%x\n", ptr[2]);	//0=short,1=long
					if((g_source_info.source != SOURCE_HDMI_IN) && (g_menu_op == OPLEVEL_VIDEO)) //no record feature for HDMI in or non-Video mode
						rbtn_click(!ptr[2], 0);
				}
				break;

			case 0x19: //Read DM5680 reg
				DM5680_get_regval(sel,ptr);
				break;

			default:
				LOGE("UART%d bad command",sel+1);
				break;
		}
		(*payload_ptr) -= (ptr[0]+1);
		ptr += (ptr[0]+1);
	}

	for(i=0;i<(*payload_ptr);i++) 
		payload[i] = (*ptr++);
	
	//LOGI("(UART%d:%d %d %d)",sel+1,*uart_buf_rptr,*uart_buf_wptr,*payload_ptr);	
}


static void *pthread_recv_dm5680l(void *arg)
{
	int     i,len=0;

	uint8_t buffer[128];
	fd_set  rd;

	uint8_t state,len8,payload_ptr,payload[128];

	//Init UART buffers
	uart_rptr[0]=0;
	uart_wptr[0]=0;
	state = len8 = payload_ptr = 0;

	for(;;)
	{
		FD_ZERO(&rd);
		FD_SET(fd_dm5680l,&rd);
		while(FD_ISSET(fd_dm5680l,&rd))
		{
		  if(select(fd_dm5680l+1,&rd,NULL,NULL,NULL) < 0)
			LOGE("UART1:select error!");
		  else
		  {
			len = uart_read(fd_dm5680l, buffer, 128);
			//if(len) LOGI("(UART1-%d)",len);
			for(i=0;i<len;i++)
			{
				uart_buffer[0][uart_wptr[0]] = buffer[i];
				uart_wptr[0]++; 

				if(uart_wptr[0] == uart_rptr[0]) 
					LOGW("UART1 fifo full!");
			}
			if(len) 
				uart_parse(0,&state, &len8, payload,&payload_ptr);
		  }
		}
	}

	return NULL;
}

static void *pthread_recv_dm5680r(void *arg)
{
	int     i,len=0;

	uint8_t buffer[128];
	fd_set  rd;

	uint8_t state,len8,payload_ptr,payload[128];

	//Init UART buffers
	uart_rptr[1]=0;
	uart_wptr[1]=0;
	state = len8 = payload_ptr = 0;

	for(;;)
	{
		FD_ZERO(&rd);
		FD_SET(fd_dm5680r,&rd);
		while(FD_ISSET(fd_dm5680r,&rd))
		{
		  if(select(fd_dm5680r+1,&rd,NULL,NULL,NULL) < 0)
			LOGE("UART2:select error!");
		  else
		  {
			len = uart_read(fd_dm5680r, buffer, 128);
			//if(len) LOGI("(UART2-%d)",len);
			for(i=0;i<len;i++)
			{
				uart_buffer[1][uart_wptr[1]] = buffer[i];
				uart_wptr[1]++; 

				if(uart_wptr[1] == uart_rptr[1]) 
					LOGW("UART2 fifo full!");
			}
			if(len) 
				uart_parse(1,&state, &len8, payload,&payload_ptr);
		  }
		}
	}

	return NULL;
}


int uart_init()
{
	static bool is_inited = false;
	pthread_t tid_l,tid_r;

	if(is_inited)
		return 0;

	fd_dm5680r = uart_open(1);
	fd_dm5680l = uart_open(2);

	pthread_create(&tid_l, NULL, pthread_recv_dm5680l, NULL);
	pthread_create(&tid_r, NULL, pthread_recv_dm5680r, NULL);
	pthread_mutex_init(&cmd_to5680_mutex, NULL);

	is_inited = true;
	return 0;
}

void UART1_TX (uint8_t d)
{
	uart_write_byte(fd_dm5680r, d);
}

void UART2_TX (uint8_t d)
{
	uart_write_byte(fd_dm5680l, d);
}

//////////////////////////////
//sel=0/URART1/Right,
//sel=1/URART2/Left
void Cmd_to_DM5680(uint8_t sel, uint8_t* cmd, uint8_t cmd_len)
{
	uint8_t i;
	pthread_mutex_lock(&cmd_to5680_mutex);

	for(i=0;i<cmd_len;i++) {
		if(sel==0)
			UART1_TX(cmd[i]);
		else 	
			UART2_TX(cmd[i]);
	}

	pthread_mutex_unlock(&cmd_to5680_mutex);
}

void DM5680_req_ver()
{
	uint8_t Cmd[4] = {0xAA, 0x55,0x01,0x1};
	Cmd_to_DM5680(0, Cmd, 4);
	Cmd_to_DM5680(1, Cmd, 4);

}

void DM5680_get_ver(uint8_t sel, uint8_t* payload)
{
	rx_status_t* rx_status_ptr = sel?(&rx_status[1]) : (&rx_status[0]);
	rx_status_ptr->rx_ver = payload[2]; 
}

void DM5680_SetFanSpeed(uint8_t sel, uint8_t speed) 
{
	uint8_t Cmd[5] = {0xAA, 0x55,0x02,0x2,0x00};
	Cmd[4]= speed;

	Cmd_to_DM5680(sel, Cmd, 5);
}

void DM5680_Beep(uint8_t on) //1=sound,0=off
{
	uint8_t Cmd[5] = {0xAA, 0x55,0x02,0x3,0x00};
	Cmd[4]= on;

	Cmd_to_DM5680(0, Cmd, 5); //Fix me, ntant 
}

void DM5680_ResetRF(uint8_t on) //Reset DM6302, 0=reset
{
	uint8_t Cmd[5] = {0xAA, 0x55,0x02,0x4,0x00};
	Cmd[4]= on;

	Cmd_to_DM5680(0, Cmd, 5);
	Cmd_to_DM5680(1, Cmd, 5);
}

void DM5680_ResetHDMI_TX(uint8_t on) //Reset HDMI_TX,0=reset
{
	uint8_t Cmd[5] = {0xAA, 0x55,0x02,0x5,0x00};
	Cmd[4]= on;

	Cmd_to_DM5680(0, Cmd, 5);
}

void DM5680_ResetHDMI_RX(uint8_t on) //Reset HDMI_RX,0=reset
{
	uint8_t Cmd[5] = {0xAA, 0x55,0x02,0x5,0x00};
	Cmd[4]= on;

	Cmd_to_DM5680(1, Cmd, 5);
}

void DM5680_SetBB(uint8_t on) //Reset DM5680, 0 =reset
{
	uint8_t Cmd[5] = {0xAA, 0x55,0x02,0x10,0x00};
	Cmd[4]= on;

	Cmd_to_DM5680(0, Cmd, 5);
	Cmd_to_DM5680(1, Cmd, 5);
}

void DM5680_SetFPS(uint8_t fps) //0=60fps, 1=90fps
{
	uint8_t Cmd[5] = {0xAA, 0x55,0x02,0x17,0x00};
	Cmd[4]= fps;
	Cmd_to_DM5680(0, Cmd, 5);
	Cmd_to_DM5680(1, Cmd, 5);
}

void DM5680_req_rssi()
{
	uint8_t Cmd[4] = {0xAA, 0x55,0x01,0x12};
	Cmd_to_DM5680(0, Cmd, 4);
	Cmd_to_DM5680(1, Cmd, 4);
}

void DM5680_WriteReg(uint8_t page, uint8_t addr, uint8_t wdat) 
{
	uint8_t Cmd[7] = {0xAA, 0x55,0x04,0x18,0x00,0x00,0x00};
	Cmd[4]= page;
	Cmd[5]= addr;
	Cmd[6]= wdat;
	Cmd_to_DM5680(0, Cmd, 7);
	Cmd_to_DM5680(1, Cmd, 7);
}

void DM5680_ReadReg(uint8_t page, uint8_t addr)
{
	uint8_t Cmd[6] = {0xAA, 0x55,0x03,0x19,0x00,0x00};
	Cmd[4]= page;
	Cmd[5]= addr;
	Cmd_to_DM5680(0, Cmd, 6);
	Cmd_to_DM5680(1, Cmd, 6);
}

void filter_rssi(uint8_t *out, uint8_t in, uint8_t* buf)
{
	uint8_t i,min;
	min = in;
	for(i=0;i<9;i++) {
		buf[i] = buf[i+1];
		if(buf[i+1] < min) min = buf[i+1];
	}
	buf[9] = in;
	*out = min;
}

void DM5680_get_rssi(uint8_t sel, uint8_t* payload)
{
	uint8_t crc;
	static uint8_t avg_buf[40];
	rx_status_t* rx_status_ptr = sel? &rx_status[1] : &rx_status[0];

	crc = payload[2]+payload[3]+payload[4]+payload[5];
	if(crc == payload[6]) {
		filter_rssi(&rx_status_ptr->rx_rssi[0], payload[2],&avg_buf[sel*20]);
		filter_rssi(&rx_status_ptr->rx_rssi[1], payload[3],&avg_buf[sel*20+10]);
		rx_status_ptr->rx_DLQ  = payload[4];
		rx_status_ptr->rx_Stat = payload[5];
		#if 0 
		LOGI("(RSSI %x %x %x %x)",	rx_status[0].rx_rssi[0],
										rx_status[0].rx_rssi[1],
										rx_status[1].rx_rssi[0],
										rx_status[1].rx_rssi[1]);
		#endif								
	}
	else {
		perror("GetRssi: CRC Error\n");
	}
}


void DM5680_req_vtxinfo(uint8_t sel)
{
	uint8_t Cmd[4] = {0xAA, 0x55,0x01,0x14};
	Cmd_to_DM5680(sel, Cmd, 4);
}

void DM5680_get_vtxinfo(uint8_t sel, uint8_t* payload)
{
	rx_status_t* rx_status_ptr = sel? &rx_status[1] : &rx_status[0];
	uint8_t crc = payload[2]+payload[3]+payload[4];
	if(crc == payload[5]) {
		rx_status_ptr->rx_vtx_type = payload[2];
		rx_status_ptr->rx_vtx_ver  = payload[3];
		rx_status_ptr->rx_vtx_stat = payload[4];
	}
	else {
		perror("GetVTXinfo: CRC Error\n");
	}
}

void DM5680_req_vldflg()
{
	uint8_t Cmd[4] = {0xAA, 0x55,0x01,0x11};
	Cmd_to_DM5680(0, Cmd, 4);
	Cmd_to_DM5680(1, Cmd, 4);
}

void DM5680_clear_vldflg()
{
	rx_status[0].rx_valid    = 0;
	rx_status[1].rx_valid    = 0;
}

void DM5680_get_vldflg(uint8_t sel, uint8_t* payload)
{
	rx_status_t* rx_status_ptr = sel? &rx_status[1] : &rx_status[0];
	rx_status_ptr->rx_valid    = payload[2];
}

void DM5680_get_regval(uint8_t sel, uint8_t* payload)
{
	rx_status_t* rx_status_ptr = sel? &rx_status[1] : &rx_status[0];
	rx_status_ptr->rx_regval  = payload[2];
}

void DM5680_OSD_parse(uint8_t *buf, uint8_t len)
{
	if(g_menu_op != OPLEVEL_VIDEO)
		return;

	recive_one_frame(buf,len);
	lqStatistics();
}
