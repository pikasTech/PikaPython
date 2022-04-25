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

#include "dataArg.h"
#include "PikaObj.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataString.h"
#include "stdlib.h"

uint16_t arg_getTotleSize(Arg* self) {
    return content_totleSize(self);
}

/**
 * time33 hash
 */
Hash hash_time33(char* str) {
    Hash hash = 5381;
    while (*str) {
        hash += (hash << 5) + (*str++);
    }
    return (hash & 0x7FFFFFFF);
}

static uint8_t* content_init_hash(Hash nameHash,
                                  ArgType type,
                                  uint8_t* content,
                                  uint32_t size,
                                  uint8_t* next) {
    __arg* self = (__arg*)pikaMalloc(sizeof(__arg) + size);

    self->next = (__arg*)next;
    self->size = size;
    self->name_hash = nameHash;
    self->type = type;

    if (NULL != content) {
        __platform_memcpy(self->content, content, size);
    } else {
        __platform_memset(self->content, 0, size);
    }

    return (uint8_t*)self;
}

static uint8_t* content_init(char* name,
                             ArgType type,
                             uint8_t* content,
                             uint16_t size,
                             uint8_t* next) {
    Hash nameHash = hash_time33(name);
    return content_init_hash(nameHash, type, content, size, next);
}

uint16_t content_totleSize(uint8_t* self) {
    return ((__arg*)self)->size + sizeof(__arg);
}

void arg_freeContent(Arg* self) {
    if (NULL != self) {
        content_deinit(self);
    }
}

uint8_t* content_deinit(uint8_t* self) {
    uint16_t totleSize = content_totleSize(self);
    pikaFree(self, totleSize);
    return 0;
}

uint8_t* content_setContent(uint8_t* self, uint8_t* content, uint16_t size) {
    if (NULL == self) {
        /* malloc */
        return content_init("", ARG_TYPE_VOID, content, size, NULL);
    }

    /* only copy */
    if (content_getSize(self) == size) {
        __platform_memcpy(((__arg*)self)->content, content, size);
        return self;
    }

    /* realloc */
    Hash nameHash = content_getNameHash(self);
    ArgType type = content_getType(self);
    uint8_t* next = content_getNext(self);
    uint8_t* newContent =
        content_init_hash(nameHash, type, content, size, next);
    content_deinit(self);
    return newContent;
}

uint8_t* content_setNameHash(uint8_t* self, Hash nameHash) {
    if (NULL == self) {
        return content_init_hash(nameHash, ARG_TYPE_VOID, NULL, 0, NULL);
    }
    __arg* arg = (__arg*)self;
    arg->name_hash = nameHash;
    return self;
}

uint8_t* content_setName(uint8_t* self, char* name) {
    return content_setNameHash(self, hash_time33(name));
}

uint8_t* content_setType(uint8_t* self, ArgType type) {
    if (NULL == self) {
        return content_init("", type, NULL, 0, NULL);
    }

    __arg* arg = (__arg*)self;
    arg->type = type;

    return self;
}

ArgType content_getType(uint8_t* self) {
    __arg* arg = (__arg*)self;
    return (ArgType)arg->type;
}

Arg* arg_setBytes(Arg* self, char* name, uint8_t* src, size_t size) {
    self = arg_newContent(self, size + sizeof(size_t));
    self = arg_setName(self, name);
    self = arg_setType(self, ARG_TYPE_BYTES);
    void* dir = arg_getContent(self);
    __platform_memcpy(dir, &size, sizeof(size_t));
    __platform_memcpy((void*)((uintptr_t)dir + sizeof(size_t)), src, size);
    return self;
}

Arg* arg_newContent(Arg* self, uint32_t size) {
    uint8_t* newContent = content_init("", ARG_TYPE_VOID, NULL, size, NULL);
    arg_freeContent(self);
    return newContent;
}

Arg* arg_setContent(Arg* self, uint8_t* content, uint32_t size) {
    return content_setContent(self, content, size);
}

uint8_t* arg_getBytes(Arg* self) {
    return content_getContent(self) + sizeof(size_t);
}

void arg_printBytes(Arg* self) {
    size_t bytes_size = arg_getBytesSize(self);
    uint8_t* bytes = arg_getBytes(self);
    __platform_printf("b\'");
    for (size_t i = 0; i < bytes_size; i++) {
        __platform_printf("\\x%02x", bytes[i]);
    }
    __platform_printf("\'\r\n");
}

size_t arg_getBytesSize(Arg* self) {
    size_t mem_size = 0;
    void* content = (void*)arg_getContent(self);
    if (NULL == content) {
        return 0;
    }
    __platform_memcpy(&mem_size, content, sizeof(size_t));
    return mem_size;
}

Arg* arg_setStruct(Arg* self,
                   char* name,
                   void* struct_ptr,
                   uint32_t struct_size) {
    if (NULL == struct_ptr) {
        return NULL;
    }
    Arg* struct_arg = arg_setContent(NULL, (uint8_t*)struct_ptr, struct_size);
    struct_arg = arg_setType(struct_arg, ARG_TYPE_STRUCT);
    struct_arg = arg_setName(struct_arg, name);
    return struct_arg;
}

