#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define constrain(value, min, max) \
    (value < min ? min : (value > max ? max : value))

#define fmap(value, in_min, in_max, out_min, out_max) \
    (((value) - (in_min)) * ((out_max) - (out_min)) / ((in_max) - (in_min)) + (out_min))

float normalize(float value, float start, float end);
void rotate(float pn[3], const float rot[3]);
void safe_update_value(int min, int max, int *val, int delta);

#ifdef __cplusplus
}
#endif
