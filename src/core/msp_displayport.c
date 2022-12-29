//#include "common.h"
//#include "uart.h"
//#include "global.h"
//#include "print.h"
//#include "cmd_struct.h"
#include <string.h>
#include <stdio.h>
#include "msp_displayport.h"
#include "osd.h"
//#include "i2c_device.h"
//#include "isr.h"
//#include "hardware.h"


int seconds = 0;

uint8_t crc8tab[256] = {
    0x00, 0xD5, 0x7F, 0xAA, 0xFE, 0x2B, 0x81, 0x54, 0x29, 0xFC, 0x56, 0x83, 0xD7, 0x02, 0xA8, 0x7D,
    0x52, 0x87, 0x2D, 0xF8, 0xAC, 0x79, 0xD3, 0x06, 0x7B, 0xAE, 0x04, 0xD1, 0x85, 0x50, 0xFA, 0x2F,
    0xA4, 0x71, 0xDB, 0x0E, 0x5A, 0x8F, 0x25, 0xF0, 0x8D, 0x58, 0xF2, 0x27, 0x73, 0xA6, 0x0C, 0xD9,
    0xF6, 0x23, 0x89, 0x5C, 0x08, 0xDD, 0x77, 0xA2, 0xDF, 0x0A, 0xA0, 0x75, 0x21, 0xF4, 0x5E, 0x8B,
    0x9D, 0x48, 0xE2, 0x37, 0x63, 0xB6, 0x1C, 0xC9, 0xB4, 0x61, 0xCB, 0x1E, 0x4A, 0x9F, 0x35, 0xE0,
    0xCF, 0x1A, 0xB0, 0x65, 0x31, 0xE4, 0x4E, 0x9B, 0xE6, 0x33, 0x99, 0x4C, 0x18, 0xCD, 0x67, 0xB2,
    0x39, 0xEC, 0x46, 0x93, 0xC7, 0x12, 0xB8, 0x6D, 0x10, 0xC5, 0x6F, 0xBA, 0xEE, 0x3B, 0x91, 0x44,
    0x6B, 0xBE, 0x14, 0xC1, 0x95, 0x40, 0xEA, 0x3F, 0x42, 0x97, 0x3D, 0xE8, 0xBC, 0x69, 0xC3, 0x16,
    0xEF, 0x3A, 0x90, 0x45, 0x11, 0xC4, 0x6E, 0xBB, 0xC6, 0x13, 0xB9, 0x6C, 0x38, 0xED, 0x47, 0x92,
    0xBD, 0x68, 0xC2, 0x17, 0x43, 0x96, 0x3C, 0xE9, 0x94, 0x41, 0xEB, 0x3E, 0x6A, 0xBF, 0x15, 0xC0,
    0x4B, 0x9E, 0x34, 0xE1, 0xB5, 0x60, 0xCA, 0x1F, 0x62, 0xB7, 0x1D, 0xC8, 0x9C, 0x49, 0xE3, 0x36,
    0x19, 0xCC, 0x66, 0xB3, 0xE7, 0x32, 0x98, 0x4D, 0x30, 0xE5, 0x4F, 0x9A, 0xCE, 0x1B, 0xB1, 0x64,
    0x72, 0xA7, 0x0D, 0xD8, 0x8C, 0x59, 0xF3, 0x26, 0x5B, 0x8E, 0x24, 0xF1, 0xA5, 0x70, 0xDA, 0x0F,
    0x20, 0xF5, 0x5F, 0x8A, 0xDE, 0x0B, 0xA1, 0x74, 0x09, 0xDC, 0x76, 0xA3, 0xF7, 0x22, 0x88, 0x5D,
    0xD6, 0x03, 0xA9, 0x7C, 0x28, 0xFD, 0x57, 0x82, 0xFF, 0x2A, 0x80, 0x55, 0x01, 0xD4, 0x7E, 0xAB,
    0x84, 0x51, 0xFB, 0x2E, 0x7A, 0xAF, 0x05, 0xD0, 0xAD, 0x78, 0xD2, 0x07, 0x53, 0x86, 0x2C, 0xF9
};

