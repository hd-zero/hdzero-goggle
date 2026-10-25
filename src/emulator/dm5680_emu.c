// dm5680_emu.c - Emulator-only mock of the DM5680 receiver's serial link.
//
// On the goggle, two DM5680 chips talk to the app over UART. The app polls them
// (version, valid-channel, RSSI) and they answer; dm5680.c's recv threads block in
// select() on the real serial fd until a reply arrives. The desktop emulator has no
// DM5680, so uart_open() hands back a socketpair instead of a tty (see uart.c). This
// module sits on the *device* end of those socketpairs and plays the receiver: it
// reads the goggle's command frames and writes back plausible telemetry, so the app
// sees a connected receiver -- and its recv threads block on real data at 0% CPU
// rather than spinning on a dead fd.
//
// Wire format (see dm5680.c):
//   host -> device command:  0xAA 0x55 LEN CMD [payload...]   (LEN = bytes after LEN)
//   device -> host response:  0xCC 0x33 LEN CMD [data...]      (LEN = bytes after LEN)

#ifdef EMULATOR_BUILD

#include "dm5680_emu.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <log/log.h>

#include "driver/uart.h"

// Canned telemetry the mock reports. Values chosen to look like a healthy link.
#define EMU_RX_VER   0x0F // receiver firmware version byte
#define EMU_RSSI_0   90
#define EMU_RSSI_1   88
#define EMU_DLQ      100  // downlink quality (0..100)
#define EMU_STAT     0
// Whether the mock reports a locked HDZero channel. 0 = present but no lock, so the
// app does NOT auto-tune out of the menu on boot/scan (the FPV view still shows the
// fake video -- compositing is gated on the view, not this flag). Set to 1 to
// emulate a live signal that autoscan will lock onto and switch to video.
#define EMU_CHANNEL_VALID 0

// DM5680 UART ports (must match uart_open() calls in uart_init()).
static const int kPorts[2] = {1, 2};

typedef struct {
    int fd; // device end of the UART socketpair
    pthread_t tid;
} emu_port_t;

static emu_port_t g_ports[2];

// Frame a response payload (CMD + data) as 0xCC 0x33 LEN <payload> and send it.
static void send_resp(int fd, const uint8_t *payload, uint8_t len) {
    uint8_t frame[64];
    if (len > sizeof(frame) - 3)
        return;
    frame[0] = 0xCC;
    frame[1] = 0x33;
    frame[2] = len;
    memcpy(&frame[3], payload, len);
    ssize_t rc = write(fd, frame, (size_t)len + 3);
    (void)rc;
}

// Answer one command from the goggle. Only the polling queries need a reply;
// config writes (fan speed, resets, etc.) are accepted silently, like real HW.
static void handle_cmd(int fd, uint8_t cmd) {
    switch (cmd) {
    case 0x01: { // version request
        uint8_t d[] = {0x01, EMU_RX_VER};
        send_resp(fd, d, sizeof(d));
        break;
    }
    case 0x11: { // valid-channel request -> report the channel as valid
        uint8_t d[] = {0x11, 1};
        send_resp(fd, d, sizeof(d));
        break;
    }
    case 0x12: { // RSSI request -> rssi0, rssi1, DLQ, Stat, crc(sum of the four)
        uint8_t crc = (uint8_t)(EMU_RSSI_0 + EMU_RSSI_1 + EMU_DLQ + EMU_STAT);
        uint8_t d[] = {0x12, EMU_RSSI_0, EMU_RSSI_1, EMU_DLQ, EMU_STAT, crc};
        send_resp(fd, d, sizeof(d));
        break;
    }
    case 0x14: { // VTX info -> type, ver, stat, crc(sum of the three)
        uint8_t type = 1, ver = 1, stat = 1;
        uint8_t crc = (uint8_t)(type + ver + stat);
        uint8_t d[] = {0x14, type, ver, stat, crc};
        send_resp(fd, d, sizeof(d));
        break;
    }
    default:
        break; // no reply expected
    }
}

// Read host command bytes and drive a tiny 0xAA/0x55/LEN/CMD/payload parser. The
// read() blocks until the goggle actually sends something, so an idle link costs
// no CPU -- exactly like waiting on a real UART.
static void *port_thread(void *arg) {
    emu_port_t *p = (emu_port_t *)arg;
    uint8_t buf[128];
    int state = 0;
    uint8_t remaining = 0;

    for (;;) {
        ssize_t n = read(p->fd, buf, sizeof(buf));
        if (n <= 0) {
            if (n == 0)
                break; // peer closed
            usleep(5000);
            continue;
        }
        for (ssize_t i = 0; i < n; i++) {
            uint8_t b = buf[i];
            switch (state) {
            case 0:
                if (b == 0xAA)
                    state = 1;
                break;
            case 1:
                state = (b == 0x55) ? 2 : 0;
                break;
            case 2: // LEN = number of bytes after LEN (CMD + payload)
                remaining = b;
                state = remaining ? 3 : 0;
                break;
            case 3: // CMD
                handle_cmd(p->fd, b);
                remaining--;
                state = remaining ? 4 : 0;
                break;
            case 4: // consume this command's payload bytes
                if (--remaining == 0)
                    state = 0;
                break;
            }
        }
    }
    return NULL;
}

void dm5680_emu_start(void) {
    int started = 0;
    for (int i = 0; i < 2; i++) {
        int fd = uart_emu_peer_fd(kPorts[i]);
        if (fd < 0)
            continue;
        g_ports[i].fd = fd;
        if (pthread_create(&g_ports[i].tid, NULL, port_thread, &g_ports[i]) == 0)
            started++;
    }
    LOGI("dm5680_emu: mock receiver attached on %d UART(s) (version, valid channel, RSSI)", started);
}

#endif // EMULATOR_BUILD
