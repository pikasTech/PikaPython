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

static PIKA_BOOL _arg_cache_push(Arg* self, uint32_t size) {
#if !PIKA_ARG_CACHE_ENABLE
    return PIKA_FALSE;
#else
    if (PIKA_FALSE == __pks_hook_arg_cache_filter(self)) {
        return PIKA_FALSE;
    }
    extern PikaMemInfo pikaMemInfo;
    if (self->heap_size < PIKA_ARG_CACHE_SIZE ||
        self->heap_size > 2 * PIKA_ARG_CACHE_SIZE) {
        return PIKA_FALSE;
    }
    if (PIKA_ARG_CACHE_POOL_SIZE <= pikaMemInfo.cache_pool_top) {
        return PIKA_FALSE;
    }
    pikaMemInfo.cache_pool[pikaMemInfo.cache_pool_top++] = (uint8_t*)self;
    pikaMemInfo.heapUsed -= mem_align(sizeof(Arg) + size);
    return PIKA_TRUE;
#endif
}

static Arg* _arg_cache_pop(uint32_t size) {
#if !PIKA_ARG_CACHE_ENABLE
    return NULL;
#else
    uint32_t req_heap_size = mem_align(sizeof(Arg) + size);
    extern PikaMemInfo pikaMemInfo;
    if (req_heap_size > PIKA_ARG_CACHE_SIZE) {
        return NULL;
    }
    if (!(pikaMemInfo.cache_pool_top > 0)) {
        return NULL;
    }
    --pikaMemInfo.cache_pool_top;
    Arg* self = (Arg*)pikaMemInfo.cache_pool[pikaMemInfo.cache_pool_top];
    pikaMemInfo.heapUsed += mem_align(sizeof(Arg) + size);
    return self;
#endif
}

uint32_t arg_getTotleSize(Arg* self) {
    return arg_totleSize(self);
}

/**
 * time33 hash
 */

Hash hash_time33EndWith(char* str, char end) {
    pika_assert(str != NULL);
    if (*str == 0) {
        return 5381;
    }
    Hash hash = 5381;
    while (*str && *str != end) {
        hash += (hash << 5) + (*str++);
    }
    return (hash & 0x7FFFFFFF);
}

Hash hash_time33(char* str) {
    pika_assert(str != NULL);
    if (*str == 0) {
        return 5381;
    }
    Hash hash = 5381;
    while (*str) {
        hash += (hash << 5) + (*str++);
    }
    return (hash & 0x7FFFFFFF);
}

static Arg* _arg_set_hash(Arg* self,
                          Hash nameHash,
                          ArgType type,
                          uint8_t* content,
                          uint32_t size,
                          Arg* next) {
    /* create arg if not exist */
    if (NULL == self || self->size < size) {
        self = _arg_cache_pop(size);
        uint32_t heap_size = sizeof(Arg) + size;
#if PIKA_ARG_CACHE_ENABLE
        // if (heap_size < PIKA_ARG_CACHE_SIZE) {
        //     heap_size = PIKA_ARG_CACHE_SIZE;
        // }
        extern PikaMemInfo pikaMemInfo;
        pikaMemInfo.alloc_times++;
        pikaMemInfo.alloc_times_cache++;
#endif
        if (NULL == self) {
            self = (Arg*)pikaMalloc(heap_size);
#if PIKA_ARG_CACHE_ENABLE
            extern PikaMemInfo pikaMemInfo;
            pikaMemInfo.alloc_times_cache--;
            self->heap_size = mem_align(heap_size);
#endif
        }
        self->size = size;
        self->flag = 0;
        arg_setSerialized(self, PIKA_TRUE);
        // arg_setIsKeyword(self, PIKA_FALSE);
        arg_setNext(self, next);
    }
    self->name_hash = nameHash;
    self->type = type;
    if (NULL != content) {
        __platform_memcpy(arg_getContent(self), content, size);
    } else {
        __platform_memset(arg_getContent(self), 0,
                          aline_by(size, sizeof(uint32_t)));
    }
    pika_assert(self->flag < ARG_FLAG_MAX);
    return self;
}

static Arg* arg_create_hash(Hash nameHash,
                            ArgType type,
                            uint8_t* content,
                            uint32_t size,
                            Arg* next) {
    return _arg_set_hash(NULL, nameHash, type, content, size, next);
}

static Arg* arg_create(char* name,
                       ArgType type,
                       uint8_t* content,
                       uint32_t size,
                       Arg* next) {
    Hash nameHash = hash_time33(name);
    return arg_create_hash(nameHash, type, content, size, next);
}

