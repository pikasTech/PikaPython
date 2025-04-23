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
#ifdef __cplusplus
extern "C" {
#endif

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
void args_deinit_ex(Args* self, pika_bool is_object);
void args_init(Args* self, Args* args);
int32_t args_getSize(Args* self);
LinkNode* args_getNode(Args* self, char* name);

Arg* args_getArgByIndex(Args* self, int index);
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
PIKA_RES args_setNone(Args* self, char* name);
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
pika_bool args_getBool(Args* self, char* name);

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
typedef int32_t (*fn_args_foreach)(Arg* argEach, void* context);

PIKA_RES args_foreach(Args* self, fn_args_foreach eachHandle, void* context);

PIKA_RES args_reverse(Args* self);

char* args_getBuff(Args* self, int32_t size);
PIKA_RES args_pushArg(Args* self, Arg* arg);
PIKA_RES args_pushArg_name(Args* self, char* name, Arg* arg);
void* args_getHeapStruct(Args* self, char* name);
PIKA_RES args_removeArg_notDeinitArg(Args* self, Arg* argNow);
uint8_t* args_getBytes(Args* self, char* name);
PIKA_RES args_setBytes(Args* self, char* name, uint8_t* src, size_t size);
size_t args_getBytesSize(Args* self, char* name);

Args* New_args(Args* args);

char* args_cacheStr(Args* self, char* str);
char* strsFormatArg(Args* out_buffs, char* fmt, Arg* arg);

#endif

#ifdef __cplusplus
}
#endif
