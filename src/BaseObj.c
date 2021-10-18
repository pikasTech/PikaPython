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

int32_t obj_newObj(PikaObj* self,
                   char* objName,
                   char* className,
                   NewFun newFunPtr) {
    /* class means subprocess init */
    Args* buffs = New_strBuff();

    /* class means subprocess init */
    char* mataObjName = strsAppend(buffs, "[mate]", objName);
    obj_setPtr(self, mataObjName, newFunPtr);
    /* add void process Ptr, no inited */
    args_setObjectWithClass(self->attributeList, objName, className, NULL);

    args_deinit(buffs);
    return 0;
}

Arg* arg_setMetaObj(char* objName, char* className, void* objPtr) {
    Args* buffs = New_strBuff();
    char* typeWithClass = strsAppend(buffs, "_class-[mate]", className);
    Arg* argNew = New_arg(NULL);
    argNew = arg_setPtr(argNew, objName, typeWithClass, objPtr);
    args_deinit(buffs);
    return argNew;
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


PikaObj* BaseObj(Args* args) {
    PikaObj* self = TinyObj(args);
    class_defineMethod(self, "print(val:any)", print);
    return self;
}
