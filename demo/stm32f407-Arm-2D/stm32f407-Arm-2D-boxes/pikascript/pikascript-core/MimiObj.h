#ifndef _Process__H
#define _Process__H
#include "dataArgs.h"
#include "dataLink.h"
#include "dataMemory.h"
#include "method.h"
typedef struct Class_process MimiObj;
typedef MimiObj *(*NewFun)(Args *args);
typedef MimiObj *(*InitFun)(MimiObj *self, Args *args);

struct Class_process
{
    /* attribute */
    DMEM *mem;

    /* list */
    Args *attributeList;
    char *name;

    /* virtual operation */
    void (*_beforDinit)(MimiObj *self);
    void (*_updateHandle)(MimiObj *self);
};

/* operation */
int32_t obj_deinit(MimiObj *self);
int32_t obj_init(MimiObj *self, Args *args);
int32_t obj_update(MimiObj *self);
int32_t obj_enable(MimiObj *self);
int32_t obj_disable(MimiObj *self);

// arg type operations
int32_t obj_setInt(MimiObj *self, char *argPath, int64_t val);
int32_t obj_setPtr(MimiObj *self, char *argPath, void *pointer);
int32_t obj_setFloat(MimiObj *self, char *argPath, float value);
int32_t obj_setStr(MimiObj *self, char *argPath, char *str);
int32_t obj_setArg(MimiObj *self, char *argPath, Arg *arg);

void *obj_getPtr(MimiObj *self, char *argPath);
float obj_getFloat(MimiObj *self, char *argPath);
char *obj_getStr(MimiObj *self, char *argPath);
int64_t obj_getInt(MimiObj *self, char *argPath);
Arg *obj_getArg(MimiObj *self, char *argPath);

// arg general operations
int32_t obj_bind(MimiObj *self, char *type, char *name, void *pointer);
int32_t obj_set(MimiObj *self, char *name, char *valStr);

int32_t obj_bindInt(MimiObj *self, char *name, int32_t *valPtr);
int32_t obj_bindFloat(MimiObj *self, char *name, float *valPtr);
int32_t obj_bindString(MimiObj *self, char *name, char **valPtr);

char *obj_print(MimiObj *self, char *name);

// args operations
int32_t obj_load(MimiObj *self, Args *args, char *name);

// subObject
int32_t obj_addOther(MimiObj *self, char *subObjectName, void *new_projcetFun);
int32_t obj_setObjWithoutClass(MimiObj *self, char *subObjectName, void *new_projcetFun);
MimiObj *obj_getObjDirect(MimiObj *self, char *name);
MimiObj *obj_getObj(MimiObj *self, char *processPathectory, int32_t deepth);

// subProcess
int32_t obj_freeObj(MimiObj *self, char *subObjectName);

/* method */
int32_t class_defineMethod(MimiObj *self,
                     char *declearation,
                     void (*methodPtr)(MimiObj *self, Args *args));

void obj_runNoRes(MimiObj *slef, char *cmd);
void obj_run(MimiObj *self, char *cmd);
Args *obj_runDirect(MimiObj *self, char *cmd);
int32_t obj_removeArg(MimiObj *self, char *argPath);
int32_t obj_isArgExist(MimiObj *self, char *argPath);
MimiObj *obj_getClassObjByNewFun(MimiObj *self, char *name, void *(*newClassFun)(Args *initArgs));
MimiObj *newRootObj(char *name, void *newObjFun);

#endif