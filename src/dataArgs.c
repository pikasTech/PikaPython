/*
  Author: lyon
  Tencent QQ: 645275593
*/

#include "dataArgs.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataLink.h"
#include "dataMemory.h"
#include "dataString.h"
#include "dataStrs.h"

void args_deinit(Args* self) {
    link_deinit(self);
}

int32_t args_setFloat(Args* self, char* name, float argFloat) {
    Arg* argNew = New_arg(NULL);
    argNew = arg_setFloat(argNew, name, argFloat);
    args_setArg(self, argNew);
    return 0;
}

void* args_getPtr(Args* self, char* name) {
    void* pointer = NULL;
    Arg* arg = args_getArg(self, name);
    if (NULL == arg) {
        return NULL;
    }

    pointer = arg_getPtr(arg);
    return pointer;
}

int32_t args_setPtr(Args* self, char* name, void* argPointer) {
    int32_t errCode = 0;
    Arg* argNew = New_arg(NULL);
    argNew = arg_setPtr(argNew, name, "pointer", argPointer);
    args_setArg(self, argNew);
    return errCode;
}

int32_t args_setStr(Args* self, char* name, char* strIn) {
    int32_t errCode = 0;
    Arg* argNew = New_arg(NULL);
    argNew = arg_setStr(argNew, name, strIn);
    args_setArg(self, argNew);
    return errCode;
}

void setArgDirect(Args* self, Arg* arg) {
    link_addNode(self, arg);
}

char* args_getBuff(Args* self, int32_t size) {
    Arg* argNew = New_arg(NULL);
    argNew = arg_newContent(argNew, size + 1);
    setArgDirect(self, argNew);
    return (char*)arg_getContent(argNew);
}

char* args_getStr(Args* self, char* name) {
    if (NULL == self) {
        return NULL;
    }
    Arg* arg = args_getArg(self, name);
    if (NULL == arg) {
        return NULL;
    }
    if (NULL == arg_getContent(arg)) {
        return NULL;
    }
    return (char*)arg_getContent(arg);
}

int32_t args_setInt(Args* self, char* name, int64_t int64In) {
    Arg* argNew = New_arg(NULL);
    argNew = arg_setInt(argNew, name, int64In);
    args_setArg(self, argNew);
    return 0;
}

int64_t args_getInt(Args* self, char* name) {
    Arg* arg = args_getArg(self, name);
    if (NULL == arg) {
        return -999999999;
    }
    return arg_getInt(arg);
}

int32_t args_getSize(Args* self) {
    return link_getSize(self);
}

char* args_getType(Args* self, char* name) {
    Arg* arg = NULL;
    arg = args_getArg(self, name);
    if (NULL == arg) {
        return NULL;
    }
    return arg_getType(arg);
}

float args_getFloat(Args* self, char* name) {
    Arg* arg = args_getArg(self, name);
    if (NULL == arg) {
        return -999999999.0;
    }
    return arg_getFloat(arg);
}

int32_t args_copyArg(Args* self, Arg* argToBeCopy) {
    if (NULL == argToBeCopy) {
        return 1;
    }
    Arg* argCopied = arg_copy(argToBeCopy);
    args_setArg(self, argCopied);

    return 0;
}

int32_t args_setStruct(Args* self, char *name, void * struct_ptr, uint32_t struct_size){
    if (NULL == struct_ptr) {
        return 1;
    }
    Arg* structArg = arg_setContent(NULL, (uint8_t*)&struct_ptr, struct_size);
    structArg = arg_setName(structArg, name);
    args_setArg(self, structArg);
	return 0;
}

void* args_getStruct(Args* self, char *name){
    return arg_getContent(args_getArg(self, name));
}

int32_t args_copyArgByName(Args* self, char* name, Args* directArgs) {
    Arg* argToBeCopy = args_getArg(self, name);
    args_copyArg(directArgs, argToBeCopy);
    return 0;
}

int32_t args_isArgExist(Args* self, char* name) {
    if (NULL == name) {
        return 0;
    }
    if (NULL != args_getArg(self, name)) {
        return 1;
    }
    return 0;
}

int32_t updateArg(Args* self, Arg* argNew) {
    LinkNode* nodeToUpdate = NULL;
    LinkNode* nodeNow = self->firstNode;
    LinkNode* priorNode = NULL;
    char* name = arg_getName(argNew);
    while (1) {
        if (strEqu(content_getName(nodeNow), name)) {
            nodeToUpdate = nodeNow;
            break;
        }
        if (content_getNext(nodeNow) == NULL) {
            // error, node no found
            goto exit;
        }
        priorNode = nodeNow;
        nodeNow = content_getNext(nodeNow);
    }

    nodeToUpdate = arg_setContent(nodeToUpdate, arg_getContent(argNew),
                                  arg_getContentSize(argNew));

    nodeToUpdate = arg_setType(nodeToUpdate, arg_getType(argNew));
    // update privior link, because arg_getContent would free origin pointer
    if (NULL == priorNode) {
        self->firstNode = nodeToUpdate;
        goto exit;
    }

    content_setNext(priorNode, nodeToUpdate);
    goto exit;
exit:
    arg_deinit(argNew);
    return 0;
}

int32_t args_setArg(Args* self, Arg* arg) {
    char* name = arg_getName(arg);
    if (!args_isArgExist(self, name)) {
        setArgDirect(self, arg);
        return 0;
    }
    updateArg(self, arg);
    return 0;
}