video_resolution_t CAM_MODE = VR_720P60; 
char fc_variant[5] = "BTFL"; // 4 char ASCII from FC
uint8_t link_quality = 0;   // bit[7:0]: LQ(8~0)
uint8_t vtxTempInfo = 0;    // bit[7]: temp enbale
                            // bit[6:0]: temp(0~9)

uint8_t fontType = 0;       // bit[7:0]: ASCII ('0'~'9' & 'A'~'Z')
uint8_t vtxVersion = 0;
uint8_t vtxType = 0;
uint8_t vtxFcLock = 0;
//bit[0] msp_displayport_is_OK
//bit[1] VTX_serial_is_OK
//bit[3] Unlocked VTX
uint8_t cam_4_3 = 0;  //1=16:9;0=4:3

osd_resolution_t resolution = SD_3016;
static osd_resolution_t resolution_last = HD_5018;

uint8_t lq_err_cnt = 0;
uint8_t lq_rcv_cnt = 0;

uint16_t osd_buf[HD_VMAX][HD_HMAX];
uint16_t osd_buf_shadow[HD_VMAX][HD_HMAX];
uint8_t osd_init_done = 0;
uint8_t fc_init_done = 0;

uint16_t last_rcv_seconds0 = 0;
uint16_t last_rcv_seconds1 = 0;

/*
void fc_msp_displayport()
{
    if(!fc_init_done){
        fc_init();
        osd_init_done = 1;
        fc_init_done = 1;
        LOGI("fc_init_done");
    }else{
        recive_one_frame();
        lqStatistics();
    }
}
*/
void fc_init()
{
	clear_screen();
    vtxType = 0;
    vtxTempInfo = 0;
}

void recive_one_frame(uint8_t* uart_buf,uint8_t uart_buf_len)
{
    static uint8_t rx_buf[RXBUF_SIZE];
    static uint8_t rx_state = RX_HEADER0;
    static uint8_t length = 0;
    static uint8_t ptr = 0;
    static uint8_t function = 0;// 0:osd;  1:config
    static uint8_t index = 0;
    static uint8_t crc0;
    static uint8_t crc1;
	uint8_t rx;
     
    uint8_t uart_buf_ptr;
    //while(RS_ready1())
    for(uart_buf_ptr=0; uart_buf_ptr<uart_buf_len; uart_buf_ptr++)
    {
        //rx = RS_rx1();
        rx = uart_buf[uart_buf_ptr];

        last_rcv_seconds1 = seconds;
        #if(0)
        _outchar(rx);
        #endif
        switch(rx_state)
        {
		
            case RX_HEADER0:
			crc0 = 0;
            crc1 = 0;
            crc0 ^= rx;
            crc1 = crc8tab[crc1 ^ rx];
			if(rx == HEADER0)
				rx_state = RX_HEADER1;

			break;

            case RX_HEADER1:
            crc0 ^= rx;
            crc1 = crc8tab[crc1 ^ rx];
			if(rx == HEADER1)
				rx_state = RX_INDEX;
			else
				rx_state = RX_HEADER0;
				
			break;
	

            case RX_INDEX:
                crc0 ^= rx;
                crc1 = crc8tab[crc1 ^ rx];
                index = rx;
                if(index == 0xff)
                    function = 1;
                else
                    function = 0;
                //_outchar(index);
                rx_state = RX_LENGTH;
                break;
            
            case RX_LENGTH:
                crc0 ^= rx;
                crc1 = crc8tab[crc1 ^ rx];
                length = rx;
                if(length >= 64)
                    rx_state = RX_HEADER0;
                else if(length == 0)
                    rx_state = RX_CRC0;
                else{
                    if(function)
                        rx_state = RX_CONFIG;
                    else
                        rx_state = RX_OSD;
                }
                ptr = 0;
                rx_buf[ptr] = length;
                ptr ++;
                break;
            
            case RX_CONFIG:
                crc0 ^= rx;
                crc1 = crc8tab[crc1 ^ rx];
                rx_buf[ptr] = rx;
                ptr ++;
                length --;
                if(length == 0)
                    rx_state = RX_CRC0;
                break;
            
            case RX_OSD:
                crc0 ^= rx;
                crc1 = crc8tab[crc1 ^ rx];
                rx_buf[ptr] = rx;
                ptr ++;
                length --;
                if(length == 0)
                    rx_state = RX_CRC0;
                break;

            case RX_CRC0:
                if(rx == crc0)
				{
                    rx_state = RX_CRC1;
				}
                else
				{
                    rx_state = RX_HEADER0;
#if 0
					for(int i=0; i<10;i++)
					{
						LOGI("%02X ", rx_buf[i]);
					}
					LOGI("");
#endif
			//		LOGI("crc0 err");
				}
                break;

            case RX_CRC1:
                if(rx == crc1){
                    parser_rx(function, index, rx_buf);
                    last_rcv_seconds0 = seconds;
                }
                rx_state = RX_HEADER0;
                break;

            default:
                rx_state = RX_HEADER0;
                break;

        }// switch(rx_state)
    }// while(RS_ready1())
}

