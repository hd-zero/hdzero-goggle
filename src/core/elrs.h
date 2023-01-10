#ifndef __ELRS_H__
#define __ELRS_H__

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    MSP_IDLE,
    MSP_HEADER_START,
    MSP_HEADER_X,

    MSP_HEADER_V2_NATIVE,
    MSP_PAYLOAD_V2_NATIVE,
    MSP_CHECKSUM_V2_NATIVE,

    MSP_COMMAND_RECEIVED
} mspState_e;

typedef enum {
    MSP_PACKET_UNKNOWN = '!',
    MSP_PACKET_COMMAND = '<',
    MSP_PACKET_RESPONSE = '>'
} mspPacketType_e;

typedef struct __attribute__((packed)) {
    uint8_t  flags;
    uint16_t function;
    uint16_t payload_size;
} mspHeaderV2_t;

#define MSP_GET_BAND_CHAN   0x0300
#define MSP_SET_BAND_CHAN   0x0301
#define MSP_GET_FREQ        0x0302
#define MSP_SET_FREQ        0x0303
#define MSP_GET_REC_STATE   0x0304
#define MSP_SET_REC_STATE   0x0305
#define MSP_GET_VRX_MODE    0x0306
#define MSP_SET_VRX_MODE    0x0307
#define MSP_GET_RSSI        0x0308
#define MSP_GET_BAT_VOLTS   0x0309
#define MSP_GET_VERSION     0x030A
#define MSP_SET_BUZZER      0x030B
#define MSP_SET_OSD_ELEM    0x00B6
#define MSP_SET_MODE        0x0380    // goggles to backpack
#define MSP_GET_BP_VERSION  0x0381    // goggles to backpack
#define MSP_GET_BP_STATUS   0x0382    // goggles to backpack

#define MSP_PORT_INBUF_SIZE 64

typedef struct {
    mspPacketType_e type;
    uint8_t         flags;
    uint16_t        function;
    uint16_t        payload_size;
    uint8_t         payload[MSP_PORT_INBUF_SIZE];
    uint16_t        payload_offset;
    bool            read_error;
} mspPacket_t;

void elrs_init();

void msp_send_packet(uint16_t function, mspPacketType_e type, uint16_t payload_size, uint8_t *payload);
bool msp_read_resposne(uint16_t function, uint16_t *payload_size, uint8_t *payload);
bool msp_await_resposne(uint16_t function, uint16_t payload_size, uint8_t *payload, uint32_t timeout_ms);

#endif //__ELRS_H__