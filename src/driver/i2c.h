#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void iic_init();
uint8_t i2c_read(int port, uint8_t slave_address, uint8_t addr);
int i2c_write(int port, uint8_t slave_address, uint8_t addr, uint8_t val);

int8_t i2c_read_n(int port, uint8_t slave_address, uint8_t addr, uint8_t *data, uint16_t len);
int8_t i2c_write_n(int port, uint8_t slave_address, uint8_t addr, uint8_t *val, uint16_t len);

#define BMI_I2C_WRITE(addr, val, len) i2c_write_n(1, 0x68, addr, val, len)
#define BMI_I2C_READ(addr, val, len)  i2c_read_n(1, 0x68, addr, val, len)

///////////////////////////////////////////////////////////////////////////////
// I2C devices
//
// Name   Ports    Device    Address
// R_I2C	 1      IT66121	    0x4C (HDMI Out)
//	            MCP3021	    0x4D
//	            BMI270	    0x68
//	            NCT75	    0x48
//
// M_I2C	 2      Main FPGA	0x64
//	            AL FPGA	    0x65
//	            GM7150	    0x5D
//	            NCT75	    0x48
//
// L_I2C	 3      IT66021	    0x49 (HDMI In)
//	            NCT75	    0x48
//

#define ADDR_IT66121 0x4C
#define ADDR_MCP3021 0x4D
#define ADDR_BMI270  0x68
#define ADDR_NCT75   0x48

#define ADDR_FPGA   0x64
#define ADDR_AL     0x65
#define ADDR_TP2825 0x44

#define ADDR_IT66021 0x49

#define I2C_Write(s, a, d) i2c_write(2, s, a, d)
#define I2C_Read(s, a)     i2c_read(2, s, a)

#define I2C_R_Write(s, a, d) i2c_write(1, s, a, d)
#define I2C_R_Read(s, a)     i2c_read(1, s, a)

#define I2C_L_Write(s, a, d) i2c_write(3, s, a, d)
#define I2C_L_Read(s, a)     i2c_read(3, s, a)

#ifdef __cplusplus
}
#endif
