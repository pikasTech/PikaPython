#include "PikaPlatformEx.h"

//----------------------------- mutex -------------------------------
// 带超时的互斥锁加锁
int pika_platform_thread_mutex_timedlock(pika_platform_thread_mutex_t* m,
                                         pika_bool block,
                                         Arg* timeout) {
#if defined(__linux) || (PIKA_WIN_PTHREAD_ENABLE)
    ArgType timout_type = arg_getType(timeout);
    pika_float timeout_f;
    int result;
    if (!(timout_type == ARG_TYPE_FLOAT || timout_type == ARG_TYPE_INT ||
          timout_type == ARG_TYPE_NONE)) {
        return PIKA_RES_ERR_INVALID_PARAM;
    }

    if (timout_type == ARG_TYPE_FLOAT || timout_type == ARG_TYPE_INT) {
        // printf("==== #01\n");
        if (timout_type == ARG_TYPE_FLOAT) {
            timeout_f = arg_getFloat(timeout);
        }
        if (timout_type == ARG_TYPE_INT) {
            int timeout_d = arg_getInt(timeout);
            timeout_f = (pika_float)timeout_d;
            // printf("==== #04  %lf\n", timeout_f);
        }
        if (timeout_f < 0.0f) {
            return PIKA_RES_ERR_INVALID_PARAM;
        }

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);  // 获取当前时间

        // 将浮点数秒转换为秒和纳秒
        long sec = (long)timeout_f;
        long nsec = (long)((timeout_f - (pika_float)sec) * 1000000000.0);

        ts.tv_sec += sec;
        ts.tv_nsec += nsec;

        // 如果纳秒数超过 1 秒，则需要调整秒数和纳秒数
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_nsec -= 1000000000;  // 减去 1 秒的纳秒数
            ts.tv_sec += 1;            // 增加 1 秒
        }

        pika_GIL_EXIT();
        result = pthread_mutex_timedlock(&m->mutex, &ts);
        pika_GIL_ENTER();
        return result == 0 ? 0 : -1;

    } else if (timout_type == ARG_TYPE_NONE) {
        if (block) {
            // printf("==== #02\n");
            pika_GIL_EXIT();
            result = pthread_mutex_lock(&m->mutex);
            pika_GIL_ENTER();
            return result == 0 ? 0 : -1;
        } else {
            // printf("==== #03\n");
            pika_GIL_EXIT();
            result = pthread_mutex_trylock(&m->mutex);
            pika_GIL_ENTER();
            return result == 0 ? 0 : -1;
        }
    } else {
        return PIKA_RES_ERR_INVALID_PARAM;
    }

#elif PIKA_FREERTOS_ENABLE
    if (pdTRUE == xSemaphoreTake(m->mutex, (TickType_t)(timeout * 1000.0f))) {
        return 0;
    }
    return -1;

#elif PIKA_RTTHREAD_ENABLE
    return rt_mutex_take((m->mutex), (rt_tick_t)(timeout * RT_TICK_PER_SECOND));

#elif PIKA_ZEUSOS_ENABLE
    return zos_mutex_lock(m->mutex, (uint32_t)(timeout * 1000.0f));

#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR(_);
    return -1;
#endif
}

//----------------------------- rtmutex -------------------------------
// 初始化递归互斥锁
void pika_platform_thread_rtmutex_init(pika_platform_thread_rtmutex_t* rtm) {
#if defined(__linux) || (PIKA_WIN_PTHREAD_ENABLE)
    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr) != 0) {
        perror("pthread_mutexattr_init");
        exit(EXIT_FAILURE);
    }
    // 设置互斥锁类型为递归互斥锁
    if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0) {
        perror("pthread_mutexattr_settype");
        pthread_mutexattr_destroy(&attr);
        exit(EXIT_FAILURE);
    }

    pthread_mutex_init(&rtm->mutex, &attr);
    pthread_cond_init(&rtm->cond, NULL);
    rtm->owner = (pthread_t)0;
    rtm->count = 0;
#elif PIKA_FREERTOS_ENABLE

