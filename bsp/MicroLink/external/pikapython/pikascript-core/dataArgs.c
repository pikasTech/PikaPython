/*
 * This file is part of the PikaPython project.
 * http://github.com/pikastech/pikapython
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon liang6516@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include "dataArgs.h"
#include "PikaObj.h"
#include "PikaPlatform.h"
#include "dataLink.h"
#include "dataMemory.h"
#include "dataString.h"
#include "dataStrs.h"

void args_deinit_ex(Args* self, pika_bool is_object) {
    pika_assert(self != NULL);
    link_deinit_ex(self, is_object);
}

void args_deinit(Args* self) {
    args_deinit_ex(self, 0);
}

void args_deinit_stack(Args* self) {
    link_deinit_stack(self);
}

PIKA_RES args_setFloat(Args* self, char* name, pika_float argFloat) {
    Arg* argNew = New_arg(NULL);
    argNew = arg_setFloat(argNew, name, argFloat);
    args_setArg(self, argNew);
    return PIKA_RES_OK;
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

PIKA_RES args_setPtrWithType(Args* self, char* name, ArgType type, void* val) {
    Arg* arg = args_getArg(self, name);
    if (NULL == arg) {
        args_pushArg_name(self, name, arg_newPtr(type, val));
        return PIKA_RES_OK;
    }
    void** val_ptr = (void**)arg_getContent(arg);
    *val_ptr = val;
    arg_setType(arg, type);
    return PIKA_RES_OK;
}

PIKA_RES args_setPtr(Args* self, char* name, void* argPointer) {
    return args_setPtrWithType(self, name, ARG_TYPE_POINTER, argPointer);
}

PIKA_RES args_setRef(Args* self, char* name, void* argPointer) {
    PIKA_RES errCode = PIKA_RES_OK;
    Arg* aNewRef = New_arg(NULL);
    aNewRef = arg_setRef(aNewRef, name, argPointer);
    // pikaGC_enable(arg_getPtr(aNewRef));
    args_setArg(self, aNewRef);
    return errCode;
}

PIKA_RES args_setStr(Args* self, char* name, char* strIn) {
    pika_assert(NULL != self);
    PIKA_RES errCode = PIKA_RES_OK;
    Arg* argNew = New_arg(NULL);
    argNew = arg_setStr(argNew, name, strIn);
    if (NULL == argNew) {
        return PIKA_RES_ERR_INVALID_PTR;
    }
    args_setArg(self, argNew);
    return errCode;
}

PIKA_RES args_setNone(Args* self, char* name) {
    PIKA_RES errCode = PIKA_RES_OK;
    Arg* argNew = arg_newNone();
    arg_setName(argNew, name);
    args_setArg(self, argNew);
    return errCode;
}

PIKA_RES args_pushArg(Args* self, Arg* arg) {
    Arg* new_arg = NULL;
    if (!arg_isSerialized(arg)) {
        new_arg = arg_copy(arg);
        arg_deinit(arg);
    } else {
        new_arg = arg;
    }
    link_addNode(self, new_arg);
    return PIKA_RES_OK;
}

PIKA_RES args_pushArg_name(Args* self, char* name, Arg* arg) {
    return args_pushArg(self, arg_setName(arg, name));
}

PIKA_RES args_setBytes(Args* self, char* name, uint8_t* src, size_t size) {
    Arg* argNew = arg_setBytes(NULL, name, src, size);
    args_setArg(self, argNew);
    return PIKA_RES_OK;
}

char* args_getBuff(Args* self, int32_t size) {
    Arg* argNew = New_arg(NULL);
    argNew = arg_newContent(size + 1);
    args_pushArg(self, argNew);
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

uint8_t* args_getBytes(Args* self, char* name) {
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
    return arg_getBytes(arg);
}

size_t args_getBytesSize(Args* self, char* name) {
    if (NULL == self) {
        return 0;
    }
    Arg* arg = args_getArg(self, name);
    if (NULL == arg) {
        return 0;
    }
    if (NULL == arg_getContent(arg)) {
        return 0;
    }
    return arg_getBytesSize(arg);
}

PIKA_RES args_setInt(Args* self, char* name, int64_t val) {
    Arg* arg = args_getArg(self, name);
    if (NULL == arg || arg_getType(arg) != ARG_TYPE_INT) {
        args_pushArg_name(self, name, arg_newInt(val));
        return PIKA_RES_OK;
    }
    int64_t* val_ptr = (int64_t*)arg_getContent(arg);
    *val_ptr = val;
    return PIKA_RES_OK;
}

int64_t args_getInt(Args* self, char* name) {
    Arg* arg = args_getArg(self, name);
    if (NULL == arg) {
        return _PIKA_INT_ERR;
    }
    ArgType arg_type = arg_getType(arg);
    if (arg_type == ARG_TYPE_INT) {
        return arg_getInt(arg);
    } else if (arg_type == ARG_TYPE_FLOAT) {
        return (int64_t)arg_getFloat(arg);
    } else if (arg_type == ARG_TYPE_BOOL) {
        return arg_getBool(arg);
    }
    return _PIKA_INT_ERR;
}

pika_bool args_getBool(Args* self, char* name) {
    Arg* arg = args_getArg(self, name);
    if (NULL == arg) {
        return _PIKA_BOOL_ERR;
    }
    return arg_getBool(arg);
}

int32_t args_getSize(Args* self) {
    pika_assert(NULL != self);
    return link_getSize(self);
}

ArgType args_getType(Args* self, char* name) {
    Arg* arg = NULL;
    arg = args_getArg(self, name);
    if (NULL == arg) {
        return ARG_TYPE_NONE;
    }
    return arg_getType(arg);
}

pika_float args_getFloat(Args* self, char* name) {
    Arg* arg = args_getArg(self, name);
    if (NULL == arg) {
        return _PIKA_FLOAT_ERR;
    }
    ArgType arg_type = arg_getType(arg);
    if (arg_type == ARG_TYPE_FLOAT) {
        return arg_getFloat(arg);
    } else if (arg_type == ARG_TYPE_INT) {
        return (pika_float)arg_getInt(arg);
    }
    return _PIKA_FLOAT_ERR;
}

PIKA_RES args_copyArg(Args* self, Arg* argToBeCopy) {
    if (NULL == argToBeCopy) {
        return PIKA_RES_ERR_INVALID_PTR;
    }
    Arg* argCopied = arg_copy(argToBeCopy);
    args_setArg(self, argCopied);

    return PIKA_RES_OK;
}

PIKA_RES args_setStructWithSize(Args* self,
                                char* name,
                                void* struct_ptr,
                                uint32_t struct_size) {
    Arg* struct_arg = arg_setStruct(NULL, name, struct_ptr, struct_size);
    if (NULL == struct_arg) {
        /* failed */
        return PIKA_RES_ERR_ARG_NO_FOUND;
    }
    args_setArg(self, struct_arg);
    return PIKA_RES_OK;
}