static Arg* arg_set(Arg* self,
                    char* name,
                    ArgType type,
                    uint8_t* content,
                    uint32_t size) {
    Hash nameHash = hash_time33(name);
    return _arg_set_hash(self, nameHash, type, content, size, NULL);
}

void arg_init_stack(Arg* self, uint8_t* buffer, uint32_t size) {
    self->_.buffer = buffer;
    self->size = size;
}

uint32_t arg_totleSize(Arg* self) {
    return ((Arg*)self)->size + sizeof(Arg);
}

void arg_freeContent(Arg* self) {
    pika_assert(NULL != self);
    if (_arg_cache_push(self, self->size)) {
        return;
    }
    pikaFree(self, arg_totleSize(self));
    return;
}

Arg* arg_setContent(Arg* self, uint8_t* content, uint32_t size) {
    if (NULL == self) {
        /* malloc */
        return arg_create("", ARG_TYPE_NONE, content, size, NULL);
    }

    /* only copy */
    if (arg_getSize(self) >= size) {
        __platform_memcpy(arg_getContent((Arg*)self), content, size);
        return self;
    }

    /* realloc */
    Hash nameHash = arg_getNameHash(self);
    ArgType type = arg_getType(self);
    Arg* next = arg_getNext(self);
    Arg* newContent = arg_create_hash(nameHash, type, content, size, next);
    arg_freeContent(self);
    return newContent;
}

Arg* arg_setNameHash(Arg* self, Hash nameHash) {
    if (NULL == self) {
        return arg_create_hash(nameHash, ARG_TYPE_NONE, NULL, 0, NULL);
    }
    Arg* arg = (Arg*)self;
    arg->name_hash = nameHash;
    return self;
}

Arg* arg_setName(Arg* self, char* name) {
    pika_assert(NULL != name);
    return arg_setNameHash(self, hash_time33(name));
}

Arg* arg_setBytes(Arg* self, char* name, uint8_t* src, size_t size) {
    self = arg_newContent(size + sizeof(size_t) + 1);
    if (NULL == self) {
        return NULL;
    }
    self = arg_setName(self, name);
    pika_assert(NULL != self);
    arg_setType(self, ARG_TYPE_BYTES);
    void* dir = arg_getContent(self);
    /* set content all to 0 */
    __platform_memset(dir, 0, size + sizeof(size_t) + 1);
    /* setsize */
    __platform_memcpy(dir, &size, sizeof(size_t));

    /* set init value */
    if (NULL != src) {
        __platform_memcpy((void*)((uintptr_t)dir + sizeof(size_t)), src, size);
    }
    pika_assert(self->flag < ARG_FLAG_MAX);
    return self;
}

Arg* arg_newContent(uint32_t size) {
    Arg* newContent = arg_create("", ARG_TYPE_NONE, NULL, size, NULL);
    return newContent;
}

uint8_t* arg_getBytes(Arg* self) {
    return arg_getContent(self) + sizeof(size_t);
}

char* __printBytes(PikaObj* self, Arg* arg) {
    Args buffs = {0};
    size_t bytes_size = arg_getBytesSize(arg);
    uint8_t* bytes = arg_getBytes(arg);
    Arg* str_arg = arg_newStr("b\'");
    for (size_t i = 0; i < bytes_size; i++) {
        char* str_item = strsFormat(&buffs, 16, "\\x%02x", bytes[i]);
        str_arg = arg_strAppend(str_arg, str_item);
    }
    str_arg = arg_strAppend(str_arg, "\'");
    char* str_res = obj_cacheStr(self, arg_getStr(str_arg));
    strsDeinit(&buffs);
    arg_deinit(str_arg);
    return str_res;
}

void arg_printBytes(Arg* self, char* end) {
    PikaObj* obj = New_PikaObj();
    __platform_printf("%s%s", __printBytes(obj, self), end);
    obj_deinit(obj);
}

void arg_singlePrint(Arg* self, PIKA_BOOL in_REPL, char* end) {
    ArgType type = arg_getType(self);
    if (ARG_TYPE_NONE == type) {
        __platform_printf("None%s", end);
        return;
    }
    if (argType_isObject(type)) {
        char* res = obj_toStr(arg_getPtr(self));
        __platform_printf("%s%s", res, end);
        return;
    }
    if (type == ARG_TYPE_INT) {
#if PIKA_PRINT_LLD_ENABLE
        __platform_printf("%lld%s", (long long int)arg_getInt(self), end);
#else
        __platform_printf("%d%s", (int)arg_getInt(arg), end);
#endif
        return;
    }
    if (type == ARG_TYPE_FLOAT) {
        __platform_printf("%f%s", arg_getFloat(self), end);
        return;
    }
    if (type == ARG_TYPE_STRING) {
        if (in_REPL) {
            __platform_printf("'%s'%s", arg_getStr(self), end);
            return;
        }
        __platform_printf("%s%s", arg_getStr(self), end);
        return;
    }
    if (type == ARG_TYPE_BYTES) {
        arg_printBytes(self, end);
        return;
    }
    if (ARG_TYPE_POINTER == type || ARG_TYPE_METHOD_NATIVE_CONSTRUCTOR) {
        __platform_printf("%p%s", arg_getPtr(self), end);
        return;
    }
    return;
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
    return arg_set(self, name, ARG_TYPE_STRUCT, (uint8_t*)struct_ptr,
                   struct_size);
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
    pika_assert(NULL != struct_arg);
    arg_setType(struct_arg, ARG_TYPE_STRUCT_HEAP);
    struct_arg = arg_setName(struct_arg, name);
    return struct_arg;
}

