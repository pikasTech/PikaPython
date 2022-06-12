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

float PikaMath_Math_acos(PikaObj *self, float x)
{
    return acos(x);
}
float PikaMath_Math_asin(PikaObj *self, float x)
{
    return asin(x);
}
float PikaMath_Math_atan(PikaObj *self, float x)
{
    return atan(x);
}
float PikaMath_Math_atan2(PikaObj *self, float x, float y)
{
    return atan2(x,y);
}
int PikaMath_Math_ceil(PikaObj *self, float x)
{
    return ceil(x);
}
float PikaMath_Math_cos(PikaObj *self, float x)
{
    return cos(x);
}
float PikaMath_Math_cosh(PikaObj *self, float x)
{
    return cosh(x);
}
float PikaMath_Math_degrees(PikaObj *self, float x)
{
    return x*180.0/PI;
}
float PikaMath_Math_exp(PikaObj *self, float x)
{
    return exp(x);
}
float PikaMath_Math_fabs(PikaObj *self, float x)
{
    return fabs(x);
}
int PikaMath_Math_floor(PikaObj *self, float x)
{
    return floor(x);
}
float PikaMath_Math_fmod(PikaObj *self, float x, float y)
{
    return fmod(x,y);
}
float PikaMath_Math_log(PikaObj *self, float x)
{
    return log(x);
}
float PikaMath_Math_log10(PikaObj *self, float x)
{
    return log10(x);
}
float PikaMath_Math_log2(PikaObj *self, float x)
{
    return log2(x);
}
float PikaMath_Math_pow(PikaObj *self, float x, float y)
{
    return pow(x,y);
}
float PikaMath_Math_radians(PikaObj *self, float x)
{
    return x*PI/180.0;
}
float PikaMath_Math_remainder(PikaObj *self, float x, float y)
{
    return remainder(x,y);
}
float PikaMath_Math_sin(PikaObj *self, float x)
{
    return sin(x);
}
float PikaMath_Math_sinh(PikaObj *self, float x)
{
    return sinh(x);
}
float PikaMath_Math_sqrt(PikaObj *self, float x)
{
    return sqrt(x);
}
float PikaMath_Math_tan(PikaObj *self, float x)
{
    return tan(x);
}
float PikaMath_Math_tanh(PikaObj *self, float x)
{
    return tanh(x);
}
float PikaMath_Math_trunc(PikaObj *self, float x)
{
    return trunc(x);
}