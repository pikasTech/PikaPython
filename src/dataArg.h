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

#ifndef _arg__H
#define _arg__H

#include "dataLink.h"
#include "dataMemory.h"

typedef uint32_t Hash;
typedef enum {
    ARG_TYPE_UNDEF = 0,
    ARG_TYPE_NONE,
    ARG_TYPE_INT,
    ARG_TYPE_FLOAT,
    ARG_TYPE_STRING,
    ARG_TYPE_BYTES,
    ARG_TYPE_POINTER,
    ARG_TYPE_OBJECT,
    ARG_TYPE_OBJECT_META,
    ARG_TYPE_OBJECT_NEW,
    ARG_TYPE_METHOD_NATIVE,
    ARG_TYPE_METHOD_NATIVE_CONSTRUCTOR,
    ARG_TYPE_METHOD_CONSTRUCTOR,
    ARG_TYPE_METHOD_OBJECT,
    ARG_TYPE_METHOD_STATIC,
    ARG_TYPE_STRUCT,
    ARG_TYPE_STRUCT_HEAP,
    ARG_TYPE_TUPLE,
} ArgType;

typedef void (*StructDeinitFun)(void* struct_);

typedef struct Arg Arg;

typedef union {
    Arg* next;
    uint8_t* buffer;
} _arg_union;

struct Arg {
    _arg_union _;       // 32/64 bit
    uint32_t size;      // 32 bit
    uint8_t type;       // 8 bit
    PIKA_BOOL flag;     //
    Hash name_hash;     // 32bit
    uint8_t content[];  // n bit
};

Arg* arg_getNext(Arg* self);
uint32_t arg_getSize(Arg* self);
uint8_t* arg_getContent(Arg* self);
uint32_t arg_totleSize(Arg* self);
void arg_setNext(Arg* self, Arg* next);
uint32_t arg_getTotleSize(Arg* self);
void arg_freeContent(Arg* self);

Arg* arg_setName(Arg* self, char* name);
Arg* arg_setContent(Arg* self, uint8_t* content, uint32_t size);
Arg* arg_newContent(Arg* self, uint32_t size);
Arg* arg_setType(Arg* self, ArgType type);
Hash arg_getNameHash(Arg* self);
ArgType arg_getType(Arg* self);
uint32_t arg_getContentSize(Arg* self);
Hash hash_time33(char* str);

Arg* arg_setInt(Arg* self, char* name, int64_t val);
Arg* arg_setFloat(Arg* self, char* name, double val);
Arg* arg_setPtr(Arg* self, char* name, ArgType type, void* pointer);
Arg* arg_setStr(Arg* self, char* name, char* string);
Arg* arg_setNull(Arg* self);
Arg* arg_setBytes(Arg* self, char* name, uint8_t* src, size_t size);

#define arg_newInt(val) arg_setInt(NULL, "", (val))
#define arg_newFloat(val) arg_setFloat(NULL, "", (val))
#define arg_newPtr(type, pointer) arg_setPtr(NULL, "", (type), (pointer))
#define arg_newStr(string) arg_setStr(NULL, "", (string))
#define arg_newNull() arg_setNull(NULL)
#define arg_newBytes(src, size) arg_setBytes(NULL, "", (src), (size))

int64_t arg_getInt(Arg* self);
double arg_getFloat(Arg* self);
void* arg_getPtr(Arg* self);
char* arg_getStr(Arg* self);
uint8_t* arg_getBytes(Arg* self);
size_t arg_getBytesSize(Arg* self);
Arg* arg_copy(Arg* argToBeCopy);
Arg* arg_copy_noalloc(Arg* argToBeCopy, Arg* argToBeCopyTo);

uint8_t* arg_getContent(Arg* self);
void arg_deinit(Arg* self);

Arg* New_arg(void* voidPointer);
Arg* arg_append(Arg* arg_in, void* new_content, size_t new_size);
Arg* arg_setStruct(Arg* self,
                   char* name,
                   void* struct_ptr,
                   uint32_t struct_size);
Arg* arg_setHeapStruct(Arg* self,
                       char* name,
                       void* struct_ptr,
                       uint32_t struct_size,
                       void* struct_deinit_fun);
void* arg_getHeapStruct(Arg* self);
void arg_deinitHeap(Arg* self);
void arg_printBytes(Arg* self);
Arg* arg_loadFile(Arg* self, char* filename);
uint8_t argType_isObject(ArgType type);

#define arg_getNext(self) ((self)->_.next)
#define arg_getSize(self) ((self)->size)

#define ARG_FLAG_MASK_SERIALIZED 0x01
#define ARG_FLAG_MASK_ISKEYWORD 0x02

#define arg_getSerialized(self) ((self)->flag & ARG_FLAG_MASK_SERIALIZED)
#define arg_setSerialized(self, _serialized)                     \
    ((self)->flag = ((self)->flag & ~ARG_FLAG_MASK_SERIALIZED) | \
                    ((_serialized) ? ARG_FLAG_MASK_SERIALIZED : 0))
#define arg_getIsKeyword(self) ((self)->flag & ARG_FLAG_MASK_ISKEYWORD)

#define arg_setIsKeyword(self, _isKeyword)                      \
    ((self)->flag = ((self)->flag & ~ARG_FLAG_MASK_ISKEYWORD) | \
                    ((_isKeyword) ? ARG_FLAG_MASK_ISKEYWORD : 0))

#define arg_getContent(self) \
    (arg_getSerialized(self) ? (self)->content : ((self)->_.buffer))
#define arg_getNext(self) ((self)->_.next)
#define arg_setNext(self, __next) ((self)->_.next = (__next))

#define argType_isObject(type) \
    ((type) == ARG_TYPE_OBJECT || (type) == ARG_TYPE_OBJECT_NEW)

#define argType_isCallable(type)                                             \
    ((type) == ARG_TYPE_METHOD_CONSTRUCTOR ||                                \
     (type) == ARG_TYPE_METHOD_OBJECT || (type) == ARG_TYPE_METHOD_STATIC || \
     (type) == ARG_TYPE_METHOD_NATIVE ||                                     \
     (type) == ARG_TYPE_METHOD_NATIVE_CONSTRUCTOR)

#endif

#define arg_newReg(__name, __size)           \
    Arg __name = {0};                        \
    uint8_t __##__name##_buff[__size] = {0}; \
    arg_init_stack(&__name, __##__name##_buff, __size)

void arg_init_stack(Arg* self, uint8_t* buffer, uint32_t size);
