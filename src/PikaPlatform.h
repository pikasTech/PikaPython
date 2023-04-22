/*
 * This file is part of the PikaPython project.
 * http://github.com/pikastech/pikapython
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
#ifdef __cplusplus
extern "C" {
#endif

/* adapter for old api */
#include "./pika_adapter_old_api.h"

#ifndef __PIKA_PALTFORM__H
#define __PIKA_PALTFORM__H
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef __linux
#include <unistd.h>
#endif

/* clang-format off */
#if PIKA_ASSERT_ENABLE
    #define pika_assert(expr) \
    if(!(expr)) { \
        pika_platform_printf((char*)"Assertion \"%s\" failed, in function: %s(). \r\n  (at %s:%d)\n", #expr, __FUNCTION__, __FILE__, __LINE__); \
        pika_platform_abort_handler(); \
        abort(); \
    }
#else
    #define pika_assert(...) (void)0;
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
#define pika_platform_printf(...) rt_kprintf(__VA_ARGS__)
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
    PIKA_RES_ERR_INDEX = -19,
} PIKA_RES;

/* clang-format off */

/* pikascript bool type */
#define PIKA_BOOL int64_t
#define PIKA_TRUE 1
#define PIKA_FALSE 0
#define _PIKA_BOOL_ERR -1

#define _PIKA_INT_ERR (-999999999)
#define _PIKA_FLOAT_ERR (-999999999.0)

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
void pika_platform_enable_irq_handle(void);
void pika_platform_disable_irq_handle(void);
void pika_platform_abort_handler(void);

/* printf family config */
#ifndef pika_platform_printf
void pika_platform_printf(char* fmt, ...);
#endif
int pika_vprintf(char* fmt, va_list args);
int pika_sprintf(char* buff, char* fmt, ...);
int pika_vsprintf(char* buff, char* fmt, va_list args);
int pika_platform_vsnprintf(char* buff,
                            size_t size,
                            const char* fmt,
                            va_list args);
int pika_snprintf(char* buff, size_t size, const char* fmt, ...);
char* pika_platform_strdup(const char* src);
size_t pika_platform_tick_from_millisecond(size_t ms);

/* libc config */
void* pika_platform_malloc(size_t size);
void* pika_platform_realloc(void* ptr, size_t size);
void* pika_platform_calloc(size_t num, size_t size);
void pika_platform_free(void* ptr);
void* pika_platform_memset(void* mem, int ch, size_t size);
void* pika_platform_memcpy(void* dir, const void* src, size_t size);
int pika_platform_memcmp(const void* s1, const void* s2, size_t n);
void* pika_platform_memmove(void* s1, void* s2, size_t n);

/* pika memory pool config */
void pika_platform_wait(void);

/* support shell */
char pika_platform_getchar(void);
int pika_platform_putchar(char ch);

/* file API */
FILE* pika_platform_fopen(const char* filename, const char* modes);
int pika_platform_fclose(FILE* stream);
size_t pika_platform_fwrite(const void* ptr,
                            size_t size,
                            size_t n,
                            FILE* stream);
size_t pika_platform_fread(void* ptr, size_t size, size_t n, FILE* stream);
int pika_platform_fseek(FILE* stream, long offset, int whence);
long pika_platform_ftell(FILE* stream);

/* error */
void pika_platform_error_handle(void);

/* panic */
void pika_platform_panic_handle(void);

int64_t pika_platform_get_tick(void);

void pika_platform_sleep_ms(uint32_t ms);

void pika_hook_instruct(void);
PIKA_BOOL pika_hook_arg_cache_filter(void* self);
void* pika_user_malloc(size_t size);
void pika_user_free(void* ptr, size_t size);
uint8_t pika_is_locked_pikaMemory(void);

#if PIKA_FLOAT_TYPE_DOUBLE
#define pika_float double
#else
#define pika_float float
#endif

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

/* Thread Platform */
#ifdef __linux
#include <pthread.h>
typedef struct pika_platform_thread {
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} pika_platform_thread_t;
#elif PIKA_FREERTOS_ENABLE
#include "FreeRTOS.h"
#include "task.h"
typedef struct pika_platform_thread {
    TaskHandle_t thread;
} pika_platform_thread_t;
#else
typedef struct pika_platform_thread {
    void* platform_data;
} pika_platform_thread_t;
#endif

pika_platform_thread_t* pika_platform_thread_init(const char* name,
                                                  void (*entry)(void*),
                                                  void* const param,
                                                  unsigned int stack_size,
                                                  unsigned int priority,
                                                  unsigned int tick);
uint64_t pika_platform_thread_self(void);
void pika_platform_thread_delay(void);
void pika_platform_thread_startup(pika_platform_thread_t* thread);
void pika_platform_thread_stop(pika_platform_thread_t* thread);
void pika_platform_thread_start(pika_platform_thread_t* thread);
void pika_platform_thread_destroy(pika_platform_thread_t* thread);
void pika_platform_thread_exit(pika_platform_thread_t* thread);

#ifdef __linux
#include <pthread.h>
typedef pthread_mutex_t pika_mutex_platform_data_t;
#elif PIKA_FREERTOS_ENABLE
#include "FreeRTOS.h"
#include "semphr.h"
typedef SemaphoreHandle_t pika_mutex_platform_data_t;
#else
typedef void* pika_mutex_platform_data_t;
#endif

typedef struct pika_platform_thread_mutex {
    pika_mutex_platform_data_t mutex;
    volatile int is_init;
    volatile int is_first_lock;
    volatile int lock_times;
} pika_platform_thread_mutex_t;

int pika_platform_thread_mutex_init(pika_platform_thread_mutex_t* m);
int pika_platform_thread_mutex_lock(pika_platform_thread_mutex_t* m);
int pika_platform_thread_mutex_trylock(pika_platform_thread_mutex_t* m);
int pika_platform_thread_mutex_unlock(pika_platform_thread_mutex_t* m);
int pika_platform_thread_mutex_destroy(pika_platform_thread_mutex_t* m);

#ifdef __linux
#include <sys/time.h>
typedef struct pika_platform_timer {
    struct timeval time;
} pika_platform_timer_t;
#elif PIKA_FREERTOS_ENABLE
#include "FreeRTOS.h"
#include "task.h"
typedef struct pika_platform_timer {
    uint32_t time;
} pika_platform_timer_t;
#else
typedef struct pika_platform_timer {
    void* platform_data;
} pika_platform_timer_t;
#endif

void pika_platform_thread_timer_init(pika_platform_timer_t* timer);
void pika_platform_thread_timer_cutdown(pika_platform_timer_t* timer,
                                        unsigned int timeout);
char pika_platform_thread_timer_is_expired(pika_platform_timer_t* timer);
int pika_platform_thread_timer_remain(pika_platform_timer_t* timer);
unsigned long pika_platform_thread_timer_now(void);
void pika_platform_thread_timer_usleep(unsigned long usec);
void pika_platform_reboot(void);
void pika_platform_clear(void);

#define WEAK_FUNCTION_NEED_OVERRIDE_ERROR(_)                               \
    pika_platform_printf("Error: weak function `%s()` need override.\r\n", \
                         __FUNCTION__);                                    \
    pika_platform_panic_handle();

#endif

#ifdef __cplusplus
}
#endif
