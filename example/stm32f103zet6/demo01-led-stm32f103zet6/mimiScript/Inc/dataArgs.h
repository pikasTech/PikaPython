#ifndef _dataArgs__H
#define _dataArgs__H
#include "dataLink.h"
#include "dataString.h"
#include "dataMemory.h"
#include "dataArg.h"

typedef struct Class_Arglist Args;
struct Class_Arglist
{
    /* attribute */
    DMEM *mem;
    void *context;
    Link *argLinkList;

    /* virtual operation */

    /* object */
};

/* operation */
void args_deinit(Args *self);
void args_init(Args *self, Args *args);
int args_getSize(Args *self);

Arg *args_getArgByIndex(Args *self, int index);
Arg *args_getArg(Args *self, char *name);
int args_removeArg(Args *self, char *name);

int args_setArg(Args *self, Arg *arg);

int args_copyArgByName(Args *self, char *name, Args *directList);
int args_copyArg(Args *self, Arg *argToBeCopy);

char *args_getType(Args *self, char *name);
int args_isArgExist(Args *self, char *name);

int args_setStr(Args *self, char *name, char *strIn);
int args_setStrWithDefaultName(Args *self, char *strIn);
char *args_getStr(Args *self, char *name);
char *args_getStrByIndex(Args *self, int index);

int args_setFloatWithDefaultName(Args *self, float argFloat);
int args_setFloat(Args *self, char *name, float argFloat);
float args_getFloatByIndex(Args *self, int index);
float args_getFloat(Args *self, char *name);

int args_setPtr(Args *self, char *name, void *argPointer);
void *args_getPtr(Args *self, char *name);
void *args_getPointerByIndex(Args *self, int index);

int args_setInt(Args *self, char *name, long long int64In);
long long args_getInt(Args *self, char *name);
long long args_getIntByIndex(Args *self, int index);

void args_bindInt(Args *self, char *name, int *intPtr);
void args_bindFloat(Args *self, char *name, float *floatPtr);
void args_bindStr(Args *self, char *name, char **stringPtr);
/* arg general opeartion */
void args_bind(Args *self, char *type, char *name, void *pointer);
char *args_print(Args *self, char *name);

int args_set(Args *self, char *name, char *valueStr);
int args_setObjectWithClass(Args *self, char *objectName, char *className, void *objectPtr);
int args_setPtrWithType(Args *self, char *objName, char *type, void *objPtr);
int args_foreach (Args *self, int (*eachHandle)(Arg *argEach, Args *handleArgs), Args * handleArgs);

char *args_getBuff(Args *self, int size);

Args *New_args(Args *args);
#endif
