#include "PikaObj.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataString.h"
#include "dataStrs.h"
#include "PikaInvoke.h"
#include <stdarg.h>
#include "PikaBlock.h"
#include "PikaIf.h"

void *getNewObjFunByClass(PikaObj *obj, char *classPath)
{
    PikaObj *classLoader = args_getPtr(obj->attributeList, "_clsld");
    if (NULL == classLoader)
    {
        return NULL;
    }
    void *(*newObjFun)(Args * initArgs) = args_getPtr(classLoader->attributeList, classPath);
    return newObjFun;
}

int32_t deinitEachSubObj(Arg *argEach, Args *handleArgs)
{
    if (NULL != handleArgs)
    {
        /* error: tOhis handle not need handle args */
        return 1;
    }
    char *type = arg_getType(argEach);
    if (strIsStartWith(type, "_class"))
    {
        PikaObj *subObj = arg_getPtr(argEach);
        if (NULL != subObj)
        {
            obj_deinit(subObj);
        }
    }
    return 0;
}

void deinitAllSubObj(PikaObj *self)
{
    Args *args = self->attributeList;
    args_foreach(args, deinitEachSubObj, NULL);
}

int32_t obj_deinit(PikaObj *self)
{
    deinitAllSubObj(self);
    args_deinit(self->attributeList);
    //DynMemPut(self->mem);
    pikaFree(self, sizeof(PikaObj));
    self = NULL;
    return 0;
}

int32_t obj_enable(PikaObj *self)
{
    obj_setInt(self, "isEnable", 1);
    return 0;
}

int32_t obj_disable(PikaObj *self)
{
    obj_setInt(self, "isEnable", 0);
    return 0;
}

int32_t obj_setInt(PikaObj *self, char *argPath, int64_t val)
{
    PikaObj *obj = obj_getObj(self, argPath, 1);
    if (NULL == obj)
    {
        /* [error] object no found */
        return 1;
    }
    Args *buffs = New_strBuff();
    char *name = strsGetLastToken(buffs, argPath, '.');
    args_setInt(obj->attributeList, name, val);
    args_deinit(buffs);
    return 0;
}

int32_t obj_setPtr(PikaObj *self, char *argPath, void *pointer)
{
    PikaObj *obj = obj_getObj(self, argPath, 1);
    if (NULL == obj)
    {
        return 1;
    }
    Args *buffs = New_strBuff();
    char *name = strsGetLastToken(buffs, argPath, '.');
    args_setPtr(obj->attributeList,
                name, pointer);
    args_deinit(buffs);
    return 0;
}

int32_t obj_setFloat(PikaObj *self, char *argPath, float value)
{
    PikaObj *obj = obj_getObj(self, argPath, 1);
    if (NULL == obj)
    {
        return 1;
    }
    Args *buffs = New_strBuff();
    char *name = strsGetLastToken(buffs, argPath, '.');
    args_setFloat(obj->attributeList,
                  name, value);
    args_deinit(buffs);
    return 0;
}

int32_t obj_setStr(PikaObj *self, char *argPath, char *str)
{
    PikaObj *obj = obj_getObj(self, argPath, 1);
    if (NULL == obj)
    {
        return 1;
    }
    Args *buffs = New_strBuff();
    char *name = strsGetLastToken(buffs, argPath, '.');
    args_setStr(obj->attributeList,
                name, str);
    args_deinit(buffs);
    return 0;
}

int64_t obj_getInt(PikaObj *self, char *argPath)
{
    PikaObj *obj = obj_getObj(self, argPath, 1);
    if (NULL == obj)
    {
        return -999999999;
    }
    Args *buffs = New_strBuff();
    char *argName = strsGetLastToken(buffs, argPath, '.');
    int res = args_getInt(obj->attributeList, argName);
    args_deinit(buffs);
    return res;
}

Arg *obj_getArg(PikaObj *self, char *argPath)
{
    PikaObj *obj = obj_getObj(self, argPath, 1);
    if (NULL == obj)
    {
        return NULL;
    }
    Args *buffs = New_strBuff();
    char *argName = strsGetLastToken(buffs, argPath, '.');
    Arg *res = args_getArg(obj->attributeList, argName);
    args_deinit(buffs);
    return res;
}

int32_t obj_setArg(PikaObj *self, char *argPath, Arg *arg)
{
    /* setArg would copy arg */
    PikaObj *obj = obj_getObj(self, argPath, 1);
    if (NULL == obj)
    {
        /* object no found */
        return 1;
    }
    args_copyArg(obj->attributeList, arg);
    return 0;
}

