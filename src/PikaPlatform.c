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

#include "PikaPlatform.h"
#include <stdio.h>
#include <stdlib.h>
#if defined(_WIN32) && !defined(CROSS_BUILD)
#include <Windows.h>
#endif

void pikaFree(void* mem, uint32_t size);
void* pikaMalloc(uint32_t size);

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

PIKA_WEAK int64_t pika_platform_get_tick(void) {
#if PIKA_FREERTOS_ENABLE
    return platform_uptime_ms();
#elif defined(__linux)
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#else
    return -1;
#endif
}

int pika_vsprintf(char* buff, char* fmt, va_list args) {
    /* vsnprintf */
    return pika_platform_vsnprintf(buff, PIKA_SPRINTF_BUFF_SIZE, fmt, args);
}

int pika_snprintf(char* buff, size_t size, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = pika_platform_vsnprintf(buff, size, fmt, args);
    va_end(args);
    return ret;
}

PIKA_WEAK int pika_platform_putchar(char ch) {
    return putchar(ch);
}

int pika_pvsprintf(char** buff, const char* fmt, va_list args) {
    int required_size;
    int current_size = PIKA_SPRINTF_BUFF_SIZE;
    *buff = (char*)pika_platform_malloc(current_size * sizeof(char));

    if (*buff == NULL) {
        return -1;  // Memory allocation failed
    }

    va_list args_copy;
    va_copy(args_copy, args);

    required_size =
        pika_platform_vsnprintf(*buff, current_size, fmt, args_copy);
    va_end(args_copy);

    while (required_size >= current_size) {
        current_size *= 2;
        char* new_buff =
            (char*)pika_platform_realloc(*buff, current_size * sizeof(char));

        if (new_buff == NULL) {
            pika_platform_free(*buff);
            return -1;  // Memory allocation failed
        } else {
            *buff = new_buff;
        }

        va_copy(args_copy, args);
        required_size =
            pika_platform_vsnprintf(*buff, current_size, fmt, args_copy);
        va_end(args_copy);
    }

    return required_size;
}

static PIKA_BOOL _check_no_buff_format(char* format) {
    while (*format) {
        if (*format == '%') {
            ++format;
            if (*format != 's' && *format != '%') {
                return PIKA_FALSE;
            }
        }
        ++format;
    }
    return PIKA_TRUE;
}

static int _no_buff_vprintf(char* fmt, va_list args) {
    int written = 0;
    while (*fmt) {
        if (*fmt == '%') {
            ++fmt;
            if (*fmt == 's') {
                const char* str = va_arg(args, const char*);
                int len = strlen(str);
                written += len;
                for (int i = 0; i < len; i++) {
                    pika_platform_putchar(str[i]);
                }
            } else if (*fmt == '%') {
                pika_platform_putchar('%');
                ++written;
            }
        } else {
            pika_platform_putchar(*fmt);
            ++written;
        }
        ++fmt;
    }
    return written;
}

int pika_vprintf(char* fmt, va_list args) {
    if (_check_no_buff_format(fmt)) {
        _no_buff_vprintf(fmt, args);
        return 0;
    }

    char* buff = NULL;
    int required_size = pika_pvsprintf(&buff, fmt, args);

    if (required_size < 0) {
        return -1;  // Memory allocation or other error occurred
    }

    // putchar
    for (int i = 0; i < strlen(buff); i++) {
        pika_platform_putchar(buff[i]);
    }

    pika_platform_free(buff);
    return 0;
}

#ifndef pika_platform_printf
PIKA_WEAK void pika_platform_printf(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    pika_vprintf(fmt, args);
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

int pika_sprintf(char* buff, char* fmt, ...) {
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
    pika_assert(NULL != stream);
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
#if defined(__linux) || defined(_WIN32)
    return;
#elif PIKA_FREERTOS_ENABLE
    vTaskDelay(1);
#else
    return;
#endif
}

PIKA_WEAK void pika_platform_sleep_ms(uint32_t ms) {
#if defined(__linux)
    usleep(ms * 1000);
#elif defined(_WIN32) && !defined(CROSS_BUILD)
    Sleep(ms);
#else
    pika_platform_printf(
        "Error: pika_platform_sleep_ms need implementation!\r\n");
    while (1) {
    }
#endif
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
    thread = pikaMalloc(sizeof(pika_platform_thread_t));

    res = pthread_create(&thread->thread, NULL, thread_entry, param);
    if (res != 0) {
        pikaFree(thread, sizeof(pika_platform_thread_t));
    }

    thread->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    thread->cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

    return thread;
#elif PIKA_FREERTOS_ENABLE
    BaseType_t err;
    pika_platform_thread_t* thread;

    thread = pikaMalloc(sizeof(pika_platform_thread_t));

    (void)tick;

    err = xTaskCreate(entry, name, stack_size, param, priority, thread->thread);

    if (pdPASS != err) {
        pikaFree(thread, sizeof(pika_platform_thread_t));
        return NULL;
    }

    return thread;
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
    return NULL;
#endif
}

uint64_t pika_platform_thread_self(void) {
#ifdef __linux
    return (uint64_t)pthread_self();
#elif PIKA_FREERTOS_ENABLE
    return (uint64_t)xTaskGetCurrentTaskHandle();
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
    return 0;
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
        pikaFree(thread, sizeof(pika_platform_thread_t));
        thread = NULL;
        return;
    }
#elif PIKA_FREERTOS_ENABLE
    if (NULL != thread) {
        vTaskDelete(thread->thread);
        pikaFree(thread, sizeof(pika_platform_thread_t));
        return;
    }
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK void pika_platform_thread_exit(pika_platform_thread_t* thread) {
#ifdef __linux
    return pika_platform_thread_destroy(thread);
#elif PIKA_FREERTOS_ENABLE
    vTaskDelete(NULL);  // test on esp32c3
    // vTaskDelete(thread->thread);
    return;
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

PIKA_WEAK void pika_platform_thread_timer_init(pika_platform_timer_t* timer) {
#ifdef __linux
    timer->time = (struct timeval){0, 0};
#elif PIKA_FREERTOS_ENABLE
    timer->time = 0;
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK void pika_platform_thread_timer_cutdown(pika_platform_timer_t* timer,
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

PIKA_WEAK char pika_platform_thread_timer_is_expired(
    pika_platform_timer_t* timer) {
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

PIKA_WEAK int pika_platform_thread_timer_remain(pika_platform_timer_t* timer) {
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

PIKA_WEAK unsigned long pika_platform_thread_timer_now(void) {
#ifdef __linux
    return (unsigned long)time(NULL);
#elif PIKA_FREERTOS_ENABLE
    return (unsigned long)platform_uptime_ms();
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
    return 1;
#endif
}

PIKA_WEAK void pika_platform_thread_timer_usleep(unsigned long usec) {
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

PIKA_WEAK void pika_platform_reboot(void) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK void pika_platform_clear(void) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

PIKA_WEAK void pika_platform_abort_handler(void) {
    return;
}