void* args_getStruct(Args* self, char* name) {
    Arg* struct_arg = args_getArg(self, name);
    if (NULL == struct_arg) {
        return NULL;
    }
    return arg_getContent(struct_arg);
}

void* args_getHeapStruct(Args* self, char* name) {
    Arg* struct_arg = args_getArg(self, name);
    if (NULL == struct_arg) {
        return NULL;
    }
    return arg_getHeapStruct(struct_arg);
}

PIKA_RES args_setHeapStructWithSize(Args* self,
                                    char* name,
                                    void* struct_ptr,
                                    uint32_t struct_size,
                                    void* struct_deinit_fun) {
    Arg* struct_arg = arg_setHeapStruct(NULL, name, struct_ptr, struct_size,
                                        struct_deinit_fun);
    if (NULL == struct_arg) {
        /* failed */
        return PIKA_RES_ERR_ARG_NO_FOUND;
    }
    args_setArg(self, struct_arg);
    return PIKA_RES_OK;
}

PIKA_RES args_copyArgByName(Args* self, char* name, Args* directArgs) {
    Arg* argToBeCopy = args_getArg(self, name);
    args_copyArg(directArgs, argToBeCopy);
    return PIKA_RES_OK;
}

int32_t args_isArgExist_hash(Args* self, Hash nameHash) {
    if (NULL != args_getArg_hash(self, nameHash)) {
        return 1;
    }
    return 0;
}