void parser_rx(uint8_t function, uint8_t index, uint8_t* rx_buf)
{
    if(function)
        parser_config(rx_buf);
    else
        parser_osd(index, rx_buf);
}

void camTypeDetect(uint8_t rData)
{
    static video_resolution_t cur_cam = VR_720P60;
    static video_resolution_t last_cam = VR_720P50;

    switch(rData) {
        case 0xAA:  
        case 0x99:  
                last_cam = cur_cam;
                cur_cam = VR_720P60;
                break;

        case 0x66:  
                last_cam = cur_cam;
                cur_cam = VR_720P50;
                break;

        case 0xCC:  
                last_cam = cur_cam;
                cur_cam = VR_720P30;
                break;

        case 0xEE:  
                last_cam = cur_cam;
                cur_cam = VR_540P90;
                break;

        case 0x55:  
                last_cam = cur_cam;
                cur_cam = VR_960x720P60;
                break;
        
        case 0x44:  
                last_cam = cur_cam;
                cur_cam = VR_540P90_CROP;
                break;
    }
    if(cur_cam == last_cam)
        CAM_MODE = cur_cam;

    //LOGI("Cam:%d",CAM_MODE);
}

void fcTypeDetect(uint8_t* rData)
{
    uint8_t i;
    char fc_variant_rcv[5] = "    ";
    
    for(i=0;i<4;i++)
        fc_variant_rcv[i] = rData[i];
    
    if(strcmp(fc_variant_rcv, fc_variant))
    {
        for(i=0;i<4;i++)
            fc_variant[i] = fc_variant_rcv[i];
        
        load_fc_osd_font();
    }
#if(0)
    LOGI("fc_variant_rcv:%s", fc_variant_rcv);
#endif
}

void lqDetect(uint8_t rData)
{
    static uint8_t last_lq = 0;
    
    if(rData != last_lq + 1)
        lq_err_cnt += (rData - last_lq);
    lq_rcv_cnt ++;
    last_lq = rData;
}
void lqStatistics()
{
    static uint16_t last_sec = 0;
    
    if(seconds != last_sec){
        if(lq_rcv_cnt>=8)
            link_quality = 8;
        else if(lq_rcv_cnt == 7){
            if(lq_err_cnt == 0)
                link_quality = 8;
            else
                link_quality = 7;
        }
        else if(lq_rcv_cnt == 6){
            if(lq_err_cnt == 0)
                link_quality = 8;
            else if(lq_err_cnt == 1)
                link_quality = 7;
            else
                link_quality = 6;
        }
        else
            link_quality = lq_rcv_cnt;
        
        last_sec = seconds;
        lq_rcv_cnt = 0;
        lq_err_cnt = 0;
    }
}

void vtxTempDetect(uint8_t rData)
{
    vtxTempInfo = rData;
    //LOGI("temp:%bx",vtxTempInfo);
}

void fontTypeDetect(uint8_t rData)
{
    fontType = rData;
    //LOGI("font:%bx",fontType);
}

void vtxVersionDetect(uint8_t rData)
{
    vtxVersion = rData;
}

void vtxTypeDetect(uint8_t rData)
{
    vtxType = rData;
}

void vtxFcLockDetect(uint8_t rData)
{
    vtxFcLock = rData;
}

