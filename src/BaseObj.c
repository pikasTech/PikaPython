/*
  Author: lyon
  Tencent QQ: 645275593
*/

#include "BaseObj.h"
#include "PikaObj.h"
#include "TinyObj.h"
#include "dataMemory.h"
#include "dataString.h"
#include "dataStrs.h"

static void* getClassPtr(PikaObj* classObj, char* classPath) {
    char* ptrPath = classPath;
    return obj_getPtr(classObj, ptrPath);
}

Arg* arg_setMetaObj(char* objName, char* className, NewFun objPtr) {
    Args* buffs = New_strBuff();
    char* typeWithClass = strsAppend(buffs, "_class-[mate]", className);
    Arg* argNew = New_arg(NULL);
    argNew = arg_setPtr(argNew, objName, typeWithClass, objPtr);
    args_deinit(buffs);
    return argNew;
}

int32_t obj_newObj(PikaObj* self,
                   char* objName,
                   char* className,
                   NewFun newFunPtr) {
    /* add mate Obj, no inited */
    Arg* mateObj = arg_setMetaObj(objName, className, newFunPtr);
    args_setArg(self->attributeList, mateObj);
    return 0;
}

static void print(PikaObj* self, Args* args) {
    obj_setErrorCode(self, 0);
    char* res = args_print(args, "val");
    if (NULL == res) {
        obj_setSysOut(self, "[error] print: can not print val");
        obj_setErrorCode(self, 1);
        return;
    }
    /* not empty */
    obj_setSysOut(self, res);
}

PikaObj* New_BaseObj(Args* args) {
    PikaObj* self = New_TinyObj(args);
    class_defineMethod(self, "print(val:any)", print);
    return self;
}
