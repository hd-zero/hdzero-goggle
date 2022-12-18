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
    MSP_PACKET_UNKNOWN,
    MSP_PACKET_COMMAND,
    MSP_PACKET_RESPONSE
} mspPacketType_e;

typedef struct {
    uint8_t  flags;
    uint16_t function;
    uint16_t payload_size;
} mspHeaderV2_t;

#define MSP_GET_BAND_CHAN 0x0300
#define MSP_SET_BAND_CHAN 0x0301
#define MSP_GET_FREQ      0x0302
#define MSP_SET_FREQ      0x0303
#define MSP_GET_REC_STATE 0x0304
#define MSP_SET_REC_STATE 0x0305
#define MSP_GET_VRX_MODE  0x0306
#define MSP_SET_VRX_MODE  0x0307
#define MSP_GET_RSSI      0x0308
#define MSP_GET_BAT_VOLTS 0x0309
#define MSP_GET_VERSION   0x030A
#define MSP_SET_BUZZER    0x030B
#define MSP_SET_OSD_ELEM  0x00B6
#define MSP_SET_MODE      0x0380    // goggles to backpack

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

uint8_t msp_crc8_dvb_s2(uint8_t crc, uint8_t a);
bool msp_process_byte(uint8_t c);
void msp_process_packet(mspPacket_t *packet);
void msp_reset_packet(mspPacket_t *packet);
