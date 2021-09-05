#include "PikaObj.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataString.h"
#include "dataStrs.h"
#include "SysObj.h"

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
    pikaFree(self, self->memSize);
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
    args_setPtr(initArgs, "__context", self);
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

int32_t removeEachMethodInfo(Arg *argNow, Args *argList)
{
    if (strIsStartWith(arg_getName(argNow), "[methodDec]"))
    {
        args_removeArg(argList, arg_getName(argNow));
        return 0;
    }
    if (strIsStartWith(arg_getName(argNow), "[methodPtr]"))
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

PikaObj *obj_getClassObjByNewFun(PikaObj *context, char *name, NewFun newClassFun)
{
    Args *initArgs = New_args(NULL);
    args_setPtr(initArgs, "__context", context);
    args_setStr(initArgs, "__name", name);
    PikaObj *thisClass = newClassFun(initArgs);
    obj_setPtr(thisClass, "__classPtr", newClassFun);
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

void *getNewObjFunByClass(PikaObj *obj, char *classPath)
{
    PikaObj *classHost = args_getPtr(obj->attributeList, "__classLoader");
    if (NULL == classHost)
    {
        return NULL;
    }
    void *(*newObjFun)(Args * initArgs) = args_getPtr(classHost->attributeList, classPath);
    return newObjFun;
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

PikaObj *newRootObj(char *name, NewFun newObjFun)
{
    PikaObj *thisClass = obj_getClassObjByNewFun(NULL, name, newObjFun);
    PikaObj *newObj = removeMethodInfo(thisClass);
    return newObj;
}

static void removeClassLoader(PikaObj *obj)
{
    PikaObj *classObj = args_getPtr(obj->attributeList, "__classLoader");
    if (NULL != classObj)
    {
        obj_deinit(classObj);
        args_removeArg(obj->attributeList, "__classLoader");
    }
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
    /* delete "__classLoader" object */
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

void loadMethodInfo(PikaObj *methodHost, char *methodName, char *methodDeclearation, void *methodPtr)
{
    Args *buffs = New_strBuff();
    char *methodPtrPath = strsAppend(buffs, "[methodPtr]", methodName);
    char *methodDeclearationPath = strsAppend(buffs, "[methodDec]", methodName);
    obj_setPtr(methodHost, methodPtrPath, methodPtr);
    obj_setStr(methodHost, methodDeclearationPath, methodDeclearation);
    args_deinit(buffs);
}

static char *getMethodDeclearation(PikaObj *obj, char *methodName)
{
    Args *buffs = New_strBuff();
    char *methodDeclearationPath = strsAppend(buffs, "[methodDec]", methodName);
    char *res = obj_getStr(obj, methodDeclearationPath);
    args_deinit(buffs);
    return res;
}

static void *getMethodPtr(PikaObj *methodHost, char *methodName)
{
    Args *buffs = New_strBuff();
    char *methodPtrPath = strsAppend(buffs, "[methodPtr]", methodName);
    void *res = obj_getPtr(methodHost, methodPtrPath);
    args_deinit(buffs);
    return res;
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

char *getDirectStr(Args *buffs, char *argPath)
{
    char *directStr = NULL;
    directStr = strsCut(buffs, argPath, '"', '"');
    if (NULL != directStr)
    {
        return directStr;
    }
    directStr = strsCut(buffs, argPath, '\'', '\'');
    if (NULL != directStr)
    {
        return directStr;
    }
    return NULL;
}

static int32_t loadArgByType(PikaObj *self,
                             char *definedName,
                             char *definedType,
                             char *argPath,
                             Args *args)
{
    if (strEqu(definedType, "any"))
    {
        char *directStr = getDirectStr(args, argPath);
        if (NULL != directStr)
        {
            /* direct string value */
            args_setStr(args, definedName, directStr);
            /* ok */
            return 0;
        }
        if ((argPath[0] >= '0') && (argPath[0] <= '9'))
        {
            /* direct number value */
            char *argName = argPath;
            if (strIsContain(argName, '.'))
            {
                args_setFloat(args, definedName, 0);
                args_set(args, definedName, argPath);
                /* succeed */
                return 0;
            }
            args_setInt(args, definedName, 0);
            args_set(args, definedName, argPath);
            /* succeed */
            return 0;
        }
        /* get reference arg */
        Arg *arg = obj_getArg(self, argPath);
        if (arg == NULL)
        {
            /* can not get arg */
            return 3;
        }
        Arg *argCopied = arg_copy(arg);
        arg_setName(argCopied, definedName);
        args_setArg(args, argCopied);
        return 0;
    }
    if (strEqu(definedType, "str"))
    {
        /* solve the string type */
        char *directStr = getDirectStr(args, argPath);
        if (NULL != directStr)
        {
            /* direct value */
            args_setStr(args, definedName, directStr);
            /* ok */
            return 0;
        }
        /* reference value */
        char *refStr = obj_getStr(self, argPath);
        if (NULL == refStr)
        {
            /* faild */
            return 1;
        }
        args_setStr(args, definedName, refStr);
        /* succeed */
        return 0;
    }
    if (strEqu(definedType, "int"))
    {
        /* solve the int32_t type */
        args_setInt(args, definedName, 0);
        if ((argPath[0] >= '0') && (argPath[0] <= '9'))
        {
            /* direct value */
            args_set(args, definedName, argPath);
            /* succeed */
            return 0;
        }
        /* reference value */
        if (!obj_isArgExist(self, argPath))
        {
            /* can not get reference */
            return 3;
        }
        int32_t referenceVal = obj_getInt(self, argPath);
        args_setInt(args, definedName, referenceVal);
        /* succeed */
        return 0;
    }
    if (strEqu(definedType, "float"))
    {
        /* solve the float type */
        args_setFloat(args, definedName, 0);
        if ((argPath[0] >= '0') && (argPath[0] <= '9'))
        {
            /* direct value */
            args_set(args, definedName, argPath);
            /* succeed */
            return 0;
        }
        /* reference value */
        if (!obj_isArgExist(self, argPath))
        {
            /* can not get reference */
            return 3;
        }
        float referenceVal = obj_getFloat(self, argPath);
        args_setFloat(args, definedName, referenceVal);
        /* succeed */
        return 0;
    }
    if (strEqu(definedType, "pointer"))
    {
        /* only support reference value */
        if (!obj_isArgExist(self, argPath))
        {
            /* can not get reference */
            return 3;
        }
        void *ptr = obj_getPtr(self, argPath);
        args_setPtr(args, definedName, ptr);
        return 0;
    }
    /* type match faild */
    return 2;
}

char *getTypeVal(Args *buffs, char *typeToken)
{
    if (!strIsContain(typeToken, ':'))
    {
        return strsCopy(buffs, "");
    }
    return strsGetLastToken(buffs, typeToken, ':');
}

static Args *getArgsBySort(PikaObj *self, char *typeList, char *argList)
{
    Args *buffs = New_strBuff();
    char *typeListBuff = strsCopy(buffs, typeList);
    char *argListBuff = strsCopy(buffs, argList);
    Args *args = New_args(NULL);
    while (1)
    {
        char *typeToken = strsPopToken(buffs, typeListBuff, ',');
        char *argToken = strsPopToken(buffs, argListBuff, ',');
        if ((0 == argToken[0]) || (0 == typeToken[0]))
        {
            /* arg or type poped finised */
            break;
        }

        char *typeName = strsGetFirstToken(buffs, typeToken, ':');
        char *typeVal = getTypeVal(buffs, typeToken);
        char *argPath = argToken;

        if (0 != loadArgByType(self,
                               typeName,
                               typeVal,
                               argPath,
                               args))
        {
            args_deinit(args);
            args_deinit(buffs);
            return NULL;
        }
    }
    args_deinit(buffs);
    return args;
}

static Args *getArgsByNameMatch(PikaObj *self, char *typeList, char *argList)
{
    Args *buffs = New_strBuff();
    char *typeListBuff = strsCopy(buffs, typeList);
    Args *args = New_args(NULL);
    while (1)
    {
        char *typeToken = strsPopToken(buffs, typeListBuff, ',');
        /* poped all type from typeList */
        if (0 == typeToken[0])
        {
            break;
        }

        char *typeName = strsGetFirstToken(buffs, typeToken, ':');
        char *typeVal = getTypeVal(buffs, typeToken);

        char *argListBuff = strsCopy(buffs, argList);
        while (1)
        {
            char *argToken = strsPopToken(buffs, argListBuff, ',');
            char *argName = strsGetFirstToken(buffs, argToken, '=');
            char *argVal = strsGetLastToken(buffs, argToken, '=');

            if (0 == argToken[0])
            {
                /* arg poped finised */
                break;
            }

            if (!strEqu(typeName, argName))
            {
                /* name not match */
                continue;
            }

            if (0 != loadArgByType(self,
                                   typeName,
                                   typeVal,
                                   argVal,
                                   args))
            {
                args_deinit(args);
                args_deinit(buffs);
                return NULL;
            }
        }
    }
    args_deinit(buffs);
    return args;
}

static Args *getArgsBySentence(PikaObj *self, char *typeList, char *argList)
{
    if (strIsContain(argList, '='))
    {
        return getArgsByNameMatch(self, typeList, argList);
    }
    return getArgsBySort(self, typeList, argList);
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

char *getMethodPath(Args *buffs, char *methodToken)
{
    if (strIsContain(methodToken, '='))
    {
        return strsGetLastToken(buffs, methodToken, '=');
    }
    else
    {
        return methodToken;
    }
}

static char *getCleanCmd(Args *buffs, char *cmd)
{
    int32_t size = strGetSize(cmd);
    char *strOut = args_getBuff(buffs, size);
    int32_t iOut = 0;
    char delChar = ' ';
    int32_t isInStr = 0;
    for (int32_t i = 0; i < strGetSize(cmd); i++)
    {
        if ('\'' == cmd[i])
        {
            isInStr = !isInStr;
        }
        if ((delChar == cmd[i]) && (!isInStr))
        {
            /* do not load char */
            continue;
        }
        strOut[iOut] = cmd[i];
        iOut++;
    }
    /* add \0 */
    strOut[iOut] = 0;
    return strOut;
}

Args *obj_runDirect(PikaObj *self, char *cmd)
{
    /* the Args returned need to be deinit */
    Args *res = New_args(NULL);
    args_setInt(res, "errCode", 0);
    Args *buffs = New_strBuff();
    char *cleanCmd = getCleanCmd(buffs, cmd);
    char *methodToken = strsGetFirstToken(buffs, cleanCmd, '(');
    char *methodPath = getMethodPath(buffs, methodToken);
    Args *args = NULL;

    PikaObj *methodHostObj = obj_getObj(self, methodPath, 1);
    PikaObj *methodHostClass = NULL;
    if (NULL == methodHostObj)
    {
        /* error, not found object */
        args_setInt(res, "errCode", 1);
        method_sysOut(res, "[error] runner: object no found.");
        goto exit;
    }
    char *methodName = strsGetLastToken(buffs, methodPath, '.');

    void *classPtr = obj_getPtr(methodHostObj, "__classPtr");
    char *methodHostClassName = strsAppend(buffs, "classObj-", methodHostObj->name);
    methodHostClass = obj_getClassObjByNewFun(methodHostObj, methodHostClassName, classPtr);
    /* get method Ptr */
    void (*methodPtr)(PikaObj * self, Args * args) = getMethodPtr(methodHostClass, methodName);
    char *methodDecInClass = getMethodDeclearation(methodHostClass, methodName);
    /* assert method*/
    if ((NULL == methodDecInClass) || (NULL == methodPtr))
    {
        /* error, method no found */
        args_setInt(res, "errCode", 2);
        method_sysOut(res, "[error] runner: method no found.");
        goto exit;
    }
    char *methodDec = strsCopy(buffs, methodDecInClass);
    /* free method host class to save memory */
    obj_deinit(methodHostClass);
    methodHostClass = NULL;

    /* get type list */
    char *typeList = strsCut(buffs, methodDec, '(', ')');
    if (typeList == NULL)
    {
        /* typeList no found */
        args_setInt(res, "errCode", 3);
        method_sysOut(res, "[error] runner: type list no found.");
        goto exit;
    }

    /* get arg list */
    char *argList = strsCut(buffs, cleanCmd, '(', ')');
    {
        if (argList == NULL)
        {
            /* argL List no found */
            args_setInt(res, "errCode", 4);
            method_sysOut(res, "[error] runner: arg list no found.");
            goto exit;
        }
    }

    /* get return type */
    char *returnType = strsGetLastToken(buffs, methodDec, ')');
    /* get args */
    args = getArgsBySentence(self, typeList, argList);
    if (NULL == args)
    {
        /* get args faild */
        args_setInt(res, "errCode", 5);
        method_sysOut(res, "[error] runner: solve arg faild.");
        goto exit;
    }
    /* run method */
    methodPtr(methodHostObj, args);

    /* transfer return */
    if (strIsContain(methodToken, '='))
    {
        char *returnName = strsGetFirstToken(buffs, methodToken, '=');
        transferReturnVal(self, returnType, returnName, args);
    }
    /* transfer sysOut */
    char *sysOut = args_getStr(args, "sysOut");
    if (NULL != sysOut)
    {
        args_setStr(res, "sysOut", args_getStr(args, "sysOut"));
    }
    /* solve errCode */
    if (!args_isArgExist(args, "errCode"))
    {
        goto exit;
    }
    int32_t errCode = args_getInt(args, "errCode");

    if (0 != errCode)
    {
        /* method error */
        args_setInt(res, "errCode", 6);
    }
    goto exit;
exit:
    if (NULL != buffs)
    {
        args_deinit(buffs);
    }
    if (NULL != methodHostClass)
    {
        obj_deinit(methodHostClass);
    }
    if (NULL != args)
    {
        args_deinit(args);
    }
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
    char *sysOut = args_getStr(res, "sysOut");
    if (NULL != sysOut)
    {
        printf("%s\r\n", sysOut);
    }
    if (0 != args_getInt(res, "errCode"))
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
