#include "dataArgs.h"
#include "dataStrs.h"
#include "dataLink.h"
#include "dataMemory.h"
#include "dataString.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

void args_deinit(Args *self)
{
    // DynMemPut(self->mem);
    link_deinit(self->argLinkList);
    pikaFree(self, self->memSize);
    self = NULL;
}

char *args_getStrByIndex(Args *self, int32_t index)
{
    Arg *arg = args_getArgByIndex(self, index);
    if (NULL == arg)
    {
        return NULL;
    }
    return (char *)arg_getcontent(arg);
}

int32_t args_setStrWithDefaultName(Args *self, char *strIn)
{
    Args *buffs = New_strBuff();
    char *name = strsFormat(buffs, "arg%d", (int)self->argLinkList->TopId);
    args_setStr(self, name, strIn);
    args_deinit(buffs);
    return 0;
}

int32_t args_setFloatWithDefaultName(Args *self, float argFloat)
{
    Args *buffs = New_strBuff();
    char *name = strsFormat(buffs, "arg%d", (int)self->argLinkList->TopId);
    args_setFloat(self, name, argFloat);
    args_deinit(buffs);
    return 0;
}

int32_t args_setFloat(Args *self, char *name, float argFloat)
{
    Arg *argNew = New_arg(NULL);
    arg_setType(argNew, "float");
    arg_setName(argNew, name);
    arg_setFloat(argNew, argFloat);
    args_setArg(self, argNew);
    return 0;
}

float args_getFloatByIndex(Args *self, int32_t index)
{
    float val = 0;
    Arg *arg = args_getArgByIndex(self, index);
    val = arg_getFloat(arg);
    return val;
}

void *args_getPointerByIndex(Args *self, int32_t index)
{
    void *pointer = NULL;
    Arg *arg = args_getArgByIndex(self, index);
    pointer = arg_getPtr(arg);
    return pointer;
}

void *args_getPtr(Args *self, char *name)
{
    void *pointer = NULL;
    Arg *arg = args_getArg(self, name);
    if (NULL == arg)
    {
        return NULL;
    }

    pointer = arg_getPtr(arg);
    return pointer;
}

int32_t args_setPtr(Args *self, char *name, void *argPointer)
{
    int32_t errCode = 0;
    Arg *argNew = New_arg(NULL);
    arg_setType(argNew, "pointer");
    arg_setName(argNew, name);
    arg_setPtr(argNew, argPointer);
    args_setArg(self, argNew);
    return errCode;
}

int32_t args_setStr(Args *self, char *name, char *strIn)
{
    int32_t errCode = 0;
    Arg *argNew = New_arg(NULL);
    arg_setType(argNew, "str");
    arg_setStr(argNew, strIn);
    arg_setName(argNew, name);
    args_setArg(self, argNew);
    return errCode;
}

void setArgDirect(Args *self, Arg *arg)
{
    link_addNode(self->argLinkList,
                 arg,
                 (contentDeinitFun)arg_deinit);
}

char *args_getBuff(Args *self, int32_t size)
{
    Arg *argNew = New_arg(NULL);
    arg_newcontent(argNew, size + 1);
    setArgDirect(self, argNew);
    return (char *)arg_getcontent(argNew);
}

char *args_getStr(Args *self, char *name)
{
    Arg *arg = args_getArg(self, name);
    if (NULL == arg)
    {
        return NULL;
    }
    if (NULL == arg_getcontent(arg))
    {
        return NULL;
    }
    return (char *)arg_getcontent(arg);
}

int32_t args_setInt(Args *self, char *name, int64_t int64In)
{
    Arg *argNew = New_arg(NULL);
    arg_setName(argNew, name);
    arg_setInt(argNew, int64In);
    arg_setType(argNew, "int");
    args_setArg(self, argNew);
    return 0;
}

int64_t args_getIntByIndex(Args *self, int32_t index)
{
    Arg *arg = args_getArgByIndex(self, index);
    if (NULL == arg)
    {
        return -999999999;
    }
    return arg_getInt(arg);
}

int64_t args_getInt(Args *self, char *name)
{
    Arg *arg = args_getArg(self, name);
    if (NULL == arg)
    {
        return -999999999;
    }
    return arg_getInt(arg);
}

int32_t args_getSize(Args *self)
{
    return link_getSize(self->argLinkList);
}

char *args_getType(Args *self, char *name)
{
    Arg *arg = NULL;
    arg = args_getArg(self, name);
    if (NULL == arg)
    {
        return NULL;
    }
    return arg_getType(arg);
}

Arg *args_getArgByIndex(Args *self, int32_t index)
{
    Arg *arg;
    if (index == -1)
    {
        return NULL;
    }
    LinkNode *node = link_getNode(self->argLinkList, index);
    if (NULL == node)
    {
        return NULL;
    }
    arg = node->content;
    return arg;
}

