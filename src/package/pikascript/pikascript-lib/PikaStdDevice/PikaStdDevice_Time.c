#include "BaseObj.h"
#include "PikaStdDeivce_Time.h"

void PikaStdDeivce_Time_sleep_ms(PikaObj *self, int ms)
{
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}
void PikaStdDeivce_Time_sleep_s(PikaObj *self, int s)
{
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}