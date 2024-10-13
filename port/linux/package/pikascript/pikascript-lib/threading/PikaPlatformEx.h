#ifndef ___PikaPlatformEx__H
#define ___PikaPlatformEx__H
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include "PikaObj.h"
#include "PikaPlatform.h"

int pika_platform_thread_mutex_timedlock(pika_platform_thread_mutex_t* m,
                                         pika_bool block,
                                         Arg* timeout);

//-------------------------------

// Recursive mutex with timeout structure
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_t owner;  // Thread ID of the current lock owner
    int count;        // Recursion depth
} pika_platform_thread_rtmutex_t;

// Initialize recursive mutex
void pika_platform_thread_rtmutex_init(pika_platform_thread_rtmutex_t* rtm);

// Destroy recursive mutex
void pika_platform_thread_rtmutex_destroy(pika_platform_thread_rtmutex_t* rtm);

// Lock recursive mutex with timeout
int pika_platform_thread_rtmutex_lock(pika_platform_thread_rtmutex_t* rtm,
                                      pika_bool block,
                                      Arg* timeout);

// Unlock recursive mutex
int pika_platform_thread_rtmutex_unlock(pika_platform_thread_rtmutex_t* rtm);

// Check if the lock is held
int pika_platform_thread_rtmutex_locked(pika_platform_thread_rtmutex_t* rtm);

//-------------------------------------

typedef struct {
    pika_platform_thread_rtmutex_t rtmutex;
    pthread_cond_t cond;
    pthread_t owner;  // Thread ID of the current lock owner
} pika_platform_thread_cond_t;

void pika_platform_thread_cond_init(pika_platform_thread_cond_t* cond);
void pika_platform_thread_cond_destroy(pika_platform_thread_cond_t* cond);
int pika_platform_thread_cond_signal(pika_platform_thread_cond_t* cond);
int pika_platform_thread_cond_broadcast(pika_platform_thread_cond_t* cond);
int pika_platform_thread_cond_timedwait(pika_platform_thread_cond_t* cond,
                                        Arg* timeout);

#endif
