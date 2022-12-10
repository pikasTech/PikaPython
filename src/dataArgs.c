/*
 * This file is part of the PikaScript project.
 * http://github.com/pikastech/pikascript
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon 李昂 liang6516@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "dataArgs.h"
#include "PikaObj.h"
#include "PikaPlatform.h"
#include "dataLink.h"
#include "dataMemory.h"
#include "dataString.h"
#include "dataStrs.h"

void args_deinit(Args* self) {
    pika_assert(self != NULL);
    link_deinit(self);
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
    Arg* argNew = New_arg(NULL);
    argNew = arg_setRef(argNew, name, argPointer);
    args_setArg(self, argNew);
    return errCode;
}

PIKA_RES args_setStr(Args* self, char* name, char* strIn) {
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
    Arg* argNew = arg_newNull();
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
        return -999999999;
    }
    ArgType arg_type = arg_getType(arg);
    if (arg_type == ARG_TYPE_INT) {
        return arg_getInt(arg);
    } else if (arg_type == ARG_TYPE_FLOAT) {
        return (int)arg_getFloat(arg);
    }
    return -999999999;
}

int32_t args_getSize(Args* self) {
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
        return -999999999.0;
    }
    ArgType arg_type = arg_getType(arg);
    if (arg_type == ARG_TYPE_FLOAT) {
        return arg_getFloat(arg);
    } else if (arg_type == ARG_TYPE_INT) {
        return (pika_float)arg_getInt(arg);
    }
    return -999999999.0;
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

PIKA_RES __updateArg(Args* self, Arg* argNew) {
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

    nodeToUpdate = (LinkNode*)arg_setContent(
        (Arg*)nodeToUpdate, arg_getContent(argNew), arg_getSize(argNew));

    pika_assert(NULL != nodeToUpdate);
    arg_setType((Arg*)nodeToUpdate, arg_getType(argNew));
    // update privior link, because arg_getContent would free origin pointer
    if (NULL == priorNode) {
        self->firstNode = nodeToUpdate;
        res = PIKA_RES_OK;
        goto exit;
    }

    arg_setNext((Arg*)priorNode, (Arg*)nodeToUpdate);
    res = PIKA_RES_OK;
    goto exit;
exit:
    if (!arg_isSerialized(argNew)) {
        return res;
    }
    arg_freeContent(argNew);
    return res;
}

PIKA_RES args_setArg(Args* self, Arg* arg) {
    if (PIKA_RES_OK == __updateArg(self, arg)) {
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

Arg* args_getArgByidex(Args* self, int index) {
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

PIKA_RES args_foreach(Args* self,
                      int32_t (*eachHandle)(Arg* argEach, Args* context),
                      Args* context) {
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

PikaDict* New_pikaDict(void) {
    PikaDict* self = (PikaDict*)New_args(NULL);
    return self;
}

PikaList* New_pikaList(void) {
    PikaList* self = (PikaList*)New_args(NULL);
    /* set top index for append */
    args_pushArg_name(&self->super, "top", arg_newInt(0));
    return self;
}

PIKA_RES pikaList_setArg(PikaList* self, int index, Arg* arg) {
    char buff[11];
    char* i_str = fast_itoa(buff, index);
    int top = args_getInt(&self->super, "top");
    if (index > top) {
        return PIKA_RES_ERR_OUT_OF_RANGE;
    }
    Arg* new_arg = arg_copy(arg);
    new_arg = arg_setName(new_arg, i_str);
    args_setArg(&self->super, new_arg);
    return PIKA_RES_OK;
}

Arg* pikaList_getArg(PikaList* self, int index) {
    pika_assert(NULL != self);
    char buff[11];
    char* i_str = fast_itoa(buff, index);
    return args_getArg(&self->super, i_str);
}

int pikaList_getInt(PikaList* self, int index) {
    Arg* arg = pikaList_getArg(self, index);
    return arg_getInt(arg);
}

pika_float pikaList_getFloat(PikaList* self, int index) {
    Arg* arg = pikaList_getArg(self, index);
    return arg_getFloat(arg);
}

char* pikaList_getStr(PikaList* self, int index) {
    Arg* arg = pikaList_getArg(self, index);
    return arg_getStr(arg);
}

void* pikaList_getPtr(PikaList* self, int index) {
    Arg* arg = pikaList_getArg(self, index);
    return arg_getPtr(arg);
}

PIKA_RES pikaList_append(PikaList* self, Arg* arg) {
    if (NULL == arg) {
        return PIKA_RES_ERR_ARG_NO_FOUND;
    }
    int top = args_getInt(&self->super, "top");
    char buff[11];
    char* topStr = fast_itoa(buff, top);
    Arg* arg_to_push = arg_copy(arg);
    arg_setName(arg_to_push, topStr);
    args_setArg(&self->super, arg_to_push);
    /* top++ */
    return args_setInt(&self->super, "top", top + 1);
}

