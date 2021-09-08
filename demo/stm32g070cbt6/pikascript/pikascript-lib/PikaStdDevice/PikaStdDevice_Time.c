#include "BaseObj.h"
#include "PikaStdDevice_Time.h"

void PikaStdDevice_Time_sleep_ms(PikaObj *self, int ms)
{
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}
void PikaStdDevice_Time_sleep_s(PikaObj *self, int s)
{
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}