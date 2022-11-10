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

PIKA_WEAK void* __platform_realloc(void* ptr, size_t size) {
    return realloc(ptr, size);
}

PIKA_WEAK void* __platform_calloc(size_t num, size_t size) {
    return calloc(num, size);
}

PIKA_WEAK void __platform_free(void* ptr) {
    free(ptr);
}

PIKA_WEAK void* __user_malloc(size_t size) {
    return __platform_malloc(size);
}

PIKA_WEAK void __user_free(void* ptr, size_t size) {
    __platform_free(ptr);
}

PIKA_WEAK void __platform_error_handle() {
    return;
}

PIKA_WEAK uint8_t __is_locked_pikaMemory(void) {
    return 0;
}

PIKA_WEAK int64_t __platform_getTick(void) {
    return -1;
}

#ifndef __platform_printf
PIKA_WEAK void __platform_printf(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
#endif

PIKA_WEAK int __platform_vsprintf(char* buff, char* fmt, va_list args) {
    return vsprintf(buff, fmt, args);
}

PIKA_WEAK int __platform_snprintf(char* buff,
                                  size_t size,
                                  const char* fmt,
                                  ...) {
    va_list args;
    va_start(args, fmt);
    int ret = __platform_vsnprintf(buff, size, fmt, args);
    va_end(args);
    return ret;
}

PIKA_WEAK char* __platform_strdup(const char* src) {
    char* dst = (char*)__platform_malloc(strlen(src) + 1);
    if (dst) {
        strcpy(dst, src);
    }
    return dst;
}

PIKA_WEAK size_t __platform_tick_from_millisecond(size_t ms) {
    return ms;
}

PIKA_WEAK int __platform_vsnprintf(char* buff,
                                   size_t size,
                                   const char* fmt,
                                   va_list args) {
    return vsnprintf(buff, size, fmt, args);
}

PIKA_WEAK int __platform_sprintf(char* buff, char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int res = vsnprintf(buff, PIKA_SPRINTF_BUFF_SIZE, fmt, args);
    va_end(args);
    if (res >= PIKA_SPRINTF_BUFF_SIZE) {
        __platform_printf(
            "OverflowError: sprintf buff size overflow, please use bigger "
            "PIKA_SPRINTF_BUFF_SIZE\r\n");
        __platform_printf("Info: buff size request: %d\r\n", res);
        __platform_printf("Info: buff size now: %d\r\n",
                          PIKA_SPRINTF_BUFF_SIZE);
        while (1)
            ;
    }
    return res;
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

PIKA_WEAK int __platform_memcmp(const void* s1, const void* s2, size_t n) {
    return memcmp(s1, s2, n);
}

PIKA_WEAK void* __platform_memmove(void* s1, void* s2, size_t n) {
    return memmove(s1, s2, n);
}

PIKA_WEAK char __platform_getchar(void) {
#if defined(__linux) || defined(_WIN32)
    return getchar();
#else
    __platform_printf("Error: __platform_getchar need implementation!\r\n");
    while (1) {
    }
#endif
}

/* fopen */
PIKA_WEAK FILE* __platform_fopen(const char* filename, const char* modes) {
#if defined(__linux) || defined(_WIN32)
    return fopen(filename, modes);
#else
    __platform_printf("Error: __platform_fopen need implementation!\r\n");
    while (1) {
    }
#endif
}

/* fclose */
PIKA_WEAK int __platform_fclose(FILE* stream) {
#if defined(__linux) || defined(_WIN32)
    return fclose(stream);
#else
    __platform_printf("Error: __platform_fclose need implementation!\r\n");
    while (1) {
    }
#endif
}

/* fwrite */
PIKA_WEAK size_t __platform_fwrite(const void* ptr,
                                   size_t size,
                                   size_t n,
                                   FILE* stream) {
#if defined(__linux) || defined(_WIN32)
    return fwrite(ptr, size, n, stream);
#else
    __platform_printf("Error: __platform_fwrite need implementation!\r\n");
    while (1) {
    }
#endif
}

/* fread */
PIKA_WEAK size_t __platform_fread(void* ptr,
                                  size_t size,
                                  size_t n,
                                  FILE* stream) {
#if defined(__linux) || defined(_WIN32)
    return fread(ptr, size, n, stream);
#else
    __platform_printf("Error: __platform_fread need implementation!\r\n");
    while (1) {
    }
#endif
}

/* fseek */
PIKA_WEAK int __platform_fseek(FILE* stream, long offset, int whence) {
#if defined(__linux) || defined(_WIN32)
    return fseek(stream, offset, whence);
#else
    __platform_printf("Error: __platform_fseek need implementation!\r\n");
    while (1) {
    }
#endif
}

/* ftell */
PIKA_WEAK long __platform_ftell(FILE* stream) {
#if defined(__linux) || defined(_WIN32)
    return ftell(stream);
#else
    __platform_printf("Error: __platform_ftell need implementation!\r\n");
    while (1) {
    }
#endif
}

PIKA_WEAK void __pks_hook_instruct(void) {
    return;
}

PIKA_WEAK PIKA_BOOL __pks_hook_arg_cache_filter(void* self) {
    return PIKA_TRUE;
}

PIKA_WEAK void __platform_thread_delay(void) {
    return;
}