void *obj_getPtr(PikaObj *self, char *argPath)
{
    PikaObj *obj = obj_getObj(self, argPath, 1);
    if (NULL == obj)
    {
        return NULL;
    }
    Args *buffs = New_strBuff();
    char *argName = strsGetLastToken(buffs, argPath, '.');
    void *res = args_getPtr(obj->attributeList, argName);
    args_deinit(buffs);
    return res;
}

float obj_getFloat(PikaObj *self, char *argPath)
{
    PikaObj *obj = obj_getObj(self, argPath, 1);
    if (NULL == obj)
    {
        return -999.999;
    }
    Args *buffs = New_strBuff();
    char *argName = strsGetLastToken(buffs, argPath, '.');
    float res = args_getFloat(obj->attributeList,
                              argName);
    args_deinit(buffs);
    return res;
}

char *obj_getStr(PikaObj *self, char *argPath)
{
    PikaObj *obj = obj_getObj(self, argPath, 1);
    if (NULL == obj)
    {
        return NULL;
    }
    Args *buffs = New_strBuff();
    char *argName = strsGetLastToken(buffs, argPath, '.');
    char *res = args_getStr(obj->attributeList,
                            argName);
    args_deinit(buffs);
    return res;
}

int32_t obj_load(PikaObj *self, Args *args, char *name)
{
    args_copyArgByName(args, name, self->attributeList);
    return 0;
}

int32_t obj_setObjWithoutClass(PikaObj *self, char *objName, void *newFun)
{
    /* class means subprocess init */
    Args *buffs = New_strBuff();
    char *mataObjName = strsAppend(buffs, "[mate]", objName);
    obj_setPtr(self, mataObjName, newFun);
    /* add void object Ptr, no inited */
    args_setObjectWithClass(self->attributeList, objName, "none", NULL);
    args_deinit(buffs);
    return 0;
}

int32_t obj_addOther(PikaObj *self, char *subObjectName, void *new_ObjectFun)
{
    Args *initArgs = New_args(NULL);
    args_setPtr(initArgs, "_ctx", self);
    void *(*new_Object)(Args * initArgs) = (void *(*)(Args *))new_ObjectFun;
    void *subObject = new_Object(initArgs);
    obj_setPtr(self, subObjectName, subObject);
    args_deinit(initArgs);
    return 0;
}

int32_t obj_freeObj(PikaObj *self, char *objPath)
{
    PikaObj *obj = obj_getPtr(self, objPath);
    obj_deinit(obj);
    return 0;
}

int32_t obj_bind(PikaObj *self, char *type, char *name, void *pointer)
{
    args_bind(self->attributeList, type, name, pointer);
    return 0;
}

char *obj_print(PikaObj *self, char *name)
{
    if (NULL == self)
    {
        return NULL;
    }
    return args_print(self->attributeList, name);
}

int32_t obj_bindInt(PikaObj *self, char *name, int32_t *valPtr)
{
    args_bindInt(self->attributeList, name, valPtr);
    return 0;
}

int32_t obj_bindFloat(PikaObj *self, char *name, float *valPtr)
{
    args_bindFloat(self->attributeList, name, valPtr);
    return 0;
}

int32_t obj_bindString(PikaObj *self, char *name, char **valPtr)
{
    args_bindStr(self->attributeList, name, valPtr);
    return 0;
}

int32_t obj_set(PikaObj *self, char *argPath, char *valStr)
{
    PikaObj *obj = obj_getObj(self, argPath, 1);
    if (NULL == obj)
    {
        /* cant get object */
        return 3;
    }
    Args *buffs = New_strBuff();
    char *argName = strsGetLastToken(buffs, argPath, '.');
    int32_t res = args_set(obj->attributeList, argName, valStr);
    args_deinit(buffs);
    if (res == 1)
    {
        /* do not get arg */
        return 1;
    }
    if (res == 2)
    {
        /* type not match */
        return 2;
    }
    /* succeed */
    return 0;
}

PikaObj *obj_getClassObjByNewFun(PikaObj *context, char *name, NewFun newClassFun)
{
    Args *initArgs = New_args(NULL);
    args_setPtr(initArgs, "_ctx", context);
    args_setStr(initArgs, "_n", name);
    PikaObj *thisClass = newClassFun(initArgs);
    obj_setPtr(thisClass, "_clsptr", newClassFun);
    args_deinit(initArgs);
    return thisClass;
}

