#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define MOTOR_RPM_COUNT           4
#define MOTOR_RPM_Q_BITS          12
#define MOTOR_RPM_Q_MAX           4095
#define MOTOR_RPM_STEP            32
#define MOTOR_RPM_MAX_REPR        131040u /* 4095 * 32 */
#define MOTOR_RPM_PACKED_BYTES    6
#define MOTOR_RPM_SERVICE_LEGACY  15
#define MOTOR_RPM_SERVICE_LEN     21 /* legacy 15 + 6 RPM */
#define MOTOR_RPM_SERVICE_OFF     16 /* first RPM byte in VRX rx_buf */

uint16_t motor_rpm_quantize(uint32_t rpm);
uint32_t motor_rpm_dequantize(uint16_t q);
void motor_rpm_pack(const uint16_t q[MOTOR_RPM_COUNT], uint8_t out[MOTOR_RPM_PACKED_BYTES]);
void motor_rpm_unpack(const uint8_t in[MOTOR_RPM_PACKED_BYTES], uint16_t q[MOTOR_RPM_COUNT]);

/* MSP_MOTOR_TELEMETRY payload → motors 0..3. Returns 1 on success. */
int motor_rpm_from_msp_telemetry(const uint8_t *payload, uint8_t len, uint32_t rpm[MOTOR_RPM_COUNT]);

#ifdef __cplusplus
}
#endif
