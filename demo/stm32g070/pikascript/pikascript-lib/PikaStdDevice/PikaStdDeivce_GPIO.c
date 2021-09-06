#include "BaseObj.h"
#include "PikaStdDeivce_GPIO.h"

void PikaStdDeivce_GPIO_init(PikaObj *self)
{
    obj_setInt(self, "isEnable", 0);
    obj_setStr(self, "pin", "PA0");
    obj_setStr(self, "mode", "out");
    obj_setInt(self, "isOn", 0);
}

void PikaStdDeivce_GPIO_disable(PikaObj *self)
{
    obj_setInt(self, "isEnable", 0);
    obj_run(self, "platformDisable()");
}

void PikaStdDeivce_GPIO_enable(PikaObj *self)
{
    obj_setInt(self, "isEnable", 1);
    obj_run(self, "platformEnable()");
}

char * PikaStdDeivce_GPIO_getMode(PikaObj *self)
{
    return obj_getStr(self, "mode");
}

char * PikaStdDeivce_GPIO_getPin(PikaObj *self)
{
    return obj_getStr(self, "pin");
}

void PikaStdDeivce_GPIO_off(PikaObj *self)
{
    obj_setInt(self, "isOn", 0);
    obj_run(self, "platformOff()");
}

void PikaStdDeivce_GPIO_on(PikaObj *self)
{
    obj_setInt(self, "isOn", 1);
    obj_run(self, "platformOn()");
}

void PikaStdDeivce_GPIO_setMode(PikaObj *self, char * mode)
{
    obj_setStr(self, "mode", mode);
    obj_run(self, "platformSetMode(mode)");
}

void PikaStdDeivce_GPIO_setPin(PikaObj *self, char * pinName)
{
    obj_setStr(self, "pin", pinName);
}

void PikaStdDeivce_GPIO_platFormDisable(PikaObj *self)
{
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDeivce_GPIO_platFormEnable(PikaObj *self)
{
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}
void PikaStdDeivce_GPIO_platFormOff(PikaObj *self)
{
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDeivce_GPIO_platFormOn(PikaObj *self)
{
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDeivce_GPIO_platFormSetMode(PikaObj *self, char * mode)
{
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDeivce_GPIO_platformDisable(PikaObj *self)
{
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDeivce_GPIO_platformEnable(PikaObj *self)
{
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDeivce_GPIO_platformOff(PikaObj *self)
{
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDeivce_GPIO_platformOn(PikaObj *self)
{
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDeivce_GPIO_platformSetMode(PikaObj *self, char * mode)
{
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}
