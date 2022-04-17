#ifndef __CTYPES_UTILS__H
#define __CTYPES_UTILS__H

void ctypesUtils_setInt(PikaObj* self, int val);
void ctypesUtils_setStr(PikaObj* self, char* val);
void ctypesUtils_setFloat(PikaObj* self, float val);
void ctypesUtils_setPtr(PikaObj* self, void* ptr);

int ctypesUtils_getInt(PikaObj* self);
char* ctypesUtils_getStr(PikaObj* self);
float ctypesUtils_getFloat(PikaObj* self);
void *ctypesUtils_getPtr(PikaObj* self);


#endif