float args_getFloat(Args *self, char *name)
{
    Arg *arg = args_getArg(self, name);
    if (NULL == arg)
    {
        return -999999999.0;
    }
    return arg_getFloat(arg);
}

int32_t args_copyArg(Args *self, Arg *argToBeCopy)
{
    if (NULL == argToBeCopy)
    {
        return 1;
    }
    Arg *argCopied = arg_copy(argToBeCopy);
    args_setArg(self, argCopied);

    return 0;
}

int32_t args_copyArgByName(Args *self, char *name, Args *directArgs)
{
    Arg *argToBeCopy = args_getArg(self, name);
    args_copyArg(directArgs, argToBeCopy);
    return 0;
}

int32_t args_isArgExist(Args *self, char *name)
{
    if (NULL == name)
    {
        return 0;
    }
    if (NULL != args_getArg(self, name))
    {
        return 1;
    }
    return 0;
}

int32_t updateArg(Args *self, Arg *argNew)
{
    // arg New must be a new arg
    Arg *argOld = args_getArg(self, arg_getName(argNew));

    // check type
    if (!strEqu(arg_getType(argOld), arg_getType(argNew)))
    {
        return 1;
        // type do not match
    }
    arg_setcontent(argOld, arg_getcontent(argNew), argNew->contentSize);
    arg_deinit(argNew);
    return 0;
}

int32_t args_setArg(Args *self, Arg *arg)
{
    char *name = arg_getName(arg);
    if (!args_isArgExist(self, name))
    {
        setArgDirect(self, arg);
        return 0;
    }
    updateArg(self, arg);
    return 0;
}

Arg *args_getArg(Args *self, char *name)
{
    LinkNode *nodeNow = self->argLinkList->firstNode;
    if (NULL == nodeNow)
    {
        return NULL;
    }
    while (1)
    {
        Arg *arg = nodeNow->content;
        if (strEqu(name, arg_getName(arg)))
        {
            return arg;
        }
        if (NULL == nodeNow->nextNode)
        {
            return NULL;
        }
        nodeNow = nodeNow->nextNode;
    }
}

void args_bind(Args *self, char *type, char *name, void *pointer)
{
    Args *buffs = New_strBuff();
    char *typeWithBind = strsAppend(buffs, "_bind-", type);
    Arg *argNew = New_arg(NULL);
    arg_setType(argNew, typeWithBind);
    arg_setName(argNew, name);
    arg_setPtr(argNew, pointer);
    args_setArg(self, argNew);
    args_deinit(buffs);
    return;
}

void args_bindInt(Args *self, char *name, int32_t *intPtr)
{
    args_bind(self, "int", name, intPtr);
}

void args_bindFloat(Args *self, char *name, float *floatPtr)
{
    args_bind(self, "float", name, floatPtr);
}

void args_bindStr(Args *self, char *name, char **stringPtr)
{
    args_bind(self, "str", name, stringPtr);
}

char *getPrintSring(Args *self, char *name, char *valString)
{
    Args *buffs = New_strBuff();
    char *printName = strsAppend(buffs, "[printBuff]", name);
    char *printString = strsCopy(buffs, valString);
    args_setStr(self, printName, printString);
    char *res = args_getStr(self, printName);
    args_deinit(buffs);
    return res;
}

char *getPrintStringFromInt(Args *self, char *name, int32_t val)
{
    Args *buffs = New_strBuff();
    char *res = NULL;
    char *valString = strsFormat(buffs, "%d", val);
    res = getPrintSring(self, name, valString);
    args_deinit(buffs);
    return res;
}

char *getPrintStringFromFloat(Args *self, char *name, float val)
{
    Args *buffs = New_strBuff();
    char *res = NULL;
    char *valString = strsFormat(buffs, "%f", val);
    res = getPrintSring(self, name, valString);
    args_deinit(buffs);
    return res;
}

char *getPrintStringFromPtr(Args *self, char *name, void *val)
{
    Args *buffs = New_strBuff();
    char *res = NULL;
    uint64_t intVal = (uint64_t)val;
    char *valString = strsFormat(buffs, "0x%llx", intVal);
    res = getPrintSring(self, name, valString);
    args_deinit(buffs);
    return res;
}

