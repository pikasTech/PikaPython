/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-23 19:26:27
 * @LastEditTime: 2020-02-23 16:19:07
 * @Description: the code belongs to jiejie, please keep the author information
 * and source code according to the license.
 */
#include "platform_thread.h"
#include "platform_memory.h"

PIKA_WEAK platform_thread_t* platform_thread_init(const char* name,
                                                  void (*entry)(void*),
                                                  void* const param,
                                                  unsigned int stack_size,
                                                  unsigned int priority,
                                                  unsigned int tick) {
#ifdef __linux
    int res;
    platform_thread_t* thread;
    void* (*thread_entry)(void*);

    thread_entry = (void* (*)(void*))entry;
    thread = platform_memory_alloc(sizeof(platform_thread_t));

    res = pthread_create(&thread->thread, NULL, thread_entry, param);
    if (res != 0) {
        platform_memory_free(thread);
    }

    thread->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    thread->cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

    return thread;
#elif PIKA_FREERTOS_ENABLE
    BaseType_t err;
    platform_thread_t* thread;

    thread = platform_memory_alloc(sizeof(platform_thread_t));

    (void)tick;

    err = xTaskCreate(entry, name, stack_size, param, priority, thread->thread);

    if (pdPASS != err) {
        platform_memory_free(thread);
        return NULL;
    }

    return thread;
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK void platform_thread_startup(platform_thread_t* thread) {
    (void)thread;
}

PIKA_WEAK void platform_thread_stop(platform_thread_t* thread) {
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

PIKA_WEAK void platform_thread_start(platform_thread_t* thread) {
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

PIKA_WEAK void platform_thread_destroy(platform_thread_t* thread) {
#ifdef __linux
    if (NULL != thread)
        pthread_detach(thread->thread);
#elif PIKA_FREERTOS_ENABLE
    if (NULL != thread)
        vTaskDelete(thread->thread);
    platform_memory_free(thread);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}
