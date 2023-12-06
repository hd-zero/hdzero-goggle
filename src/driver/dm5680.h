#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "dm6302.h"
#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>

typedef struct {
    uint8_t rx_ver;      // CMD1
    uint8_t rx_valid;    // CMD11
    uint8_t rx_rssi[2];  // CMD12
    uint8_t rx_DLQ;      //  0-8, 0=worst,8=best
    uint8_t rx_Stat;     // b0=video OK, b4=50/60
    uint8_t rx_vtx_type; // CMD14
    uint8_t rx_vtx_ver;
    uint8_t rx_vtx_stat;
    uint8_t rx_rbtn; // CMD20,0=short press,1=long press
    uint8_t rx_regval;
} rx_status_t;

extern rx_status_t rx_status[2];
extern atomic_int g_osd_update_cnt;

void UART1_TX(uint8_t d);
void UART2_TX(uint8_t d);

int uart_init();

void DM5680_req_ver();
void DM5680_get_ver(uint8_t sel, uint8_t *payload);

void DM5680_SetFanSpeed(uint8_t sel, uint8_t speed); // sel=0/URART1/Right,sel=1/URART2/Left
void DM5680_Beep(uint8_t on);

void DM5680_SetFPS(uint8_t fps);
void DM5680_SetBB(uint8_t on);
void DM5680_ResetHDMI_RX(uint8_t on);
void DM5680_ResetHDMI_TX(uint8_t on);
void DM5680_Power_AnalogModule(uint8_t on);
void DM5680_ResetRF(uint8_t on);
void DM5680_SetBR(uint8_t on);

void DM5680_req_rssi();
void DM5680_get_rssi(uint8_t sel, uint8_t *payload);

void DM5680_WriteReg(uint8_t page, uint8_t addr, uint8_t wdat);
void DM5680_ReadReg(uint8_t page, uint8_t addr);

void DM5680_req_vtxinfo(uint8_t sel);
void DM5680_get_vtxinfo(uint8_t sel, uint8_t *payload);

void DM5680_req_vldflg();
void DM5680_clear_vldflg();
void DM5680_get_vldflg(uint8_t sel, uint8_t *payload);
void DM5680_get_regval(uint8_t sel, uint8_t *payload);

void DM5680_OSD_parse(uint8_t *buf, uint8_t len);

#ifdef __cplusplus
}
#endif
