/*
 * This file is part of the PikaPython project.
 * http://github.com/pikastech/pikapython
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon liang6516@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "PikaPlatform.h"
#include <stdio.h>
#include <stdlib.h>
#if defined(_WIN32) && !defined(CROSS_BUILD)
#include <Windows.h>
#include <io.h>
#include <direct.h>
#endif

#if defined(__linux) || PIKA_LINUX_COMPATIBLE
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "unistd.h"
#endif

#if (defined(__linux) || PIKA_LINUX_COMPATIBLE) || defined(_WIN32)
#include <dirent.h>
#endif

void pikaFree(void* mem, uint32_t size);
void* pikaMalloc(uint32_t size);
int pika_pvsprintf(char** buff, const char* fmt, va_list args);

/* stdio platform */

int pika_putchar(char ch) {
    int ret = pika_platform_putchar(ch);
#if PIKA_UNBUFFERED_ENABLE
    pika_platform_fflush(stdout);
#endif
    return ret;
}

PIKA_WEAK void pika_platform_clear(void) {
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
}

#if !PIKA_PLATFORM_NO_WEAK

PIKA_WEAK void pika_platform_disable_irq_handle(void) {
    /* disable irq to support thread */
}

PIKA_WEAK void pika_platform_enable_irq_handle(void) {
    /* disable irq to support thread */
}

/* memory support */
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

PIKA_WEAK uint8_t pika_is_locked_pikaMemory(void) {
    return 0;
}

/* time support */
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

PIKA_WEAK void pika_platform_sleep_ms(uint32_t ms) {
#if defined(__linux)
    usleep(ms * 1000);
#elif defined(_WIN32) && !defined(CROSS_BUILD)
    Sleep(ms);
#elif PIKA_RTTHREAD_ENABLE
    rt_thread_mdelay(ms);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR_LOWLEVEL(_);
#endif
}

PIKA_WEAK int64_t pika_platform_get_tick(void) {
#if PIKA_FREERTOS_ENABLE
    return platform_uptime_ms();
#elif defined(__linux)
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#elif PIKA_RTTHREAD_ENABLE
    uint32_t tick = rt_tick_get() * 1000;
    return (uint32_t)((tick + RT_TICK_PER_SECOND - 1) / RT_TICK_PER_SECOND);
#else
    return -1;
#endif
}

PIKA_WEAK int pika_platform_fflush(void* stream) {
#if PIKA_UNBUFFERED_ENABLE
    return fflush(stream);
#else
    return 0;
#endif
}

PIKA_WEAK int pika_platform_putchar(char ch) {
    return putchar(ch);
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
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR_LOWLEVEL();
#endif
}

/* file system support */
PIKA_WEAK FILE* pika_platform_fopen(const char* filename, const char* modes) {
#if defined(__linux) || defined(_WIN32)
    return fopen(filename, modes);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR_LOWLEVEL();
#endif
}

PIKA_WEAK int pika_platform_fclose(FILE* stream) {
#if defined(__linux) || defined(_WIN32)
    return fclose(stream);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR_LOWLEVEL();
#endif
}

PIKA_WEAK size_t pika_platform_fwrite(const void* ptr,
                                      size_t size,
                                      size_t n,
                                      FILE* stream) {
    pika_assert(NULL != stream);
#if defined(__linux) || defined(_WIN32)
    return fwrite(ptr, size, n, stream);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR_LOWLEVEL();
#endif
}

PIKA_WEAK size_t pika_platform_fread(void* ptr,
                                     size_t size,
                                     size_t n,
                                     FILE* stream) {
#if defined(__linux) || defined(_WIN32)
    return fread(ptr, size, n, stream);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR_LOWLEVEL(_);
#endif
}

PIKA_WEAK int pika_platform_fseek(FILE* stream, long offset, int whence) {
#if defined(__linux) || defined(_WIN32)
    return fseek(stream, offset, whence);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR_LOWLEVEL(_);
#endif
}

PIKA_WEAK long pika_platform_ftell(FILE* stream) {
#if defined(__linux) || defined(_WIN32)
    return ftell(stream);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR_LOWLEVEL(_);
#endif
}

PIKA_WEAK char* pika_platform_getcwd(char* buf, size_t size) {
#if defined(__linux) || defined(_WIN32)
    return getcwd(buf, size);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR_LOWLEVEL(_);
#endif
}

