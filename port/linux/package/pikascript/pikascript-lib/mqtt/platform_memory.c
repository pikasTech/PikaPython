/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-14 22:02:07
 * @LastEditTime: 2020-02-19 20:26:04
 * @Description: the code belongs to jiejie, please keep the author
 * information and source code according to the license.
 */
#include "platform_memory.h"

void* platform_memory_alloc(size_t size) {
    return __platform_malloc(size);
}

void* platform_memory_calloc(size_t num, size_t size) {
    return __platform_calloc(num, size);
}

void platform_memory_free(void* ptr) {
    __platform_free(ptr);
}
