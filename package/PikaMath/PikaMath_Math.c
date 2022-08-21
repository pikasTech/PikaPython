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

double PikaMath_Math_acos(PikaObj *self, double x)
{
    return acos(x);
}
double PikaMath_Math_asin(PikaObj *self, double x)
{
    return asin(x);
}
double PikaMath_Math_atan(PikaObj *self, double x)
{
    return atan(x);
}
double PikaMath_Math_atan2(PikaObj *self, double x, double y)
{
    return atan2(x,y);
}
int PikaMath_Math_ceil(PikaObj *self, double x)
{
    return ceil(x);
}
double PikaMath_Math_cos(PikaObj *self, double x)
{
    return cos(x);
}
double PikaMath_Math_cosh(PikaObj *self, double x)
{
    return cosh(x);
}
double PikaMath_Math_degrees(PikaObj *self, double x)
{
    return x*180.0/PI;
}
double PikaMath_Math_exp(PikaObj *self, double x)
{
    return exp(x);
}
double PikaMath_Math_fabs(PikaObj *self, double x)
{
    return fabs(x);
}
int PikaMath_Math_floor(PikaObj *self, double x)
{
    return floor(x);
}
double PikaMath_Math_fmod(PikaObj *self, double x, double y)
{
    return fmod(x,y);
}
double PikaMath_Math_log(PikaObj *self, double x)
{
    return log(x);
}
double PikaMath_Math_log10(PikaObj *self, double x)
{
    return log10(x);
}
double PikaMath_Math_log2(PikaObj *self, double x)
{
    return log2(x);
}
double PikaMath_Math_pow(PikaObj *self, double x, double y)
{
    return pow(x,y);
}
double PikaMath_Math_radians(PikaObj *self, double x)
{
    return x*PI/180.0;
}
double PikaMath_Math_remainder(PikaObj *self, double x, double y)
{
    return remainder(x,y);
}
double PikaMath_Math_sin(PikaObj *self, double x)
{
    return sin(x);
}
double PikaMath_Math_sinh(PikaObj *self, double x)
{
    return sinh(x);
}
double PikaMath_Math_sqrt(PikaObj *self, double x)
{
    return sqrt(x);
}
double PikaMath_Math_tan(PikaObj *self, double x)
{
    return tan(x);
}
double PikaMath_Math_tanh(PikaObj *self, double x)
{
    return tanh(x);
}
double PikaMath_Math_trunc(PikaObj *self, double x)
{
    return trunc(x);
}