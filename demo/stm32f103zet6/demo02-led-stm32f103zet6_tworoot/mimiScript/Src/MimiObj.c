#include "MimiObj.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataString.h"
#include "dataStrs.h"
#include "sysObj.h"

int deinitEachSubObj(Arg *argEach, Args *handleArgs)
{
    if (NULL != handleArgs)
    {
        /* error: tOhis handle not need handle args */
        return 1;
    }
    char *type = arg_getType(argEach);
    if (strIsStartWith(type, "_class"))
    {
        MimiObj *subObj = arg_getPtr(argEach);
        if (NULL != subObj)
        {
            obj_deinit(subObj);
        }
    }
    return 0;
}

void deinitAllSubObj(MimiObj *self)
{
    Args *args = self->attributeList;
    args_foreach(args, deinitEachSubObj, NULL);
}

int obj_deinit(MimiObj *self)
{
    self->_beforDinit(self);
    deinitAllSubObj(self);
    args_deinit(self->attributeList);
    DynMemPut(self->mem);
    self = NULL;
    return 0;
}

int obj_update(MimiObj *self)
{
    // return if is not enable
    if (0 == obj_getInt(self, "isEnable"))
    {
        return 1;
    }
    self->_updateHandle(self);
    return 0;
}
void _UpdateHandle(MimiObj *self)
{
    // override the handle function here
}

int obj_enable(MimiObj *self)
{
    obj_setInt(self, "isEnable", 1);
    return 0;
}

int obj_disable(MimiObj *self)
{
    obj_setInt(self, "isEnable", 0);
    return 0;
}

