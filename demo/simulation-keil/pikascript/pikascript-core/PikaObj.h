#ifndef _Process__H
#define _Process__H
#include "dataArgs.h"
#include "dataLink.h"
#include "dataMemory.h"
#include "method.h"
typedef struct PikaObj_t PikaObj;
typedef PikaObj *(*NewFun)(Args *args);
typedef PikaObj *(*InitFun)(PikaObj *self, Args *args);

struct PikaObj_t
{
    /* list */
    Args *attributeList;
    char *name;
    uint16_t memSize;
};

/* operation */
int32_t obj_deinit(PikaObj *self);
int32_t obj_init(PikaObj *self, Args *args);
int32_t obj_update(PikaObj *self);
int32_t obj_enable(PikaObj *self);
int32_t obj_disable(PikaObj *self);

// arg type operations
int32_t obj_setInt(PikaObj *self, char *argPath, int64_t val);
int32_t obj_setPtr(PikaObj *self, char *argPath, void *pointer);
int32_t obj_setFloat(PikaObj *self, char *argPath, float value);
int32_t obj_setStr(PikaObj *self, char *argPath, char *str);
int32_t obj_setArg(PikaObj *self, char *argPath, Arg *arg);

void *obj_getPtr(PikaObj *self, char *argPath);
float obj_getFloat(PikaObj *self, char *argPath);
char *obj_getStr(PikaObj *self, char *argPath);
int64_t obj_getInt(PikaObj *self, char *argPath);
Arg *obj_getArg(PikaObj *self, char *argPath);

// arg general operations
int32_t obj_bind(PikaObj *self, char *type, char *name, void *pointer);
int32_t obj_set(PikaObj *self, char *name, char *valStr);

int32_t obj_bindInt(PikaObj *self, char *name, int32_t *valPtr);
int32_t obj_bindFloat(PikaObj *self, char *name, float *valPtr);
int32_t obj_bindString(PikaObj *self, char *name, char **valPtr);

char *obj_print(PikaObj *self, char *name);

// args operations
int32_t obj_load(PikaObj *self, Args *args, char *name);

// subObject
int32_t obj_addOther(PikaObj *self, char *subObjectName, void *new_projcetFun);
int32_t obj_setObjWithoutClass(PikaObj *self, char *subObjectName, void *new_projcetFun);
PikaObj *obj_getObjDirect(PikaObj *self, char *name);
PikaObj *obj_getObj(PikaObj *self, char *processPathectory, int32_t deepth);

// subProcess
int32_t obj_freeObj(PikaObj *self, char *subObjectName);

/* method */
int32_t class_defineMethod(PikaObj *self,
                           char *declearation,
                           void (*methodPtr)(PikaObj *self, Args *args));

void obj_runNoRes(PikaObj *slef, char *cmd);
void obj_run(PikaObj *self, char *cmd);
Args *obj_runDirect(PikaObj *self, char *cmd);
int32_t obj_removeArg(PikaObj *self, char *argPath);
int32_t obj_isArgExist(PikaObj *self, char *argPath);
PikaObj *obj_getClassObjByNewFun(PikaObj *self, char *name, NewFun newClassFun);
PikaObj *newRootObj(char *name, NewFun newObjFun);

void obj_setErrorCode(PikaObj *self, int32_t errCode);
int32_t obj_getErrorCode(PikaObj *self);
void obj_setSysOut(PikaObj *self, char *str);
char *args_getSysOut(Args *args);
void args_setErrorCode(Args *args, int32_t errCode);
int32_t args_getErrorCode(Args *args);
void args_setSysOut(Args *args, char *str);
char *obj_getSysOut(PikaObj *self);
void obj_sysPrintf(PikaObj *self, char *fmt, ...);

#endif