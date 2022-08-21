#ifndef __CTYPES_UTILS__H
#define __CTYPES_UTILS__H

void ctypesUtils_setInt(PikaObj* self, int val);
void ctypesUtils_setStr(PikaObj* self, char* val);
void ctypesUtils_setFloat(PikaObj* self, double val);
void ctypesUtils_setPtr(PikaObj* self, void* ptr);
void ctypesUtils_setBytes(PikaObj* self, uint8_t* src, size_t size);

int ctypesUtils_getInt(PikaObj* self);
char* ctypesUtils_getStr(PikaObj* self);
double ctypesUtils_getFloat(PikaObj* self);
void* ctypesUtils_getPtr(PikaObj* self);

size_t ctypesUtils_getBytesSize(PikaObj* self);
uint8_t* ctypesUtils_getBytes(PikaObj* self);

#endif