/*
  Author: lyon
  Tencent QQ: 645275593
*/

#include "PikaObj.h"
#include <stdarg.h>
#include "BaseObj.h"
#include "PikaVM.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataString.h"
#include "dataStrs.h"

int32_t deinitEachSubObj(Arg* argEach, Args* handleArgs) {
    if (NULL != handleArgs) {
        /* error: tOhis handle not need handle args */
        return 1;
    }
    char* type = arg_getType(argEach);
    /* deinit sub object */
    if (strEqu(type, "c")) {
        PikaObj* subObj = arg_getPtr(argEach);
        obj_deinit(subObj);
    }
    return 0;
}

void deinitAllSubObj(PikaObj* self) {
    Args* args = self->list;
    args_foreach(args, deinitEachSubObj, NULL);
}

int32_t obj_deinit(PikaObj* self) {
    deinitAllSubObj(self);
    args_deinit(self->list);
    // DynMemPut(self->mem);
    pikaFree(self, sizeof(PikaObj));
    self = NULL;
    return 0;
}

int32_t obj_enable(PikaObj* self) {
    obj_setInt(self, "isEnable", 1);
    return 0;
}

int32_t obj_disable(PikaObj* self) {
    obj_setInt(self, "isEnable", 0);
    return 0;
}

int32_t obj_setInt(PikaObj* self, char* argPath, int64_t val) {
    PikaObj* obj = obj_getObj(self, argPath, 1);
    if (NULL == obj) {
        /* [error] object no found */
        return 1;
    }
    Args* buffs = New_strBuff();
    char* name = strsGetLastToken(buffs, argPath, '.');
    args_setInt(obj->list, name, val);
    args_deinit(buffs);
    return 0;
}

int32_t obj_setPtr(PikaObj* self, char* argPath, void* pointer) {
    PikaObj* obj = obj_getObj(self, argPath, 1);
    if (NULL == obj) {
        return 1;
    }
    Args* buffs = New_strBuff();
    char* name = strsGetLastToken(buffs, argPath, '.');
    args_setPtr(obj->list, name, pointer);
    args_deinit(buffs);
    return 0;
}

int32_t obj_setFloat(PikaObj* self, char* argPath, float value) {
    PikaObj* obj = obj_getObj(self, argPath, 1);
    if (NULL == obj) {
        return 1;
    }
    Args* buffs = New_strBuff();
    char* name = strsGetLastToken(buffs, argPath, '.');
    args_setFloat(obj->list, name, value);
    args_deinit(buffs);
    return 0;
}

int32_t obj_setStr(PikaObj* self, char* argPath, char* str) {
    PikaObj* obj = obj_getObj(self, argPath, 1);
    if (NULL == obj) {
        return 1;
    }
    Args* buffs = New_strBuff();
    char* name = strsGetLastToken(buffs, argPath, '.');
    args_setStr(obj->list, name, str);
    args_deinit(buffs);
    return 0;
}

int64_t obj_getInt(PikaObj* self, char* argPath) {
    PikaObj* obj = obj_getObj(self, argPath, 1);
    if (NULL == obj) {
        return -999999999;
    }
    Args* buffs = New_strBuff();
    char* argName = strsGetLastToken(buffs, argPath, '.');
    int res = args_getInt(obj->list, argName);
    args_deinit(buffs);
    return res;
}

Arg* obj_getArg(PikaObj* self, char* argPath) {
    PikaObj* obj = obj_getObj(self, argPath, 1);
    if (NULL == obj) {
        return NULL;
    }
    Args* buffs = New_strBuff();
    char* argName = strsGetLastToken(buffs, argPath, '.');
    Arg* res = args_getArg(obj->list, argName);
    args_deinit(buffs);
    return res;
}

int32_t obj_setArg(PikaObj* self, char* argPath, Arg* arg) {
    /* setArg would copy arg */
    PikaObj* obj = obj_getObj(self, argPath, 1);
    if (NULL == obj) {
        /* object no found */
        return 1;
    }
    args_copyArg(obj->list, arg);
    return 0;
}

void* obj_getPtr(PikaObj* self, char* argPath) {
    PikaObj* obj = obj_getObj(self, argPath, 1);
    if (NULL == obj) {
        return NULL;
    }
    Args* buffs = New_strBuff();
    char* argName = strsGetLastToken(buffs, argPath, '.');
    void* res = args_getPtr(obj->list, argName);
    args_deinit(buffs);
    return res;
}

