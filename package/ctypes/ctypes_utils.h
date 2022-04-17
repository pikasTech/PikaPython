#ifndef __CTYPES_UTILS__H
#define __CTYPES_UTILS__H

void ctypesUtils_setInt(PikaObj* self, int val);
void ctypesUtils_setStr(PikaObj* self, char* val);
void ctypesUtils_setFloat(PikaObj* self, float val);
void ctypesUtils_setPtr(PikaObj* self, void* ptr);
void ctypesUtils_setMem(PikaObj* self, void* src, size_t size);

int ctypesUtils_getInt(PikaObj* self);
char* ctypesUtils_getStr(PikaObj* self);
float ctypesUtils_getFloat(PikaObj* self);
void* ctypesUtils_getPtr(PikaObj* self);

size_t ctypesUtils_getMemSize(PikaObj* self);
void* ctypesUtils_getMem(PikaObj* self);

#endif