char *args_print(Args *self, char *name)
{
    char *res = NULL;
    char *type = args_getType(self, name);
    Args *buffs = New_strBuff();
    if (NULL == type)
    {
        /* can not get arg */
        res = NULL;
        goto exit;
    }

    if (strEqu(type, "int"))
    {
        int32_t val = args_getInt(self, name);
        res = getPrintStringFromInt(self, name, val);
        goto exit;
    }

    if (strEqu(type, "float"))
    {
        float val = args_getFloat(self, name);
        res = getPrintStringFromFloat(self, name, val);
        goto exit;
    }

    if (strEqu(type, "str"))
    {
        res = args_getStr(self, name);
        goto exit;
    }

    if (strEqu(type, "pointer"))
    {
        void *val = args_getPtr(self, name);
        res = getPrintStringFromPtr(self, name, val);
        goto exit;
    }

    char *bindTypePrefix = strsCopy(self, "_bind-");
    if (strIsStartWith(type, bindTypePrefix))
    {
        char *typeWithoutBind = strsRemovePrefix(buffs, type, bindTypePrefix);
        if (strEqu(typeWithoutBind, "int"))
        {
            int32_t *valPtr = args_getPtr(self, name);
            int32_t val = *valPtr;
            res = getPrintStringFromInt(self, name, val);
            goto exit;
        }
        if (strEqu(typeWithoutBind, "float"))
        {
            float *valPtr = args_getPtr(self, name);
            float val = *valPtr;
            res = getPrintStringFromFloat(self, name, val);
            goto exit;
        }
        if (strEqu(typeWithoutBind, "str"))
        {
            // the value of &string is equal to string it self
            char *string = args_getPtr(self, name);
            res = string;
            goto exit;
        }
    }
    /* can not match type */
    res = NULL;
    goto exit;

exit:
    args_deinit(buffs);
    return res;
}

int32_t args_set(Args *self, char *name, char *valStr)
{
    char *type = args_getType(self, name);
    Args *buffs = New_strBuff();
    int32_t err = 0;

    if (NULL == type)
    {
        /* do not get arg */
        err = 1;
        goto exit;
    }

    if (strEqu("int", type))
    {
        int32_t val = atoi(valStr);
        args_setInt(self, name, val);
        // operation succeed
        err = 0;
        goto exit;
    }
    if (strEqu("float", type))
    {
        float val = atof(valStr);
        args_setFloat(self, name, val);
        // operation succeed
        err = 0;
        goto exit;
    }
    if (strEqu("str", type))
    {
        args_setStr(self, name, valStr);
        // operation succeed
        err = 0;
        goto exit;
    }

    char *bindTypePrefix = strsCopy(self, "_bind-");
    if (strIsStartWith(type, bindTypePrefix))
    {
        char *typeWithoutBind = strsRemovePrefix(buffs, type, bindTypePrefix);
        if (strEqu(typeWithoutBind, "int"))
        {
            int32_t *valPtr = args_getPtr(self, name);
            int32_t val = atoi(valStr);
            *valPtr = val;
            // operation succeed
            err = 0;
            goto exit;
        }
        if (strEqu(typeWithoutBind, "float"))
        {
            float *valPtr = args_getPtr(self, name);
            float val = atof(valStr);
            *valPtr = val;
            // operation succeed
            err = 0;
            goto exit;
        }
        if (strEqu(typeWithoutBind, "str"))
        {
            char *stringBinded = args_getPtr(self, name);
            /* size add 1 to copy the '\0' */
            memcpy(stringBinded, valStr, strGetSize(valStr) + 1);
            // operation succeed
            err = 0;
            goto exit;
        }
    }
    /* type not match */
    err = 2;
    goto exit;
exit:
    args_deinit(buffs);
    return err;
}

int32_t args_setPtrWithType(Args *self, char *objName, char *type, void *objPtr)
{
    Arg *argNew = New_arg(NULL);
    arg_setName(argNew, objName);
    arg_setPtr(argNew, objPtr);
    arg_setType(argNew, type);
    args_setArg(self, argNew);
    return 0;
}

int32_t args_setObjectWithClass(Args *self, char *objName, char *className, void *objPtr)
{
    Args *buffs = New_strBuff();
    char *typeWithClass = strsAppend(buffs, "_class-", className);
    args_setPtrWithType(self, objName, typeWithClass, objPtr);
    args_deinit(buffs);
    return 0;
}

int32_t args_foreach(Args *self, int32_t (*eachHandle)(Arg *argEach, Args *handleArgs), Args *handleArgs)
{
    LinkNode *nodeNow = self->argLinkList->firstNode;
    while (1)
    {
        Arg *argNow = nodeNow->content;
        if (NULL == argNow)
        {
            continue;
        }
        LinkNode *nextNode = nodeNow->nextNode;
        eachHandle(argNow, handleArgs);

        if (NULL == nextNode)
        {
            break;
        }
        nodeNow = nextNode;
    }
    return 0;
}

int32_t args_removeArg(Args *self, char *name)
{
    Arg *argNow = args_getArg(self, name);
    if (NULL == argNow)
    {
        /* can not found arg */
        return 1;
    }
    link_removeNode(self->argLinkList, argNow);
    return 0;
}

void args_init(Args *self, Args *args)
{
    /* attribute */
    self->context = self;
    self->argLinkList = New_link(NULL);

    /* arg */

    /* object */

    /* override */
}

Args *New_args(Args *args)
{
    Args *self = pikaMalloc(sizeof(Args));
    self->memSize = sizeof(Args);
    args_init(self, args);
    return self;
}