LinkNode* args_getNode(Args* self, char* name) {
    LinkNode* nodeNow = self->firstNode;
    if (NULL == nodeNow) {
        return NULL;
    }
    while (1) {
        Arg* arg = nodeNow;
        char* thisName = arg_getName(arg);
        if (strEqu(name, thisName)) {
            return nodeNow;
        }
        if (NULL == content_getNext(nodeNow)) {
            return NULL;
        }
        nodeNow = content_getNext(nodeNow);
    }
}

Arg* args_getArg(Args* self, char* name) {
    LinkNode* node = args_getNode(self, name);
    if (NULL == node) {
        return NULL;
    }
    return node;
}

void args_bind(Args* self, char* type, char* name, void* pointer) {
    Args* buffs = New_strBuff();
    char* typeWithBind = strsAppend(buffs, "_bind-", type);
    Arg* argNew = New_arg(NULL);
    argNew = arg_setPtr(argNew, name, typeWithBind, pointer);
    args_setArg(self, argNew);
    args_deinit(buffs);
    return;
}

void args_bindInt(Args* self, char* name, int32_t* intPtr) {
    args_bind(self, "int", name, intPtr);
}

void args_bindFloat(Args* self, char* name, float* floatPtr) {
    args_bind(self, "float", name, floatPtr);
}

void args_bindStr(Args* self, char* name, char** stringPtr) {
    args_bind(self, "str", name, stringPtr);
}

char* getPrintSring(Args* self, char* name, char* valString) {
    Args* buffs = New_strBuff();
    char* printName = strsFormat(buffs, 128, "[printBuff]%s", name);
    char* printString = strsCopy(buffs, valString);
    args_setStr(self, printName, printString);
    char* res = args_getStr(self, printName);
    args_deinit(buffs);
    return res;
}

char* getPrintStringFromInt(Args* self, char* name, int32_t val) {
    Args* buffs = New_strBuff();
    char* res = NULL;
    char* valString = strsFormat(buffs, 32, "%d", val);
    res = getPrintSring(self, name, valString);
    args_deinit(buffs);
    return res;
}

char* getPrintStringFromFloat(Args* self, char* name, float val) {
    Args* buffs = New_strBuff();
    char* res = NULL;
    char* valString = strsFormat(buffs, 32, "%f", val);
    res = getPrintSring(self, name, valString);
    args_deinit(buffs);
    return res;
}

char* getPrintStringFromPtr(Args* self, char* name, void* val) {
    Args* buffs = New_strBuff();
    char* res = NULL;
    uint64_t intVal = (uint64_t)val;
    char* valString = strsFormat(buffs, 32, "0x%llx", intVal);
    res = getPrintSring(self, name, valString);
    args_deinit(buffs);
    return res;
}

char* args_print(Args* self, char* name) {
    char* res = NULL;
    char* type = args_getType(self, name);
    Args* buffs = New_strBuff();
    if (NULL == type) {
        /* can not get arg */
        res = NULL;
        goto exit;
    }

    if (strEqu(type, "int")) {
        int32_t val = args_getInt(self, name);
        res = getPrintStringFromInt(self, name, val);
        goto exit;
    }

    if (strEqu(type, "float")) {
        float val = args_getFloat(self, name);
        res = getPrintStringFromFloat(self, name, val);
        goto exit;
    }

    if (strEqu(type, "str")) {
        res = args_getStr(self, name);
        goto exit;
    }

    if (strEqu(type, "pointer")) {
        void* val = args_getPtr(self, name);
        res = getPrintStringFromPtr(self, name, val);
        goto exit;
    }

    char* bindTypePrefix = strsCopy(self, "_bind-");
    if (strIsStartWith(type, bindTypePrefix)) {
        char* typeWithoutBind = strsRemovePrefix(buffs, type, bindTypePrefix);
        if (strEqu(typeWithoutBind, "int")) {
            int32_t* valPtr = args_getPtr(self, name);
            int32_t val = *valPtr;
            res = getPrintStringFromInt(self, name, val);
            goto exit;
        }
        if (strEqu(typeWithoutBind, "float")) {
            float* valPtr = args_getPtr(self, name);
            float val = *valPtr;
            res = getPrintStringFromFloat(self, name, val);
            goto exit;
        }
        if (strEqu(typeWithoutBind, "str")) {
            // the value of &string is equal to string it self
            char* string = args_getPtr(self, name);
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

int32_t args_setPtrWithType(Args* self, char* name, char* type, void* objPtr) {
    Arg* argNew = New_arg(NULL);
    argNew = arg_setPtr(argNew, name, type, objPtr);
    args_setArg(self, argNew);
    return 0;
}

int32_t args_foreach(Args* self,
                     int32_t (*eachHandle)(Arg* argEach, Args* handleArgs),
                     Args* handleArgs) {
    if (NULL == self->firstNode) {
        return 0;
    }
    LinkNode* nodeNow = self->firstNode;
    while (1) {
        Arg* argNow = nodeNow;
        if (NULL == argNow) {
            continue;
        }
        LinkNode* nextNode = content_getNext(nodeNow);
        eachHandle(argNow, handleArgs);

        if (NULL == nextNode) {
            break;
        }
        nodeNow = nextNode;
    }
    return 0;
}

int32_t args_removeArg(Args* self, char* name) {
    Arg* argNow = args_getArg(self, name);
    if (NULL == argNow) {
        /* can not found arg */
        return 1;
    }
    link_removeNode(self, argNow);
    return 0;
}

Args* New_args(Args* args) {
    Args* self = New_link(NULL);
    return self;
}