#elif PIKA_RTTHREAD_ENABLE
#elif PIKA_ZEUSOS_ENABLE
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR(_);
#endif
}

// 销毁递归互斥锁
void pika_platform_thread_rtmutex_destroy(pika_platform_thread_rtmutex_t* rtm) {
#if defined(__linux) || (PIKA_WIN_PTHREAD_ENABLE)
    pthread_cond_destroy(&rtm->cond);
    pthread_mutex_destroy(&rtm->mutex);
#elif PIKA_FREERTOS_ENABLE
#elif PIKA_RTTHREAD_ENABLE
#elif PIKA_ZEUSOS_ENABLE
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR(_);
#endif
}

// 带超时的递归互斥锁加锁
int pika_platform_thread_rtmutex_lock(pika_platform_thread_rtmutex_t* rtm,
                                      pika_bool block,
                                      Arg* timeout) {
#if defined(__linux) || (PIKA_WIN_PTHREAD_ENABLE)
    ArgType timout_type = arg_getType(timeout);
    pika_float timeout_f;
    int result;

    if (!(timout_type == ARG_TYPE_FLOAT || timout_type == ARG_TYPE_INT ||
          timout_type == ARG_TYPE_NONE)) {
        return PIKA_RES_ERR_INVALID_PARAM;
    }

    pika_GIL_EXIT();
    pthread_mutex_lock(&rtm->mutex);
    pika_GIL_ENTER();

    if (rtm->owner == pthread_self()) {
        // 如果当前线程已经持有锁，则递归深度加1
        rtm->count++;
        // printf("rtm->count = %d\n", rtm->count);
        pthread_mutex_unlock(&rtm->mutex);
        // printf("succ\n");
        return 0;
    }

    if (timout_type == ARG_TYPE_FLOAT || timout_type == ARG_TYPE_INT) {
        // printf("==== #01\n");
        if (timout_type == ARG_TYPE_FLOAT) {
            timeout_f = arg_getFloat(timeout);
        }
        if (timout_type == ARG_TYPE_INT) {
            int timeout_d = arg_getInt(timeout);
            timeout_f = (pika_float)timeout_d;
            // printf("==== #04  %lf\n", timeout_f);
        }
        if (timeout_f < 0.0f) {
            return PIKA_RES_ERR_INVALID_PARAM;
        }

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);  // 获取当前时间

        // 将浮点数秒转换为秒和纳秒
        long sec = (long)timeout_f;
        long nsec = (long)((timeout_f - (pika_float)sec) * 1000000000.0);

        ts.tv_sec += sec;
        ts.tv_nsec += nsec;

        // 如果纳秒数超过 1 秒，则需要调整秒数和纳秒数
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_nsec -= 1000000000;  // 减去 1 秒的纳秒数
            ts.tv_sec += 1;            // 增加 1 秒
        }

        // 等待直到获得锁或超时
        while (rtm->owner != (pthread_t)0) {
            pika_GIL_EXIT();
            result = pthread_cond_timedwait(&rtm->cond, &rtm->mutex, &ts);
            pika_GIL_ENTER();

            if (result != 0) {
                pthread_mutex_unlock(&rtm->mutex);
                return -1;
            }
        }
        // 设置当前线程为锁的持有者
        rtm->owner = pthread_self();
        rtm->count = 1;
        pthread_mutex_unlock(&rtm->mutex);
        return 0;

    } else if (timout_type == ARG_TYPE_NONE) {
        if (block) {
            // 永久等待
            while (rtm->owner != (pthread_t)0) {
                pika_GIL_EXIT();
                result = pthread_cond_wait(&rtm->cond, &rtm->mutex);
                pika_GIL_ENTER();

                if (result != 0) {
                    pthread_mutex_unlock(&rtm->mutex);
                    return -1;
                }
            }
            // 设置当前线程为锁的持有者
            rtm->owner = pthread_self();
            rtm->count = 1;
            pthread_mutex_unlock(&rtm->mutex);
            return 0;

        } else {
            // 非阻塞模式
            if (rtm->owner == (pthread_t)0) {
                // 如果没有其他线程持有锁，获取锁
                rtm->owner = pthread_self();
                rtm->count = 1;
                pthread_mutex_unlock(&rtm->mutex);
                return 0;
            } else {
                // 如果已经有其他线程持有锁，立即返回 -1
                pthread_mutex_unlock(&rtm->mutex);
                return -1;
            }
        }
    } else {
        return PIKA_RES_ERR_INVALID_PARAM;
    }

