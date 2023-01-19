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

PIKA_WEAK void pika_platform_disable_irq_handle(void) {
    /* disable irq to support thread */
}

PIKA_WEAK void pika_platform_enable_irq_handle(void) {
    /* disable irq to support thread */
}

PIKA_WEAK void* pika_platform_malloc(size_t size) {
    return malloc(size);
}

PIKA_WEAK void* pika_platform_realloc(void* ptr, size_t size) {
    return realloc(ptr, size);
}

PIKA_WEAK void* pika_platform_calloc(size_t num, size_t size) {
    return calloc(num, size);
}

PIKA_WEAK void pika_platform_free(void* ptr) {
    free(ptr);
}

PIKA_WEAK void* pika_user_malloc(size_t size) {
    return pika_platform_malloc(size);
}

PIKA_WEAK void pika_user_free(void* ptr, size_t size) {
    pika_platform_free(ptr);
}

PIKA_WEAK void pika_platform_error_handle() {
    return;
}

PIKA_WEAK void pika_platform_panic_handle() {
    while (1) {
    };
}

PIKA_WEAK uint8_t pika_is_locked_pikaMemory(void) {
    return 0;
}

#if PIKA_FREERTOS_ENABLE
static uint32_t platform_uptime_ms(void) {
#if (configTICK_RATE_HZ == 1000)
    return (uint32_t)xTaskGetTickCount();
#else
    TickType_t tick = 0u;

    tick = xTaskGetTickCount() * 1000;
    return (uint32_t)((tick + configTICK_RATE_HZ - 1) / configTICK_RATE_HZ);
#endif
}
#endif

PIKA_WEAK int64_t pika_platform_getTick(void) {
#if PIKA_FREERTOS_ENABLE
    return platform_uptime_ms();
#elif defined(__linux)
    return time(NULL);
#else
    return -1;
#endif
}

PIKA_WEAK int pika_platform_vsprintf(char* buff, char* fmt, va_list args) {
    /* vsnprintf */
    return pika_platform_vsnprintf(buff, PIKA_SPRINTF_BUFF_SIZE, fmt, args);
}

PIKA_WEAK int pika_platform_snprintf(char* buff,
                                     size_t size,
                                     const char* fmt,
                                     ...) {
    va_list args;
    va_start(args, fmt);
    int ret = pika_platform_vsnprintf(buff, size, fmt, args);
    va_end(args);
    return ret;
}

PIKA_WEAK int pika_platform_putchar(char ch) {
    return putchar(ch);
}

PIKA_WEAK int pika_platform_vprintf(char* fmt, va_list args) {
    /* vsprintf to vprintf */
    char buff[PIKA_SPRINTF_BUFF_SIZE];
    pika_platform_vsprintf(buff, fmt, args);
    /* putchar */
    for (int i = 0; i < strlen(buff); i++) {
        pika_platform_putchar(buff[i]);
    }
    return 0;
}

#ifndef pika_platform_printf
PIKA_WEAK void pika_platform_printf(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    pika_platform_vprintf(fmt, args);
    va_end(args);
}
#endif

PIKA_WEAK char* pika_platform_strdup(const char* src) {
    char* dst = (char*)pika_platform_malloc(strlen(src) + 1);
    if (dst) {
        strcpy(dst, src);
    }
    return dst;
}

PIKA_WEAK size_t pika_platform_tick_from_millisecond(size_t ms) {
    return ms;
}

PIKA_WEAK int pika_platform_vsnprintf(char* buff,
                                      size_t size,
                                      const char* fmt,
                                      va_list args) {
    return vsnprintf(buff, size, fmt, args);
}

PIKA_WEAK int pika_platform_sprintf(char* buff, char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int res = pika_platform_vsnprintf(buff, PIKA_SPRINTF_BUFF_SIZE, fmt, args);
    va_end(args);
    if (res >= PIKA_SPRINTF_BUFF_SIZE) {
        pika_platform_printf(
            "OverflowError: sprintf buff size overflow, please use bigger "
            "PIKA_SPRINTF_BUFF_SIZE\r\n");
        pika_platform_printf("Info: buff size request: %d\r\n", res);
        pika_platform_printf("Info: buff size now: %d\r\n",
                             PIKA_SPRINTF_BUFF_SIZE);
        while (1)
            ;
    }
    return res;
}

PIKA_WEAK void pika_platform_wait(void) {
    while (1) {
    };
}

PIKA_WEAK void* pika_platform_memset(void* mem, int ch, size_t size) {
    return memset(mem, ch, size);
}

PIKA_WEAK void* pika_platform_memcpy(void* dir, const void* src, size_t size) {
    return memcpy(dir, src, size);
}

PIKA_WEAK int pika_platform_memcmp(const void* s1, const void* s2, size_t n) {
    return memcmp(s1, s2, n);
}

