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
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataString.h"
#include "stdlib.h"




void arg_deinit(Arg* self) {
    arg_freeContent(self);
}

uint16_t arg_getTotleSize(Arg* self) {
    return content_totleSize(self);
}

#if 0
void content_setNext(uint8_t* self, uint8_t* next) {
    uint8_t* nextDir = self + content_nextOffset(self);
    uintptr_t* p_next = (uintptr_t*)nextDir;
    *p_next = (uintptr_t)next;
}
#endif

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

static 
uint8_t* content_init_hash(Hash nameHash,
                           ArgType type,
                           uint8_t* content,
                           uint32_t size,
                           uint8_t* next) {
    const uint8_t nextLength = sizeof(uint8_t*);
    const uint8_t sizeLength = sizeof(uint32_t);
    uint16_t nameSize = sizeof(Hash);     // use hash
    uint16_t typeSize = sizeof(ArgType);  // use enum
    __arg * self = (__arg *)pikaMalloc(nextLength + sizeLength + nameSize +
                                         size + typeSize);

    self->next = (__arg *)next;
    self->size = size;
    self->name_hash = nameHash;
    
    if (NULL != content) {
        __platform_memcpy(self->content, content, size);
    } else {
        __platform_memset(self->content, 0, size);
    }
    
    (*(ArgType *)((uintptr_t)(self->content) + size)) = type;
    
    return (uint8_t *)self;
}

//! todo: should be static
uint8_t* content_init(char* name,
                      ArgType type,
                      uint8_t* content,
                      uint16_t size,
                      uint8_t* next) {
    Hash nameHash = hash_time33(name);
    return content_init_hash(nameHash, type, content, size, next);
}

uint16_t content_totleSize(uint8_t* self) {
    
    return ((__arg *)self)->size + sizeof(ArgType) + sizeof(__arg);
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
        return content_init("", TYPE_NONE, content, size, NULL);
    }
    
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
        return content_init_hash(nameHash, TYPE_NONE, NULL, 0, NULL);
    }
    __arg * arg = (__arg *)self;
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

    __arg * arg = (__arg *)self;
    (*(ArgType *)((uintptr_t)(arg->content) + arg->size)) = type;
    
    return self;
}

Arg* arg_newContent(Arg* self, uint32_t size) {
    uint8_t* newContent = content_init("", TYPE_NONE, NULL, size, NULL);
    arg_freeContent(self);
    return newContent;
}

Arg* arg_setContent(Arg* self, uint8_t* content, uint32_t size) {
    return content_setContent(self, content, size);
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

ArgType content_getType(uint8_t* self) {
    __arg * arg = (__arg *)self;
    return (*(ArgType *)((uintptr_t)(arg->content) + arg->size));
}



uint8_t* arg_getContent(Arg* self) {
    return content_getContent(self);
}

Arg* arg_setInt(Arg* self, char* name, int64_t val) {
    return content_init(name, TYPE_INT, (uint8_t *)&val, sizeof(val), NULL);
}

Arg* arg_setNull(Arg* self) {
    return content_init("", TYPE_NULL, NULL, 0, NULL);
}

Arg* arg_setFloat(Arg* self, char* name, float val) {
    return content_init(name, TYPE_FLOAT, (uint8_t *)&val, sizeof(val), NULL);
}

float arg_getFloat(Arg* self) {
    if (NULL == arg_getContent(self)) {
        return -999.999;
    }

    return *(float *)(((__arg *)self)->content);
}

Arg* arg_setPtr(Arg* self, char* name, ArgType type, void* pointer) {

    return content_init(name, type, (uint8_t *)&pointer, sizeof(uintptr_t), NULL);
}

Arg* arg_setStr(Arg* self, char* name, char* string) {
    return content_init(name, TYPE_STRING, (uint8_t*)string,
                        strGetSize(string) + 1, NULL);
}

int64_t arg_getInt(Arg* self) {
    if (NULL == arg_getContent(self)) {
        return -999999;
    }

    return *(int64_t *)(((__arg *)self)->content);
}

void* arg_getPtr(Arg* self) {
    if (NULL == arg_getContent(self)) {
        return NULL;
    }
    return *(void **)(((__arg *)self)->content);
}
char* arg_getStr(Arg* self) {
    return (char*)arg_getContent(self);
}

uint16_t content_typeOffset(uint8_t* self) {
    const uint8_t nextLength = sizeof(uint8_t*);
    const uint8_t sizeLength = sizeof(uint32_t);
    uint16_t size = content_getSize(self);
    uint16_t nameSize = sizeof(Hash);
    return nextLength + sizeLength + nameSize + size;
}

Hash arg_getNameHash(Arg* self) {
    if (NULL == self) {
        return 999999;
    }
    return content_getNameHash(self);
}

ArgType arg_getType(Arg* self) {
    if (NULL == self) {
        return TYPE_NONE;
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
    Arg* argCopied = New_arg(NULL);
    argCopied = arg_setContent(argCopied, arg_getContent(argToBeCopy),
                               arg_getContentSize(argToBeCopy));
    argCopied = arg_setNameHash(argCopied, arg_getNameHash(argToBeCopy));
    argCopied = arg_setType(argCopied, arg_getType(argToBeCopy));
    return argCopied;
}