float obj_getFloat(PikaObj* self, char* argPath) {
    PikaObj* obj = obj_getObj(self, argPath, 1);
    if (NULL == obj) {
        return -999.999;
    }
    Args* buffs = New_strBuff();
    char* argName = strsGetLastToken(buffs, argPath, '.');
    float res = args_getFloat(obj->list, argName);
    args_deinit(buffs);
    return res;
}

char* obj_getStr(PikaObj* self, char* argPath) {
    PikaObj* obj = obj_getObj(self, argPath, 1);
    if (NULL == obj) {
        return NULL;
    }
    Args* buffs = New_strBuff();
    char* argName = strsGetLastToken(buffs, argPath, '.');
    char* res = args_getStr(obj->list, argName);
    args_deinit(buffs);
    return res;
}

int32_t obj_load(PikaObj* self, Args* args, char* name) {
    args_copyArgByName(args, name, self->list);
    return 0;
}

int32_t obj_addOther(PikaObj* self, char* subObjectName, void* new_ObjectFun) {
    Args* initArgs = New_args(NULL);
    void* (*new_Object)(Args * initArgs) = (void* (*)(Args*))new_ObjectFun;
    void* subObject = new_Object(initArgs);
    obj_setPtr(self, subObjectName, subObject);
    args_deinit(initArgs);
    return 0;
}

int32_t obj_freeObj(PikaObj* self, char* objPath) {
    PikaObj* obj = obj_getPtr(self, objPath);
    obj_deinit(obj);
    return 0;
}

int32_t obj_bind(PikaObj* self, char* type, char* name, void* pointer) {
    args_bind(self->list, type, name, pointer);
    return 0;
}

char* obj_print(PikaObj* self, char* name) {
    if (NULL == self) {
        return NULL;
    }
    return args_print(self->list, name);
}

int32_t obj_bindInt(PikaObj* self, char* name, int32_t* valPtr) {
    args_bindInt(self->list, name, valPtr);
    return 0;
}

int32_t obj_bindFloat(PikaObj* self, char* name, float* valPtr) {
    args_bindFloat(self->list, name, valPtr);
    return 0;
}

int32_t obj_bindString(PikaObj* self, char* name, char** valPtr) {
    args_bindStr(self->list, name, valPtr);
    return 0;
}

PikaObj* obj_getClassObjByNewFun(PikaObj* context,
                                 char* name,
                                 NewFun newClassFun) {
    Args* initArgs = New_args(NULL);
    PikaObj* thisClass = newClassFun(initArgs);
    obj_setPtr(thisClass, "_clsptr", newClassFun);
    args_deinit(initArgs);
    return thisClass;
}

Arg* obj_getMethod(PikaObj* obj, char* methodPath) {
    Arg* method = NULL;
    Args* buffs = New_strBuff();
    char* methodName = strsGetLastToken(buffs, methodPath, '.');
    method = obj_getArg(obj, methodName);
    if (NULL != method) {
        method = arg_copy(method);
        goto exit;
    }
    PikaObj* methodHostClass = obj_getClassObj(obj);
    method = arg_copy(obj_getArg(methodHostClass, methodName));
    obj_deinit(methodHostClass);
exit:
    args_deinit(buffs);
    return method;
}

PikaObj* obj_getClassObj(PikaObj* obj) {
    Args* buffs = New_strBuff();
    void* classPtr = obj_getPtr(obj, "_clsptr");
    char* classObjName = strsAppend(buffs, "_cls-", "");
    PikaObj* classObj = obj_getClassObjByNewFun(obj, classObjName, classPtr);
    args_deinit(buffs);
    return classObj;
}

void* getNewClassObjFunByName(PikaObj* obj, char* name) {
    char* classPath = name;
    /* init the subprocess */
    void* (*newClassObjFun)(Args * initArgs) =
        args_getPtr(obj->list, classPath);
    return newClassObjFun;
}

int32_t __foreach_removeMethodInfo(Arg* argNow, Args* argList) {
    if (strEqu(arg_getType(argNow), "M")) {
        args_removeArg(argList, argNow);
        return 0;
    }
    return 0;
}

PikaObj* removeMethodInfo(PikaObj* thisClass) {
    args_foreach(thisClass->list, __foreach_removeMethodInfo, thisClass->list);
    return thisClass;
}

