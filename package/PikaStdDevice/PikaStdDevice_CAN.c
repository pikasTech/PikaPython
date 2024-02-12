#include "PikaStdDevice_CAN.h"
#include "PikaStdDevice_common.h"

void PikaStdDevice_CAN___init__(PikaObj* self) {}

void PikaStdDevice_CAN_addFilter(PikaObj* self,
                                 int hdr,
                                 int id,
                                 int ide,
                                 int mask,
                                 int mode,
                                 int rtr) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}

void PikaStdDevice_CAN_disable(PikaObj* self) {}

void PikaStdDevice_CAN_enable(PikaObj* self) {}

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
    return obj_getStr(self, "readData");
}

Arg* PikaStdDevice_CAN_readBytes(PikaObj* self, int length) {
    obj_setInt(self, "length", length);
    return arg_copy(obj_getArg(self, "readData"));
}

void PikaStdDevice_CAN_write(PikaObj* self, char* data) {
    obj_setStr(self, "writeData", data);
}

void PikaStdDevice_CAN_writeBytes(PikaObj* self, uint8_t* data, int length) {
    obj_setBytes(self, "writeData", data, length);
}