PIKA_WEAK void* pika_platform_memmove(void* s1, void* s2, size_t n) {
    return memmove(s1, s2, n);
}

PIKA_WEAK char pika_platform_getchar(void) {
#if defined(__linux) || defined(_WIN32)
    return getchar();
#else
    pika_platform_printf(
        "Error: pika_platform_getchar need implementation!\r\n");
    while (1) {
    }
#endif
}

/* fopen */
PIKA_WEAK FILE* pika_platform_fopen(const char* filename, const char* modes) {
#if defined(__linux) || defined(_WIN32)
    return fopen(filename, modes);
#else
    pika_platform_printf("Error: pika_platform_fopen need implementation!\r\n");
    while (1) {
    }
#endif
}

/* fclose */
PIKA_WEAK int pika_platform_fclose(FILE* stream) {
#if defined(__linux) || defined(_WIN32)
    return fclose(stream);
#else
    pika_platform_printf(
        "Error: pika_platform_fclose need implementation!\r\n");
    while (1) {
    }
#endif
}

/* fwrite */
PIKA_WEAK size_t pika_platform_fwrite(const void* ptr,
                                      size_t size,
                                      size_t n,
                                      FILE* stream) {
#if defined(__linux) || defined(_WIN32)
    return fwrite(ptr, size, n, stream);
#else
    pika_platform_printf(
        "Error: pika_platform_fwrite need implementation!\r\n");
    while (1) {
    }
#endif
}

/* fread */
PIKA_WEAK size_t pika_platform_fread(void* ptr,
                                     size_t size,
                                     size_t n,
                                     FILE* stream) {
#if defined(__linux) || defined(_WIN32)
    return fread(ptr, size, n, stream);
#else
    pika_platform_printf("Error: pika_platform_fread need implementation!\r\n");
    while (1) {
    }
#endif
}

/* fseek */
PIKA_WEAK int pika_platform_fseek(FILE* stream, long offset, int whence) {
#if defined(__linux) || defined(_WIN32)
    return fseek(stream, offset, whence);
#else
    pika_platform_printf("Error: pika_platform_fseek need implementation!\r\n");
    while (1) {
    }
#endif
}

/* ftell */
PIKA_WEAK long pika_platform_ftell(FILE* stream) {
#if defined(__linux) || defined(_WIN32)
    return ftell(stream);
#else
    pika_platform_printf("Error: pika_platform_ftell need implementation!\r\n");
    while (1) {
    }
#endif
}

PIKA_WEAK void pika_hook_instruct(void) {
    return;
}

PIKA_WEAK PIKA_BOOL pika_hook_arg_cache_filter(void* self) {
    return PIKA_TRUE;
}

PIKA_WEAK void pika_platform_thread_delay(void) {
    return;
}

PIKA_WEAK void pika_platform_sleep_ms(uint32_t ms) {
    pika_platform_printf(
        "Error: pika_platform_sleep_ms need implementation!\r\n");
    while (1) {
    }
}

PIKA_WEAK void pika_platform_sleep_s(uint32_t s) {
    pika_platform_printf(
        "Error: pika_platform_sleep_s need implementation!\r\n");
    while (1) {
    }
}

/* Thread Support */
PIKA_WEAK pika_platform_thread_t* pika_platform_thread_init(
    const char* name,
    void (*entry)(void*),
    void* const param,
    unsigned int stack_size,
    unsigned int priority,
    unsigned int tick) {
#ifdef __linux
    int res;
    pika_platform_thread_t* thread;
    void* (*thread_entry)(void*);

    thread_entry = (void* (*)(void*))entry;
    thread = pika_platform_malloc(sizeof(pika_platform_thread_t));

    res = pthread_create(&thread->thread, NULL, thread_entry, param);
    if (res != 0) {
        pika_platform_free(thread);
    }

    thread->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    thread->cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

    return thread;
#elif PIKA_FREERTOS_ENABLE
    BaseType_t err;
    pika_platform_thread_t* thread;

    thread = pika_platform_malloc(sizeof(pika_platform_thread_t));

    (void)tick;

    err = xTaskCreate(entry, name, stack_size, param, priority, thread->thread);

    if (pdPASS != err) {
        pika_platform_free(thread);
        return NULL;
    }

    return thread;
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
    return NULL;
#endif
}

PIKA_WEAK void pika_platform_thread_startup(pika_platform_thread_t* thread) {
    (void)thread;
}