char *obj_getClassPath(PikaObj *objHost, Args *buffs, char *objName)
{
    Arg *objArg = obj_getArg(objHost, objName);
    char *objType = arg_getType(objArg);
    char *classPath = strsRemovePrefix(buffs, objType, "_class-");
    return classPath;
}

void *getNewClassObjFunByName(PikaObj *obj, char *name)
{
    Args *buffs = New_strBuff();
    char *classPath = strsAppend(buffs, "[mate]", name);
    /* init the subprocess */
    void *(*newClassObjFun)(Args * initArgs) = args_getPtr(obj->attributeList, classPath);
    args_deinit(buffs);
    return newClassObjFun;
}

int32_t removeEachMethodInfo(Arg *argNow, Args *argList)
{
    if (strIsStartWith(arg_getName(argNow), "[md]"))
    {
        args_removeArg(argList, arg_getName(argNow));
        return 0;
    }
    if (strIsStartWith(arg_getName(argNow), "[mp]"))
    {
        args_removeArg(argList, arg_getName(argNow));
        return 0;
    }
    return 0;
}

PikaObj *removeMethodInfo(PikaObj *thisClass)
{
    args_foreach(thisClass->attributeList, removeEachMethodInfo, thisClass->attributeList);
    return thisClass;
}

static void removeClassLoader(PikaObj *obj)
{
    PikaObj *classLoader = args_getPtr(obj->attributeList, "_clsld");
    if (NULL != classLoader)
    {
        obj_deinit(classLoader);
        args_removeArg(obj->attributeList, "_clsld");
    }
}

PikaObj *newRootObj(char *name, NewFun newObjFun)
{
    PikaObj *thisClass = obj_getClassObjByNewFun(NULL, name, newObjFun);
    PikaObj *newObj = removeMethodInfo(thisClass);
    return newObj;
}

PikaObj *initObj(PikaObj *obj, char *name)
{
    PikaObj *res = NULL;
    NewFun newObjFun = getNewClassObjFunByName(obj, name);
    Args *buffs = New_args(NULL);
    if (NULL == newObjFun)
    {
        /* no such object */
        res = NULL;
        goto exit;
    }
    PikaObj *thisClass = obj_getClassObjByNewFun(obj, name, newObjFun);
    PikaObj *newObj = removeMethodInfo(thisClass);
    /* delete [mate]<objName> */
    obj_removeArg(obj, strsAppend(buffs, "[mate]", name));
    /* delete "_clsld" object */
    removeClassLoader(newObj);

    char *type = args_getType(obj->attributeList, name);
    args_setPtrWithType(obj->attributeList, name, type, newObj);
    res = obj_getPtr(obj, name);
    goto exit;
exit:
    args_deinit(buffs);
    return res;
}

PikaObj *obj_getObjDirect(PikaObj *self, char *name)
{
    if (NULL == self)
    {
        return NULL;
    }

    /* check subprocess */
    if (NULL == args_getPtr(self->attributeList, name))
    {
        /* no inited subprocess, check subprocess init fun*/
        return initObj(self, name);
    }

    /* finded subscribe, check type*/
    char *type = args_getType(self->attributeList,
                              name);
    if (!strIsStartWith(type, "_class"))
    {
        /* type error, could not found subprocess */
        return NULL;
    }
    return obj_getPtr(self, name);
}

PikaObj *obj_getObj(PikaObj *self, char *objPath, int32_t keepDeepth)
{
    Args *buffs = New_strBuff();
    char *objPathBuff = strsCopy(buffs, objPath);
    int32_t tokenNum = strGetTokenNum(objPath, '.');
    PikaObj *obj = self;
    for (int32_t i = 0; i < tokenNum - keepDeepth; i++)
    {
        char *token = strsPopToken(buffs, objPathBuff, '.');
        obj = obj_getObjDirect(obj, token);
        if (obj == NULL)
        {
            goto exit;
        }
    }
    goto exit;
exit:
    args_deinit(buffs);
    return obj;
}

char *obj_getName(PikaObj *self)
{
    return obj_getStr(self, "_n");
}

