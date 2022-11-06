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

#ifndef _dataArgs__H
#define _dataArgs__H
#include "dataArg.h"
#include "dataLink.h"
#include "dataMemory.h"
#include "dataString.h"

typedef Link Args;

/* operation */
void args_deinit(Args* self);
void args_deinit_stack(Args* self);
void args_init(Args* self, Args* args);
int32_t args_getSize(Args* self);
LinkNode* args_getNode(Args* self, char* name);

Arg* args_getArgByIndex(Args* self, int32_t index);
Arg* args_getArg(Args* self, char* name);
PIKA_RES args_removeArg(Args* self, Arg* argNow);
PIKA_RES args_moveArg(Args* self, Args* dict, Arg* arg);
Arg* args_getArg_hash(Args* self, Hash nameHash);

PIKA_RES args_setArg(Args* self, Arg* arg);

PIKA_RES args_copyArgByName(Args* self, char* name, Args* directList);
PIKA_RES args_copyArg(Args* self, Arg* argToBeCopy);

ArgType args_getType(Args* self, char* name);
int32_t args_isArgExist_hash(Args* self, Hash nameHash);
int32_t args_isArgExist(Args* self, char* name);

PIKA_RES args_setStr(Args* self, char* name, char* strIn);
PIKA_RES args_setStrWithDefaultName(Args* self, char* strIn);
char* args_getStr(Args* self, char* name);

PIKA_RES args_setFloatWithDefaultName(Args* self, pika_float argFloat);
PIKA_RES args_setFloat(Args* self, char* name, pika_float argFloat);
pika_float args_getFloat(Args* self, char* name);

PIKA_RES args_setRef(Args* self, char* name, void* argPointer);
PIKA_RES args_setPtr(Args* self, char* name, void* argPointer);
void* args_getPtr(Args* self, char* name);

PIKA_RES args_setInt(Args* self, char* name, int64_t int64In);
int64_t args_getInt(Args* self, char* name);

char* args_print(Args* self, char* name);

PIKA_RES args_setStructWithSize(Args* self,
                                char* name,
                                void* struct_ptr,
                                uint32_t struct_size);

PIKA_RES args_setHeapStructWithSize(Args* self,
                                    char* name,
                                    void* struct_ptr,
                                    uint32_t struct_size,
                                    void* struct_deinit_fun);

#define args_setStruct(Args_p_self, char_p_name, struct_)            \
    args_setStructWithSize((Args_p_self), (char_p_name), &(struct_), \
                           sizeof(struct_))

#define args_setHeapStruct(Args_p_self, char_p_name, struct_,            \
                           struct_deinit_fun)                            \
    args_setHeapStructWithSize((Args_p_self), (char_p_name), &(struct_), \
                               sizeof(struct_), (void*)struct_deinit_fun)

void* args_getStruct(Args* self, char* name);

PIKA_RES args_set(Args* self, char* name, char* valueStr);
PIKA_RES args_setObjectWithClass(Args* self,
                                 char* objectName,
                                 char* className,
                                 void* objectPtr);
PIKA_RES args_setPtrWithType(Args* self,
                             char* name,
                             ArgType type,
                             void* objPtr);
PIKA_RES args_foreach(Args* self,
                      int32_t (*eachHandle)(Arg* argEach, Args* context),
                      Args* context);

char* args_getBuff(Args* self, int32_t size);
PIKA_RES args_pushArg(Args* self, Arg* arg);
PIKA_RES args_pushArg_name(Args* self, char* name, Arg* arg);
Arg* args_getArgByidex(Args* self, int index);
void* args_getHeapStruct(Args* self, char* name);
PIKA_RES args_removeArg_notDeinitArg(Args* self, Arg* argNow);
uint8_t* args_getBytes(Args* self, char* name);
PIKA_RES args_setBytes(Args* self, char* name, uint8_t* src, size_t size);
size_t args_getBytesSize(Args* self, char* name);

Args* New_args(Args* args);

typedef struct PikaList PikaList;
struct PikaList {
    Args super;
};

typedef struct PikaTuple PikaTuple;
struct PikaTuple {
    PikaList super;
};

typedef struct PikaDict PikaDict;
struct PikaDict {
    Args super;
};

/* dict api */
PikaDict* New_dict(void);

