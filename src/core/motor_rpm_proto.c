#include "motor_rpm_proto.h"

uint16_t motor_rpm_quantize(uint32_t rpm) {
    uint32_t q = (rpm + 16u) >> 5;
    if (q > MOTOR_RPM_Q_MAX)
        q = MOTOR_RPM_Q_MAX;
    return (uint16_t)q;
}

uint32_t motor_rpm_dequantize(uint16_t q) {
    if (q > MOTOR_RPM_Q_MAX)
        q = MOTOR_RPM_Q_MAX;
    return (uint32_t)q * MOTOR_RPM_STEP;
}

void motor_rpm_pack(const uint16_t q_in[MOTOR_RPM_COUNT], uint8_t out[MOTOR_RPM_PACKED_BYTES]) {
    uint16_t q0 = q_in[0] & MOTOR_RPM_Q_MAX;
    uint16_t q1 = q_in[1] & MOTOR_RPM_Q_MAX;
    uint16_t q2 = q_in[2] & MOTOR_RPM_Q_MAX;
    uint16_t q3 = q_in[3] & MOTOR_RPM_Q_MAX;

    out[0] = (uint8_t)(q0 & 0xff);
    out[1] = (uint8_t)(((q0 >> 8) & 0x0f) | ((q1 & 0x0f) << 4));
    out[2] = (uint8_t)((q1 >> 4) & 0xff);
    out[3] = (uint8_t)(q2 & 0xff);
    out[4] = (uint8_t)(((q2 >> 8) & 0x0f) | ((q3 & 0x0f) << 4));
    out[5] = (uint8_t)((q3 >> 4) & 0xff);
}

void motor_rpm_unpack(const uint8_t in[MOTOR_RPM_PACKED_BYTES], uint16_t q[MOTOR_RPM_COUNT]) {
    q[0] = (uint16_t)(in[0] | ((in[1] & 0x0f) << 8));
    q[1] = (uint16_t)(((in[1] >> 4) & 0x0f) | (in[2] << 4));
    q[2] = (uint16_t)(in[3] | ((in[4] & 0x0f) << 8));
    q[3] = (uint16_t)(((in[4] >> 4) & 0x0f) | (in[5] << 4));
}

int motor_rpm_from_msp_telemetry(const uint8_t *payload, uint8_t len, uint32_t rpm[MOTOR_RPM_COUNT]) {
    uint8_t i;
    uint8_t base;

    if (!payload || !rpm)
        return 0;
    if (len < 53)
        return 0;
    if (payload[0] < 4)
        return 0;

    for (i = 0; i < MOTOR_RPM_COUNT; i++) {
        base = 1 + (i * 13);
        rpm[i] = (uint32_t)payload[base] |
                 ((uint32_t)payload[base + 1] << 8) |
                 ((uint32_t)payload[base + 2] << 16) |
                 ((uint32_t)payload[base + 3] << 24);
    }
    return 1;
}
