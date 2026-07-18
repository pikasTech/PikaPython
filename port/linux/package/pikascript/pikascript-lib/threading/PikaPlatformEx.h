#ifndef ___PikaPlatformEx__H
#define ___PikaPlatformEx__H
#include "PikaPlatform.h"
#include "PikaObj.h"
#include <time.h>
#include <pthread.h>
#include <errno.h>
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pika_bool locked;
} pika_platform_thread_lock_t;

void pika_platform_thread_lock_init(pika_platform_thread_lock_t* lock);
void pika_platform_thread_lock_destroy(pika_platform_thread_lock_t* lock);
int pika_platform_thread_lock_acquire(pika_platform_thread_lock_t* lock,
                                      pika_bool block,
                                      Arg* timeout);
int pika_platform_thread_lock_release(pika_platform_thread_lock_t* lock);
int pika_platform_thread_lock_locked(pika_platform_thread_lock_t* lock);

//-------------------------------

// 递归带超时互斥锁结构体
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_t owner;  // 当前持有锁的线程 ID
    int count;        // 递归深度
} pika_platform_thread_rtmutex_t;

// 初始化递归互斥锁
void pika_platform_thread_rtmutex_init(pika_platform_thread_rtmutex_t* rtm);

// 销毁递归互斥锁
void pika_platform_thread_rtmutex_destroy(pika_platform_thread_rtmutex_t* rtm);

// 带超时的递归互斥锁加锁
int pika_platform_thread_rtmutex_lock(pika_platform_thread_rtmutex_t* rtm,
                                      pika_bool block,
                                      Arg* timeout);

// 递归互斥锁解锁
int pika_platform_thread_rtmutex_unlock(pika_platform_thread_rtmutex_t* rtm);

// 是否已获得锁
int pika_platform_thread_rtmutex_locked(pika_platform_thread_rtmutex_t* rtm);

//-------------------------------------

typedef struct {
    pika_platform_thread_rtmutex_t rtmutex;
    pthread_cond_t cond;
    pthread_t owner;  // 当前持有锁的线程 ID
} pika_platform_thread_cond_t;

void pika_platform_thread_cond_init(pika_platform_thread_cond_t* cond);
void pika_platform_thread_cond_destroy(pika_platform_thread_cond_t* cond);
int pika_platform_thread_cond_signal(pika_platform_thread_cond_t* cond);
int pika_platform_thread_cond_broadcast(pika_platform_thread_cond_t* cond);
int pika_platform_thread_cond_timedwait(pika_platform_thread_cond_t* cond,
                                        Arg* timeout);
#endif