int32_t args_isArgExist(Args* self, char* name) {
    if (NULL != args_getArg(self, name)) {
        return 1;
    }
    return 0;
}

PIKA_RES _updateArg(Args* self, Arg* argNew) {
    pika_assert(NULL != self);
    pika_assert(NULL != argNew);
    LinkNode* nodeToUpdate = NULL;
    LinkNode* nodeNow = self->firstNode;
    LinkNode* priorNode = NULL;
    PIKA_RES res;
    if (NULL == self->firstNode) {
        return PIKA_RES_ERR_ARG_NO_FOUND;
    }
    Hash nameHash = arg_getNameHash(argNew);
    while (1) {
        if (arg_getNameHash((Arg*)nodeNow) == nameHash) {
            nodeToUpdate = nodeNow;
            break;
        }
        if (arg_getNext((Arg*)nodeNow) == NULL) {
            // error, node no found
            return PIKA_RES_ERR_ARG_NO_FOUND;
        }
        priorNode = nodeNow;
        nodeNow = (LinkNode*)arg_getNext((Arg*)nodeNow);
    }

    arg_deinitHeap((Arg*)nodeToUpdate);
    pika_assert(NULL != nodeToUpdate);

    nodeToUpdate = (LinkNode*)arg_copy_content((Arg*)nodeToUpdate, argNew);

    // update privior link, because arg_getContent would free origin pointer
    if (NULL == priorNode) {
        self->firstNode = nodeToUpdate;
        res = PIKA_RES_OK;
        goto __exit;
    }

    arg_setNext((Arg*)priorNode, (Arg*)nodeToUpdate);
    res = PIKA_RES_OK;
    goto __exit;
__exit:
    if (!arg_isSerialized(argNew)) {
        return res;
    }
    arg_freeContent(argNew);
    return res;
}

PIKA_RES args_setArg(Args* self, Arg* arg) {
    pika_assert(NULL != self);
    pika_assert(NULL != arg);
    if (PIKA_RES_OK == _updateArg(self, arg)) {
        return PIKA_RES_OK;
    }
    args_pushArg(self, arg);
    return PIKA_RES_OK;
}

#ifndef __PIKA_CFG_HASH_LIST_CACHE_SIZE
#define __PIKA_CFG_HASH_LIST_CACHE_SIZE 4
#endif

#define __USE_PIKA_HASH_LIST_CACHE 0

LinkNode* args_getNode_hash(Args* self, Hash nameHash) {
    LinkNode* node = self->firstNode;
#if __USE_PIKA_HASH_LIST_CACHE
    int_fast8_t n = 0;
#endif
    while (NULL != node) {
        Arg* arg = (Arg*)node;
        Hash thisNameHash = arg_getNameHash(arg);
#if __USE_PIKA_HASH_LIST_CACHE
        n++;
#endif
        if (thisNameHash == nameHash) {
#if __USE_PIKA_HASH_LIST_CACHE
            if (n > __PIKA_CFG_HASH_LIST_CACHE_SIZE) {
                /* the first __PIKA_CFG_HASH_LIST_CACHE_SIZE items in the list
                 * is considered as a cache.
                 * Don't make __PIKA_CFG_HASH_LIST_CACHE_SIZE too big, otherwise
                 * this optimisation is useless.
                 */

                /*! remove current node from the list */
                node = (LinkNode*)arg_getNext((Arg*)arg);

                /*! move the node to the cache */
                arg_setNext(arg, (Arg*)(self->firstNode));
                self->firstNode = (LinkNode*)arg;
            }
#endif
            return (LinkNode*)arg;
        }
        node = (LinkNode*)arg_getNext((Arg*)node);
    }
    return NULL;
}

