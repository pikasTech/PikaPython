#include "PikaStdDevice_SPI.h"
#include "BaseObj.h"
#include "PikaObj.h"
void PikaStdDevice_SPI___init__(PikaObj* self) {
    obj_setInt(self, "baudRate", 1000);
    obj_setInt(self, "id", 0);
    obj_setStr(self, "name", "none");
    obj_setInt(self, "phase", 0);
    obj_setStr(self, "MISOpin", "none");
    obj_setStr(self, "MOSIpin", "none");
    obj_setStr(self, "SCKpin", "none");
    obj_setInt(self, "polarity", 0);
}

void PikaStdDevice_SPI_disable(PikaObj* self) {
    obj_runNativeMethod(self, "platformDisable", NULL);
}

void PikaStdDevice_SPI_enable(PikaObj* self) {
    obj_runNativeMethod(self, "platformEnable", NULL);
}

void PikaStdDevice_SPI_write(PikaObj* self, char* data) {
    obj_setStr(self, "writeData", data);
    obj_runNativeMethod(self, "platformWrite", NULL);
}

void PikaStdDevice_SPI_writeBytes(PikaObj* self, uint8_t* data, int length) {
    obj_setBytes(self, "writeData", data, length);
    obj_runNativeMethod(self, "platformWriteBytes", NULL);
}

char* PikaStdDevice_SPI_read(PikaObj* self, int length) {
    obj_setInt(self, "length", length);
    obj_runNativeMethod(self, "platformRead", NULL);
    return obj_getStr(self, "readData");
}

Arg* PikaStdDevice_SPI_readBytes(PikaObj* self, int length) {
    obj_setInt(self, "length", length);
    obj_runNativeMethod(self, "platformReadBytes", NULL);
    return arg_copy(obj_getArg(self, "readData"));
}

void PikaStdDevice_SPI_setBaudRate(PikaObj* self, int baudRate) {
    obj_setInt(self, "baudRate", baudRate);
}

void PikaStdDevice_SPI_setId(PikaObj* self, int id) {
    obj_setInt(self, "id", id);
}

void PikaStdDevice_SPI_setName(PikaObj* self, char* name) {
    obj_setStr(self, "name", name);
}

void PikaStdDevice_SPI_setPhase(PikaObj* self, int phase) {
    obj_setInt(self, "phase", phase);
}

void PikaStdDevice_SPI_setPinMISO(PikaObj* self, char* pin) {
    obj_setStr(self, "MISOpin", pin);
}

void PikaStdDevice_SPI_setPinMOSI(PikaObj* self, char* pin) {
    obj_setStr(self, "MOSIpin", pin);
}

void PikaStdDevice_SPI_setPinSCK(PikaObj* self, char* pin) {
    obj_setStr(self, "SCKpin", pin);
}

void PikaStdDevice_SPI_setPolarity(PikaObj* self, int polarity) {
    obj_setInt(self, "polarity", polarity);
}

void PikaStdDevice_SPI_platformDisable(PikaObj* self) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}

void PikaStdDevice_SPI_platformEnable(PikaObj* self) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}

void PikaStdDevice_SPI_platformWrite(PikaObj* self) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}

void PikaStdDevice_SPI_platformRead(PikaObj* self) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}

void PikaStdDevice_SPI_platformWriteBytes(PikaObj* self) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}

void PikaStdDevice_SPI_platformReadBytes(PikaObj* self) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}
