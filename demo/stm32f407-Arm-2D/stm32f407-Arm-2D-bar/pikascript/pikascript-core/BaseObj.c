#include "MimiObj.h"
#include "TinyObj.h"
#include "BaseObj.h"
#include "dataMemory.h"
#include "dataString.h"
#include "dataStrs.h"

static void *getClassPtr(MimiObj *classObj, char *classPath)
{
    char *ptrPath = classPath;
    return obj_getPtr(classObj, ptrPath);
}

int32_t sysObj_setObjbyClassAndPtr(MimiObj *self, char *objName, char *className, void *newFunPtr)
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

int32_t sysObj_setObjbyClass(MimiObj *self, char *objName, char *classPath)
{
    /* class means subprocess init */
    Args *buffs = New_strBuff();
    MimiObj *classHost = obj_getObj(self, "__classLoader", 0);
    void *newFunPtr = getClassPtr(classHost, classPath);

    /* class means subprocess init */
    char *mataObjName = strsAppend(buffs, "[mate]", objName);
    obj_setPtr(self, mataObjName, newFunPtr);
    /* add void process Ptr, no inited */
    args_setObjectWithClass(self->attributeList, objName, classPath, NULL);

    args_deinit(buffs);
    return 0;
}

static int32_t storeClassInfo(MimiObj *self, Args *buffs, char *classPath, void *classPtr)
{
    int32_t res = 0;
    MimiObj *classHost = obj_getObj(self, classPath, 1);
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

int32_t obj_import(MimiObj *self, char *className, void *classPtr)
{
    MimiObj *classLoader = obj_getObj(self, "__classLoader", 0);
    Args *buffs = New_args(NULL);
    int32_t res = storeClassInfo(classLoader, buffs, className, classPtr);
    args_deinit(buffs);
    return res;
}

int32_t obj_newObj(MimiObj *self, char *objPath, char *classPath)
{
    MimiObj *classLoader = obj_getObj(self, "__classLoader", 0);
    Args *buffs = New_args(NULL);
    void *NewObjPtr = getClassPtr(classLoader, classPath);
    if (NULL == NewObjPtr)
    {
        return 1;
        args_deinit(buffs);
    }
    MimiObj *objHost = obj_getObj(self, objPath, 1);
    char *objName = strsGetLastToken(buffs, objPath, '.');
    sysObj_setObjbyClassAndPtr(objHost, objName, classPath, NewObjPtr);
    args_deinit(buffs);
    return 0;
}

static void init_baseObj(MimiObj *self, Args *args)
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

MimiObj *New_BaseObj(Args *args)
{
    MimiObj *self = New_TinyObj(args);
    init_baseObj(self, args);
    return self;
}
