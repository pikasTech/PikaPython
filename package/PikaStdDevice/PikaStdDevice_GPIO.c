#include "PikaStdDevice_GPIO.h"
#include "BaseObj.h"

void PikaStdDevice_GPIO_init(PikaObj* self) {
    obj_setInt(self, "isEnable", 0);
    obj_setStr(self, "pin", "none");
    obj_setStr(self, "mode", "out");
    obj_setInt(self, "isOn", 0);
    obj_setStr(self, "pull", "none");
    obj_setInt(self, "id", -999);
}

int PikaStdDevice_GPIO_getId(PikaObj* self) {
    return obj_getInt(self, "id");
}

void PikaStdDevice_GPIO_setId(PikaObj* self, int id) {
    obj_setInt(self, "id", id);
}

void PikaStdDevice_GPIO___init__(PikaObj* self) {
    PikaStdDevice_GPIO_init(self);
}

void PikaStdDevice_GPIO_disable(PikaObj* self) {
    obj_setInt(self, "isEnable", 0);
    obj_run(self, "platformDisable()");
}

void PikaStdDevice_GPIO_enable(PikaObj* self) {
    obj_setInt(self, "isEnable", 1);
    obj_run(self, "platformEnable()");
}

char* PikaStdDevice_GPIO_getMode(PikaObj* self) {
    return obj_getStr(self, "mode");
}

char* PikaStdDevice_GPIO_getPin(PikaObj* self) {
    return obj_getStr(self, "pin");
}

void PikaStdDevice_GPIO_low(PikaObj* self) {
    obj_setInt(self, "isOn", 0);
    obj_run(self, "platformLow()");
}

void PikaStdDevice_GPIO_high(PikaObj* self) {
    obj_setInt(self, "isOn", 1);
    obj_run(self, "platformHigh()");
}

int PikaStdDevice_GPIO_read(PikaObj* self) {
    obj_run(self, "platformRead()");
    return obj_getInt(self, "readBuff");
}

void PikaStdDevice_GPIO_setMode(PikaObj* self, char* mode) {
    if (strEqu(mode, "out") || strEqu(mode, "in")) {
        obj_setStr(self, "mode", mode);
        obj_run(self, "platformSetMode()");
    } else {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] GPIO mode should be 'out' or 'in'.");
    }
}

void PikaStdDevice_GPIO_setPull(PikaObj* self, char* pull) {
    if (strEqu(pull, "up") || strEqu(pull, "down") || strEqu(pull, "none")) {
        obj_setStr(self, "pull", pull);
    } else {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self,
                      "[error] GPIO pull should be 'up', 'down' or 'none'.");
    }
}

void PikaStdDevice_GPIO_setPin(PikaObj* self, char* pinName) {
    obj_setStr(self, "pin", pinName);
}

void PikaStdDevice_GPIO_platformDisable(PikaObj* self) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDevice_GPIO_platformEnable(PikaObj* self) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}
void PikaStdDevice_GPIO_platformLow(PikaObj* self) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDevice_GPIO_platformHigh(PikaObj* self) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDevice_GPIO_platformSetMode(PikaObj* self) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDevice_GPIO_platformOff(PikaObj* self) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDevice_GPIO_platformOn(PikaObj* self) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDevice_GPIO_platformRead(PikaObj* self) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}
