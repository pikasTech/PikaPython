/*
  Author: lyon
  Tencent QQ: 645275593
*/

#include <stdarg.h>
#include "PikaObj.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataString.h"
#include "dataStrs.h"

static int32_t loadArgByType(PikaObj* self,
                             char* definedName,
                             char* definedType,
                             char* sourceArgPath,
                             Args* args) {
    if (strEqu(definedType, "any")) {
        if (0 == obj_getAnyArg(self, definedName, sourceArgPath, args)) {
            return 0;
        }
        /* solve arg faild */
        return 3;
    }
    if (strEqu(definedType, "str")) {
        /* solve the string type */
        char* directStr = strsGetDirectStr(args, sourceArgPath);
        if (NULL != directStr) {
            /* direct value */
            args_setStr(args, definedName, directStr);
            /* ok */
            return 0;
        }
        /* reference value */
        char* refStr = obj_getStr(self, sourceArgPath);
        if (NULL == refStr) {
            /* faild */
            return 1;
        }
        args_setStr(args, definedName, refStr);
        /* succeed */
        return 0;
    }
    if (strEqu(definedType, "int")) {
        /* solve the int32_t type */
        args_setInt(args, definedName, 0);
        if ((sourceArgPath[0] >= '0') && (sourceArgPath[0] <= '9')) {
            /* direct value */
            args_set(args, definedName, sourceArgPath);
            /* succeed */
            return 0;
        }
        /* reference value */
        if (!obj_isArgExist(self, sourceArgPath)) {
            /* can not get reference */
            return 3;
        }
        int32_t referenceVal = obj_getInt(self, sourceArgPath);
        args_setInt(args, definedName, referenceVal);
        /* succeed */
        return 0;
    }
    if (strEqu(definedType, "float")) {
        /* solve the float type */
        args_setFloat(args, definedName, 0);
        if ((sourceArgPath[0] >= '0') && (sourceArgPath[0] <= '9')) {
            /* direct value */
            args_set(args, definedName, sourceArgPath);
            /* succeed */
            return 0;
        }
        /* reference value */
        if (!obj_isArgExist(self, sourceArgPath)) {
            /* can not get reference */
            return 3;
        }
        float referenceVal = obj_getFloat(self, sourceArgPath);
        args_setFloat(args, definedName, referenceVal);
        /* succeed */
        return 0;
    }
    if (strEqu(definedType, "pointer")) {
        /* only support reference value */
        if (!obj_isArgExist(self, sourceArgPath)) {
            /* can not get reference */
            return 3;
        }
        void* ptr = obj_getPtr(self, sourceArgPath);
        args_setPtr(args, definedName, ptr);
        return 0;
    }
    /* type match faild */
    return 2;
}

char* getTypeVal(Args* buffs, char* typeToken) {
    if (!strIsContain(typeToken, ':')) {
        return strsCopy(buffs, "");
    }
    return strsGetLastToken(buffs, typeToken, ':');
}

static Args* getArgsByNameMatch(PikaObj* self, char* typeList, char* argList) {
    Args* buffs = New_strBuff();
    char* typeListBuff = strsCopy(buffs, typeList);
    Args* args = New_args(NULL);
    while (1) {
        char* typeToken = strsPopToken(buffs, typeListBuff, ',');
        /* poped all type from typeList */
        if (0 == typeToken[0]) {
            break;
        }

        char* typeName = strsGetFirstToken(buffs, typeToken, ':');
        char* typeVal = getTypeVal(buffs, typeToken);

        char* argListBuff = strsCopy(buffs, argList);
        while (1) {
            char* argToken = strsPopToken(buffs, argListBuff, ',');
            char* argName = strsGetFirstToken(buffs, argToken, '=');
            char* argVal = strsGetLastToken(buffs, argToken, '=');

            if (0 == argToken[0]) {
                /* arg poped finised */
                break;
            }

            if (!strEqu(typeName, argName)) {
                /* name not match */
                continue;
            }

            if (0 != loadArgByType(self, typeName, typeVal, argVal, args)) {
                args_deinit(args);
                args_deinit(buffs);
                return NULL;
            }
        }
    }
    args_deinit(buffs);
    return args;
}

static Args* getArgsBySort(PikaObj* self, char* typeList, char* argList) {
    Args* buffs = New_strBuff();
    char* typeListBuff = strsCopy(buffs, typeList);
    char* argListBuff = strsCopy(buffs, argList);
    Args* args = New_args(NULL);
    while (1) {
        char* typeToken = strsPopToken(buffs, typeListBuff, ',');
        char* argToken = strsPopToken(buffs, argListBuff, ',');
        if ((0 == argToken[0]) || (0 == typeToken[0])) {
            /* arg or type poped finised */
            break;
        }

        char* typeName = strsGetFirstToken(buffs, typeToken, ':');
        char* typeVal = getTypeVal(buffs, typeToken);
        char* argPath = argToken;

        if (0 != loadArgByType(self, typeName, typeVal, argPath, args)) {
            args_deinit(args);
            args_deinit(buffs);
            return NULL;
        }
    }
    args_deinit(buffs);
    return args;
}

