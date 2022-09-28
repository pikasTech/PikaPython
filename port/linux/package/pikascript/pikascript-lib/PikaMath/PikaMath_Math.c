#include "PikaMath_Math.h"
#include <math.h>

#define PI (3.141592653589793115997963468544185161590576171875l)
#define E  (2.718281828459045090795598298427648842334747314453125l)
//初始化，填入π和e的值
void PikaMath_Math___init__(PikaObj *self)
{
    obj_setFloat(self, "pi", PI);
    obj_setFloat(self, "e", PI);
}

pika_float PikaMath_Math_acos(PikaObj *self, pika_float x)
{
    return acos(x);
}
pika_float PikaMath_Math_asin(PikaObj *self, pika_float x)
{
    return asin(x);
}
pika_float PikaMath_Math_atan(PikaObj *self, pika_float x)
{
    return atan(x);
}
pika_float PikaMath_Math_atan2(PikaObj *self, pika_float x, pika_float y)
{
    return atan2(x,y);
}
int PikaMath_Math_ceil(PikaObj *self, pika_float x)
{
    return ceil(x);
}
pika_float PikaMath_Math_cos(PikaObj *self, pika_float x)
{
    return cos(x);
}
pika_float PikaMath_Math_cosh(PikaObj *self, pika_float x)
{
    return cosh(x);
}
pika_float PikaMath_Math_degrees(PikaObj *self, pika_float x)
{
    return x*180.0/PI;
}
pika_float PikaMath_Math_exp(PikaObj *self, pika_float x)
{
    return exp(x);
}
pika_float PikaMath_Math_fabs(PikaObj *self, pika_float x)
{
    return fabs(x);
}
int PikaMath_Math_floor(PikaObj *self, pika_float x)
{
    return floor(x);
}
pika_float PikaMath_Math_fmod(PikaObj *self, pika_float x, pika_float y)
{
    return fmod(x,y);
}
pika_float PikaMath_Math_log(PikaObj *self, pika_float x)
{
    return log(x);
}
pika_float PikaMath_Math_log10(PikaObj *self, pika_float x)
{
    return log10(x);
}
pika_float PikaMath_Math_log2(PikaObj *self, pika_float x)
{
    return log2(x);
}
pika_float PikaMath_Math_pow(PikaObj *self, pika_float x, pika_float y)
{
    return pow(x,y);
}
pika_float PikaMath_Math_radians(PikaObj *self, pika_float x)
{
    return x*PI/180.0;
}
pika_float PikaMath_Math_remainder(PikaObj *self, pika_float x, pika_float y)
{
    return remainder(x,y);
}
pika_float PikaMath_Math_sin(PikaObj *self, pika_float x)
{
    return sin(x);
}
pika_float PikaMath_Math_sinh(PikaObj *self, pika_float x)
{
    return sinh(x);
}
pika_float PikaMath_Math_sqrt(PikaObj *self, pika_float x)
{
    return sqrt(x);
}
pika_float PikaMath_Math_tan(PikaObj *self, pika_float x)
{
    return tan(x);
}
pika_float PikaMath_Math_tanh(PikaObj *self, pika_float x)
{
    return tanh(x);
}
pika_float PikaMath_Math_trunc(PikaObj *self, pika_float x)
{
    return trunc(x);
}