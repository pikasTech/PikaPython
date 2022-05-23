#include "PikaStdDevice_CAN.h"

void PikaStdDevice_CAN___init__(PikaObj* self) {}

void PikaStdDevice_CAN_addFilter(PikaObj* self,
                                 int hdr,
                                 int id,
                                 int ide,
                                 int mask,
                                 int mode,
                                 int rtr) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDevice_CAN_disable(PikaObj* self) {
    obj_runNativeMethod(self, "platformDisable", NULL);
}

void PikaStdDevice_CAN_enable(PikaObj* self) {
    obj_runNativeMethod(self, "platformEnable", NULL);
}

void PikaStdDevice_CAN_setId(PikaObj* self, int id) {
    obj_setInt(self, "id", id);
}

void PikaStdDevice_CAN_setMode(PikaObj* self, char* mode) {
    obj_setStr(self, "mode", mode);
}

void PikaStdDevice_CAN_setName(PikaObj* self, char* name) {
    obj_setStr(self, "name", name);
}

void PikaStdDevice_CAN_setBaudRate(PikaObj* self, int baudRate) {
    obj_setInt(self, "baudRate", baudRate);
}

char* PikaStdDevice_CAN_read(PikaObj* self, int length) {
    obj_setInt(self, "length", length);
    obj_runNativeMethod(self, "platformRead", NULL);
    return obj_getStr(self, "readData");
}

Arg* PikaStdDevice_CAN_readBytes(PikaObj *self, int length){
    obj_setInt(self, "length", length);
    obj_runNativeMethod(self, "platformReadBytes", NULL);
    return obj_getArg(self, "readData");
}

void PikaStdDevice_CAN_write(PikaObj* self, char* data) {
    obj_setStr(self, "writeData", data);
    obj_runNativeMethod(self, "platformWrite", NULL);
}

void PikaStdDevice_CAN_writeBytes(PikaObj *self, uint8_t* data, int length){
    obj_setBytes(self, "writeData", data, length);
    obj_runNativeMethod(self, "platformWriteBytes", NULL);
}

void PikaStdDevice_CAN_platformDisable(PikaObj* self) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDevice_CAN_platformEnable(PikaObj* self) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDevice_CAN_platformRead(PikaObj* self) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDevice_CAN_platformWrite(PikaObj* self) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDevice_CAN_platformReadBytes(PikaObj *self){
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDevice_CAN_platformWriteBytes(PikaObj *self){
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}