int obj_setInt(MimiObj *self, char *argPath, long long val)
{
    MimiObj *obj = obj_getObj(self, argPath, 1);
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

int obj_setPtr(MimiObj *self, char *argPath, void *pointer)
{
    MimiObj *obj = obj_getObj(self, argPath, 1);
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

int obj_setFloat(MimiObj *self, char *argPath, float value)
{
    MimiObj *obj = obj_getObj(self, argPath, 1);
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

int obj_setStr(MimiObj *self, char *argPath, char *str)
{
    MimiObj *obj = obj_getObj(self, argPath, 1);
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

long long obj_getInt(MimiObj *self, char *argPath)
{
    MimiObj *obj = obj_getObj(self, argPath, 1);
    if (NULL == obj)
    {
        return -999999999;
    }
    Args *buffs = New_strBuff();
    char *argName = strsGetLastToken(buffs, argPath, '.');
    args_deinit(buffs);
    return args_getInt(obj->attributeList,
                       argName);
}

Arg *obj_getArg(MimiObj *self, char *argPath)
{
    MimiObj *obj = obj_getObj(self, argPath, 1);
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

int obj_setArg(MimiObj *self, char *argPath, Arg *arg)
{
    /* setArg would copy arg */
    MimiObj *obj = obj_getObj(self, argPath, 1);
    if (NULL == obj)
    {
        /* object no found */
        return 1;
    }
    args_copyArg(obj->attributeList, arg);
    return 0;
}

void *obj_getPtr(MimiObj *self, char *argPath)
{
    MimiObj *obj = obj_getObj(self, argPath, 1);
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

float obj_getFloat(MimiObj *self, char *argPath)
{
    MimiObj *obj = obj_getObj(self, argPath, 1);
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

char *obj_getStr(MimiObj *self, char *argPath)
{
    MimiObj *obj = obj_getObj(self, argPath, 1);
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

int obj_load(MimiObj *self, Args *args, char *name)
{
    args_copyArgByName(args, name, self->attributeList);
    return 0;
}

void _beforDinit(MimiObj *self)
{
    /* override in user code */
}

int obj_setObjWithoutClass(MimiObj *self, char *objName, void *newFun)
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

int obj_addOther(MimiObj *self, char *subObjectName, void *new_ObjectFun)
{
    Args *initArgs = New_args(NULL);
    args_setPtr(initArgs, "context", self);
    void *(*new_Object)(Args * initArgs) = (void *(*)(Args *))new_ObjectFun;
    void *subObject = new_Object(initArgs);
    obj_setPtr(self, subObjectName, subObject);
    args_deinit(initArgs);
    return 0;
}

int obj_freeObj(MimiObj *self, char *objPath)
{
    MimiObj *obj = obj_getPtr(self, objPath);
    obj_deinit(obj);
    return 0;
}

int obj_bind(MimiObj *self, char *type, char *name, void *pointer)
{
    args_bind(self->attributeList, type, name, pointer);
    return 0;
}

char *obj_print(MimiObj *self, char *name)
{
    if (NULL == self)
    {
        return NULL;
    }
    return args_print(self->attributeList, name);
}

int obj_bindInt(MimiObj *self, char *name, int *valPtr)
{
    args_bindInt(self->attributeList, name, valPtr);
    return 0;
}

int obj_bindFloat(MimiObj *self, char *name, float *valPtr)
{
    args_bindFloat(self->attributeList, name, valPtr);
    return 0;
}

int obj_bindString(MimiObj *self, char *name, char **valPtr)
{
    args_bindStr(self->attributeList, name, valPtr);
    return 0;
}

int obj_set(MimiObj *self, char *argPath, char *valStr)
{
    MimiObj *obj = obj_getObj(self, argPath, 1);
    if (NULL == obj)
    {
        /* cant get object */
        return 3;
    }
    Args *buffs = New_strBuff();
    char *argName = strsGetLastToken(buffs, argPath, '.');
    int res = args_set(obj->attributeList, argName, valStr);
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

int removeEachMethodInfo(Arg *argNow, Args *argList)
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

MimiObj *removeMethodInfo(MimiObj *thisClass)
{
    args_foreach(thisClass->attributeList, removeEachMethodInfo, thisClass->attributeList);
    return thisClass;
}

MimiObj *obj_getClassObjByNewFun(MimiObj *context, char *name, void *(*newClassFun)(Args *initArgs))
{
    Args *initArgs = New_args(NULL);
    args_setPtr(initArgs, "context", context);
    args_setStr(initArgs, "name", name);
    MimiObj *thisClass = newClassFun(initArgs);
    obj_setPtr(thisClass, "classPtr", newClassFun);
    args_deinit(initArgs);
    return thisClass;
}

char *obj_getClassPath(MimiObj *objHost, Args *buffs, char *objName)
{
    Arg *objArg = obj_getArg(objHost, objName);
    char *objType = arg_getType(objArg);
    char *classPath = strsRemovePrefix(buffs, objType, "_class-");
    return classPath;
}

void *getNewObjFunByClass(MimiObj *obj, char *classPath)
{
    MimiObj *classHost = args_getPtr(obj->attributeList, "classLoader");
    if (NULL == classHost)
    {
        return NULL;
    }
    void *(*newObjFun)(Args * initArgs) = args_getPtr(classHost->attributeList, classPath);
    return newObjFun;
}

void *getNewClassObjFunByName(MimiObj *obj, char *name)
{
    Args *buffs = New_strBuff();
    char *classPath = strsAppend(buffs, "[mate]", name);
    /* init the subprocess */
    void *(*newClassObjFun)(Args * initArgs) = args_getPtr(obj->attributeList, classPath);
    args_deinit(buffs);
    return newClassObjFun;
}

MimiObj *newRootObj(char *name, void *newObjFun)
{
    MimiObj *thisClass = obj_getClassObjByNewFun(NULL, name, newObjFun);
    MimiObj *newObj = removeMethodInfo(thisClass);
    return newObj;
}

static void removeClassLoader(MimiObj *obj)
{
    MimiObj *classObj = args_getPtr(obj->attributeList, "classLoader");
    if (NULL != classObj)
    {
        obj_deinit(classObj);
        args_removeArg(obj->attributeList, "classLoader");
    }
}

MimiObj *initObj(MimiObj *obj, char *name)
{
    MimiObj *res = NULL;
    void *(*newObjFun)(Args * initArgs) = getNewClassObjFunByName(obj, name);
    Args *buffs = New_args(NULL);
    if (NULL == newObjFun)
    {
        /* no such object */
        res = NULL;
        goto exit;
    }
    MimiObj *thisClass = obj_getClassObjByNewFun(obj, name, newObjFun);
    MimiObj *newObj = removeMethodInfo(thisClass);
    /* delete [mate]<objName> */
    obj_removeArg(obj, strsAppend(buffs, "[mate]", name));
    /* delete "classLoader" object */
    removeClassLoader(newObj);

    char *type = args_getType(obj->attributeList, name);
    args_setPtrWithType(obj->attributeList, name, type, newObj);
    res = obj_getPtr(obj, name);
    goto exit;
exit:
    args_deinit(buffs);
    return res;
}

MimiObj *obj_getObjDirect(MimiObj *self, char *name)
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

MimiObj *obj_getObj(MimiObj *self, char *objPath, int keepDeepth)
{
    Args *buffs = New_strBuff();
    char *objPathBuff = strsCopy(buffs, objPath);
    int tokenNum = strGetTokenNum(objPath, '.');
    MimiObj *obj = self;
    for (int i = 0; i < tokenNum - keepDeepth; i++)
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

void loadMethodInfo(MimiObj *methodHost, char *methodName, char *methodDeclearation, void *methodPtr)
{
    Args *buffs = New_strBuff();
    char *methodPtrPath = strsAppend(buffs, "[methodPtr]", methodName);
    char *methodDeclearationPath = strsAppend(buffs, "[methodDec]", methodName);
    obj_setPtr(methodHost, methodPtrPath, methodPtr);
    obj_setStr(methodHost, methodDeclearationPath, methodDeclearation);
    args_deinit(buffs);
}

static char *getMethodDeclearation(MimiObj *obj, char *methodName)
{
    Args *buffs = New_strBuff();
    char *methodDeclearationPath = strsAppend(buffs, "[methodDec]", methodName);
    char *res = obj_getStr(obj, methodDeclearationPath);
    args_deinit(buffs);
    return res;
}

static void *getMethodPtr(MimiObj *methodHost, char *methodName)
{
    Args *buffs = New_strBuff();
    char *methodPtrPath = strsAppend(buffs, "[methodPtr]", methodName);
    void *res = obj_getPtr(methodHost, methodPtrPath);
    args_deinit(buffs);
    return res;
}

int class_defineMethod(MimiObj *self,
                       char *declearation,
                       void (*methodPtr)(MimiObj *self, Args *args))
{
    int size = strGetSize(declearation);
    int res = 0;
    Args *buffs = New_strBuff();
    char *cleanDeclearation = strDeleteChar(args_getBuff(buffs, size), declearation, ' ');
    char *methodPath = strGetFirstToken(args_getBuff(buffs, size), cleanDeclearation, '(');

    MimiObj *methodHost = obj_getObj(self, methodPath, 1);
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

static int loadArgByType(MimiObj *self,
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
    if (strEqu(definedType, "string"))
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
        /* solve the int type */
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
        int referenceVal = obj_getInt(self, argPath);
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

static Args *getArgsBySort(MimiObj *self, char *typeList, char *argList)
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

static Args *getArgsByNameMatch(MimiObj *self, char *typeList, char *argList)
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

static Args *getArgsBySentence(MimiObj *self, char *typeList, char *argList)
{
    if (strIsContain(argList, '='))
    {
        return getArgsByNameMatch(self, typeList, argList);
    }
    return getArgsBySort(self, typeList, argList);
}

static void transferReturnVal(MimiObj *self, char *returnType, char *returnName, Args *args)
{
    if (strEqu("->int", returnType))
    {
        obj_setInt(self, returnName, args_getInt(args, "return"));
    }
    if (strEqu("->float", returnType))
    {
        obj_setFloat(self, returnName, args_getFloat(args, "return"));
    }
    if (strEqu("->string", returnType))
    {
        obj_setStr(self, returnName, args_getStr(args, "return"));
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
    int size = strGetSize(cmd);
    char *strOut = args_getBuff(buffs, size);
    int iOut = 0;
    char delChar = ' ';
    int isInStr = 0;
    for (int i = 0; i < strGetSize(cmd); i++)
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

Args *obj_runDirect(MimiObj *self, char *cmd)
{
    /* the Args returned need to be deinit */
    Args *res = New_args(NULL);
    args_setInt(res, "errCode", 0);
    Args *buffs = New_strBuff();
    char *cleanCmd = getCleanCmd(buffs, cmd);
    char *methodToken = strsGetFirstToken(buffs, cleanCmd, '(');
    char *methodPath = getMethodPath(buffs, methodToken);

    MimiObj *methodHostObj = obj_getObj(self, methodPath, 1);
    MimiObj *methodHostClass = NULL;
    if (NULL == methodHostObj)
    {
        /* error, not found object */
        args_setInt(res, "errCode", 1);
        method_sysOut(res, "[error] runner: object no found.");
        goto exit;
    }
    char *methodName = strsGetLastToken(buffs, methodPath, '.');

    void *classPtr = obj_getPtr(methodHostObj, "classPtr");
    char *methodHostClassName = strsAppend(buffs, "classObj-", methodHostObj->name);
    methodHostClass = obj_getClassObjByNewFun(methodHostObj, methodHostClassName, classPtr);
    /* get method Ptr */
    void (*methodPtr)(MimiObj * self, Args * args) = getMethodPtr(methodHostClass, methodName);
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
    Args *args = getArgsBySentence(self, typeList, argList);
    if (NULL == args)
    {
        /* get args faild */
        args_setInt(res, "errCode", 5);
        method_sysOut(res, "[error] runner: solve arg faild.");
        goto exit;
    }
    /* transfer return */
    Args *returnBuffs = NULL;
    char *returnName = NULL;
    if (strIsContain(methodToken, '='))
    {
        returnBuffs = New_args(NULL);
        returnName = strsGetFirstToken(returnBuffs, methodToken, '=');
    }
    /* run method */
    args_deinit(buffs);
    buffs = NULL;
    methodPtr(methodHostObj, args);
    /* transfer return */
    if (NULL != returnBuffs)
    {
        transferReturnVal(self, returnType, returnName, args);
        args_deinit(returnBuffs);
    }
    /* transfer sysOut */
    char *sysOut = args_getStr(args, "sysOut");
    if (NULL != sysOut)
    {
        args_setStr(res, "sysOut", args_getStr(args, "sysOut"));
    }
    args_deinit(args);
    /* solve errCode */
    if (!args_isArgExist(args, "errCode"))
    {
        goto exit;
    }
    int errCode = args_getInt(args, "errCode");
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
    return res;
}

int obj_removeArg(MimiObj *self, char *argPath)
{
    MimiObj *objHost = obj_getObj(self, argPath, 1);
    MimiObj *obj = obj_getObj(self, argPath, 0);
    if (NULL != obj)
    {
        obj_deinit(obj);
    }
    Args *buffs = New_strBuff();
    int err = 0;
    if (NULL == objHost)
    {
        /* [error] object no found */
        err = 1;
        goto exit;
    }
    char *argName = strsGetLastToken(buffs, argPath, '.');
    int res = args_removeArg(objHost->attributeList, argName);
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

int obj_isArgExist(MimiObj *self, char *argPath)
{
    MimiObj *obj = obj_getObj(self, argPath, 1);
    Args *buffs = New_strBuff();
    int res = 0;
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

void obj_runNoRes(MimiObj *slef, char *cmd)
{
    /* unsafe, nothing would happend when error occord */
    args_deinit(obj_runDirect(slef, cmd));
}

void obj_run(MimiObj *self, char *cmd)
{
    /* safe, stop when error occord and error info would be print */
    Args *res = obj_runDirect(self, cmd);
    char *sysOut = args_getStr(res, "sysOut");
    if (NULL != sysOut)
    {
        // printf("%s\r\n", sysOut);
    }
    if (0 != args_getInt(res, "errCode"))
    {
        // printf("[info] input commond: %s\r\n", cmd);
        while (1)
            ;
    }
    if (NULL != res)
    {
        args_deinit(res);
    }
}

MimiObj *New_TinyObj(Args *args)
{
    /* request memory */
    DMEM *mem = DynMemGet(sizeof(MimiObj));
    if (NULL == mem)
    {
        // printf("[error] memory is empty!");
        while (1)
            ;
    }
    MimiObj *self = (void *)(mem->addr);
    self->mem = mem;

    /* List */
    self->attributeList = New_args(NULL);

    /* override */
    self->_updateHandle = _UpdateHandle;
    self->_beforDinit = _beforDinit;

    /* attribute */
    obj_setPtr(self, "context", self);
    obj_setStr(self, "name", "root");

    /* load */
    if (NULL != args)
    {
        obj_load(self, args, "name");
        obj_load(self, args, "context");
    }

    /* hard attribute */
    self->name = obj_getStr(self, "name");
    return self;
}