void loadMethodInfo(PikaObj *methodHost, char *methodName, char *methodDeclearation, void *methodPtr)
{
    Args *buffs = New_strBuff();
    char *methodPtrPath = strsAppend(buffs, "[mp]", methodName);
    char *methodDeclearationPath = strsAppend(buffs, "[md]", methodName);
    obj_setPtr(methodHost, methodPtrPath, methodPtr);
    obj_setStr(methodHost, methodDeclearationPath, methodDeclearation);
    args_deinit(buffs);
}

int32_t class_defineMethod(PikaObj *self,
                           char *declearation,
                           void (*methodPtr)(PikaObj *self, Args *args))
{
    int32_t size = strGetSize(declearation);
    int32_t res = 0;
    Args *buffs = New_strBuff();
    char *cleanDeclearation = strDeleteChar(args_getBuff(buffs, size), declearation, ' ');
    char *methodPath = strGetFirstToken(args_getBuff(buffs, size), cleanDeclearation, '(');

    PikaObj *methodHost = obj_getObj(self, methodPath, 1);
    if (NULL == methodHost)
    {
        /* no found method object */
        res = 1;
        goto exit;
    }
    char *methodName = strsGetLastToken(buffs, methodPath, '.');

    loadMethodInfo(methodHost, methodName, cleanDeclearation, methodPtr);
    res = 0;
    goto exit;
exit:
    args_deinit(buffs);
    return res;
}

static void transferReturnVal(PikaObj *self, char *returnType, char *returnName, Args *args)
{
    if (strEqu("->int", returnType))
    {
        int returnVal = args_getInt(args, "return");
        obj_setInt(self, returnName, returnVal);
    }
    if (strEqu("->float", returnType))
    {
        float returnVal = args_getFloat(args, "return");
        obj_setFloat(self, returnName, returnVal);
    }
    if (strEqu("->str", returnType))
    {
        char *returnVal = args_getStr(args, "return");
        obj_setStr(self, returnName, returnVal);
    }
}

char *getRightCmd(Args *buffs, char *cmd)
{
    char *right = NULL;
    char *cmdBuff = strsCopy(buffs, cmd);
    if (strIsContain(cmd, '('))
    {
        cmdBuff = strsGetFirstToken(buffs, cmdBuff, '(');
    }
    if (strIsContain(cmdBuff, '='))
    {
        cmdBuff = strsCopy(buffs, cmd);
        strsPopToken(buffs, cmdBuff, '=');
        right = cmdBuff;
    }
    else
    {
        right = cmd;
    }
    return right;
}

Args *getRightRes(PikaObj *self, char *cmd)
{
    Args *buffs = New_strBuff();
    Args *res = NULL;
    if (strIsContain(cmd, '(') && strIsContain(cmd, ')'))
    {
        res = obj_invoke(self, cmd);
        goto exit;
    }
    res = New_args(NULL);
    args_setSysOut(res, "");
    int err = args_setLiteral(res, "return", cmd);
    if (err != 0)
    {
        args_setSysOut(res, "[error] get value faild.");
        args_setErrorCode(res, 1);
        goto exit;
    }
    char *returnType = strsAppend(buffs, "->", args_getType(res, "return"));
    args_setStr(res, "returnType", returnType);
    goto exit;

exit:
    args_deinit(buffs);
    return res;
}

Args *obj_runScript(PikaObj *self, char *cmd)
{
    Args *buffs = New_strBuff();
    Args *res = NULL;

    /* get right cmd */
    char *right = getRightCmd(buffs, cmd);

    /* get res from right cmd */
    res = getRightRes(self, right);
    if (NULL != res)
    {
        if (0 != args_getErrorCode(res))
        {
            goto exit;
        }
    }
    /* check res */
    if (NULL == res)
    {
        res = New_args(NULL);
        args_setErrorCode(res, 1);
        args_setSysOut(res, "[error] solve script format faild!");
        goto exit;
    }

    /* transfer return */
    if (strIsContain(cmd, '='))
    {
        char *returnName = strsGetFirstToken(buffs, cmd, '=');
        returnName = strsDeleteChar(buffs, returnName, ' ');
        char *returnType = args_getStr(res, "returnType");
        transferReturnVal(self, returnType, returnName, res);
    }

exit:
    args_deinit(buffs);
    return res;
}

