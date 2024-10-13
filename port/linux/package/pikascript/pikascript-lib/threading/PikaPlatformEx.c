#include "PikaPlatformEx.h"

//----------------------------- mutex -------------------------------
// Mutex lock with timeout
int pika_platform_thread_mutex_timedlock(pika_platform_thread_mutex_t* m,
                                         pika_bool block,
                                         Arg* timeout) {
#if defined(__linux) || (PIKA_WIN_PTHREAD_ENABLE)
    ArgType timeout_type = arg_getType(timeout);
    pika_float timeout_f;
    int result;
    if (!(timeout_type == ARG_TYPE_FLOAT || timeout_type == ARG_TYPE_INT ||
          timeout_type == ARG_TYPE_NONE)) {
        return PIKA_RES_ERR_INVALID_PARAM;
    }

    if (timeout_type == ARG_TYPE_FLOAT || timeout_type == ARG_TYPE_INT) {
        if (timeout_type == ARG_TYPE_FLOAT) {
            timeout_f = arg_getFloat(timeout);
        }
        if (timeout_type == ARG_TYPE_INT) {
            int timeout_d = arg_getInt(timeout);
            timeout_f = (pika_float)timeout_d;
        }
        if (timeout_f < 0.0f) {
            return PIKA_RES_ERR_INVALID_PARAM;
        }

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);  // Get current time

        // Convert floating seconds to seconds and nanoseconds
        long sec = (long)timeout_f;
        long nsec = (long)((timeout_f - (pika_float)sec) * 1000000000.0);

        ts.tv_sec += sec;
        ts.tv_nsec += nsec;

        // Adjust seconds and nanoseconds if nanoseconds exceed 1 second
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_nsec -= 1000000000;  // Subtract 1 second in nanoseconds
            ts.tv_sec += 1;            // Add 1 second
        }

        pika_GIL_EXIT();
        result = pthread_mutex_timedlock(&m->mutex, &ts);
        pika_GIL_ENTER();
        return result == 0 ? 0 : -1;

    } else if (timeout_type == ARG_TYPE_NONE) {
        if (block) {
            pika_GIL_EXIT();
            result = pthread_mutex_lock(&m->mutex);
            pika_GIL_ENTER();
            return result == 0 ? 0 : -1;
        } else {
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
// Initialize recursive mutex
void pika_platform_thread_rtmutex_init(pika_platform_thread_rtmutex_t* rtm) {
#if defined(__linux) || (PIKA_WIN_PTHREAD_ENABLE)
    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr) != 0) {
        perror("pthread_mutexattr_init");
        exit(EXIT_FAILURE);
    }
    // Set the mutex type to recursive
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

// Destroy recursive mutex
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

// Lock recursive mutex with timeout
int pika_platform_thread_rtmutex_lock(pika_platform_thread_rtmutex_t* rtm,
                                      pika_bool block,
                                      Arg* timeout) {
#if defined(__linux) || (PIKA_WIN_PTHREAD_ENABLE)
    ArgType timeout_type = arg_getType(timeout);
    pika_float timeout_f;
    int result;

    if (!(timeout_type == ARG_TYPE_FLOAT || timeout_type == ARG_TYPE_INT ||
          timeout_type == ARG_TYPE_NONE)) {
        return PIKA_RES_ERR_INVALID_PARAM;
    }

    pika_GIL_EXIT();
    pthread_mutex_lock(&rtm->mutex);
    pika_GIL_ENTER();

    if (rtm->owner == pthread_self()) {
        // If the current thread already owns the lock, increment recursion
        // depth
        rtm->count++;
        pthread_mutex_unlock(&rtm->mutex);
        return 0;
    }

    if (timeout_type == ARG_TYPE_FLOAT || timeout_type == ARG_TYPE_INT) {
        if (timeout_type == ARG_TYPE_FLOAT) {
            timeout_f = arg_getFloat(timeout);
        }
        if (timeout_type == ARG_TYPE_INT) {
            int timeout_d = arg_getInt(timeout);
            timeout_f = (pika_float)timeout_d;
        }
        if (timeout_f < 0.0f) {
            return PIKA_RES_ERR_INVALID_PARAM;
        }

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);  // Get current time

        // Convert floating seconds to seconds and nanoseconds
        long sec = (long)timeout_f;
        long nsec = (long)((timeout_f - (pika_float)sec) * 1000000000.0);

        ts.tv_sec += sec;
        ts.tv_nsec += nsec;

        // Adjust seconds and nanoseconds if nanoseconds exceed 1 second
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_nsec -= 1000000000;  // Subtract 1 second in nanoseconds
            ts.tv_sec += 1;            // Add 1 second
        }

        // Wait until the lock is acquired or timeout occurs
        while (rtm->owner != (pthread_t)0) {
            pika_GIL_EXIT();
            result = pthread_cond_timedwait(&rtm->cond, &rtm->mutex, &ts);
            pika_GIL_ENTER();

            if (result != 0) {
                pthread_mutex_unlock(&rtm->mutex);
                return -1;
            }
        }
        // Set the current thread as the owner of the lock
        rtm->owner = pthread_self();
        rtm->count = 1;
        pthread_mutex_unlock(&rtm->mutex);
        return 0;

    } else if (timeout_type == ARG_TYPE_NONE) {
        if (block) {
            // Wait indefinitely
            while (rtm->owner != (pthread_t)0) {
                pika_GIL_EXIT();
                result = pthread_cond_wait(&rtm->cond, &rtm->mutex);
                pika_GIL_ENTER();

                if (result != 0) {
                    pthread_mutex_unlock(&rtm->mutex);
                    return -1;
                }
            }
            // Set the current thread as the owner of the lock
            rtm->owner = pthread_self();
            rtm->count = 1;
            pthread_mutex_unlock(&rtm->mutex);
            return 0;

        } else {
            // Non-blocking mode
            if (rtm->owner == (pthread_t)0) {
                // Acquire the lock if no other thread owns it
                rtm->owner = pthread_self();
                rtm->count = 1;
                pthread_mutex_unlock(&rtm->mutex);
                return 0;
            } else {
                // If another thread owns the lock, return -1 immediately
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

// Release recursive mutex
int pika_platform_thread_rtmutex_unlock(pika_platform_thread_rtmutex_t* rtm) {
#if defined(__linux) || (PIKA_WIN_PTHREAD_ENABLE)
    pthread_t self = pthread_self();

    pika_GIL_EXIT();
    pthread_mutex_lock(&rtm->mutex);
    pika_GIL_ENTER();

    if (rtm->owner != self) {
        perror("Attempt to unlock a mutex not owned by the current thread");
        pthread_mutex_unlock(&rtm->mutex);
        return -1;
    }

    rtm->count--;
    if (rtm->count == 0) {
        rtm->owner = (pthread_t)0;
        pthread_cond_signal(&rtm->cond);
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

// Check if recursive mutex is held by the current thread
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
    cond->owner = (pthread_t)0;  // Reset owner after releasing resources
#elif PIKA_FREERTOS_ENABLE
#elif PIKA_RTTHREAD_ENABLE
#elif PIKA_ZEUSOS_ENABLE
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR(_);
#endif
}

// Check if the current thread owns the mutex
static int is_mutex_owned(pika_platform_thread_cond_t* cond) {
    if (pthread_mutex_trylock(&cond->rtmutex.mutex) == EBUSY) {
        if (pthread_mutex_unlock(&cond->rtmutex.mutex) == 0) {
            return 1;  // Current thread owns the lock
        }
    }

    return 0;
}

// Condition variable wait with blocking and timeout functionality
int pika_platform_thread_cond_timedwait(pika_platform_thread_cond_t* cond,
                                        Arg* timeout) {
#if defined(__linux) || (PIKA_WIN_PTHREAD_ENABLE)
    ArgType timeout_type = arg_getType(timeout);
    pika_float timeout_f;
    int result;

    // Check if the mutex is already acquired
    if (!is_mutex_owned(cond)) {
        return -1;
    }

    if (!(timeout_type == ARG_TYPE_FLOAT || timeout_type == ARG_TYPE_INT ||
          timeout_type == ARG_TYPE_NONE)) {
        return PIKA_RES_ERR_INVALID_PARAM;
    }

    if (timeout_type == ARG_TYPE_FLOAT || timeout_type == ARG_TYPE_INT) {
        if (timeout_type == ARG_TYPE_FLOAT) {
            timeout_f = arg_getFloat(timeout);
        } else {
            int timeout_d = arg_getInt(timeout);
            timeout_f = (pika_float)timeout_d;
        }

        if (timeout_f < 0.0f) {
            return PIKA_RES_ERR_INVALID_PARAM;
        }

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);  // Get current time

        // Convert floating seconds to seconds and nanoseconds
        long sec = (long)timeout_f;
        long nsec = (long)((timeout_f - (pika_float)sec) * 1000000000.0);

        ts.tv_sec += sec;
        ts.tv_nsec += nsec;

        // Adjust seconds and nanoseconds if nanoseconds exceed 1 second
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_nsec -= 1000000000;  // Subtract 1 second in nanoseconds
            ts.tv_sec += 1;            // Add 1 second
        }

        // Wait until notified or timeout occurs
        pika_GIL_EXIT();
        result = pthread_cond_timedwait(&cond->cond, &cond->rtmutex.mutex, &ts);
        pika_GIL_ENTER();

        if (result != 0) {
            if (result == ETIMEDOUT) {
                return -1;  // Timeout
            }
            perror("pthread_cond_timedwait");
            return -1;  // Other errors
        }
    } else if (timeout_type == ARG_TYPE_NONE) {
        // Wait indefinitely
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

// Signal condition variable
int pika_platform_thread_cond_signal(pika_platform_thread_cond_t* cond) {
#if defined(__linux) || (PIKA_WIN_PTHREAD_ENABLE)
    int result;
    result = pthread_cond_signal(&cond->cond);
    cond->owner = (pthread_t)0;  // Reset owner after notifying
    return result == 0 ? 0 : -1;
#elif PIKA_FREERTOS_ENABLE
#elif PIKA_RTTHREAD_ENABLE
#elif PIKA_ZEUSOS_ENABLE
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR(_);
#endif
}

// Broadcast condition variable
int pika_platform_thread_cond_broadcast(pika_platform_thread_cond_t* cond) {
#if defined(__linux) || (PIKA_WIN_PTHREAD_ENABLE)
    int result;
    result = pthread_cond_broadcast(&cond->cond);
    cond->owner = (pthread_t)0;  // Reset owner after broadcasting
    return result == 0 ? 0 : -1;
#elif PIKA_FREERTOS_ENABLE
#elif PIKA_RTTHREAD_ENABLE
#elif PIKA_ZEUSOS_ENABLE
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR(_);
#endif
}
