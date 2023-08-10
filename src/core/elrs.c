#include "elrs.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include <log/log.h>

#include "core/app_state.h"
#include "core/battery.h"
#include "core/common.hh"
#include "core/dvr.h"
#include "core/ht.h"
#include "core/msp_displayport.h"
#include "core/osd.h"
#include "core/settings.h"
#include "driver/beep.h"
#include "driver/dm5680.h"
#include "driver/hardware.h"
#include "driver/rtc.h"
#include "driver/uart.h"
#include "ui/page_common.h"
#include "ui/page_scannow.h"
#include "ui/page_version.h"

static mspState_e input_state;
static uint16_t offset;
static uint8_t input_buffer[MSP_PORT_INBUF_SIZE];
static mspPacket_t packet;
static uint8_t crc;

static int fd_esp32 = -1;
static sem_t response_semaphore;
static mspPacket_t response_packet;
static int record_state;
static uint32_t record_time = 0;
static bool headtracking_enabled = false;
static volatile bool cancelled = false;

uint16_t elrs_osd[HD_VMAX][HD_HMAX];
static uint16_t elrs_osd_overlay[HD_VMAX][HD_HMAX];

void msp_process_packet();
static void handle_osd(uint8_t *payload, uint8_t size);

static const uint16_t freq_table[] = {
    5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917, // R1-8
    5760, 5800                                      // F2/4
};

static const uint8_t channel_map[] = {
    0, 0, 0, 0, 0, 0, 0, 0,  // A
    0, 0, 0, 0, 0, 0, 0, 0,  // B
    0, 0, 0, 0, 0, 0, 0, 0,  // E
    0, 9, 0, 10, 0, 0, 0, 0, // F
    1, 2, 3, 4, 5, 6, 7, 8,  // R
    0, 0, 0, 0, 0, 0, 0, 0,  // L
};

void elrs_init() {
    sem_init(&response_semaphore, 0, 0);
    input_state = MSP_IDLE;
}

void esp32_handler_set_uart(uint32_t fd_uart) {
    fd_esp32 = fd_uart;
}

uint8_t msp_crc8_dvb_s2(uint8_t crc, uint8_t a) {
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

bool esp32_handler_process_byte(uint8_t c) {
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
            mspHeaderV2_t *header = (mspHeaderV2_t *)&input_buffer[0];
            packet.payload_size = le16toh(header->payload_size);
            packet.function = le16toh(header->function);
            packet.flags = header->flags;
            // reset the offset iterator for re-use in payload below
            offset = 0;
            if (packet.payload_size == 0)
                input_state = MSP_CHECKSUM_V2_NATIVE;
            else
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
            msp_process_packet();
        } else {
            LOGE("CRC failure on MSP packet - Got %d expected %d", c, crc);
        }
        input_state = MSP_IDLE;
        break;

    default:
        input_state = MSP_IDLE;
        break;
    }

    if (processed_byte || input_state != MSP_IDLE) {
        // LOGI("Processed %02x %d", c, input_state);
        return true;
    }
    return false;
}

void esp32_handler_timeout() {
    if (record_time != 0 && record_time <= time(NULL)) {
        record_time = 0;
        dvr_cmd(record_state);
    }
}

