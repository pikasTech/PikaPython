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

/* micro pika configuration */
#include "./pika_config_valid.h"

#ifndef __PIKA_PALTFORM__H
#define __PIKA_PALTFORM__H
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* clang-format off */
#if PIKA_ASSERT_ENABLE
    #define pika_assert(expr) \
    if(!(expr)) { \
        __platform_printf("Assertion \"%s\" failed, in function: %s(). \r\n  (at %s:%d)\n", #expr, __FUNCTION__, __FILE__, __LINE__); \
        abort(); \
    }
#else
    #define pika_assert(...)
#endif
/* clang-format on */

/* Compiler */
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 5000000) /* ARM Compiler \
                                                              */
#define PIKA_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__) /* for IAR Compiler */
#define PIKA_WEAK __weak
#elif defined(__MINGW32__) /* MINGW32 Compiler */
#define PIKA_WEAK
#elif defined(__GNUC__) /* GNU GCC Compiler */
#define PIKA_WEAK __attribute__((weak))
#endif
/* default PIKA_WEAK */
#ifndef PIKA_WEAK
#define PIKA_WEAK
#endif

/* align for bytecode */
#if defined(_WIN32)
#define PIKA_BYTECODE_ALIGN
#else
#define PIKA_BYTECODE_ALIGN __attribute__((aligned(4)))
#endif

/* OS */
#ifdef __RTTHREAD__
#include <rtthread.h>
#define __platform_printf(...) rt_kprintf(__VA_ARGS__)
#endif

typedef enum {
    PIKA_RES_OK = 0,
    PIKA_RES_ERR_RUNTIME_ERROR = -1,
    PIKA_RES_ERR_ARG_NO_FOUND = -2,
    PIKA_RES_ERR_UNKNOWN_INSTRUCTION = -3,
    PIKA_RES_ERR_OUT_OF_RANGE = -4,
    PIKA_RES_ERR_IO_ERROR = -5,
    PIKA_RES_ERR_INSUFFICIENT_RESOURCE = -6,
    PIKA_RES_ERR_INVALID_PARAM = -7,
    PIKA_RES_ERR_INVALID_PTR = -8,
    PIKA_RES_ERR_UNALIGNED_PTR = -9,
    PIKA_RES_ERR_INVALID_VERSION_NUMBER = -10,
    PIKA_RES_ERR_ILLEGAL_MAGIC_CODE = -11,
    PIKA_RES_ERR_OPERATION_FAILED = -12,
    PIKA_RES_ERR_UNKNOWN = -13,
    PIKA_RES_ERR_SYNTAX_ERROR = -14,
    PIKA_RES_ERR_IO = -15,
    PIKA_RES_ERR_ASSERT = -16,
    PIKA_RES_ERR_SIGNAL_EVENT_FULL = -17,
    PIKA_RES_ERR_SIGNAL_EVENT_EMPTY = -18,
} PIKA_RES;

/* clang-format off */

/* pikascript bool type */
typedef enum {
    PIKA_TRUE   = 1,
    PIKA_FALSE  = 0,
} PIKA_BOOL;

/* clang-format on */

/*
    [Note]:
    Create a pika_config.c to override the following weak functions to config
   PikaScript. [Example]:
    1.
   https://gitee.com/Lyon1998/pikascript/blob/master/package/STM32G030Booter/pika_config.c
    2.
   https://gitee.com/Lyon1998/pikascript/blob/master/package/pikaRTBooter/pika_config.c
*/

/* interrupt config */
void __platform_enable_irq_handle(void);
void __platform_disable_irq_handle(void);

/* printf family config */
#ifndef __platform_printf
void __platform_printf(char* fmt, ...);
#endif
int __platform_sprintf(char* buff, char* fmt, ...);
int __platform_vsprintf(char* buff, char* fmt, va_list args);
int __platform_vsnprintf(char* buff,
                         size_t size,
                         const char* fmt,
                         va_list args);
int __platform_snprintf(char* buff, size_t size, const char* fmt, ...);
char* __platform_strdup(const char* src);
size_t __platform_tick_from_millisecond(size_t ms);

/* libc config */
void* __platform_malloc(size_t size);
void* __platform_realloc(void* ptr, size_t size);
void* __platform_calloc(size_t num, size_t size);
void __platform_free(void* ptr);
void* __platform_memset(void* mem, int ch, size_t size);
void* __platform_memcpy(void* dir, const void* src, size_t size);
int __platform_memcmp(const void* s1, const void* s2, size_t n);
void* __platform_memmove(void* s1, void* s2, size_t n);

void* __user_malloc(size_t size);
void __user_free(void* ptr, size_t size);

/* pika memory pool config */
void __platform_wait(void);
uint8_t __is_locked_pikaMemory(void);

/* support shell */
char __platform_getchar(void);

/* file API */
FILE* __platform_fopen(const char* filename, const char* modes);
int __platform_fclose(FILE* stream);
size_t __platform_fwrite(const void* ptr, size_t size, size_t n, FILE* stream);
size_t __platform_fread(void* ptr, size_t size, size_t n, FILE* stream);
int __platform_fseek(FILE* stream, long offset, int whence);
long __platform_ftell(FILE* stream);

/* error */
void __platform_error_handle(void);

void __pks_hook_instruct(void);
PIKA_BOOL __pks_hook_arg_cache_filter(void* self);
PIKA_WEAK void __platform_thread_delay(void);

#if PIKA_FLOAT_TYPE_DOUBLE
#define pika_float double
#else
#define pika_float float
#endif

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

#endif