PIKA_WEAK int pika_platform_chdir(const char* path) {
#if defined(__linux) || defined(_WIN32)
    return chdir(path);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR_LOWLEVEL(_);
#endif
}

PIKA_WEAK int pika_platform_rmdir(const char* pathname) {
#if defined(__linux) || defined(_WIN32)
    return rmdir(pathname);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR_LOWLEVEL(_);
#endif
}

PIKA_WEAK int pika_platform_mkdir(const char* pathname, int mode) {
#if defined(_WIN32) && !defined(CROSS_BUILD)
    (void)(mode);
    return mkdir(pathname);
#elif defined(__linux) || PIKA_LINUX_COMPATIBLE
    return mkdir(pathname, mode);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR_LOWLEVEL(_);
#endif
}

PIKA_WEAK char* pika_platform_realpath(const char* path, char* resolved_path) {
#if defined(__linux) || PIKA_LINUX_COMPATIBLE
    return realpath(path, resolved_path);
#else
    if (!path || !resolved_path)
        return NULL;

    char* output = resolved_path;
    const char* segment_start = path;
    const char* segment_end = path;

    while (*segment_end) {
        if (*segment_end == '/' || *(segment_end + 1) == '\0') {
            size_t segment_len =
                segment_end - segment_start + (*segment_end != '/');

            if (segment_len == 1 && segment_start[0] == '.') {
                // Skip single-dot segment
            } else if (segment_len == 2 && segment_start[0] == '.' &&
                       segment_start[1] == '.') {
                // Handle double-dot segment by backtracking
                if (output > resolved_path) {
                    output--;  // Move back one char to overwrite the last slash
                    while (output > resolved_path && *output != '/') {
                        output--;
                    }
                }
            } else {
                // Copy the segment to the output
                strncpy(output, segment_start, segment_len);
                output += segment_len;
                if (*segment_end) {
                    *output = '/';
                    output++;
                }
            }

            segment_end++;  // Move past the slash
            segment_start = segment_end;
        } else {
            segment_end++;
        }
    }

    if (output != resolved_path && *(output - 1) == '/') {
        output--;  // Remove trailing slash, if any
    }

    *output = '\0';  // Null-terminate the resolved path

    return resolved_path;
#endif
}

PIKA_WEAK int pika_platform_path_exists(const char* path) {
#if defined(_WIN32) && !defined(CROSS_BUILD)
    DWORD attr = GetFileAttributesA((LPCWSTR)path);
    if (attr == INVALID_FILE_ATTRIBUTES) {
        return 0;
    }

    return 1;
#elif defined(__linux) || PIKA_LINUX_COMPATIBLE
    struct stat statbuf;
    if (stat(path, &statbuf) == -1) {
        return 0;
    }

    return 1;
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR_LOWLEVEL(_);
#endif
}