Args *obj_runDirect(PikaObj *self, char *cmd)
{
    Args *buffs = New_strBuff();
    Args *res = NULL;
    if (strIsStartWith(cmd, "if "))
    {
        obj_setInt(self, "_isInBlock", 1);
        obj_setObjWithoutClass(self, "_block", block_init());
        PikaObj *block = obj_getObj(self, "_block", 0);
        if_setAssert(block, cmd);
        /* this line processed ok */
        goto exit;
    }

    if (NULL != obj_getArg(self, "_isInBlock"))
    {
        PikaObj *block = obj_getObj(self, "_block", 0);
        if (strIsStartWith(cmd, "    "))
        {
            if (strEqu(block_getMode(block), "if"))
            {
                if_pushLine(block, cmd);
                goto exit;
            }
            goto exit;
        }
        /* the block is end */
        else
        {
            obj_removeArg(self, "_isInBlock");
            if (strEqu(block_getMode(block), "if"))
            {
                if_run(block);
            }
            /* not finished */
        }
    }

    res = obj_runScript(self, cmd);
    goto exit;
exit:
    /* check res */
    if (NULL == res)
    {
        res = New_args(NULL);
        args_setErrorCode(res, 0);
        args_setSysOut(res, "");
        goto exit;
    }
    args_deinit(buffs);
    return res;
}

int32_t obj_removeArg(PikaObj *self, char *argPath)
{
    PikaObj *objHost = obj_getObj(self, argPath, 1);
    PikaObj *obj = obj_getObj(self, argPath, 0);
    Args *buffs = New_strBuff();
    if (NULL != obj)
    {
        obj_deinit(obj);
    }
    int32_t err = 0;
    if (NULL == objHost)
    {
        /* [error] object no found */
        err = 1;
        goto exit;
    }
    char *argName = strsGetLastToken(buffs, argPath, '.');
    int32_t res = args_removeArg(objHost->attributeList, argName);
    if (1 == res)
    {
        /*[error] not found arg*/
        err = 2;
        goto exit;
    }
    goto exit;
exit:
    args_deinit(buffs);
    return err;
}

int32_t obj_isArgExist(PikaObj *self, char *argPath)
{
    PikaObj *obj = obj_getObj(self, argPath, 1);
    Args *buffs = New_strBuff();
    int32_t res = 0;
    if (NULL == obj)
    {
        /* [error] object no found */
        res = 1;
        goto exit;
    }
    char *argName = strsGetLastToken(buffs, argPath, '.');
    Arg *arg = args_getArg(obj->attributeList, argName);
    if (NULL == arg)
    {
        /* no found arg */
        res = 0;
        goto exit;
    }
    /* found arg */
    res = 1;
    goto exit;

exit:
    args_deinit(buffs);
    return res;
}

void obj_runNoRes(PikaObj *slef, char *cmd)
{
    /* unsafe, nothing would happend when error occord */
    args_deinit(obj_runDirect(slef, cmd));
}

void obj_run(PikaObj *self, char *cmd)
{
    /* safe, stop when error occord and error info would be print32_t */
    Args *res = obj_runDirect(self, cmd);
    char *sysOut = args_getSysOut(res);
    if (!strEqu("", sysOut))
    {
        printf("%s\r\n", sysOut);
    }
    if (0 != args_getErrorCode(res))
    {
        printf("[info] input commond: %s\r\n", cmd);
        while (1)
            ;
    }
    if (NULL != res)
    {
        args_deinit(res);
    }
}

void obj_setErrorCode(PikaObj *self, int32_t errCode)
{
    obj_setInt(self, "__errCode", errCode);
}

int32_t obj_getErrorCode(PikaObj *self)
{
    if (!obj_isArgExist(self, "__errCode"))
    {
        return 0;
    }
    return obj_getInt(self, "__errCode");
}

void args_setErrorCode(Args *args, int32_t errCode)
{
    args_setInt(args, "__errCode", errCode);
}

int32_t args_getErrorCode(Args *args)
{
    if (!args_isArgExist(args, "__errCode"))
    {
        return 0;
    }
    return args_getInt(args, "__errCode");
}

void obj_setSysOut(PikaObj *self, char *str)
{
    obj_setStr(self, "__sysOut", str);
}

char *obj_getSysOut(PikaObj *self)
{
    return obj_getStr(self, "__sysOut");
}

char *args_getSysOut(Args *args)
{
    return args_getStr(args, "__sysOut");
}

void args_setSysOut(Args *args, char *str)
{
    args_setStr(args, "__sysOut", str);
}

void obj_sysPrintf(PikaObj *self, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char sysOut[128] = {0};
    vsprintf(sysOut, fmt, args);
    obj_setSysOut(self, sysOut);
    va_end(args);
}