void vtxCamRatioDetect(uint8_t rData)
{
    if(rData == 0xaa)
        cam_4_3 = 1;
    else if(rData == 0x55)
        cam_4_3 = 0;
}
void parser_config(uint8_t *rx_buf)
{
    camTypeDetect(rx_buf[1]);
    fcTypeDetect(rx_buf+2);
    lqDetect(rx_buf[6]);
    vtxTempDetect(rx_buf[7]);
    fontTypeDetect(rx_buf[8]);
    vtxVersionDetect(rx_buf[9]);
    vtxTypeDetect(rx_buf[10]);
    vtxFcLockDetect(rx_buf[11]);
    vtxCamRatioDetect(rx_buf[12]);
}

/*
    scaler from hmax30 to hmax50
*/
uint8_t scalerX(uint8_t iX)
{
    uint16_t iX_u16 = (uint16_t)iX;
    uint16_t oX_u16;
    
    oX_u16 = (iX_u16 * 427) >> 8;
    
    return (uint8_t)oX_u16;
}


void parser_osd(uint8_t row, uint8_t *rx_buf)
{
    uint16_t ch;
    uint8_t i,j,ptr;
    uint8_t mask[7] = {0};
    uint16_t line_buf[HD_HMAX];
    uint8_t hmax;
    uint8_t len_mask;
    uint32_t loc_buf = 0;
    uint8_t page_buf[7]={0};
    uint8_t chNum = 0;
    uint8_t pageNum = 0;
    static uint8_t row_last = 0;

    //detect resolution
    if((row >> 5) == (row_last >> 5))
        resolution = row >> 5;
    if(resolution >= RES_MAX)
        resolution = SD_3016;

    
    if(resolution != resolution_last)
        clear_screen();
    
    resolution_last = resolution;
    row_last = row;
    
    row &= 0x1f;
    
    if(resolution == HD_5018){
        hmax = HD_HMAX;
        len_mask = 7;
        ptr = 8;
    }else{
        ptr = 9;
        len_mask = 4;
        hmax = SD_HMAX;
    }
    
    //init line_buf
    for(i=0;i<HD_HMAX;i++)
        line_buf[i] = 0x20;

    //parse mask
    for(i=0;i<len_mask;i++){
        mask[i] = rx_buf[i+1];
    }

    //parse loc
    if(resolution == SD_3016){
        loc_buf = (uint32_t)rx_buf[5] << 0;
        loc_buf += (uint32_t)rx_buf[6] << 8;
        loc_buf += (uint32_t)rx_buf[7] << 16;
        loc_buf += (uint32_t)rx_buf[8] << 24;
    }
    
    //parse page
    chNum = 0;
    for(i=0;i<hmax;i++)
        chNum += (mask[i>>3]>>(i&7))&1;
    pageNum = chNum + 7;
    pageNum = pageNum >> 3;
    
    for(i=0;i<pageNum;i++)
        page_buf[i] = rx_buf[ptr+chNum+i];

    //parse one line osd to line_buf
    uint8_t waddr = 0;
    j = 0;
    for(i=0;i<hmax;i++)
    {
        //parse ch
        if((mask[i>>3]>>(i&7))&0x01)
        {
            ch = rx_buf[ptr++];
            if((page_buf[j>>3] >> (j&7))&1)
                ch += 256;
            j++;
        }
        else
            ch = 0x20;

        if(resolution == SD_3016)
        {
            if((loc_buf >> i) & 1)
                waddr = scalerX(i);
            
            line_buf[waddr++] = ch;
            if(waddr >= HD_HMAX)
                waddr = 0;
        }
        else
        {
            line_buf[i] = ch;
        }
    }
    update_osd(line_buf, row);
}

void clear_screen()
{
	for(int i=0; i<HD_VMAX; i++)
	{
		for(int j=0; j<HD_HMAX; j++)
		{
			osd_buf[i][j] = 0x20;
		}
	}
//	draw_osd_on_console();
//	draw_osd_on_screen();
}

void update_osd(uint16_t* line_buf, uint8_t row)
{
    uint8_t i;

    for(i=0;i<HD_HMAX;i++)
	{
		if(osd_buf[row][i] != line_buf[i])
		{
        	osd_buf[row][i] = line_buf[i];
			///draw_osd_on_screen(row, i);
		}
	}
//	draw_osd_on_console();
//	draw_osd_on_screen();

}