#elif PIKA_FREERTOS_ENABLE
#elif PIKA_RTTHREAD_ENABLE
#elif PIKA_ZEUSOS_ENABLE
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR(_);
#endif
}

#if 1
// 释放递归互斥锁
int pika_platform_thread_rtmutex_unlock(pika_platform_thread_rtmutex_t* rtm) {
#if defined(__linux) || (PIKA_WIN_PTHREAD_ENABLE)
    pthread_t self = pthread_self();

    pika_GIL_EXIT();
    pthread_mutex_lock(&rtm->mutex);
    pika_GIL_ENTER();

    // printf("rtm->owner = %lu\n", rtm->owner);
    if (rtm->owner != self) {
        perror("Attempt to unlock a mutex not owned by the current thread");
        pthread_mutex_unlock(&rtm->mutex);
        return -1;
    }

    rtm->count--;
    if (rtm->count == 0) {
        rtm->owner = (pthread_t)0;
        pthread_cond_signal(&rtm->cond);
        // printf("rtm->owner = %lu\n", rtm->owner);
    }

    pthread_mutex_unlock(&rtm->mutex);
    return 0;
#elif PIKA_FREERTOS_ENABLE
#elif PIKA_RTTHREAD_ENABLE
#elif PIKA_ZEUSOS_ENABLE
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR(_);
#endif
}
#endif

// 检查递归互斥锁是否已被当前线程获取
int pika_platform_thread_rtmutex_locked(pika_platform_thread_rtmutex_t* rtm) {
#if defined(__linux) || (PIKA_WIN_PTHREAD_ENABLE)
    pthread_t self = pthread_self();
    pika_GIL_EXIT();
    pthread_mutex_lock(&rtm->mutex);
    pika_GIL_ENTER();
    int is_locked = (rtm->owner == self && rtm->count > 0);

    pthread_mutex_unlock(&rtm->mutex);

    return is_locked;
#elif PIKA_FREERTOS_ENABLE
#elif PIKA_RTTHREAD_ENABLE
#elif PIKA_ZEUSOS_ENABLE
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR(_);
#endif
}

//----------------------------- cond -------------------------------

void pika_platform_thread_cond_init(pika_platform_thread_cond_t* cond) {
#if defined(__linux) || (PIKA_WIN_PTHREAD_ENABLE)
    pika_platform_thread_rtmutex_init(&cond->rtmutex);
    pthread_cond_init(&cond->cond, NULL);
    cond->owner = (pthread_t)0;

#elif PIKA_FREERTOS_ENABLE

#elif PIKA_RTTHREAD_ENABLE
#elif PIKA_ZEUSOS_ENABLE
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR(_);
#endif
}

void pika_platform_thread_cond_destroy(pika_platform_thread_cond_t* cond) {
#if defined(__linux) || (PIKA_WIN_PTHREAD_ENABLE)
    pthread_cond_destroy(&cond->cond);
    pika_platform_thread_rtmutex_destroy(&cond->rtmutex);
    cond->owner = (pthread_t)0;  // 释放资源后重置 owner
#elif PIKA_FREERTOS_ENABLE
#elif PIKA_RTTHREAD_ENABLE
#elif PIKA_ZEUSOS_ENABLE
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR(_);
#endif
}

// 检查当前线程是否持有互斥锁
static int is_mutex_owned(pika_platform_thread_cond_t* cond) {
    // pthread_t current_thread = pthread_self();

    // 使用 pthread_mutex_trylock 来检查是否已经持有锁
    if (pthread_mutex_trylock(&cond->rtmutex.mutex) == EBUSY) {
        // 如果锁已经被持有，尝试解锁并检查是否是当前线程持有的
        if (pthread_mutex_unlock(&cond->rtmutex.mutex) == 0) {
            return 1;  // 当前线程持有锁
        }
    }

    return 0;
}