PIKA_WEAK int pika_platform_path_isdir(const char* path) {
#if defined(_WIN32) && !defined(CROSS_BUILD)
    int is_dir = 0;
    DWORD attrs = GetFileAttributes((LPCWSTR)path);
    if (attrs != INVALID_FILE_ATTRIBUTES) {
        is_dir = (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0 ? 1 : 0;
    }
    return is_dir;
#elif defined(__linux) || PIKA_LINUX_COMPATIBLE
    int is_dir = 0;
    struct stat st;
    if (stat(path, &st) == 0) {
        is_dir = S_ISDIR(st.st_mode) ? PIKA_TRUE : PIKA_FALSE;
    }
    return is_dir;
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR_LOWLEVEL(_);
#endif
}

// Returns true if the given path is a regular file, false otherwise.
PIKA_WEAK int pika_platform_path_isfile(const char* path) {
#if defined(_WIN32) && !defined(CROSS_BUILD)
    int is_file = 0;
    DWORD attrs = GetFileAttributes(path);
    if (attrs != INVALID_FILE_ATTRIBUTES) {
        is_file =
            (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0 ? PIKA_TRUE : PIKA_FALSE;
    }
    return is_file;
#elif defined(__linux) || PIKA_LINUX_COMPATIBLE
    int is_file = 0;
    struct stat st;
    if (stat(path, &st) == 0) {
        is_file = S_ISREG(st.st_mode) ? PIKA_TRUE : PIKA_FALSE;
    }
    return is_file;
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR_LOWLEVEL(_);
#endif
}

PIKA_WEAK int pika_platform_remove(const char* pathname) {
#if defined(__linux) || defined(_WIN32)
    return remove(pathname);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR_LOWLEVEL();
#endif
}

PIKA_WEAK int pika_platform_rename(const char* oldpath, const char* newpath) {
#if defined(__linux) || defined(_WIN32)
    return rename(oldpath, newpath);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR_LOWLEVEL();
#endif
}

PIKA_WEAK char** pika_platform_listdir(const char* path, int* count) {
#if defined(__linux)
    struct dirent* dp;
    DIR* dir = opendir(path);

    char** filenames = NULL;
    *count = 0;

    if (dir != NULL) {
        while ((dp = readdir(dir)) != NULL) {
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
                filenames = (char**)pika_platform_realloc(
                    filenames, (*count + 1) * sizeof(char*));
                filenames[*count] = pika_platform_strdup(dp->d_name);
                (*count)++;
            }
        }
        closedir(dir);
    }
    return filenames;
#elif defined(_WIN32) && !defined(CROSS_BUILD)
    struct _finddata_t fb;
    intptr_t handle = 0;
    char dirpath[256] = {0};
    char* currentPath = _getcwd(dirpath, 256);
    strcat(dirpath, path);
    strcat(dirpath, "\\*");

    char** filenames = NULL;
    *count = 0;

    handle = _findfirst(dirpath, &fb);
    if (handle != -1L) {
        do {
            if (strcmp(fb.name, ".") != 0 && strcmp(fb.name, "..") != 0) {
                filenames = (char**)pika_platform_realloc(
                    filenames, (*count + 1) * sizeof(char*));
                filenames[*count] = pika_platform_strdup(fb.name);
                (*count)++;
            }
        } while (_findnext(handle, &fb) == 0);
        _findclose(handle);
    }
    return filenames;
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR_LOWLEVEL();
#endif
}

/* thread support */
PIKA_WEAK void pika_platform_thread_yield(void) {
    pika_thread_idle_hook();
#if PIKA_FREERTOS_ENABLE
    vTaskDelay(1);
#elif defined(__linux) || defined(_WIN32)
    return;
#elif PIKA_RTTHREAD_ENABLE
    rt_thread_yield();
#else
    return;
#endif
}

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
    pika_platform_thread_t* thread;

    thread = pikaMalloc(sizeof(pika_platform_thread_t));
#if PIKA_THREAD_MALLOC_STACK_ENABLE
    thread->thread_stack_size = stack_size;
    thread->thread_stack = pikaMalloc(thread->thread_stack_size);
#endif

    (void)tick;

#if PIKA_THREAD_MALLOC_STACK_ENABLE
    thread->thread =
        xTaskCreateStatic(entry, name, stack_size, param, priority,
                          thread->thread_stack, &thread->task_buffer);
#else
    int err =
        xTaskCreate(entry, name, stack_size, param, priority, &thread->thread);
#endif

#if PIKA_THREAD_MALLOC_STACK_ENABLE
    if (NULL == thread->thread) {
        pikaFree(thread->thread_stack, thread->thread_stack_size);
        pikaFree(thread, sizeof(pika_platform_thread_t));
        return NULL;
    }
#else
    if (pdPASS != err) {
        pikaFree(thread, sizeof(pika_platform_thread_t));
        return NULL;
    }
#endif

    return thread;
#elif PIKA_RTTHREAD_ENABLE
    pika_platform_thread_t* thread;
    thread = pikaMalloc(sizeof(pika_platform_thread_t));
    if (RT_NULL == thread) {
        return RT_NULL;
    }
    thread->thread = rt_thread_create((const char*)name, entry, param,
                                      stack_size, priority, tick);

    if (thread->thread == RT_NULL) {
        pikaFree(thread, sizeof(pika_platform_thread_t));
        return RT_NULL;
    } else {
        return thread;
    }
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
    return NULL;
#endif
}

PIKA_WEAK uint64_t pika_platform_thread_self(void) {
#ifdef __linux
    return (uint64_t)pthread_self();
#elif PIKA_FREERTOS_ENABLE
    return (uint64_t)xTaskGetCurrentTaskHandle();
#elif PIKA_RTTHREAD_ENABLE
    return (uint64_t)rt_thread_self();
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
    return 0;
#endif
}

