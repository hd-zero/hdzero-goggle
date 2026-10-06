// uart_emu.c - Emulator-only UART driver.
//
// CMake compiles THIS instead of the goggle's src/driver/uart.c when EMULATOR_BUILD,
// so the hardware driver stays completely free of emulator special-casing. It
// implements the same uart.h interface, plus uart_emu_peer_fd() for a mock device.
//
// POSIX (Linux / macOS): each UART is backed by a socketpair, giving the fd true
// UART semantics -- read()/select() block in the kernel until bytes arrive. The
// DM5680 recv threads then sleep at 0% CPU instead of busy-spinning on a dead fd,
// and a mock device (dm5680_emu) answers commands on the peer end.
//
// Windows: the compat select() is a stub (no real fd multiplexing), so there is no
// selectable UART to back. uart_open() reports "no device" (-1); the DM5680 recv
// threads stay off (guarded in dm5680.c) and the UART is simply inert. This keeps
// the behaviour Windows already had -- no busy-spin -- without a socket layer.

#ifdef EMULATOR_BUILD

#include "driver/uart.h"

#include <stdint.h>
#include <unistd.h>
#ifndef _WIN32
#include <sys/socket.h>
#endif

#ifndef _WIN32
// Device (peer) end of each UART socketpair; the mock reads/writes here. -1 = closed.
static int uart_emu_peer[UART_PORTS] = {-1, -1, -1, -1};
#endif

int uart_emu_peer_fd(int port_num) {
#ifndef _WIN32
    if (port_num < 0 || port_num >= UART_PORTS)
        return -1;
    return uart_emu_peer[port_num];
#else
    (void)port_num;
    return -1; // no mock device on the Windows emulator
#endif
}

int uart_open(int port_num) {
#ifndef _WIN32
    if (port_num < 0 || port_num >= UART_PORTS)
        return -1;
    int sp[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sp) != 0)
        return -1;
    uart_emu_peer[port_num] = sp[1]; // device side, for the mock
    return sp[0];                    // app side, the "UART fd"
#else
    (void)port_num;
    return -1;
#endif
}

int uart_set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop) {
    // The emulator UART is a socketpair, not a tty; no line configuration applies.
    (void)fd, (void)nSpeed, (void)nBits, (void)nEvent, (void)nStop;
    return 0;
}

int uart_read(int fd, uint8_t *data, int len) {
    return (int)read(fd, data, len);
}

int uart_write(int fd, uint8_t *data, int len) {
    return (int)write(fd, data, len);
}

void uart_close(int fd) {
    close(fd);
}

int uart_read_byte(int fd, uint8_t *data) {
    return uart_read(fd, data, 1);
}

int uart_write_byte(int fd, uint8_t data) {
    return uart_write(fd, &data, 1);
}

#endif // EMULATOR_BUILD
