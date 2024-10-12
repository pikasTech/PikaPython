#include "threading_Lock.h"
#include "PikaPlatformEx.h"

void threading_Lock___del__(PikaObj* self) {
    pika_platform_thread_mutex_t* m = obj_getPtr(self, "_mutex_");
    // pika_platform_thread_mutex_unlock(m);
    pika_platform_thread_mutex_destroy(m);
    pikaFree(m, sizeof(pika_platform_thread_mutex_t));
}

void threading_Lock___init__(PikaObj* self) {
    pika_platform_thread_mutex_t* m =
        pikaMalloc(sizeof(pika_platform_thread_mutex_t));
    pika_platform_thread_mutex_init(m);
    obj_setPtr(self, "_mutex_", m);
}
pika_bool threading_Lock_acquire(PikaObj* self, pika_bool block, Arg* timeout) {
    pika_platform_thread_mutex_t* m = obj_getPtr(self, "_mutex_");
    int result = pika_platform_thread_mutex_timedlock(m, block, timeout);
    if (result == PIKA_RES_ERR_INVALID_PARAM) {
        obj_setErrorCode(self, PIKA_RES_ERR_INVALID_PARAM);
        obj_setSysOut(self, "invalid param!");
    }
    return result == 0 ? pika_true : pika_false;
}
pika_bool threading_Lock_locked(PikaObj* self) {
    pika_platform_thread_mutex_t* m = obj_getPtr(self, "_mutex_");
    pika_GIL_EXIT();
    int result = pika_platform_thread_mutex_trylock(m);
    pika_GIL_ENTER();
    if (result == 0) {
        // 成功获得了锁，需要解锁
        // pika_GIL_EXIT();
        pika_platform_thread_mutex_unlock(m);
        // pika_GIL_ENTER();/*  */
        return pika_false;  // 锁未被占用/*  */
    } else {
        // 锁已被占用或发生了其他错误
        // perror("pthread_mutex_trylock");
        return pika_true;
    }
}

void threading_Lock_release(PikaObj* self) {
    pika_platform_thread_mutex_t* m = obj_getPtr(self, "_mutex_");
    // pika_GIL_EXIT();
    pika_platform_thread_mutex_unlock(m);
    // pika_GIL_ENTER();
}
