#include "PikaObj.h"
#include "TinyObj.h"
#include "BaseObj.h"
#include "dataMemory.h"
#include "dataString.h"
#include "dataStrs.h"

static void *getClassPtr(PikaObj *classObj, char *classPath)
{
    char *ptrPath = classPath;
    return obj_getPtr(classObj, ptrPath);
}

int32_t obj_newObjByFun(PikaObj *self, char *objName, char *className, void *newFunPtr)
{
    /* class means subprocess init */
    Args *buffs = New_strBuff();

    /* class means subprocess init */
    char *mataObjName = strsAppend(buffs, "[mate]", objName);
    obj_setPtr(self, mataObjName, newFunPtr);
    /* add void process Ptr, no inited */
    args_setObjectWithClass(self->attributeList, objName, className, NULL);

    args_deinit(buffs);
    return 0;
}

int32_t sysObj_setObjbyClass(PikaObj *self, char *objName, char *classPath)
{
    /* class means subprocess init */
    Args *buffs = New_strBuff();
    PikaObj *classLoader = obj_getObj(self, "__classLoader", 0);
    void *newFunPtr = getClassPtr(classLoader, classPath);

    /* class means subprocess init */
    char *mataObjName = strsAppend(buffs, "[mate]", objName);
    obj_setPtr(self, mataObjName, newFunPtr);
    /* add void process Ptr, no inited */
    args_setObjectWithClass(self->attributeList, objName, classPath, NULL);

    args_deinit(buffs);
    return 0;
}

static int32_t storeClassInfo(PikaObj *self, Args *buffs, char *classPath, void *classPtr)
{
    int32_t res = 0;
    PikaObj *classHost = obj_getObj(self, classPath, 1);
    if (NULL == classHost)
    {
        res = 1;
        goto exit;
    }
    char *className = strsGetLastToken(buffs, classPath, '.');
    char *classStoreName = className;
    obj_setPtr(classHost, classStoreName, classPtr);
    res = 0;
    goto exit;
exit:
    return res;
}

int32_t obj_import(PikaObj *self, char *className, NewFun classPtr)
{
    PikaObj *classLoader = obj_getObj(self, "__classLoader", 0);
    Args *buffs = New_args(NULL);
    int32_t res = storeClassInfo(classLoader, buffs, className, classPtr);
    args_deinit(buffs);
    return res;
}

int32_t obj_newObj(PikaObj *self, char *objPath, char *classPath)
{
    PikaObj *classLoader = obj_getObj(self, "__classLoader", 0);
    Args *buffs = New_args(NULL);
    int res = 0;
    void *NewObjPtr = getClassPtr(classLoader, classPath);
    if (NULL == NewObjPtr)
    {
        res = 1;
        goto exit;
    }
    PikaObj *objHost = obj_getObj(self, objPath, 1);
    if (NULL == objHost)
    {
        res = 2;
        goto exit;
    }
    char *objName = strsGetLastToken(buffs, objPath, '.');
    obj_newObjByFun(objHost, objName, classPath, NewObjPtr);
    res = 0;
    goto exit;

    exit:
    args_deinit(buffs);
    return res;
}

static void print(PikaObj *self, Args *args)
{
    obj_setErrorCode(self, 0);
    char *res = args_print(args, "val");
    if (NULL == res)
    {
        obj_setSysOut(self, "[error] print: can not print32_t val");
        obj_setErrorCode(self, 1);
        return;
    }
    /* not empty */
    obj_setSysOut(self, res);
}

static void set(PikaObj *self, Args *args)
{
    obj_setErrorCode(self, 0);
    char *argPath = method_getStr(args, "argPath");
    if (obj_isArgExist(self, argPath))
    {
        /* update arg */
        char *valStr = args_print(args, "val");
        int32_t res = obj_set(self, argPath, valStr);
        if (1 == res)
        {
            obj_setSysOut(self, "[error] set: arg no found.");
            obj_setErrorCode(self, 1);
            return;
        }
        if (2 == res)
        {
            obj_setSysOut(self, "[error] set: type not match.");
            obj_setErrorCode(self, 1);
            return;
        }
        if (3 == res)
        {
            obj_setSysOut(self, "[error] set: object not found.");
            obj_setErrorCode(self, 1);
            return;
        }
        return;
    }
    /* new arg */
    Arg *val = args_getArg(args, "val");
    Arg *newArg = arg_copy(val);
    char *argName = strsGetLastToken(args, argPath, '.');
    arg_setName(newArg, argName);
    int32_t res = obj_setArg(self, argPath, newArg);
    if (res == 1)
    {
        obj_setSysOut(self, "[error] set: object not found.");
        obj_setErrorCode(self, 1);
    }
    arg_deinit(newArg);
    newArg = NULL;
    return;
}


PikaObj *New_BaseObj(Args *args)
{
    PikaObj *self = New_TinyObj(args);
    obj_setObjWithoutClass(self, "__classLoader", New_TinyObj);
    obj_getObj(self, "__classLoader", 0);
    class_defineMethod(self, "print(val:any)", print);
    class_defineMethod(self, "set(argPath:str, val:any)", set);
    return self;
}
