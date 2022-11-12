/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 18:27:19
 * @LastEditTime: 2020-02-23 15:01:06
 * @Description: the code belongs to jiejie, please keep the author information
 * and source code according to the license.
 */
#include "platform_mutex.h"

PIKA_WEAK int platform_mutex_init(platform_mutex_t* m) {
#ifdef __linux
    return pthread_mutex_init(&(m->mutex), NULL);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int platform_mutex_lock(platform_mutex_t* m) {
#ifdef __linux
    return pthread_mutex_lock(&(m->mutex));
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int platform_mutex_trylock(platform_mutex_t* m) {
#ifdef __linux
    return pthread_mutex_trylock(&(m->mutex));
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

int platform_mutex_unlock(platform_mutex_t* m) {
#ifdef __linux
    return pthread_mutex_unlock(&(m->mutex));
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

int platform_mutex_destroy(platform_mutex_t* m) {
#ifdef __linux
    return pthread_mutex_destroy(&(m->mutex));
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}
