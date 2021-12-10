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

#include "PikaPlatform.h"
#include <stdio.h>
#include <stdlib.h>

PIKA_WEAK void __platform_disable_irq_handle(void) {
    /* disable irq to support thread */
}
PIKA_WEAK void __platform_enable_irq_handle(void) {
    /* disable irq to support thread */
}
PIKA_WEAK void* __platform_malloc(size_t size) {
    return malloc(size);
}
PIKA_WEAK void __platform_free(void* ptr) {
    free(ptr);
}
PIKA_WEAK void* __impl_pikaMalloc(size_t size) {
    return __platform_malloc(size);
}
PIKA_WEAK void __impl_pikaFree(void* ptrm, size_t size) {
    __platform_free(ptrm);
}
PIKA_WEAK void __platform_pool_init(void) {}
PIKA_WEAK uint8_t __is_quick_malloc(void) {
    return 0;
}
PIKA_WEAK void __quick_malloc_enable(void) {}
PIKA_WEAK void __quick_malloc_disable(void) {}
PIKA_WEAK uint8_t __is_locked_pikaMemory(void) {
    return 0;
}

#ifndef __platform_printf
PIKA_WEAK void __platform_printf(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
#endif

PIKA_WEAK int __platform_sprintf(char* buff, char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int res = vsprintf(buff, fmt, args);
    va_end(args);
    return res;
}

PIKA_WEAK int __platform_vsprintf(char* buff, char* fmt, va_list args) {
    return vsprintf(buff, fmt, args);
}

PIKA_WEAK int __platform_vsnprintf(char* buff,
                                   size_t size,
                                   const char* fmt,
                                   va_list args) {
    return vsnprintf(buff, size, fmt, args);
}

PIKA_WEAK char* __platform_load_pikaAsm(void) {
    /* faild */
    return NULL;
}
PIKA_WEAK int32_t __platform_save_pikaAsm(char* PikaAsm) {
    /* faild */
    return 1;
}
PIKA_WEAK uint8_t __platform_Asm_is_to_flash(char* pyMultiLine) {
    /* not to flash */
    return 0;
}
PIKA_WEAK int32_t __platform_save_pikaAsm_EOF(void) {
    return 1;
}
PIKA_WEAK void __platform_wait(void) {
    while (1) {
    };
}
PIKA_WEAK void* __platform_memset(void* mem, int ch, size_t size) {
    return memset(mem, ch, size);
}

PIKA_WEAK void* __platform_memcpy(void* dir, const void* src, size_t size) {
    return memcpy(dir, src, size);
}
