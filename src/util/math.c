#include <math.h>
#include <memory.h>

#include "math.h"

// Normalizes any number to an arbitrary range
// by assuming the range wraps around when going below min or above max
float normalize(float value, float start, float end) {
    float width = end - start;         //
    float offsetValue = value - start; // value relative to 0

    return (offsetValue - (floor(offsetValue / width) * width)) + start;
    // + start to reset back to start of original range
}

// Rotate a point (pn) in space in Order X -> Y -> Z
void rotate(float pn[3], const float rot[3]) {
    float out[3];

    // X-axis Rotation
    if (rot[0] != 0) {
        out[0] = pn[0] * 1 + pn[1] * 0 + pn[2] * 0;
        out[1] = pn[0] * 0 + pn[1] * cos(rot[0]) - pn[2] * sin(rot[0]);
        out[2] = pn[0] * 0 + pn[1] * sin(rot[0]) + pn[2] * cos(rot[0]);
        memcpy(pn, out, sizeof(out[0]) * 3);
    }

    // Y-axis Rotation
    if (rot[1] != 0) {
        out[0] = pn[0] * cos(rot[1]) - pn[1] * 0 + pn[2] * sin(rot[1]);
        out[1] = pn[0] * 0 + pn[1] * 1 + pn[2] * 0;
        out[2] = -pn[0] * sin(rot[1]) + pn[1] * 0 + pn[2] * cos(rot[1]);
        memcpy(pn, out, sizeof(out[0]) * 3);
    }

    // Z-axis Rotation
    if (rot[2] != 0) {
        out[0] = pn[0] * cos(rot[2]) - pn[1] * sin(rot[2]) + pn[2] * 0;
        out[1] = pn[0] * sin(rot[2]) + pn[1] * cos(rot[2]) + pn[2] * 0;
        out[2] = pn[0] * 0 + pn[1] * 0 + pn[2] * 1;
        memcpy(pn, out, sizeof(out[0]) * 3);
    }
}

void safe_update_value(int min, int max, int *val, int delta) {
    int new_val = *val + delta;

    if (new_val > max)
        *val = max;
    else if (new_val < min)
        *val = min;
    else
        *val = new_val;
}