void msp_process_packet() {
    if (packet.type == MSP_PACKET_COMMAND) {
        switch (packet.function) {
        case MSP_GET_BAND_CHAN: {
            uint8_t chan, ch, band;
            ch = g_setting.scan.channel;
            band = g_setting.source.hdzero_band;

            if (band == SETTING_SOURCES_HDZERO_BAND_RACEBAND) {
                if (ch <= 8) {
                    chan = ch - 1 + (4 * 8); // Map R1..8
                } else {
                    chan = ((ch - 9) * 2) + (3 * 8) + 1; // Map F2/4
                }
            } else {                   // if (band == SETTING_SOURCES_HDZERO_BAND_LOWBAND)
                chan = ch - 1 + 5 * 8; // Map L1..8
            }
            msp_send_packet(MSP_GET_BAND_CHAN, MSP_PACKET_RESPONSE, 1, &chan);
        } break;
        case MSP_SET_BAND_CHAN: {
            uint8_t ch, chan = packet.payload[0];
            if (g_source_info.source == SOURCE_HDZERO) { // HDZero mode
                chan = chan < 48 ? channel_map[chan] : 0;
                ch = g_setting.scan.channel & 0xF;
                if (chan != 0 && (chan != ch || g_app_state != APP_STATE_VIDEO)) {
                    g_setting.scan.channel = chan;
                    beep();
                    pthread_mutex_lock(&lvgl_mutex);
                    dvr_cmd(DVR_STOP);
                    app_switch_to_hdzero(true);
                    app_state_push(APP_STATE_VIDEO);
                    pthread_mutex_unlock(&lvgl_mutex);
                }
            }
        } break;
        case MSP_GET_FREQ: {
            uint8_t ch = g_setting.scan.channel & 0xF;
            uint16_t freq = freq_table[ch - 1];
            uint8_t buf[2] = {freq & 0xFF, freq >> 8};
            msp_send_packet(MSP_GET_FREQ, MSP_PACKET_RESPONSE, sizeof(buf), buf);
        } break;
        case MSP_SET_FREQ: {
            uint16_t freq = packet.payload[0] | (uint16_t)packet.payload[1] << 8;
            uint8_t ch = g_setting.scan.channel & 0xF;
            if (g_source_info.source == SOURCE_HDZERO) { // HDZero mode
                for (int i = 0; i < 10; i++) {
                    int chan = i + 1;
                    if (freq == freq_table[i] && (ch != chan || g_app_state != APP_STATE_VIDEO) && chan > 0 && chan < 11) {
                        g_setting.scan.channel = chan;
                        beep();
                        pthread_mutex_lock(&lvgl_mutex);
                        app_switch_to_hdzero(true);
                        app_state_push(APP_STATE_VIDEO);
                        pthread_mutex_unlock(&lvgl_mutex);
                        break;
                    }
                }
            }
        } break;
        case MSP_GET_REC_STATE: {
            uint8_t buf = dvr_is_recording ? 1 : 0;
            msp_send_packet(MSP_GET_REC_STATE, MSP_PACKET_RESPONSE, 1, &buf);
        } break;
        case MSP_SET_REC_STATE: {
            if (g_app_state == APP_STATE_VIDEO) {
                record_state = packet.payload[0] == 0 ? 1 : 2;
                uint32_t delay = packet.payload[1] | (uint32_t)packet.payload[2] << 8;
                if (delay == 0)
                    dvr_cmd(record_state);
                else
                    record_time = time(NULL) + delay;
            }
        } break;
        case MSP_GET_VRX_MODE:
            break;
        case MSP_SET_VRX_MODE:
            break;
        case MSP_GET_RSSI: {
            uint8_t buf[5] = {4, rx_status[0].rx_rssi[0], rx_status[0].rx_rssi[1], rx_status[1].rx_rssi[0], rx_status[1].rx_rssi[1]};
            msp_send_packet(MSP_GET_VERSION, MSP_PACKET_RESPONSE, sizeof(buf), buf);
        } break;
        case MSP_GET_BAT_VOLTS: {
            uint8_t buf[2] = {g_battery.voltage & 0xFF, g_battery.voltage >> 8};
            msp_send_packet(MSP_GET_BAT_VOLTS, MSP_PACKET_RESPONSE, sizeof(buf), buf);
        } break;
        case MSP_GET_VERSION: {
            sys_version_t sys_version;
            if (generate_current_version(&sys_version) == 0) {
                uint8_t buf[4] = {3, sys_version.app_major, sys_version.app_minor, sys_version.app_patch};
                msp_send_packet(MSP_GET_VERSION, MSP_PACKET_RESPONSE, sizeof(buf), buf);
            } else {
                msp_send_packet(MSP_GET_VERSION, MSP_PACKET_UNKNOWN, 0, NULL);
            }
        } break;
        case MSP_SET_BUZZER:
            beep_dur((packet.payload[0] | packet.payload[1] << 8));
            break;
        case MSP_SET_OSD_ELEM:
            handle_osd(packet.payload, packet.payload_size);
            break;
        case MSP_SET_HT_ENABLE:
            if (packet.payload_size > 0) {
                headtracking_enabled = packet.payload[0];
                if (headtracking_enabled) {
                    ht_set_center_position();
                }
            }
            break;
        case MSP_SET_RTC:
            if (packet.payload_size > 0) {
                struct rtc_date rd;
                rd.year = packet.payload[0] + 1900;
                rd.month = packet.payload[1] + 1;
                rd.day = packet.payload[2];
                rd.hour = packet.payload[3];
                rd.min = packet.payload[4];
                rd.sec = packet.payload[5];
                rtc_set_clock(&rd);
            }
            break;
        }
    } else if (packet.type == MSP_PACKET_RESPONSE) {
        memcpy(&response_packet, &packet, sizeof(response_packet));
        sem_post(&response_semaphore);
    }
}

bool msp_read_resposne(uint16_t function, uint16_t *payload_size, uint8_t *payload) {
    if (sem_trywait(&response_semaphore))
        return false;
    if (response_packet.function != function)
        return false;
    uint16_t size = response_packet.payload_size < *payload_size ? response_packet.payload_size : *payload_size;
    memcpy(payload, response_packet.payload, size);
    *payload_size = size;
    return true;
}