PikaObj* newRootObj(char* name, NewFun newObjFun) {
    PikaObj* thisClass = obj_getClassObjByNewFun(NULL, name, newObjFun);
    PikaObj* newObj = removeMethodInfo(thisClass);
    return newObj;
}

PikaObj* initObj(PikaObj* obj, char* name) {
    PikaObj* res = NULL;
    NewFun newObjFun = getNewClassObjFunByName(obj, name);
    Args* buffs = New_args(NULL);
    if (NULL == newObjFun) {
        /* no such object */
        res = NULL;
        goto exit;
    }
    PikaObj* thisClass = obj_getClassObjByNewFun(obj, name, newObjFun);
    PikaObj* newObj = removeMethodInfo(thisClass);

    char* mateObjType = args_getType(obj->list, name);
    char* pureType = strsGetLastToken(buffs, mateObjType, ']');
    char* objType = strsAppend(buffs, "c", pureType);
    args_setPtrWithType(obj->list, name, "c", newObj);
    res = obj_getPtr(obj, name);
    goto exit;
exit:
    args_deinit(buffs);
    return res;
}

PikaObj* obj_getObjDirect(PikaObj* self, char* name) {
    if (NULL == self) {
        return NULL;
    }
    /* finded object, check type*/
    char* type = args_getType(self->list, name);
    /* found mate Object */
    if (strEqu(type, "m")) {
        return initObj(self, name);
    }
    /* found Objcet */
    if (strEqu(type, "c")) {
        return obj_getPtr(self, name);
    }
    return NULL;
}

PikaObj* obj_getObj(PikaObj* self, char* objPath, int32_t keepDeepth) {
    Args* buffs = New_strBuff();
    char* objPathBuff = strsCopy(buffs, objPath);
    int32_t tokenNum = strGetTokenNum(objPath, '.');
    PikaObj* obj = self;
    for (int32_t i = 0; i < tokenNum - keepDeepth; i++) {
        char* token = strsPopToken(buffs, objPathBuff, '.');
        obj = obj_getObjDirect(obj, token);
        if (obj == NULL) {
            goto exit;
        }
    }
    goto exit;
exit:
    args_deinit(buffs);
    return obj;
}

void* methodArg_getPtr(Arg* method_arg) {
    uint32_t size_ptr = sizeof(void*);
    void* info = arg_getContent(method_arg);
    void* ptr = NULL;
    memcpy(&ptr, info, size_ptr);
    return ptr;
}

char* methodArg_getDec(Arg* method_arg) {
    uint32_t size_ptr = sizeof(void*);
    void* info = arg_getContent(method_arg);
    return info + size_ptr;
}

void obj_saveMethodInfo(PikaObj* self,
                        char* method_name,
                        char* method_dec,
                        void* method_ptr) {
    Args* buffs = New_strBuff();
    char* pars = strsRemovePrefix(buffs, method_dec, method_name);
    Arg* arg = New_arg(NULL);
    uint32_t size_ptr = sizeof(void*);
    uint32_t size_pars = strGetSize(pars);
    uint32_t size_info = size_ptr + size_pars + 1;
    void* info = args_getBuff(buffs, size_info);
    memcpy(info, &method_ptr, size_ptr);
    /* +1 to add \0 */
    memcpy(info + size_ptr, pars, size_pars + 1);
    arg = arg_setName(arg, method_name);
    arg = arg_setType(arg, "M");
    arg = arg_setContent(arg, info, size_info);

    args_setArg(self->list, arg);
    args_deinit(buffs);
}

int32_t class_defineMethod(PikaObj* self,
                           char* declearation,
                           void (*methodPtr)(PikaObj* self, Args* args)) {
    int32_t size = strGetSize(declearation);
    int32_t res = 0;
    Args* buffs = New_strBuff();
    char* cleanDeclearation =
        strDeleteChar(args_getBuff(buffs, size), declearation, ' ');
    char* methodPath =
        strGetFirstToken(args_getBuff(buffs, size), cleanDeclearation, '(');

    PikaObj* methodHost = obj_getObj(self, methodPath, 1);
    if (NULL == methodHost) {
        /* no found method object */
        res = 1;
        goto exit;
    }
    char* methodName = strsGetLastToken(buffs, methodPath, '.');

    obj_saveMethodInfo(methodHost, methodName, cleanDeclearation, methodPtr);
    res = 0;
    goto exit;
exit:
    args_deinit(buffs);
    return res;
}

