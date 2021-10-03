#ifndef _MimiObj_baseObj__H
#define _MimiObj_baseObj__H
#include "dataMemory.h"
#include "MimiObj.h"
#include "TinyObj.h"

MimiObj *New_BaseObj(Args *args);
int32_t obj_newObj(MimiObj *self, char *objPath, char *classPath);
int32_t obj_import(MimiObj *self, char *className, void *classPtr);
#endif
