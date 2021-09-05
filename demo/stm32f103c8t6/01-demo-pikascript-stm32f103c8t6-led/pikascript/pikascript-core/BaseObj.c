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

int32_t sysObj_setObjbyClassAndPtr(PikaObj *self, char *objName, char *className, void *newFunPtr)
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
    PikaObj *classHost = obj_getObj(self, "__classLoader", 0);
    void *newFunPtr = getClassPtr(classHost, classPath);

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
    sysObj_setObjbyClassAndPtr(objHost, objName, classPath, NewObjPtr);
    res = 0;
    goto exit;

    exit:
    args_deinit(buffs);
    return res;
}

static void init_baseObj(PikaObj *self, Args *args)
{
    /* attribute */
    /* object */
    obj_setObjWithoutClass(self, "__classLoader", New_TinyObj);
    /* 
        init classLoader now, in order to the 
        find it after inited the self object.
    */
    obj_getObj(self, "__classLoader", 0);

    /* operation */

    /* object */

    /* override */
}

PikaObj *New_BaseObj(Args *args)
{
    PikaObj *self = New_TinyObj(args);
    init_baseObj(self, args);
    return self;
}