PIKA_WEAK int __runExtern_contral(PikaObj* self, char* cmd) {
    return 0;
}

Parameters* obj_runDirect(PikaObj* self, char* cmd) {
    Parameters* globals = NULL;

    globals = pikaVM_run(self, cmd);
    goto exit;

exit:
    return globals;
}

int32_t obj_removeArg(PikaObj* self, char* argPath) {
    PikaObj* objHost = obj_getObj(self, argPath, 1);
    PikaObj* obj = obj_getObj(self, argPath, 0);
    Args* buffs = New_strBuff();
    if (NULL != obj) {
        obj_deinit(obj);
    }
    int32_t err = 0;
    if (NULL == objHost) {
        /* [error] object no found */
        err = 1;
        goto exit;
    }
    char* argName = strsGetLastToken(buffs, argPath, '.');
    int32_t res =
        args_removeArg(objHost->list, args_getArg(objHost->list, argName));
    if (1 == res) {
        /*[error] not found arg*/
        err = 2;
        goto exit;
    }
    goto exit;
exit:
    args_deinit(buffs);
    return err;
}

int32_t obj_isArgExist(PikaObj* self, char* argPath) {
    PikaObj* obj = obj_getObj(self, argPath, 1);
    Args* buffs = New_strBuff();
    int32_t res = 0;
    if (NULL == obj) {
        /* [error] object no found */
        res = 1;
        goto exit;
    }
    char* argName = strsGetLastToken(buffs, argPath, '.');
    Arg* arg = args_getArg(obj->list, argName);
    if (NULL == arg) {
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

void obj_runNoRes(PikaObj* slef, char* cmd) {
    /* unsafe, nothing would happend when error occord */
    obj_deinit(obj_runDirect(slef, cmd));
}

void obj_run(PikaObj* self, char* cmd) {
    /* safe, stop when error occord and error info would be print32_t */
    Parameters* globals = obj_runDirect(self, cmd);
    obj_deinit(globals);
}

void obj_setErrorCode(PikaObj* self, int32_t errCode) {
    obj_setInt(self, "__errCode", errCode);
}

int32_t obj_getErrorCode(PikaObj* self) {
    if (!obj_isArgExist(self, "__errCode")) {
        return 0;
    }
    return obj_getInt(self, "__errCode");
}

void args_setErrorCode(Args* args, int32_t errCode) {
    args_setInt(args, "__errCode", errCode);
}

int32_t args_getErrorCode(Args* args) {
    if (!args_isArgExist(args, "__errCode")) {
        return 0;
    }
    return args_getInt(args, "__errCode");
}

void obj_setSysOut(PikaObj* self, char* str) {
    obj_setStr(self, "__sysOut", str);
}

char* obj_getSysOut(PikaObj* self) {
    return obj_getStr(self, "__sysOut");
}

char* args_getSysOut(Args* args) {
    return args_getStr(args, "__sysOut");
}

void args_setSysOut(Args* args, char* str) {
    args_setStr(args, "__sysOut", str);
}

void obj_sysPrintf(PikaObj* self, char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char sysOut[128] = {0};
    vsprintf(sysOut, fmt, args);
    obj_setSysOut(self, sysOut);
    va_end(args);
}

void method_returnStr(Args* args, char* val) {
    args_setStr(args, "return", val);
}
void method_returnInt(Args* args, int32_t val) {
    args_setInt(args, "return", val);
}
void method_returnFloat(Args* args, float val) {
    args_setFloat(args, "return", val);
}
void method_returnPtr(Args* args, void* val) {
    args_setPtr(args, "return", val);
}
void method_returnArg(Args* args, Arg* arg) {
    arg = arg_setName(arg, "return");
    args_setArg(args, arg);
}

int32_t method_getInt(Args* args, char* argName) {
    return args_getInt(args, argName);
}
float method_getFloat(Args* args, char* argName) {
    return args_getFloat(args, argName);
}
char* method_getStr(Args* args, char* argName) {
    return args_getStr(args, argName);
}

PikaObj* New_PikaObj(void) {
    PikaObj* self = pikaMalloc(sizeof(PikaObj));
    /* List */
    self->list = New_args(NULL);
    return self;
}