#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <stdlib.h>
#include <semaphore.h>


#include "common.hh"
#include "elrs.h"
#include "osd.h"

#include "../driver/uart.h"
#include "../driver/dm5680.h"
#include "../driver/mcp3021.h"

#include "../page/page_common.h"
#include "../page/page_scannow.h"
#include "../page/page_version.h"

static mspState_e  input_state;
static uint16_t    offset;
static uint8_t     input_buffer[MSP_PORT_INBUF_SIZE];
static mspPacket_t packet;
static uint8_t     crc;

static int fd_esp32 = -1;
static sem_t response_semaphore;
static mspPacket_t response_packet;

void msp_process_packet(mspPacket_t *packet);

static const uint16_t freq_table[] = {
    5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917, // R
    5740, 5760 // F
};

void elrs_init()
{
	sem_init(&response_semaphore, 0, 0);
}

void esp32_handler_set_uart(uint32_t fd_uart)
{
    fd_esp32 = fd_uart;
}

uint8_t msp_crc8_dvb_s2(uint8_t crc, uint8_t a)
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

bool esp32_handler_process_byte(uint8_t c)
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
