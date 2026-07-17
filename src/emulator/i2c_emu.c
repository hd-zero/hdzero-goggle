// i2c_emu.c - Emulator-only I2C driver.
//
// CMake compiles this instead of src/driver/i2c.c when EMULATOR_BUILD, so the
// hardware driver stays free of host/emulator #ifdefs. The emulator has no I2C bus,
// so every access no-ops -- exactly the "device not available" behaviour the goggle
// driver already exhibits on a dev box with no /dev/i2c-* nodes (reads return 0,
// writes report failure). Callers (bmi270, fans, page_version, ...) tolerate this.
#ifdef EMULATOR_BUILD

#include "i2c.h"

#include <stdbool.h>

bool iic_is_port_ready(int port) {
    (void)port;
    return false;
}

void iic_init(void) {}

uint8_t i2c_read(int port, uint8_t slave_address, uint8_t addr) {
    (void)port, (void)slave_address, (void)addr;
    return 0;
}

int i2c_write(int port, uint8_t slave_address, uint8_t addr, uint8_t val) {
    (void)port, (void)slave_address, (void)addr, (void)val;
    return -1;
}

int8_t i2c_read_n(int port, uint8_t slave_address, uint8_t addr, uint8_t *data, uint16_t len) {
    (void)port, (void)slave_address, (void)addr, (void)data, (void)len;
    return -1;
}

int8_t i2c_write_n(int port, uint8_t slave_address, uint8_t addr, uint8_t *val, uint16_t len) {
    (void)port, (void)slave_address, (void)addr, (void)val, (void)len;
    return -1;
}

#endif // EMULATOR_BUILD