LinkNode* args_getNode(Args* self, char* name) {
    return args_getNode_hash(self, hash_time33(name));
}

Arg* args_getArg_hash(Args* self, Hash nameHash) {
    LinkNode* node = args_getNode_hash(self, nameHash);
    if (NULL == node) {
        return NULL;
    }
    return (Arg*)node;
}

Arg* args_getArg(Args* self, char* name) {
    pika_assert(NULL != self);
    LinkNode* node = args_getNode(self, name);
    return (Arg*)node;
}

Arg* args_getArgByIndex(Args* self, int index) {
    pika_assert(NULL != self);
    LinkNode* nodeNow = self->firstNode;
    if (NULL == nodeNow) {
        return NULL;
    }
    for (int i = 0; i < index; i++) {
        nodeNow = (LinkNode*)arg_getNext((Arg*)nodeNow);
    }
    return (Arg*)nodeNow;
}

PIKA_RES args_foreach(Args* self, fn_args_foreach eachHandle, void* context) {
    if (NULL == self->firstNode) {
        return PIKA_RES_OK;
    }
    LinkNode* nodeNow = self->firstNode;
    while (1) {
        Arg* argNow = (Arg*)nodeNow;
        if (NULL == argNow) {
            continue;
        }
        LinkNode* nextNode = (LinkNode*)arg_getNext((Arg*)nodeNow);
        eachHandle(argNow, context);

        if (NULL == nextNode) {
            break;
        }
        nodeNow = nextNode;
    }
    return PIKA_RES_OK;
}

PIKA_RES args_reverse(Args* self) {
    pika_assert(NULL != self);
    LinkNode* nodeNow = self->firstNode;
    LinkNode* nodeNext = NULL;
    LinkNode* nodePrior = NULL;
    while (NULL != nodeNow) {
        nodeNext = (LinkNode*)arg_getNext((Arg*)nodeNow);
        arg_setNext((Arg*)nodeNow, (Arg*)nodePrior);
        nodePrior = nodeNow;
        nodeNow = nodeNext;
    }
    self->firstNode = nodePrior;
    return PIKA_RES_OK;
}

PIKA_RES args_removeArg(Args* self, Arg* argNow) {
    if (NULL == argNow) {
        return PIKA_RES_ERR_INVALID_PTR;
    }
    link_removeNode(self, argNow);
    return PIKA_RES_OK;
}

PIKA_RES args_removeArg_notDeinitArg(Args* self, Arg* argNow) {
    if (NULL == argNow) {
        return PIKA_RES_ERR_INVALID_PTR;
    }
    link_removeNode_notDeinitNode(self, argNow);
    return PIKA_RES_OK;
}

PIKA_RES args_moveArg(Args* self, Args* dict, Arg* argNow) {
    if (NULL == argNow) {
        return PIKA_RES_ERR_INVALID_PTR;
    }
    link_removeNode_notDeinitNode(self, argNow);
    args_pushArg(dict, argNow);
    return PIKA_RES_OK;
}

Args* New_args(Args* args) {
    Args* self = New_link(NULL);
    return self;
}

