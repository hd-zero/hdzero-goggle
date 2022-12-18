
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
#include "msp.h"

static mspState_e  input_state;
static uint16_t    offset;
static uint8_t     input_buffer[MSP_PORT_INBUF_SIZE];
static mspPacket_t packet;
static uint8_t     crc;

uint8_t msp_crc8_dvb_s2(uint8_t crc, unsigned char a)
{
    crc ^= a;
    for (int ii = 0; ii < 8; ++ii) {
        if (crc & 0x80) {
            crc = (crc << 1) ^ 0xD5;
        } else {
            crc = crc << 1;
        }
    }
    return crc;
}

bool msp_process_byte(uint8_t c)
{
    bool processed_byte = false;
    switch (input_state) {

        case MSP_IDLE:
            // Wait for framing char
            if (c == '$') {
                input_state = MSP_HEADER_START;
                processed_byte = true;
            }
            break;

        case MSP_HEADER_START:
            // Waiting for 'X' (MSPv2 native)
            switch (c) {
                case 'X':
                    input_state = MSP_HEADER_X;
                    break;
                default:
                    input_state = MSP_IDLE;
                    break;
            }
            break;

        case MSP_HEADER_X:
            // Wait for the packet type (cmd or req)
            input_state = MSP_HEADER_V2_NATIVE;

            // Start of a new packet
            // reset the packet, offset iterator, and CRC
            packet.type = MSP_PACKET_UNKNOWN;
            packet.flags = 0;
            packet.function = 0;
            packet.payload_size = 0;
            packet.payload_offset = 0;
            packet.read_error = false;

            offset = 0;
            crc = 0;

            switch (c) {
                case '<':
                    packet.type = MSP_PACKET_COMMAND;
                    break;
                case '>':
                    packet.type = MSP_PACKET_RESPONSE;
                    break;
                default:
                    packet.type = MSP_PACKET_UNKNOWN;
                    input_state = MSP_IDLE;
                    break;
            }
            break;

        case MSP_HEADER_V2_NATIVE:
            // Read bytes until we have a full header
            input_buffer[offset++] = c;
            crc = msp_crc8_dvb_s2(crc, c);

            // If we've received the correct amount of bytes for a full header
            if (offset == sizeof(mspHeaderV2_t)) {
                // Copy header values into packet
                mspHeaderV2_t* header = (mspHeaderV2_t*)&input_buffer[0];
                packet.payload_size = header->payload_size;
                packet.function = header->function;
                packet.flags = header->flags;
                // reset the offset iterator for re-use in payload below
                offset = 0;
                input_state = MSP_PAYLOAD_V2_NATIVE;
            }
            break;

        case MSP_PAYLOAD_V2_NATIVE:
            // Read bytes until we reach payload_size
            packet.payload[offset++] = c;
            crc = msp_crc8_dvb_s2(crc, c);

            // If we've received the correct amount of bytes for payload
            if (offset == packet.payload_size) {
                // Then we're up to the CRC
                input_state = MSP_CHECKSUM_V2_NATIVE;
            }
            break;

        case MSP_CHECKSUM_V2_NATIVE:
            processed_byte = true;
            input_state = MSP_IDLE;
            // Assert that the checksums match
            if (crc == c) {
                msp_process_packet(&packet);
            }
            else {
                Printf("CRC failure on MSP packet - Got %d expected %d", c, crc);
            }
            break;

        default:
            input_state = MSP_IDLE;
            break;
    }

    if (processed_byte || input_state != MSP_IDLE) {
        return true;
    }
    return false;
}

mspPacket_t* msp_get_received_packet()
{
    return &packet;
}

void msp_mark_packet_received()
{
    // Set input state to idle, ready to receive the next packet
    // The current packet data will be discarded internally
    input_state = MSP_IDLE;
}
