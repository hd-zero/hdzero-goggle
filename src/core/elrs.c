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

static const uint16_t freq_table[ANALOG_CHANNEL_NUM] = {
    5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725, // A
    5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866, // B
    5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945, // E
    5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, // F
    5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917, // R
    5362, 5399, 5436, 5473, 5510, 5547, 5584, 5621  // L
};

// Note: F8 and R7 are both same frequency (5880 MHz), so they are both mapped to 7.
static const uint8_t hdzero_channel_map[ANALOG_CHANNEL_NUM] = {
    0, 0, 0, 0, 0, 0, 0, 0,    // A
    0, 0, 0, 0, 0, 0, 0, 0,    // B
    9, 0, 0, 0, 0, 0, 0, 0,    // E
    10, 11, 0, 12, 0, 0, 0, 7, // F
    1, 2, 3, 4, 5, 6, 7, 8,    // R
    0, 0, 0, 0, 0, 0, 0, 0,    // L
};

static int get_freq_index(uint16_t const freq) {
    for (size_t i = 0; i < ANALOG_CHANNEL_NUM; i++) {
        if (freq_table[i] == freq) {
            return i;
        }
    }
    return -1; // Not found
}

static uint8_t hdz_ch2index(uint8_t is_lowband, uint8_t ch) {
    uint8_t chan;
    if (is_lowband == SETTING_SOURCES_HDZERO_BAND_RACEBAND) {
        if (ch <= 8) {
            chan = ch - 1 + (4 * 8); // Map R1..8
        } else if (ch == 9) {
            chan = 2 * 8; // Map E1
        } else if (ch == 10) {
            chan = 3 * 8; // Map F1
        } else if (ch == 11) {
            chan = 3 * 8 + 1; // Map F2
        } else if (ch == 12) {
            chan = 3 * 8 + 3; // Map F4
        }
    } else {
        chan = ch - 1 + 5 * 8; // Map L1..8
    }
    return chan;
}

static uint8_t hdz_index2ch(uint8_t index) {
    uint8_t chan;

    if (index < 48)
        chan = hdzero_channel_map[index];
    else
        chan = 0;

    return chan;
}

static void channel_channel_hdzero(uint8_t const channel) {
    if (channel == 0 || channel > HDZERO_CHANNEL_NUM) {
        LOGE("Invalid HDZero channel %d", channel);
        return;
    }
    if ((g_setting.scan.channel & 0xF) != channel || g_app_state != APP_STATE_VIDEO) {
        g_setting.scan.channel = channel;
        beep();
        pthread_mutex_lock(&lvgl_mutex);
        dvr_cmd(DVR_STOP);
        app_switch_to_hdzero(true);
        app_state_push(APP_STATE_VIDEO);
        pthread_mutex_unlock(&lvgl_mutex);
    }
}

static void change_channel_analog(uint8_t const channel) {
    if (channel == 0 || channel > ANALOG_CHANNEL_NUM) {
        LOGE("Invalid analog channel %d", channel);
        return;
    }
    if (g_setting.scan.channel != channel || g_app_state != APP_STATE_VIDEO) {
        g_setting.source.analog_channel = channel;
        beep();
        pthread_mutex_lock(&lvgl_mutex);
        dvr_cmd(DVR_STOP);
        app_switch_to_analog(0);
        app_state_push(APP_STATE_VIDEO);
        pthread_mutex_unlock(&lvgl_mutex);
    }
}

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
            uint8_t chan = 0;
            if (g_source_info.source == SOURCE_HDZERO) {
                chan = hdz_ch2index(g_setting.source.hdzero_band, g_setting.scan.channel);
            } else if (g_source_info.source == SOURCE_AV_MODULE) {
                chan = g_setting.scan.channel - 1;
            }
            msp_send_packet(MSP_GET_BAND_CHAN, MSP_PACKET_RESPONSE, 1, &chan);
        } break;
        case MSP_SET_BAND_CHAN: {
            uint8_t const chan = packet.payload[0];
            if (g_source_info.source == SOURCE_HDZERO) {
                channel_channel_hdzero(hdz_index2ch(chan));
            } else {
#if defined(HDZBOXPRO) || defined(HDZGOGGLE2)
                if (g_source_info.source == SOURCE_AV_MODULE) {
                    change_channel_analog(chan + 1);
                }
#endif
            }
        } break;
        case MSP_GET_FREQ: {
            uint8_t ch = 0;
            uint16_t freq;
            uint8_t buf[2];
            if (g_source_info.source == SOURCE_HDZERO) {
                ch = hdz_ch2index(g_setting.source.hdzero_band, g_setting.scan.channel) + 1;
            } else if (g_source_info.source == SOURCE_AV_MODULE) {
                ch = g_setting.scan.channel;
            }
            freq = ch == 0 ? 0 : freq_table[ch - 1];
            buf[0] = freq & 0xff;
            buf[1] = freq >> 8;
            msp_send_packet(MSP_GET_FREQ, MSP_PACKET_RESPONSE, sizeof(buf), buf);
        } break;
        case MSP_SET_FREQ: {
            uint16_t const freq = packet.payload[0] | (uint16_t)packet.payload[1] << 8;
            int const freq_index = get_freq_index(freq);
            if (freq_index < 0) {
                LOGE("Invalid frequency %d", freq);
                break;
            }
            if (g_source_info.source == SOURCE_HDZERO) {
                uint8_t const new_ch = hdzero_channel_map[freq_index];
                if (new_ch == 0) {
                    LOGE("Invalid HDZero channel for frequency %d", freq);
                    break;
                }
                channel_channel_hdzero(new_ch);
            } else if (g_source_info.source == SOURCE_AV_MODULE) {
                change_channel_analog(freq_index + 1);
            }
        } break;
        case MSP_GET_REC_STATE: {
            uint8_t buf = dvr_is_recording ? 1 : 0;
            msp_send_packet(MSP_GET_REC_STATE, MSP_PACKET_RESPONSE, 1, &buf);
        } break;
        case MSP_SET_REC_STATE: {
            g_setting.ht.alarm_on_arm = packet.payload[0];
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
    // Channel 1...20 for R1...8, E1, F1, F2 and F4, L1...8
    uint8_t const ch = g_setting.scan.channel;
    uint8_t const band = g_setting.source.hdzero_band;
    uint8_t chan;

    if (ch == 0 || ch > HDZERO_CHANNEL_NUM)
        return; // Invalid value -> ignore
    if (band == SETTING_SOURCES_HDZERO_BAND_RACEBAND) {
        if (ch <= 8) {
            chan = ch - 1 + (4 * 8); // Map R1..8
        } else if (ch == 9) {
            chan = 2 * 8; // Map E1
        } else if (ch == 10) {
            chan = 3 * 8; // Map F1
        } else if (ch == 11) {
            chan = 3 * 8 + 1; // Map F2
        } else if (ch == 12) {
            chan = 3 * 8 + 3; // Map F4
        }
    } else {
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