static Args* getArgsBySentence(PikaObj* self, char* typeList, char* argList) {
    if (strIsContain(argList, '=')) {
        return getArgsByNameMatch(self, typeList, argList);
    }
    return getArgsBySort(self, typeList, argList);
}

static char* getMethodDeclearation(PikaObj* obj, char* methodName) {
    Args* buffs = New_strBuff();
    char* methodDeclearationPath = strsAppend(buffs, "[md]", methodName);
    char* res = obj_getStr(obj, methodDeclearationPath);
    args_deinit(buffs);
    return res;
}

static void* getMethodPtr(PikaObj* methodHost, char* methodName) {
    Args* buffs = New_strBuff();
    char* methodPtrPath = strsAppend(buffs, "[mp]", methodName);
    void* res = obj_getPtr(methodHost, methodPtrPath);
    args_deinit(buffs);
    return res;
}

Args* obj_invoke(PikaObj* self, char* cmd) {
    /* the Args returned need to be deinit */
    Args* sysRes = New_args(NULL);
    args_setErrorCode(sysRes, 0);
    Args* buffs = New_strBuff();
    char* methodToken = strsGetFirstToken(buffs, cmd, '(');
    char* methodPath = methodToken;
    Args* args = NULL;

    PikaObj* methodHostObj = obj_getObj(self, methodPath, 1);
    PikaObj* methodHostClass = NULL;
    if (NULL == methodHostObj) {
        /* error, not found object */
        args_setErrorCode(sysRes, 1);
        args_setSysOut(sysRes, "[error] runner: object no found.");
        goto exit;
    }
    char* methodName = strsGetLastToken(buffs, methodPath, '.');

    void* classPtr = obj_getPtr(methodHostObj, "_clsptr");
    char* methodHostClassName =
        strsAppend(buffs, "classObj-", obj_getName(methodHostObj));
    methodHostClass =
        obj_getClassObjByNewFun(methodHostObj, methodHostClassName, classPtr);
    /* get method Ptr */
    void (*methodPtr)(PikaObj * self, Args * args) =
        getMethodPtr(methodHostClass, methodName);
    char* methodDecInClass = getMethodDeclearation(methodHostClass, methodName);
    /* assert method*/
    if ((NULL == methodDecInClass) || (NULL == methodPtr)) {
        /* error, method no found */
        args_setErrorCode(sysRes, 2);
        args_setSysOut(sysRes, "[error] runner: method no found.");
        goto exit;
    }
    char* methodDec = strsCopy(buffs, methodDecInClass);
    /* free method host class to save memory */
    obj_deinit(methodHostClass);
    methodHostClass = NULL;

    /* get type list */
    char* typeList = strsCut(buffs, methodDec, '(', ')');
    if (typeList == NULL) {
        /* typeList no found */
        args_setErrorCode(sysRes, 3);
        args_setSysOut(sysRes, "[error] runner: type list no found.");
        goto exit;
    }

    /* get arg list */
    char* argList = strsCut(buffs, cmd, '(', ')');
    {
        if (argList == NULL) {
            /* argL List no found */
            args_setErrorCode(sysRes, 4);
            args_setSysOut(sysRes, "[error] runner: arg list no found.");
            goto exit;
        }
    }

    /* get return type */
    char* returnType = strsGetLastToken(buffs, methodDec, ')');

    /* get args */
    args = getArgsBySentence(self, typeList, argList);
    if (NULL == args) {
        /* get args faild */
        args_setErrorCode(sysRes, 5);
        args_setSysOut(sysRes, "[error] runner: solve arg faild.");
        goto exit;
    }
    obj_setErrorCode(methodHostObj, 0);
    obj_setSysOut(methodHostObj, "");
    /* run method */
    methodPtr(methodHostObj, args);

    /* transfer return type */
    args_setStr(sysRes, "returnType", returnType);
    /* transfer return */
    args_copyArgByName(args, "return", sysRes);
    /* transfer sysOut */
    char* sysOut = obj_getSysOut(methodHostObj);
    if (NULL != sysOut) {
        args_setSysOut(sysRes, sysOut);
    }
    /* transfer errCode */
    if (0 != obj_getErrorCode(methodHostObj)) {
        /* method error */
        args_setErrorCode(sysRes, 6);
    }
    goto exit;
exit:
    if (NULL != buffs) {
        args_deinit(buffs);
    }
    if (NULL != methodHostClass) {
        obj_deinit(methodHostClass);
    }
    if (NULL != args) {
        args_deinit(args);
    }
    return sysRes;
}
