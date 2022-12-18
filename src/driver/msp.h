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

bool msp_process_byte(uint8_t c);
void msp_process_packet(mspPacket_t *packet);