Arg* pikaList_pop(PikaList* list) {
    int top = args_getInt(&list->super, "top");
    if (top <= 0) {
        return NULL;
    }
    Arg* arg = pikaList_getArg(list, top - 1);
    Arg* res = arg_copy(arg);
    args_removeArg(&list->super, arg);
    args_setInt(&list->super, "top", top - 1);
    return res;
}

PIKA_RES pikaList_remove(PikaList* list, Arg* arg) {
    int top = args_getInt(&list->super, "top");
    int i_remove = 0;
    if (top <= 0) {
        return PIKA_RES_ERR_OUT_OF_RANGE;
    }
    for (int i = 0; i < top; i++) {
        Arg* arg_now = pikaList_getArg(list, i);
        if (arg_isEqual(arg_now, arg)) {
            i_remove = i;
            args_removeArg(&list->super, arg_now);
            break;
        }
    }
    /* move args */
    for (int i = i_remove + 1; i < top; i++) {
        char buff[11];
        char* i_str = fast_itoa(buff, i - 1);
        Arg* arg_now = pikaList_getArg(list, i);
        arg_setName(arg_now, i_str);
    }
    args_setInt(&list->super, "top", top - 1);
    return PIKA_RES_OK;
}

PIKA_RES pikaList_insert(PikaList* self, int index, Arg* arg) {
    int top = args_getInt(&self->super, "top");
    if (index > top) {
        return PIKA_RES_ERR_OUT_OF_RANGE;
    }
    /* move args */
    for (int i = top - 1; i >= index; i--) {
        char buff[11];
        char* i_str = fast_itoa(buff, i + 1);
        Arg* arg_now = pikaList_getArg(self, i);
        arg_setName(arg_now, i_str);
    }
    char buff[11];
    char* i_str = fast_itoa(buff, index);
    Arg* arg_to_push = arg_copy(arg);
    arg_setName(arg_to_push, i_str);
    args_setArg(&self->super, arg_to_push);
    args_setInt(&self->super, "top", top + 1);
    return PIKA_RES_OK;
}

size_t pikaList_getSize(PikaList* self) {
    pika_assert(NULL != self);
    return args_getInt(&self->super, "top");
}

void pikaList_reverse(PikaList* self) {
    pika_assert(NULL != self);
    int top = pikaList_getSize(self);
    for (int i = 0; i < top / 2; i++) {
        Arg* arg_i = arg_copy(pikaList_getArg(self, i));
        Arg* arg_top = arg_copy(pikaList_getArg(self, top - i - 1));
        pikaList_setArg(self, i, arg_top);
        pikaList_setArg(self, top - i - 1, arg_i);
        arg_deinit(arg_i);
        arg_deinit(arg_top);
    }
}

PikaTuple* New_pikaTuple(void) {
    PikaTuple* self = (PikaTuple*)New_pikaList();
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
        goto exit;
    }
    if (ARG_TYPE_FLOAT == type) {
        pika_float val = arg_getFloat(arg);
        res = strsFormat(&buffs, PIKA_SPRINTF_BUFF_SIZE, fmt, val);
        goto exit;
    }
    if (ARG_TYPE_STRING == type) {
        char* val = arg_getStr(arg);
        res = strsFormat(&buffs, PIKA_SPRINTF_BUFF_SIZE, fmt, val);
        goto exit;
    }
    if (ARG_TYPE_NONE == type) {
        res = strsFormat(&buffs, PIKA_SPRINTF_BUFF_SIZE, fmt, "None");
        goto exit;
    }
exit:
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
        Arg* arg = pikaList_getArg(list, i);
        char* fmt_item = strsPopToken(&buffs_item, &fmt_buff, '%');
        fmt_item = strsAppend(&buffs_item, "%", fmt_item);
        char* str_format = strsFormatArg(&buffs_item, fmt_item, arg);
        if (NULL == str_format) {
            strsDeinit(&buffs_item);
            goto exit;
        }
        res_buff = arg_strAppend(res_buff, str_format);
        strsDeinit(&buffs_item);
    }
    goto exit;

exit:
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
    return obj_getPtr(tuple_obj, "list");
}

/* dict */
PikaDict* args_getDict(Args* self, char* name) {
    if (NULL == self) {
        return NULL;
    }
    PikaObj* tuple_obj = args_getPtr(self, name);
    return obj_getPtr(tuple_obj, "dict");
}

char* args_cacheStr(Args* self, char* str) {
    args_setStr(self, "@sc", str);
    return args_getStr(self, "@sc");
}