void* arg_getHeapStructDeinitFun(Arg* self) {
    void* deinit_fun = NULL;
    __platform_memcpy(&deinit_fun, arg_getContent(self), sizeof(void*));
    return deinit_fun;
}

Arg* arg_setInt(Arg* self, char* name, int64_t val) {
    return arg_set(self, name, ARG_TYPE_INT, (uint8_t*)&val, sizeof(val));
}

Arg* arg_setNull(Arg* self) {
    return arg_set(self, "", ARG_TYPE_NONE, NULL, 0);
}

Arg* arg_setFloat(Arg* self, char* name, pika_float val) {
    return arg_set(self, name, ARG_TYPE_FLOAT, (uint8_t*)&val, sizeof(val));
}

pika_float arg_getFloat(Arg* self) {
    if (NULL == arg_getContent(self)) {
        return -999.999;
    }

    return *(pika_float*)arg_getContent(self);
}

Arg* arg_setPtr(Arg* self, char* name, ArgType type, void* pointer) {
    return arg_set(self, name, type, (uint8_t*)&pointer, sizeof(uintptr_t));
}

Arg* arg_setStr(Arg* self, char* name, char* string) {
    if (NULL == string) {
        return NULL;
    }
    return arg_set(self, name, ARG_TYPE_STRING, (uint8_t*)string,
                   strGetSize(string) + 1);
}

int64_t arg_getInt(Arg* self) {
    pika_assert(NULL != self);
    if (NULL == arg_getContent(self)) {
        return -999999;
    }
    return *(int64_t*)arg_getContent(self);
}

void* arg_getPtr(Arg* self) {
    if (arg_getType(self) == ARG_TYPE_NONE) {
        return NULL;
    }
    if (NULL == arg_getContent(self)) {
        return NULL;
    }
    return *(void**)arg_getContent(self);
}
char* arg_getStr(Arg* self) {
    return (char*)arg_getContent(self);
}

uint32_t arg_getContentSize(Arg* self) {
    return arg_getSize(self);
}

Arg* New_arg(void* voidPointer) {
    return NULL;
}

Arg* arg_copy(Arg* arg_src) {
    if (NULL == arg_src) {
        return NULL;
    }
    pika_assert(arg_src->flag < ARG_FLAG_MAX);
    ArgType arg_type = arg_getType(arg_src);
    if (ARG_TYPE_OBJECT == arg_type) {
        obj_refcntInc((PikaObj*)arg_getPtr(arg_src));
    }
    Arg* arg_dict = New_arg(NULL);
    arg_dict = arg_setContent(arg_dict, arg_getContent(arg_src),
                              arg_getContentSize(arg_src));
    arg_dict = arg_setNameHash(arg_dict, arg_getNameHash(arg_src));
    pika_assert(NULL != arg_dict);
    arg_setType(arg_dict, arg_getType(arg_src));
    arg_setIsKeyword(arg_dict, arg_getIsKeyword(arg_src));
    return arg_dict;
}

Arg* arg_copy_noalloc(Arg* arg_src, Arg* arg_dict) {
    if (NULL == arg_src) {
        return NULL;
    }
    if (NULL == arg_dict) {
        return arg_copy(arg_src);
    }
    /* size is too big to be copied by noalloc */
    if (arg_getSize(arg_src) > arg_getSize(arg_dict)) {
        return arg_copy(arg_src);
    }
    ArgType arg_type = arg_getType(arg_src);
    if (ARG_TYPE_OBJECT == arg_type) {
        obj_refcntInc((PikaObj*)arg_getPtr(arg_src));
    }
    arg_setSerialized(arg_dict, PIKA_FALSE);
    arg_dict = arg_setContent(arg_dict, arg_getContent(arg_src),
                              arg_getContentSize(arg_src));
    arg_dict = arg_setNameHash(arg_dict, arg_getNameHash(arg_src));
    pika_assert(NULL != arg_dict);
    arg_setType(arg_dict, arg_getType(arg_src));
    arg_setIsKeyword(arg_dict, arg_getIsKeyword(arg_src));
    return arg_dict;
}

