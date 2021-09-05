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
MimiObj *New_TinyObj(Args *args);
#endif

/* operation */
int obj_deinit(MimiObj *self);
int obj_init(MimiObj *self, Args *args);
int obj_update(MimiObj *self);
int obj_enable(MimiObj *self);
int obj_disable(MimiObj *self);

// arg type operations
int obj_setInt(MimiObj *self, char *argPath, long long val);
int obj_setPtr(MimiObj *self, char *argPath, void *pointer);
int obj_setFloat(MimiObj *self, char *argPath, float value);
int obj_setStr(MimiObj *self, char *argPath, char *str);
int obj_setArg(MimiObj *self, char *argPath, Arg *arg);

void *obj_getPtr(MimiObj *self, char *argPath);
float obj_getFloat(MimiObj *self, char *argPath);
char *obj_getStr(MimiObj *self, char *argPath);
long long obj_getInt(MimiObj *self, char *argPath);
Arg *obj_getArg(MimiObj *self, char *argPath);

// arg general operations
int obj_bind(MimiObj *self, char *type, char *name, void *pointer);
int obj_set(MimiObj *self, char *name, char *valStr);

int obj_bindInt(MimiObj *self, char *name, int *valPtr);
int obj_bindFloat(MimiObj *self, char *name, float *valPtr);
int obj_bindString(MimiObj *self, char *name, char **valPtr);

char *obj_print(MimiObj *self, char *name);

// args operations
int obj_load(MimiObj *self, Args *args, char *name);

// subObject
int obj_addOther(MimiObj *self, char *subObjectName, void *new_projcetFun);
int obj_setObjWithoutClass(MimiObj *self, char *subObjectName, void *new_projcetFun);
MimiObj *obj_getObjDirect(MimiObj *self, char *name);
MimiObj *obj_getObj(MimiObj *self, char *processPathectory, int deepth);

// subProcess
int obj_freeObj(MimiObj *self, char *subObjectName);

/* method */
int class_defineMethod(MimiObj *self,
                     char *declearation,
                     void (*methodPtr)(MimiObj *self, Args *args));

void obj_runNoRes(MimiObj *slef, char *cmd);
void obj_run(MimiObj *self, char *cmd);
Args *obj_runDirect(MimiObj *self, char *cmd);
int obj_removeArg(MimiObj *self, char *argPath);
int obj_isArgExist(MimiObj *self, char *argPath);
MimiObj *obj_getClassObjByNewFun(MimiObj *self, char *name, void *(*newClassFun)(Args *initArgs));
MimiObj *newRootObj(char *name, void *newObjFun);