PIKA_WEAK void pika_platform_thread_stop(pika_platform_thread_t* thread) {
#ifdef __linux
    pthread_mutex_lock(&(thread->mutex));
    pthread_cond_wait(&(thread->cond), &(thread->mutex));
    pthread_mutex_unlock(&(thread->mutex));
#elif PIKA_FREERTOS_ENABLE
    vTaskSuspend(thread->thread);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK void pika_platform_thread_start(pika_platform_thread_t* thread) {
#ifdef __linux
    pthread_mutex_lock(&(thread->mutex));
    pthread_cond_signal(&(thread->cond));
    pthread_mutex_unlock(&(thread->mutex));
#elif PIKA_FREERTOS_ENABLE
    vTaskResume(thread->thread);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK void pika_platform_thread_destroy(pika_platform_thread_t* thread) {
#ifdef __linux
    if (NULL != thread) {
        pthread_detach(thread->thread);
        pika_platform_free(thread);
        thread = NULL;
    }
#elif PIKA_FREERTOS_ENABLE
    if (NULL != thread)
        vTaskDelete(thread->thread);
    pika_platform_free(thread);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int pika_platform_thread_mutex_init(pika_platform_thread_mutex_t* m) {
#ifdef __linux
    return pthread_mutex_init(&(m->mutex), NULL);
#elif PIKA_FREERTOS_ENABLE
    m->mutex = xSemaphoreCreateMutex();
    return 0;
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
    return -1;
#endif
}

PIKA_WEAK int pika_platform_thread_mutex_lock(pika_platform_thread_mutex_t* m) {
#ifdef __linux
    return pthread_mutex_lock(&(m->mutex));
#elif PIKA_FREERTOS_ENABLE
    return xSemaphoreTake(m->mutex, portMAX_DELAY);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
    return -1;
#endif
}

PIKA_WEAK int pika_platform_thread_mutex_trylock(
    pika_platform_thread_mutex_t* m) {
#ifdef __linux
    return pthread_mutex_trylock(&(m->mutex));
#elif PIKA_FREERTOS_ENABLE
    return xSemaphoreTake(m->mutex, 0);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
    return -1;
#endif
}

PIKA_WEAK int pika_platform_thread_mutex_unlock(
    pika_platform_thread_mutex_t* m) {
#ifdef __linux
    return pthread_mutex_unlock(&(m->mutex));
#elif PIKA_FREERTOS_ENABLE
    return xSemaphoreGive(m->mutex);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
    return -1;
#endif
}

PIKA_WEAK int pika_platform_thread_mutex_destroy(
    pika_platform_thread_mutex_t* m) {
#ifdef __linux
    return pthread_mutex_destroy(&(m->mutex));
#elif PIKA_FREERTOS_ENABLE
    vSemaphoreDelete(m->mutex);
    return 0;
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
    return -1;
#endif
}

PIKA_WEAK void pika_platform_timer_init(pika_platform_timer_t* timer) {
#ifdef __linux
    timer->time = (struct timeval){0, 0};
#elif PIKA_FREERTOS_ENABLE
    timer->time = 0;
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK void pika_platform_timer_cutdown(pika_platform_timer_t* timer,
                                           unsigned int timeout) {
#ifdef __linux
    struct timeval now;
    gettimeofday(&now, NULL);
    struct timeval interval = {timeout / 1000, (timeout % 1000) * 1000};
    timeradd(&now, &interval, &timer->time);
#elif PIKA_FREERTOS_ENABLE
    timer->time = platform_uptime_ms();
    timer->time += timeout;
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK char pika_platform_timer_is_expired(pika_platform_timer_t* timer) {
#ifdef __linux
    struct timeval now, res;
    gettimeofday(&now, NULL);
    timersub(&timer->time, &now, &res);
    return ((res.tv_sec < 0) || (res.tv_sec == 0 && res.tv_usec <= 0));
#elif PIKA_FREERTOS_ENABLE
    return platform_uptime_ms() > timer->time ? 1 : 0;
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
    return 1;
#endif
}

PIKA_WEAK int pika_platform_timer_remain(pika_platform_timer_t* timer) {
#ifdef __linux
    struct timeval now, res;
    gettimeofday(&now, NULL);
    timersub(&timer->time, &now, &res);
    return (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000;
#elif PIKA_FREERTOS_ENABLE
    uint32_t now;
    now = platform_uptime_ms();
    if (timer->time <= now) {
        return 0;
    }
    return timer->time - now;
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
    return -1;
#endif
}

PIKA_WEAK unsigned long pika_platform_timer_now(void) {
#ifdef __linux
    return (unsigned long)time(NULL);
#elif PIKA_FREERTOS_ENABLE
    return (unsigned long)platform_uptime_ms();
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
    return 1;
#endif
}

PIKA_WEAK void pika_platform_timer_usleep(unsigned long usec) {
#ifdef __linux
    usleep(usec);
#elif PIKA_FREERTOS_ENABLE
    TickType_t tick = 1;
    if (usec != 0) {
        tick = usec / portTICK_PERIOD_MS;

        if (tick == 0)
            tick = 1;
    }
    vTaskDelay(tick);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK void pika_platform_reboot(void){
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}