char* strsFormatArg(Args* out_buffs, char* fmt, Arg* arg) {
    Args buffs = {0};
    char* res = NULL;
    ArgType type = arg_getType(arg);
    const char* syms[] = {"%s", "%r"};
    for (size_t i = 0; i < sizeof(syms) / sizeof(char*); i++) {
        char* sym = (char*)syms[i];
        if (strstr(fmt, sym)) {
            if (type == ARG_TYPE_STRING) {
                fmt = strsReplace(&buffs, fmt, sym, "%s");
                break;
            }
            if (type == ARG_TYPE_INT) {
                fmt = strsReplace(&buffs, fmt, sym, "%d");
                break;
            }
            if (type == ARG_TYPE_FLOAT) {
                fmt = strsReplace(&buffs, fmt, sym, "%f");
                break;
            }
            if (type == ARG_TYPE_POINTER) {
                fmt = strsReplace(&buffs, fmt, sym, "%p");
                break;
            }
        }
    }
    if (ARG_TYPE_INT == type) {
        int val = arg_getInt(arg);
        res = strsFormat(&buffs, PIKA_SPRINTF_BUFF_SIZE, fmt, val);
        goto __exit;
    }
    if (ARG_TYPE_FLOAT == type) {
        pika_float val = arg_getFloat(arg);
        res = strsFormat(&buffs, PIKA_SPRINTF_BUFF_SIZE, fmt, val);
        goto __exit;
    }
    if (ARG_TYPE_STRING == type) {
        char* val = arg_getStr(arg);
        res = strsFormat(&buffs, PIKA_SPRINTF_BUFF_SIZE, fmt, val);
        goto __exit;
    }
    if (ARG_TYPE_NONE == type) {
        res = strsFormat(&buffs, PIKA_SPRINTF_BUFF_SIZE, fmt, "None");
        goto __exit;
    }
__exit:
    if (NULL != res) {
        res = strsCopy(out_buffs, res);
    }
    strsDeinit(&buffs);
    return res;
}

char* strsFormatList(Args* out_buffs, char* fmt, PikaList* list) {
    Args buffs = {0};
    char* res = NULL;
    char* fmt_buff = strsCopy(&buffs, fmt);
    char* fmt_item = strsPopToken(&buffs, &fmt_buff, '%');
    Arg* res_buff = arg_newStr(fmt_item);

    for (size_t i = 0; i < pikaList_getSize(list); i++) {
        Args buffs_item = {0};
        Arg* arg = pikaList_get(list, i);
        char* fmt_item = strsPopToken(&buffs_item, &fmt_buff, '%');
        fmt_item = strsAppend(&buffs_item, "%", fmt_item);
        char* str_format = strsFormatArg(&buffs_item, fmt_item, arg);
        if (NULL == str_format) {
            strsDeinit(&buffs_item);
            goto __exit;
        }
        res_buff = arg_strAppend(res_buff, str_format);
        strsDeinit(&buffs_item);
    }
    goto __exit;

__exit:
    res = strsCopy(out_buffs, arg_getStr(res_buff));
    strsDeinit(&buffs);
    arg_deinit(res_buff);
    return res;
}

/* tuple */
PikaTuple* args_getTuple(Args* self, char* name) {
    if (NULL == self) {
        return NULL;
    }
    PikaObj* tuple_obj = args_getPtr(self, name);
    return tuple_obj;
}

/* dict */
PikaDict* args_getDict(Args* self, char* name) {
    if (NULL == self) {
        return NULL;
    }
    PikaObj* dict_obj = args_getPtr(self, name);
    return dict_obj;
}

char* args_cacheStr(Args* self, char* str) {
    args_setStr(self, "@sc", str);
    return args_getStr(self, "@sc");
}

void _link_deinit_pyload(Link* self, pika_bool is_object) {
    LinkNode* nowNode = self->firstNode;
    Arg* nowArg = (Arg*)nowNode;
    while (NULL != nowNode) {
        LinkNode* nodeNext = (LinkNode*)arg_getNext(nowArg);
        if (is_object) {
#if 0
            if (argType_isObjectMethodActive(arg_getType(nowArg))) {
                PikaObj* host = methodArg_getHostObj(nowArg);
                obj_GC(host);
            }
#endif
        }
        linkNode_deinit(nowNode);
        nowNode = nodeNext;
        nowArg = (Arg*)nowNode;
    }
    pika_platform_memset(self, 0, sizeof(Link));
}