Arg* arg_setHeapStruct(Arg* self,
                       char* name,
                       void* struct_ptr,
                       uint32_t struct_size,
                       void* struct_deinit_fun) {
    if (NULL == struct_ptr) {
        return NULL;
    }
    Arg* struct_arg =
        arg_setContent(NULL, (uint8_t*)&struct_deinit_fun, sizeof(void*));
    struct_arg = arg_append(struct_arg, (uint8_t*)struct_ptr, struct_size);
    struct_arg = arg_setType(struct_arg, ARG_TYPE_HEAP_STRUCT);
    struct_arg = arg_setName(struct_arg, name);
    return struct_arg;
}

void* arg_getHeapStructDeinitFun(Arg* self) {
    void* deinit_fun = NULL;
    __platform_memcpy(&deinit_fun, arg_getContent(self), sizeof(void*));
    return deinit_fun;
}

Arg* arg_setName(Arg* self, char* name) {
    return content_setName(self, name);
}

Arg* arg_setNameHash(Arg* self, Hash nameHash) {
    return content_setNameHash(self, nameHash);
}

Arg* arg_setType(Arg* self, ArgType type) {
    return content_setType(self, type);
}

Arg* arg_setInt(Arg* self, char* name, int64_t val) {
    return content_init(name, ARG_TYPE_INT, (uint8_t*)&val, sizeof(val), NULL);
}

Arg* arg_setNull(Arg* self) {
    return content_init("", ARG_TYPE_NULL, NULL, 0, NULL);
}

Arg* arg_setFloat(Arg* self, char* name, float val) {
    return content_init(name, ARG_TYPE_FLOAT, (uint8_t*)&val, sizeof(val),
                        NULL);
}

float arg_getFloat(Arg* self) {
    if (NULL == arg_getContent(self)) {
        return -999.999;
    }

    return *(float*)(((__arg*)self)->content);
}

Arg* arg_setPtr(Arg* self, char* name, ArgType type, void* pointer) {
    return content_init(name, type, (uint8_t*)&pointer, sizeof(uintptr_t),
                        NULL);
}

Arg* arg_setStr(Arg* self, char* name, char* string) {
    return content_init(name, ARG_TYPE_STRING, (uint8_t*)string,
                        strGetSize(string) + 1, NULL);
}

int64_t arg_getInt(Arg* self) {
    if (NULL == arg_getContent(self)) {
        return -999999;
    }
    return *(int64_t*)(((__arg*)self)->content);
}

void* arg_getPtr(Arg* self) {
    if (NULL == arg_getContent(self)) {
        return NULL;
    }
    return *(void**)(((__arg*)self)->content);
}
char* arg_getStr(Arg* self) {
    return (char*)arg_getContent(self);
}

Hash arg_getNameHash(Arg* self) {
    if (NULL == self) {
        return 999999;
    }
    return content_getNameHash(self);
}

ArgType arg_getType(Arg* self) {
    if (NULL == self) {
        return ARG_TYPE_NULL;
    }
    return content_getType(self);
}

uint16_t arg_getContentSize(Arg* self) {
    return content_getSize(self);
}

Arg* New_arg(void* voidPointer) {
    return NULL;
}

Arg* arg_copy(Arg* argToBeCopy) {
    if (NULL == argToBeCopy) {
        return NULL;
    }
    if (ARG_TYPE_OBJECT == arg_getType(argToBeCopy)) {
        obj_refcntInc(arg_getPtr(argToBeCopy));
    }
    Arg* argCopied = New_arg(NULL);
    argCopied = arg_setContent(argCopied, arg_getContent(argToBeCopy),
                               arg_getContentSize(argToBeCopy));
    argCopied = arg_setNameHash(argCopied, arg_getNameHash(argToBeCopy));
    argCopied = arg_setType(argCopied, arg_getType(argToBeCopy));
    return argCopied;
}

Arg* arg_append(Arg* arg_in, void* new_content, size_t new_size) {
    uint8_t* old_content = arg_getContent(arg_in);
    size_t old_size = arg_getContentSize(arg_in);
    /* create arg_out */
    Arg* arg_out = arg_setContent(NULL, NULL, old_size + new_size);
    arg_setType(arg_out, arg_getType(arg_in));
    arg_setNameHash(arg_out, arg_getNameHash(arg_in));
    /* copy old content */
    __platform_memcpy(arg_getContent(arg_out), old_content, old_size);
    /* copy new content */
    __platform_memcpy(arg_getContent(arg_out) + old_size, new_content,
                      new_size);
    arg_deinit(arg_in);
    return arg_out;
}

void* arg_getHeapStruct(Arg* self) {
    return arg_getContent(self) + sizeof(void*);
}

void arg_deinitHeap(Arg* self) {
    if (arg_getType(self) == ARG_TYPE_HEAP_STRUCT) {
        /* deinit heap strcut */
        StructDeinitFun struct_deinit_fun =
            (StructDeinitFun)arg_getHeapStructDeinitFun(self);
        struct_deinit_fun(arg_getHeapStruct(self));
    }
}

void arg_deinit(Arg* self) {
    arg_deinitHeap(self);
    arg_freeContent(self);
}

void arg_refCntInc(Arg* self) {
    ((__arg*)self)->ref_cnt++;
}

void arg_refCntDec(Arg* self) {
    ((__arg*)self)->ref_cnt--;
}

uint8_t arg_getRefCnt(Arg* self) {
    return ((__arg*)self)->ref_cnt;
}
