#ifndef __MSP_DISPLAYPORT_H
#define __MSP_DISPLAYPORT_H

#include <stdint.h>

#define SD_HMAX     30
#define SD_VMAX     16
#define HD_HMAX     50
#define HD_VMAX     18
#define RXBUF_SIZE  64

//#define DEBUG_DISPLAYPORT

#define HEADER0 0x56
#define HEADER1 0x80

typedef enum{
    SD_3016,
    HD_5018,
    HD_3016,
    RES_MAX
} osd_resolution_t;

typedef enum{
    VR_720P50 = 0,
    VR_720P60 = 1,
    VR_720P30 = 2,
    VR_540P90 = 3,
    VR_540P60 = 4,
    VR_960x720P60 = 5,
    VR_540P90_CROP = 6
} video_resolution_t;

typedef enum{
    RX_HEADER0,
    RX_HEADER1,
    RX_INDEX,
    RX_LENGTH,
    RX_CONFIG,
    RX_OSD,
    RX_CRC0,
    RX_CRC1
}rx_state_e;

//void fc_msp_displayport();
void fc_init();
void mcu_osd_init();
void osd8336_init();
void recive_one_frame(uint8_t* uart_buf,uint8_t uart_buf_len);
void parser_rx(uint8_t function, uint8_t index, uint8_t* rx_buf);
void parser_config(uint8_t* rx_buf);
void parser_osd(uint8_t raw, uint8_t* rx_buf);
void clear_screen();
void write_string(uint8_t ch, uint8_t* line_buf, uint8_t col);
void update_osd(uint16_t* line_buf, uint8_t raw);

void camTypeDetect(uint8_t rData);
void fcTypeDetect(uint8_t* rData);
void lqDetect(uint8_t rData);
void lqStatistics();
void vtxTempDetect(uint8_t rData);

extern video_resolution_t CAM_MODE;

extern char             fc_variant[5];
extern uint8_t          link_quality;
extern uint8_t          vtxTempInfo;  // bit[7]: temp enbale bit[6:0]: temp(0~9)
extern uint8_t          fontType;
extern osd_resolution_t resolution;
extern uint8_t          vtxVersion;
extern uint8_t          vtxType;
extern uint8_t          vtxFcLock;
extern uint8_t          cam_4_3;
extern int              seconds;

extern uint8_t fc_init_done;
extern uint16_t osd_buf[HD_VMAX][HD_HMAX];
extern uint16_t osd_buf_shadow[HD_VMAX][HD_HMAX];
extern uint8_t loc_buf[HD_VMAX][4];
extern uint8_t osd_page_buf[HD_VMAX][7];

extern uint16_t last_rcv_seconds0;
extern uint16_t last_rcv_seconds1;

#endif
