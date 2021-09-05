#ifndef _arg__H
#define _arg__H
#include "dataMemory.h"

typedef struct Class_arg Arg;
struct Class_arg
{
    /* attribute */
    DMEM *mem;
    void *context;

    DMEM *contantDynMem;
    DMEM *nameDynMem;
    DMEM *typeDynMem;
};

void arg_setName(Arg *self, char *name);
void arg_setContant(Arg *self, char *contant, int size);
void arg_newContant(Arg *self, int size);
void arg_setType(Arg *self, char *type);
char *arg_getName(Arg *self);
char *arg_getType(Arg *self);
char *arg_getContant(Arg *self);

void arg_setInt(Arg *self, long long val);
void arg_setFloat(Arg *self, float val);
void arg_setPtr(Arg *self, void *pointer);
void arg_setStr(Arg *self, char *string);

long long arg_getInt(Arg *self);
float arg_getFloat(Arg *self);
void *arg_getPtr(Arg *self);
char *arg_getStr(Arg *self);
Arg *arg_copy(Arg *argToBeCopy);


void arg_init(Arg *self, void *voidPointer);
void arg_deinit(Arg *self);

Arg *New_arg(void *voidPointer);
#endif
