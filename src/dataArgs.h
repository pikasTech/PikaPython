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
#define dict_setInt(self, name, val) \
    args_setInt((&((self)->super)), (name), (val))
#define dict_setFloat(self, name, val) \
    args_setFloat((&((self)->super)), (name), (val))
#define dict_setStr(self, name, val) \
    args_setStr((&((self)->super)), (name), (val))
#define dict_setPtr(self, name, val) \
    args_setPtr((&((self)->super)), (name), (val))
#define dict_setArg(self, val) args_setArg((&((self)->super)), (val))
#define dict_removeArg(self, val) args_removeArg((&((self)->super)), (val))
#define dict_setBytes(self, name, val, size) \
    args_setBytes((&((self)->super)), (name), (val), (size))
#define dict_getInt(self, name) (args_getInt((&((self)->super)), (name)))
#define dict_getFloat(self, name) (args_getFloat((&((self)->super)), (name)))
#define dict_getStr(self, name) (args_getStr((&((self)->super)), (name)))
#define dict_getPtr(self, name) (args_getPtr((&((self)->super)), (name)))
#define dict_getArg(self, name) (args_getArg((&((self)->super)), (name)))
#define dict_getBytes(self, name) (args_getBytes((&((self)->super)), (name)))
#define dict_getType(self, name) (args_getType((&((self)->super)), (name)))
#define dict_getBytesSize(self, name) \
    (args_getBytesSize((&((self)->super)), (name)))
#define dict_deinit(self) (args_deinit((&((self)->super))))

#define list_deinit(self) (args_deinit((&((self)->super))))
PIKA_RES list_append(PikaList* self, Arg* arg);
PIKA_RES list_setArg(PikaList* self, int index, Arg* arg);
int list_getInt(PikaList* self, int index);
pika_float list_getFloat(PikaList* self, int index);
char* list_getStr(PikaList* self, int index);
void* list_getPtr(PikaList* self, int index);
Arg* list_getArg(PikaList* self, int index);
size_t list_getSize(PikaList* self);
void list_reverse(PikaList* self);
char* strsFormatArg(Args* out_buffs, char* fmt, Arg* arg);

/* tuple api */
#define tuple_deinit(self) (list_deinit((&((self)->super))))
#define tuple_getArg(self, index) (list_getArg((&((self)->super)), (index)))
#define tuple_getSize(self) (list_getSize((&((self)->super))))
#define tuple_getInt(self, index) (list_getInt((&((self)->super)), (index)))
#define tuple_getFloat(self, index) \
    (list_getFloat((&((self)->super)), (index)))
#define tuple_getStr(self, index) (list_getStr((&((self)->super)), (index)))
#define tuple_getPtr(self, index) (list_getPtr((&((self)->super)), (index)))
#define tuple_getType(self, index) (list_getType((&((self)->super)), (index)))

PikaList* New_list(void);
PikaTuple* New_tuple(void);
PikaTuple* args_getTuple(Args* self, char* name);
PikaDict* args_getDict(Args* self, char* name);

char* strsFormatList(Args* out_buffs, char* fmt, PikaList* list);
char* args_cacheStr(Args* self, char* str);
PIKA_RES list_insert(PikaList* self, int index, Arg* arg);
Arg* list_pop(PikaList* list);
PIKA_RES list_remove(PikaList* list, Arg* arg);

#endif