Arg* arg_append(Arg* self, void* new_content, size_t new_size) {
    uint8_t* old_content = arg_getContent(self);
    size_t old_size = arg_getContentSize(self);
    Arg* new_arg = NULL;
#if PIKA_ARG_CACHE_ENABLE
    /* create arg_out */
    if (self->heap_size > mem_align(sizeof(Arg) + old_size + new_size)) {
        new_arg = self;
        new_arg->size = old_size + new_size;
        extern PikaMemInfo pikaMemInfo;
        pikaMemInfo.heapUsed += mem_align(sizeof(Arg) + old_size + new_size) -
                                mem_align(sizeof(Arg) + old_size);
    }
#endif
    if (NULL == new_arg) {
        new_arg = arg_setContent(NULL, NULL, old_size + new_size);
    }
    pika_assert(NULL != new_arg);
    arg_setType(new_arg, arg_getType(self));
    arg_setNameHash(new_arg, arg_getNameHash(self));
    if (self != new_arg) {
        /* copy old content */
        __platform_memcpy(arg_getContent(new_arg), old_content, old_size);
    }
    /* copy new content */
    __platform_memcpy(arg_getContent(new_arg) + old_size, new_content,
                      new_size);
    if (self != new_arg) {
        arg_deinit(self);
    }
    return new_arg;
}

void* arg_getHeapStruct(Arg* self) {
    return arg_getContent(self) + sizeof(void*);
}

void arg_deinitHeap(Arg* self) {
    if (arg_getIsWeakRef(self)) {
        return;
    }
    ArgType type = arg_getType(self);
    /* deinit heap struct */
    if (type == ARG_TYPE_STRUCT_HEAP) {
        /* deinit heap strcut */
        StructDeinitFun struct_deinit_fun =
            (StructDeinitFun)arg_getHeapStructDeinitFun(self);
        struct_deinit_fun(arg_getHeapStruct(self));
        return;
    }
    /* deinit sub object */
    if (ARG_TYPE_OBJECT == type) {
        PikaObj* subObj = arg_getPtr(self);
        obj_refcntDec(subObj);
        int ref_cnt = obj_refcntNow(subObj);
        if (ref_cnt <= 0) {
            obj_deinit(subObj);
        }
        return;
    }
}

/* load file as byte array */
Arg* arg_loadFile(Arg* self, char* filename) {
    size_t file_size = 0;
    char* file_buff = __platform_malloc(PIKA_READ_FILE_BUFF_SIZE);
    Arg* res = New_arg(NULL);
    __platform_memset(file_buff, 0, PIKA_READ_FILE_BUFF_SIZE);
    FILE* input_file = __platform_fopen(filename, "rb");
    if (NULL == input_file) {
        __platform_printf("Error: Couldn't open file '%s'\n", filename);
        res = NULL;
        goto exit;
    }
    file_size =
        __platform_fread(file_buff, 1, PIKA_READ_FILE_BUFF_SIZE, input_file);

    if (file_size >= PIKA_READ_FILE_BUFF_SIZE) {
        __platform_printf("Error: Not enough buff for input file.\r\n");
        return NULL;
    }
    /* add '\0' to the end of the string */
    res = arg_setBytes(res, "", (uint8_t*)file_buff, file_size + 1);

exit:
    __platform_free(file_buff);
    if (NULL != input_file) {
        __platform_fclose(input_file);
    }
    return res;
}

void arg_deinit(Arg* self) {
    pika_assert(NULL != self);
    /* deinit arg pointed heap */
    arg_deinitHeap(self);
    if (!arg_isSerialized(self)) {
        return;
    }
    /* free the ref */
    arg_freeContent(self);
}

PIKA_BOOL arg_isEqual(Arg* self, Arg* other) {
    if (NULL == self || NULL == other) {
        return PIKA_FALSE;
    }
    if (arg_getType(self) != arg_getType(other)) {
        return PIKA_FALSE;
    }
    if (arg_getType(self) == ARG_TYPE_OBJECT) {
        if (arg_getPtr(self) != arg_getPtr(other)) {
            return PIKA_FALSE;
        }
    }
    if (arg_getType(self) == ARG_TYPE_STRING) {
        if (strEqu(arg_getStr(self), arg_getStr(other))) {
            return PIKA_TRUE;
        }
    }
    if (0 != __platform_memcmp(arg_getContent(self), arg_getContent(other),
                               arg_getContentSize(self))) {
        return PIKA_FALSE;
    }
    return PIKA_TRUE;
}
