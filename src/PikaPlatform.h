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

#ifndef __PIKA_PALTFORM__H
#define __PIKA_PALTFORM__H
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Compiler */
#if defined(__CC_ARM) || defined(__CLANG_ARM) /* ARM Compiler */
#define PIKA_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__) /* for IAR Compiler */
#define PIKA_WEAK __weak
#elif defined(__GNUC__) /* GNU GCC Compiler */
#define PIKA_WEAK __attribute__((weak))
#endif
/* default PIKA_WEAK */
#ifndef PIKA_WEAK
#define PIKA_WEAK __attribute__((weak))
#endif

/* OS */
#ifdef __RTTHREAD__
#include <rtthread.h>
#define __platform_printf(...)  rt_kprintf(__VA_ARGS__)
#endif


/* 
    [Note]:
    Create a pika_config.c to override the following weak functions to config PikaScript.
    [Example]:
    1. https://gitee.com/Lyon1998/pikascript/blob/master/package/STM32G030Booter/pika_config.c
    2. https://gitee.com/Lyon1998/pikascript/blob/master/package/pikaRTBooter/pika_config.c
*/

/* interrupt config */
void    __platform_enable_irq_handle(void);
void    __platform_disable_irq_handle(void);

/* printf family config */
#ifndef __platform_printf
void    __platform_printf(char* fmt, ...);
#endif
int     __platform_sprintf(char* buff, char* fmt, ...);
int     __platform_vsprintf(char* buff, char* fmt, va_list args);
int     __platform_vsnprintf(char* buff,
                        size_t size,
                        const char* fmt,
                        va_list args);

/* libc config */
void*   __platform_malloc(size_t size);
void    __platform_free(void* ptr);
void*   __platform_memset(void *mem, int ch, size_t size);
void*   __platform_memcpy(void* dir, const void* src, size_t size);

/* pika memory pool config */
void    __platform_pool_init(void);
void    __platform_wait(void);
void*   __impl_pikaMalloc(size_t size);
void    __impl_pikaFree(void* ptrm, size_t size);
uint8_t __is_locked_pikaMemory(void);
uint8_t __is_quick_malloc(void);
void    __quick_malloc_enable(void);
void    __quick_malloc_disable(void);
#endif

/* pika asm config */
int32_t __platform_save_pikaAsm(char* PikaAsm);
uint8_t __platform_Asm_is_to_flash(char* pyMultiLine);
int32_t __platform_save_pikaAsm_EOF(void);
char*   __platform_load_pikaAsm(void);