mspAwaitResposne_e msp_await_resposne(uint16_t function, uint16_t payload_size, uint8_t *payload, uint32_t timeout_ms) {
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        LOGE("clock_gettime failed");
        return false;
    }

    // relative time to absolute time
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec %= 1000000000;
    }
    ts.tv_sec += timeout_ms / 1000;

    cancelled = false;
    while (sem_timedwait(&response_semaphore, &ts) == 0) {
        if (cancelled)
            return AWAIT_CANCELLED;
        if (response_packet.function == function) {
            if (response_packet.payload_size >= payload_size &&
                memcmp(response_packet.payload, payload, payload_size) == 0) {
                return AWAIT_SUCCESS;
            }
            return AWAIT_FAILED;
        }
    }
    return AWAIT_TIMEDOUT;
}

void msp_cancel_await() {
    cancelled = true;
    sem_post(&response_semaphore);
}

void msp_send_packet(uint16_t function, mspPacketType_e type, uint16_t payload_size, uint8_t *payload) {
    uint8_t buffer[16] = {'$', 'X', type, 0x00, function & 0xFF, function >> 8, payload_size & 0xFF, payload_size >> 8};
    memcpy(buffer + 8, payload, payload_size);
    uint8_t crc = 0;
    while (!sem_trywait(&response_semaphore))
        ;
    for (int i = 3; i < payload_size + 8; i++)
        crc = msp_crc8_dvb_s2(crc, buffer[i]);
    buffer[payload_size + 8] = crc;
    response_packet.function = 0;
    uart_write(fd_esp32, buffer, payload_size + 9);
}

void msp_ht_update(uint16_t pan, uint16_t tilt, uint16_t roll) {
    if (g_setting.elrs.enable) {
        uint8_t payload[6] = {pan & 0xFF, pan >> 8, tilt & 0xFF, tilt >> 8, roll & 0xFF, roll >> 8};
        msp_send_packet(MSP_SET_PTR, MSP_PACKET_COMMAND, sizeof(payload), payload);
    }
}

bool elrs_headtracking_enabled() {
    return headtracking_enabled;
}

void msp_channel_update() {
    // Channel 1...18 for R1...8, F2 and F4, L1...8
    uint8_t const ch = g_setting.scan.channel;
    uint8_t const band = g_setting.source.hdzero_band;
    uint8_t chan;

    if (ch == 0 || ch > CHANNEL_NUM)
        return; // Invalid value -> ignore
    if (band == SETTING_SOURCES_HDZERO_BAND_RACEBAND) {
        if (ch <= 8) {
            chan = ch - 1 + (4 * 8); // Map R1..8
        } else {
            chan = ((ch - 9) * 2) + (3 * 8) + 1; // Map F2/4
        }
    } else {                   // if (band == SETTING_SOURCES_HDZERO_BAND_LOWBAND)
        chan = ch - 1 + 5 * 8; // Map L1..8
    }
    msp_send_packet(MSP_SET_BAND_CHAN, MSP_PACKET_COMMAND, sizeof(chan), &chan);
    LOGI("MSPv2 MSP_SET_BAND_CHAN %d sent", chan);
}

void elrs_clear_osd() {
    for (int i = 0; i < HD_VMAX; i++) {
        for (int j = 0; j < HD_HMAX; j++) {
            elrs_osd_overlay[i][j] = 0x20;
        }
    }
}

static void handle_osd(uint8_t payload[], uint8_t size) {
    switch (payload[0]) {
    case 0x00: // hearbeat
        break;
    case 0x01: // release port
        elrs_clear_osd();
        memcpy(elrs_osd, elrs_osd_overlay, sizeof(elrs_osd));
        osd_signal_update();
        break;
    case 0x02: // clear screen
        elrs_clear_osd();
        break;
    case 0x03: // write string
    {
        uint8_t row = payload[1];
        uint8_t col = payload[2];
        uint8_t attr = payload[3];
        // HDZero only allows 2 pages of characters (i.e. 512 characters)
        int page = (attr & 1) * 256;
        uint8_t len = size - 4 < HD_HMAX - col ? size - 4 : HD_HMAX - col;
        for (int j = 0; j < len; j++) {
            elrs_osd_overlay[row][col + j] = payload[j + 4] + page;
        }
    } break;
    case 0x04: // draw screen
        memcpy(elrs_osd, elrs_osd_overlay, sizeof(elrs_osd));
        osd_signal_update();
        break;
    }
}
