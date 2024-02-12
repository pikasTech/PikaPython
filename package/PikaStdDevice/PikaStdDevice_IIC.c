#include "PikaStdDevice_IIC.h"
#include "PikaStdDevice_common.h"

void PikaStdDevice_IIC_init(PikaObj* self) {
    obj_setInt(self, "deviceAddr", 0);
    obj_setStr(self, "SCLpin", "PA0");
    obj_setStr(self, "SDApin", "PA1");
}

void PikaStdDevice_IIC___init__(PikaObj* self) {
    PikaStdDevice_IIC_init(self);
}

void PikaStdDevice_IIC_enable(PikaObj* self) {}

void PikaStdDevice_IIC_disable(PikaObj* self) {}

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
}

void PikaStdDevice_IIC_writeBytes(PikaObj* self,
                                  int addr,
                                  uint8_t* data,
                                  int length) {
    obj_setBytes(self, "writeData", data, length);
    obj_setInt(self, "writeAddr", addr);
}

char* PikaStdDevice_IIC_read(PikaObj* self, int addr, int length) {
    obj_setInt(self, "length", length);
    obj_setInt(self, "readAddr", addr);
    return obj_getStr(self, "readData");
}

Arg* PikaStdDevice_IIC_readBytes(PikaObj* self, int addr, int length) {
    obj_setInt(self, "length", length);
    obj_setInt(self, "readAddr", addr);
    return arg_copy(obj_getArg(self, "readData"));
}
