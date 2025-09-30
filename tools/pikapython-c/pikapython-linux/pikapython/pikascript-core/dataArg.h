/*
 * This file is part of the PikaPython project.
 * http://github.com/pikastech/pikapython
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon liang6516@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _arg__H
#define _arg__H

#include "dataLink.h"
#include "dataMemory.h"

typedef uint32_t Hash;
typedef enum {
    ARG_TYPE_UNDEF = 0,
    ARG_TYPE_NONE,
    ARG_TYPE_INT,
    ARG_TYPE_BOOL,
    ARG_TYPE_FLOAT,
    ARG_TYPE_STRING,
    ARG_TYPE_BYTES,
    ARG_TYPE_POINTER,
    ARG_TYPE_BIG_ARG_PTR,
    ARG_TYPE_OBJECT,
    ARG_TYPE_OBJECT_WEAK,
    ARG_TYPE_OBJECT_META,
    ARG_TYPE_OBJECT_NEW,
    ARG_TYPE_METHOD_NATIVE,
    ARG_TYPE_METHOD_NATIVE_ACTIVE,
    ARG_TYPE_METHOD_NATIVE_CONSTRUCTOR,
    ARG_TYPE_METHOD_CONSTRUCTOR,
    ARG_TYPE_METHOD_OBJECT,
    ARG_TYPE_METHOD_STATIC,
    ARG_TYPE_STRUCT,
    ARG_TYPE_STRUCT_HEAP,
    ARG_TYPE_TUPLE,
} ArgType;

typedef void (*StructDeinitFun)(void* struct_);

typedef struct PikaObj PikaObj;
typedef struct Arg Arg;

typedef union {
    Arg* next;
    uint8_t* buffer;
} _arg_union;

typedef union {
    int64_t int_;
    pika_bool bool_;
    pika_float float_;
    PikaObj* obj;
    void* ptr;
} _arg_value;

struct Arg {
    _arg_union _;
    uint32_t size;
#if PIKA_ARG_CACHE_ENABLE
    uint32_t heap_size;
#endif
    ArgType type;
    uint8_t flag;
    Hash name_hash;
#if PIKA_KERNAL_DEBUG_ENABLE
    char* name;
    _arg_value* value;
    char* str;
    uint8_t* bytes;
    char _name_buff[PIKA_NAME_BUFF_SIZE];
#endif
    uint8_t content[];
};

typedef struct ConstArg {
    _arg_union _;
    uint32_t size;
#if PIKA_ARG_CACHE_ENABLE
    uint32_t heap_size;
#endif
    ArgType type;
    uint8_t flag;
    Hash name_hash;
#if PIKA_KERNAL_DEBUG_ENABLE
    char* name;
    _arg_value* value;
    char* str;
    uint8_t* bytes;
    char _name_buff[PIKA_NAME_BUFF_SIZE];
#endif
} ConstArg;

uint32_t arg_totleSize(Arg* self);
uint32_t arg_getTotleSize(Arg* self);
void arg_freeContent(Arg* self);

Arg* arg_setName(Arg* self, char* name);
Arg* arg_setNameHash(Arg* self, Hash nameHash);
Arg* arg_setContent(Arg* self, uint8_t* content, uint32_t size);
Arg* arg_newContent(uint32_t size);
void arg_refcntInc(Arg* self);
void arg_refcntDec(Arg* self);

static inline void arg_setType(Arg* self, ArgType type) {
    self->type = type;
}

static inline Hash arg_getNameHash(Arg* self) {
    pika_assert(self != NULL);
    return self->name_hash;
}

static inline ArgType arg_getType(Arg* self) {
    pika_assert(self != 0);
    return (ArgType)self->type;
}

uint32_t arg_getContentSize(Arg* self);
Hash hash_time33(char* str);

Arg* arg_setInt(Arg* self, char* name, int64_t val);
Arg* arg_setBool(Arg* self, char* name, pika_bool val);
Arg* arg_setFloat(Arg* self, char* name, pika_float val);
Arg* arg_setPtr(Arg* self, char* name, ArgType type, void* pointer);
Arg* arg_setStr(Arg* self, char* name, char* string);
Arg* arg_setStrN(Arg* self, char* name, char* string, size_t len);
Arg* arg_setNone(Arg* self);
Arg* arg_setBytes(Arg* self, char* name, uint8_t* src, size_t size);

static inline Arg* arg_newInt(int64_t val) {
    return arg_setInt(NULL, (char*)"", (val));
}

static inline Arg* arg_newBool(pika_bool val) {
    return arg_setBool(NULL, (char*)"", (val));
}

static inline Arg* arg_newFloat(pika_float val) {
    return arg_setFloat(NULL, (char*)"", (val));
}

static inline Arg* arg_newPtr(ArgType type, void* pointer) {
    return arg_setPtr(NULL, (char*)"", (type), (pointer));
}

static inline Arg* arg_newStr(char* string) {
    return arg_setStr(NULL, (char*)"", (string));
}

static inline Arg* arg_newStrN(char* string, size_t size) {
    return arg_setStrN(NULL, (char*)"", (string), size);
}

static inline Arg* arg_newNone() {
    return arg_setNone(NULL);
}

static inline Arg* arg_newBytes(uint8_t* src, size_t size) {
    return arg_setBytes(NULL, (char*)"", (src), (size));
}

int64_t arg_getInt(Arg* self);
pika_bool arg_getBool(Arg* self);
pika_float arg_getFloat(Arg* self);
void* arg_getPtr(Arg* self);
char* arg_getStr(Arg* self);
uint8_t* arg_getBytes(Arg* self);
size_t arg_getBytesSize(Arg* self);
Arg* arg_copy(Arg* argToBeCopy);
Arg* arg_copy_noalloc(Arg* argToBeCopy, Arg* argToBeCopyTo);
Arg* arg_copy_content(Arg* arg_dict, Arg* arg_src);

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
Arg* arg_toStrArg(Arg* arg);
void arg_print(Arg* self, pika_bool in_REPL, char* end);
Arg* arg_loadFile(Arg* self, char* filename);

#define ARG_FLAG_SERIALIZED 0x01
#define ARG_FLAG_KEYWORD 0x02
#define ARG_FLAG_WEAK_REF 0x04
#define ARG_FLAG_STARRED 0x08
#define ARG_FLAG_DOUBLE_STARRED 0x10
#define ARG_FLAG_MAX 0x18

static inline Arg* arg_getNext(Arg* self) {
    return self->_.next;
}

static inline void arg_setNext(Arg* self, Arg* next) {
    pika_assert(NULL != self);
    self->_.next = next;
}

static inline uint32_t arg_getSize(Arg* self) {
    pika_assert(NULL != self);
    return self->size;
}

static inline uint8_t arg_isSerialized(Arg* self) {
    pika_assert(NULL != self);
    pika_assert(self->flag <= ARG_FLAG_MAX);
    return self->flag & ARG_FLAG_SERIALIZED;
}

static inline void arg_setSerialized(Arg* self, uint8_t serialized) {
    self->flag = (self->flag & ~ARG_FLAG_SERIALIZED) |
                 (serialized ? ARG_FLAG_SERIALIZED : 0);
}

static inline uint8_t arg_getIsKeyword(Arg* self) {
    pika_assert(self->flag <= ARG_FLAG_MAX);
    return self->flag & ARG_FLAG_KEYWORD;
}

static inline void arg_setIsKeyword(Arg* self, uint8_t isKeyword) {
    self->flag =
        (self->flag & ~ARG_FLAG_KEYWORD) | (isKeyword ? ARG_FLAG_KEYWORD : 0);
}

static inline uint8_t arg_getIsWeakRef(Arg* self) {
    pika_assert(self->flag <= ARG_FLAG_MAX);
    return self->flag & ARG_FLAG_WEAK_REF;
}

static inline void arg_setIsWeakRef(Arg* self, uint8_t isWeakRef) {
    pika_assert(NULL != self);
    self->flag =
        (self->flag & ~ARG_FLAG_WEAK_REF) | (isWeakRef ? ARG_FLAG_WEAK_REF : 0);
}

static inline void arg_setIsStarred(Arg* self, uint8_t isStarred) {
    self->flag =
        (self->flag & ~ARG_FLAG_STARRED) | (isStarred ? ARG_FLAG_STARRED : 0);
}

static inline uint8_t arg_getIsStarred(Arg* self) {
    pika_assert(self->flag <= ARG_FLAG_MAX);
    return self->flag & ARG_FLAG_STARRED;
}

static inline void arg_setIsDoubleStarred(Arg* self, uint8_t isDoubleStarred) {
    self->flag = (self->flag & ~ARG_FLAG_DOUBLE_STARRED) |
                 (isDoubleStarred ? ARG_FLAG_DOUBLE_STARRED : 0);
}

static inline uint8_t arg_getIsDoubleStarred(Arg* self) {
    pika_assert(self->flag <= ARG_FLAG_MAX);
    return self->flag & ARG_FLAG_DOUBLE_STARRED;
}

static inline uint8_t* arg_getContent(Arg* self) {
    pika_assert(NULL != self);
    pika_assert(self->flag <= ARG_FLAG_MAX);
    return (arg_isSerialized(self)) ? (self)->content : ((self)->_.buffer);
}

static inline uint8_t argType_isObject(ArgType type) {
    return ((type) == ARG_TYPE_OBJECT || (type) == ARG_TYPE_OBJECT_NEW ||
            (type) == ARG_TYPE_OBJECT_WEAK);
}

static inline uint8_t argType_isCallable(ArgType type) {
    return ((type) == ARG_TYPE_METHOD_CONSTRUCTOR ||
            (type) == ARG_TYPE_METHOD_OBJECT ||
            (type) == ARG_TYPE_METHOD_STATIC ||
            (type) == ARG_TYPE_METHOD_NATIVE ||
            (type) == ARG_TYPE_METHOD_NATIVE_CONSTRUCTOR ||
            (type) == ARG_TYPE_METHOD_NATIVE_ACTIVE);
}

static inline uint8_t argType_isObjectMethod(ArgType type) {
    return ((type) == ARG_TYPE_METHOD_OBJECT ||
            (type) == ARG_TYPE_METHOD_NATIVE);
}

static inline uint8_t argType_isObjectMethodActive(ArgType type) {
    return ((type) == ARG_TYPE_METHOD_OBJECT ||
            (type) == ARG_TYPE_METHOD_NATIVE_ACTIVE);
}

static inline uint8_t argType_isConstructor(ArgType type) {
    return ((type) == ARG_TYPE_METHOD_CONSTRUCTOR ||
            (type) == ARG_TYPE_METHOD_NATIVE_CONSTRUCTOR);
}

static inline uint8_t argType_isNative(ArgType type) {
    return ((type) == ARG_TYPE_METHOD_NATIVE ||
            (type) == ARG_TYPE_METHOD_NATIVE_CONSTRUCTOR ||
            (type) == ARG_TYPE_METHOD_NATIVE_ACTIVE);
}

static inline uint8_t _argType_or(ArgType type, ArgType type1, ArgType type2) {
    return ((type) == (type1) || (type) == (type2));
}

static inline uint8_t argType_isEqual(ArgType type1, ArgType type2) {
    if (type1 == type2) {
        return 1;
    }
    if (_argType_or(type1, ARG_TYPE_METHOD_NATIVE,
                    ARG_TYPE_METHOD_NATIVE_ACTIVE) &&
        _argType_or(type2, ARG_TYPE_METHOD_NATIVE,
                    ARG_TYPE_METHOD_NATIVE_ACTIVE)) {
        return 1;
    }
    return 0;
}

static inline uint8_t argType_isIterable(ArgType type) {
    return ((type) == ARG_TYPE_STRING || (type) == ARG_TYPE_BYTES ||
            argType_isObject(type));
}

#define arg_isObject(__self) \
    ((__self != NULL) && (argType_isObject(arg_getType(__self))))
#define arg_isCallable(__self) \
    ((__self != NULL) && (argType_isCallable(arg_getType(__self))))
#define arg_isConstructor(__self) \
    ((__self != NULL) && (argType_isConstructor(arg_getType(__self))))
#define arg_isNative(__self) \
    ((__self != NULL) && (argType_isNative(arg_getType(__self))))
#define arg_isIterable(__self) \
    ((__self != NULL) && (argType_isIterable(arg_getType(__self))))

#define arg_newReg(__name, __size)           \
    Arg __name = {0};                        \
    uint8_t __##__name##_buff[__size] = {0}; \
    arg_init_stack(&__name, __##__name##_buff, __size)

void arg_init_stack(Arg* self, uint8_t* buffer, uint32_t size);
pika_bool arg_isEqual(Arg* self, Arg* other);
Hash hash_time33EndWith(char* str, char end);

#endif

#ifdef __cplusplus
}
#endif