// 带阻塞和超时功能的条件变量等待
int pika_platform_thread_cond_timedwait(pika_platform_thread_cond_t* cond,
                                        Arg* timeout) {
#if defined(__linux) || (PIKA_WIN_PTHREAD_ENABLE)
    ArgType timout_type = arg_getType(timeout);
    pika_float timeout_f;
    int result;

    // 检查是否已经获得了互斥锁
    if (!is_mutex_owned(cond)) {
        return -1;
    }

    if (!(timout_type == ARG_TYPE_FLOAT || timout_type == ARG_TYPE_INT ||
          timout_type == ARG_TYPE_NONE)) {
        return PIKA_RES_ERR_INVALID_PARAM;
    }

    if (timout_type == ARG_TYPE_FLOAT || timout_type == ARG_TYPE_INT) {
        if (timout_type == ARG_TYPE_FLOAT) {
            timeout_f = arg_getFloat(timeout);
        } else {
            int timeout_d = arg_getInt(timeout);
            timeout_f = (pika_float)timeout_d;
        }

        if (timeout_f < 0.0f) {
            return PIKA_RES_ERR_INVALID_PARAM;
        }

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);  // 获取当前时间

        // 将浮点数秒转换为秒和纳秒
        long sec = (long)timeout_f;
        long nsec = (long)((timeout_f - (pika_float)sec) * 1000000000.0);

        ts.tv_sec += sec;
        ts.tv_nsec += nsec;

        // 如果纳秒数超过 1 秒，则需要调整秒数和纳秒数
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_nsec -= 1000000000;  // 减去 1 秒的纳秒数
            ts.tv_sec += 1;            // 增加 1 秒
        }

        // 等待直到被通知或超时
        pika_GIL_EXIT();
        result = pthread_cond_timedwait(&cond->cond, &cond->rtmutex.mutex, &ts);
        pika_GIL_ENTER();

        if (result != 0) {
            if (result == ETIMEDOUT) {
                return -1;  // 超时
            }
            perror("pthread_cond_timedwait");
            return -1;  // 其他错误
        }
    } else if (timout_type == ARG_TYPE_NONE) {
        // 永久等待
        pika_GIL_EXIT();
        result = pthread_cond_wait(&cond->cond, &cond->rtmutex.mutex);
        pika_GIL_ENTER();

        if (result != 0) {
            perror("pthread_cond_wait");
            return -1;
        }
    } else {
        return PIKA_RES_ERR_INVALID_PARAM;
    }

    return 0;

#elif PIKA_FREERTOS_ENABLE
#elif PIKA_RTTHREAD_ENABLE
#elif PIKA_ZEUSOS_ENABLE
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR(_);
#endif
}
// 信号量通知
int pika_platform_thread_cond_signal(pika_platform_thread_cond_t* cond) {
#if defined(__linux) || (PIKA_WIN_PTHREAD_ENABLE)
    int result;
    result = pthread_cond_signal(&cond->cond);
    cond->owner = (pthread_t)0;  // 通知后重置 owner
    return result == 0 ? 0 : -1;
#elif PIKA_FREERTOS_ENABLE
#elif PIKA_RTTHREAD_ENABLE
#elif PIKA_ZEUSOS_ENABLE
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR(_);
#endif
}

// 信号量广播
int pika_platform_thread_cond_broadcast(pika_platform_thread_cond_t* cond) {
#if defined(__linux) || (PIKA_WIN_PTHREAD_ENABLE)
    int result;
    result = pthread_cond_broadcast(&cond->cond);
    cond->owner = (pthread_t)0;  // 广播后重置 owner
    return result == 0 ? 0 : -1;
#elif PIKA_FREERTOS_ENABLE
#elif PIKA_RTTHREAD_ENABLE
#elif PIKA_ZEUSOS_ENABLE
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR(_);
#endif
}
