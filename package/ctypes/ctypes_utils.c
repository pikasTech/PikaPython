#include "PikaObj.h"

void ctypesUtils_setInt(PikaObj* self, int val) {
    obj_setInt(self, "value", val);
}

void ctypesUtils_setStr(PikaObj* self, char* val) {
    obj_setStr(self, "value", val);
}

void ctypesUtils_setFloat(PikaObj* self, float val) {
    obj_setFloat(self, "value", val);
}

void ctypesUtils_setPtr(PikaObj* self, void* ptr) {
    obj_setPtr(self, "value", ptr);
}

int ctypesUtils_getInt(PikaObj* self) {
    return obj_getInt(self, "value");
}

char* ctypesUtils_getStr(PikaObj* self) {
    return obj_getStr(self, "value");
}

float ctypesUtils_getFloat(PikaObj* self) {
    return obj_getFloat(self, "value");
}

void* ctypesUtils_getPtr(PikaObj* self) {
    return obj_getPtr(self, "value");
}

void ctypesUtils_setMem(PikaObj* self, void* src, size_t size) {
    obj_setInt(self, "size", size);
}

size_t ctypesUtils_getMemSize(PikaObj* self) {
    return obj_getInt(self, "size");
}

void* ctypesUtils_getMem(PikaObj* self) {
    return NULL;
}