PIKA_WEAK void pika_platform_thread_startup(pika_platform_thread_t* thread) {
    (void)thread;
#if PIKA_RTTHREAD_ENABLE
    rt_thread_startup(thread->thread);
#endif
}

PIKA_WEAK void pika_platform_thread_stop(pika_platform_thread_t* thread) {
#ifdef __linux
    pthread_mutex_lock(&(thread->mutex));
    pthread_cond_wait(&(thread->cond), &(thread->mutex));
    pthread_mutex_unlock(&(thread->mutex));
#elif PIKA_FREERTOS_ENABLE
    vTaskSuspend(thread->thread);
#elif PIKA_RTTHREAD_ENABLE
    rt_thread_suspend(thread->thread);
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
#elif PIKA_RTTHREAD_ENABLE
    rt_thread_resume(thread->thread);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK void pika_platform_thread_destroy(pika_platform_thread_t* thread) {
#ifdef __linux
    if (NULL != thread) {
        pthread_detach(thread->thread);
        // pthread_join(thread->thread, NULL);
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
#elif PIKA_RTTHREAD_ENABLE
    if (NULL != thread) {
        rt_thread_delete(thread->thread);
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
    // vTaskDelete(NULL);  // test on esp32c3
    if (NULL == thread) {
        vTaskDelete(NULL);
        return;
    }
    vTaskDelete(thread->thread);
    return;
#elif PIKA_RTTHREAD_ENABLE
    if (NULL == thread) {
        rt_thread_delete(NULL);
        return;
    }
    rt_thread_delete(thread->thread);
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
#elif PIKA_RTTHREAD_ENABLE
    m->mutex = rt_mutex_create("pika_platform_mutex", RT_IPC_FLAG_PRIO);
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
#elif PIKA_RTTHREAD_ENABLE
    return rt_mutex_take((m->mutex), RT_WAITING_FOREVER);
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
#elif PIKA_RTTHREAD_ENABLE
    return rt_mutex_take((m->mutex), 0);
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
#elif PIKA_RTTHREAD_ENABLE
    return rt_mutex_release((m->mutex));
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
#elif PIKA_RTTHREAD_ENABLE
    return rt_mutex_delete((m->mutex));
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
#elif PIKA_RTTHREAD_ENABLE
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
#elif PIKA_RTTHREAD_ENABLE
    rt_uint32_t tick = rt_tick_get() * 1000;
    timer->time =
        (uint32_t)((tick + RT_TICK_PER_SECOND - 1) / RT_TICK_PER_SECOND);
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
#elif PIKA_RTTHREAD_ENABLE
    uint32_t tick = rt_tick_get() * 1000;
    uint32_t time =
        (uint32_t)((tick + RT_TICK_PER_SECOND - 1) / RT_TICK_PER_SECOND);
    return time > timer->time ? 1 : 0;
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
#elif PIKA_RTTHREAD_ENABLE
    uint32_t now;
    uint32_t tick = rt_tick_get() * 1000;
    now = (uint32_t)((tick + RT_TICK_PER_SECOND - 1) / RT_TICK_PER_SECOND);
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
#elif PIKA_RTTHREAD_ENABLE
    uint32_t tick = rt_tick_get() * 1000;
    return (unsigned long)((tick + RT_TICK_PER_SECOND - 1) /
                           RT_TICK_PER_SECOND);
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
#elif PIKA_RTTHREAD_ENABLE
    uint32_t tick = 1;
    if (usec != 0) {
        tick = usec / RT_TICK_PER_SECOND;

        if (tick == 0)
            tick = 1;
    }
    vTaskDelay(tick);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

/* system support */

PIKA_WEAK void pika_platform_reboot(void) {
#if __linux
    pika_platform_printf("reboot\n");
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

/* hook */
PIKA_WEAK void pika_platform_error_handle() {
    return;
}

PIKA_WEAK void pika_platform_panic_handle() {
    while (1) {
    };
}

PIKA_WEAK void pika_hook_instruct(void) {
    return;
}

PIKA_WEAK PIKA_BOOL pika_hook_arg_cache_filter(void* self) {
    return PIKA_TRUE;
}

PIKA_WEAK void pika_platform_abort_handler(void) {
    return;
}

PIKA_WEAK void pika_thread_idle_hook(void) {
    return;
}

#endif
