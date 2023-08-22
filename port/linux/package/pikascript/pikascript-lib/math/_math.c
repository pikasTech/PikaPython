#include <math.h>
#include "_math.h"

#define PI (3.141592653589793115997963468544185161590576171875l)
#define E (2.718281828459045090795598298427648842334747314453125l)
void _math___init__(PikaObj* self) {
    obj_setFloat(self, "pi", PI);
    obj_setFloat(self, "e", E);
}

pika_float _math_acos(PikaObj* self, pika_float x) {
    return acos(x);
}
pika_float _math_asin(PikaObj* self, pika_float x) {
    return asin(x);
}
pika_float _math_atan(PikaObj* self, pika_float x) {
    return atan(x);
}
pika_float _math_atan2(PikaObj* self, pika_float x, pika_float y) {
    return atan2(x, y);
}
int _math_ceil(PikaObj* self, pika_float x) {
    return ceil(x);
}
pika_float _math_cos(PikaObj* self, pika_float x) {
    return cos(x);
}
pika_float _math_cosh(PikaObj* self, pika_float x) {
    return cosh(x);
}
pika_float _math_degrees(PikaObj* self, pika_float x) {
    return x * 180.0 / PI;
}
pika_float _math_exp(PikaObj* self, pika_float x) {
    return exp(x);
}
pika_float _math_fabs(PikaObj* self, pika_float x) {
    return fabs(x);
}
int _math_floor(PikaObj* self, pika_float x) {
    return floor(x);
}
pika_float _math_fmod(PikaObj* self, pika_float x, pika_float y) {
    return fmod(x, y);
}
pika_float _math_log(PikaObj* self, pika_float x) {
    return log(x);
}
pika_float _math_log10(PikaObj* self, pika_float x) {
    return log10(x);
}
pika_float _math_log2(PikaObj* self, pika_float x) {
    return log2(x);
}
pika_float _math_pow(PikaObj* self, pika_float x, pika_float y) {
    return pow(x, y);
}
pika_float _math_radians(PikaObj* self, pika_float x) {
    return x * PI / 180.0;
}
pika_float _math_remainder(PikaObj* self, pika_float x, pika_float y) {
    return remainder(x, y);
}
pika_float _math_sin(PikaObj* self, pika_float x) {
    return sin(x);
}
pika_float _math_sinh(PikaObj* self, pika_float x) {
    return sinh(x);
}
pika_float _math_sqrt(PikaObj* self, pika_float x) {
    return sqrt(x);
}
pika_float _math_tan(PikaObj* self, pika_float x) {
    return tan(x);
}
pika_float _math_tanh(PikaObj* self, pika_float x) {
    return tanh(x);
}
pika_float _math_trunc(PikaObj* self, pika_float x) {
    return trunc(x);
}
