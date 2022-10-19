#include "PikaStdDevice_IIC.h"
#include "BaseObj.h"
#include "PikaObj.h"
void PikaStdDevice_IIC_init(PikaObj* self) {
    obj_setInt(self, "deviceAddr", 0);
    obj_setStr(self, "SCLpin", "PA0");
    obj_setStr(self, "SDApin", "PA1");
}

void PikaStdDevice_IIC___init__(PikaObj* self) {
    PikaStdDevice_IIC_init(self);
}

void PikaStdDevice_IIC_enable(PikaObj* self) {
    obj_runNativeMethod(self, "platformEnable", NULL);
}

void PikaStdDevice_IIC_disable(PikaObj* self) {
    obj_runNativeMethod(self, "platformDisable", NULL);
}

void PikaStdDevice_IIC_setDeviceAddr(PikaObj* self, int addr) {
    obj_setInt(self, "deviceAddr", addr);
}

void PikaStdDevice_IIC_setPinSCL(PikaObj* self, char* pin) {
    obj_setStr(self, "SCLpin", pin);
}

void PikaStdDevice_IIC_setPinSDA(PikaObj* self, char* pin) {
    obj_setStr(self, "SDApin", pin);
}

void PikaStdDevice_IIC_write(PikaObj* self, int addr, char* data) {
    obj_setStr(self, "writeData", data);
    obj_setInt(self, "writeAddr", addr);
    obj_runNativeMethod(self, "platformWrite", NULL);
}

void PikaStdDevice_IIC_writeBytes(PikaObj* self,
                                  int addr,
                                  uint8_t* data,
                                  int length) {
    obj_setBytes(self, "writeData", data, length);
    obj_setInt(self, "writeAddr", addr);
    obj_runNativeMethod(self, "platformWriteBytes", NULL);
}

char* PikaStdDevice_IIC_read(PikaObj* self, int addr, int length) {
    obj_setInt(self, "length", length);
    obj_setInt(self, "readAddr", addr);
    obj_runNativeMethod(self, "platformRead", NULL);
    return obj_getStr(self, "readData");
}

Arg* PikaStdDevice_IIC_readBytes(PikaObj* self, int addr, int length) {
    obj_setInt(self, "length", length);
    obj_setInt(self, "readAddr", addr);
    obj_runNativeMethod(self, "platformReadBytes", NULL);
    return arg_copy(obj_getArg(self, "readData"));
}

void PikaStdDevice_IIC_platformWrite(PikaObj* self) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}

void PikaStdDevice_IIC_platformEnable(PikaObj* self) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}

void PikaStdDevice_IIC_platformDisable(PikaObj* self) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}

void PikaStdDevice_IIC_platformRead(PikaObj* self) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}

void PikaStdDevice_IIC_platformReadBytes(PikaObj* self) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}

void PikaStdDevice_IIC_platformWriteBytes(PikaObj* self) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}
