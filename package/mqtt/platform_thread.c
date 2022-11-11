/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-23 19:26:27
 * @LastEditTime: 2020-02-23 16:19:07
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "platform_thread.h"
#include "platform_memory.h"

platform_thread_t *platform_thread_init( const char *name,
                                        void (*entry)(void *),
                                        void * const param,
                                        unsigned int stack_size,
                                        unsigned int priority,
                                        unsigned int tick)
{
    int res;
    platform_thread_t *thread;
    void *(*thread_entry) (void *);

    thread_entry = (void *(*)(void*))entry;
    thread = platform_memory_alloc(sizeof(platform_thread_t));
    
    res = pthread_create(&thread->thread, NULL, thread_entry, param);
    if(res != 0) {
        platform_memory_free(thread);
    }

    thread->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    thread->cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

    return thread;
}

void platform_thread_startup(platform_thread_t* thread)
{
    (void) thread;
}

void platform_thread_stop(platform_thread_t* thread)
{
    pthread_mutex_lock(&(thread->mutex));
    pthread_cond_wait(&(thread->cond), &(thread->mutex));
    pthread_mutex_unlock(&(thread->mutex));
}

void platform_thread_start(platform_thread_t* thread)
{
    pthread_mutex_lock(&(thread->mutex)); 
    pthread_cond_signal(&(thread->cond));
    pthread_mutex_unlock(&(thread->mutex));
}

void platform_thread_destroy(platform_thread_t* thread)
{
    if (NULL != thread)
        pthread_detach(thread->thread);
}