static inline PIKA_RES dict_setInt(PikaDict* self, char* name, int64_t val) {
    return args_setInt((&((self)->super)), (name), (val));
}
static inline PIKA_RES dict_setFloat(PikaDict* self,
                                     char* name,
                                     pika_float val) {
    return args_setFloat((&((self)->super)), (name), (val));
}
static inline PIKA_RES dict_setStr(PikaDict* self, char* name, char* val) {
    return args_setStr((&((self)->super)), (name), (val));
}
static inline PIKA_RES dict_setPtr(PikaDict* self, char* name, void* val) {
    return args_setPtr((&((self)->super)), (name), (val));
}

static inline PIKA_RES dict_setArg(PikaDict* self, Arg* val) {
    return args_setArg((&((self)->super)), (val));
}

static inline PIKA_RES dict_removeArg(PikaDict* self, Arg* val) {
    return args_removeArg((&((self)->super)), (val));
}

static inline PIKA_RES dict_setBytes(PikaDict* self,
                                     char* name,
                                     uint8_t* val,
                                     size_t size) {
    return args_setBytes((&((self)->super)), (name), (val), (size));
}

static inline int64_t dict_getInt(PikaDict* self, char* name) {
    return args_getInt((&((self)->super)), (name));
}

static inline pika_float dict_getFloat(PikaDict* self, char* name) {
    return args_getFloat((&((self)->super)), (name));
}

static inline char* dict_getStr(PikaDict* self, char* name) {
    return args_getStr((&((self)->super)), (name));
}

static inline void* dict_getPtr(PikaDict* self, char* name) {
    return args_getPtr((&((self)->super)), (name));
}

static inline Arg* dict_getArg(PikaDict* self, char* name) {
    return args_getArg((&((self)->super)), (name));
}

static inline uint8_t* dict_getBytes(PikaDict* self, char* name) {
    return args_getBytes((&((self)->super)), (name));
}

static inline ArgType dict_getType(PikaDict* self, char* name) {
    return args_getType((&((self)->super)), (name));
}

static inline size_t dict_getBytesSize(PikaDict* self, char* name) {
    return args_getBytesSize((&((self)->super)), (name));
}

static inline void dict_deinit(PikaDict* self) {
    args_deinit((&((self)->super)));
}

/* list api */
PIKA_RES list_append(PikaList* self, Arg* arg);
PIKA_RES list_setArg(PikaList* self, int index, Arg* arg);
int list_getInt(PikaList* self, int index);
pika_float list_getFloat(PikaList* self, int index);
char* list_getStr(PikaList* self, int index);
void* list_getPtr(PikaList* self, int index);
Arg* list_getArg(PikaList* self, int index);
size_t list_getSize(PikaList* self);
void list_reverse(PikaList* self);
PIKA_RES list_insert(PikaList* self, int index, Arg* arg);
Arg* list_pop(PikaList* list);
PIKA_RES list_remove(PikaList* list, Arg* arg);
static inline void list_deinit(PikaList* self) {
    args_deinit((&((self)->super)));
}

static inline ArgType list_getType(PikaList* self, int index) {
    Arg* arg = list_getArg(self, index);
    return arg_getType(arg);
}

/* tuple api */
static inline void tuple_deinit(PikaTuple* self) {
    list_deinit((&((self)->super)));
}

static inline Arg* tuple_getArg(PikaTuple* self, int index) {
    return list_getArg((&((self)->super)), (index));
}

static inline size_t tuple_getSize(PikaTuple* self) {
    return list_getSize((&((self)->super)));
}

static inline int64_t tuple_getInt(PikaTuple* self, int index) {
    return list_getInt((&((self)->super)), (index));
}

static inline pika_float tuple_getFloat(PikaTuple* self, int index) {
    return list_getFloat((&((self)->super)), (index));
}

static inline char* tuple_getStr(PikaTuple* self, int index) {
    return list_getStr((&((self)->super)), (index));
}

static inline void* tuple_getPtr(PikaTuple* self, int index) {
    return list_getPtr((&((self)->super)), (index));
}

static inline ArgType tuple_getType(PikaTuple* self, int index) {
    return list_getType((&((self)->super)), (index));
}

PikaList* New_list(void);
PikaTuple* New_tuple(void);
PikaTuple* args_getTuple(Args* self, char* name);
PikaDict* args_getDict(Args* self, char* name);

char* strsFormatList(Args* out_buffs, char* fmt, PikaList* list);
char* args_cacheStr(Args* self, char* str);
char* strsFormatArg(Args* out_buffs, char* fmt, Arg* arg);

#endif