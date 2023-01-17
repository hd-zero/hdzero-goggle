#ifndef __UTIL_MATH_H__
#define __UTIL_MATH_H__

#define constrain(value, min, max)  (value < min ? min : (value > max ? max : value))

float normalize(float value, float start, float end);
void rotate(float pn[3], const float rot[3]);

#endif // __UTIL_MATH_H__