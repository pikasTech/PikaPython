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
        // Successfully acquired the lock, need to unlock it
        pika_platform_thread_mutex_unlock(m);
        return pika_false;  // Lock is not held
    } else {
        // Lock is held or an error occurred
        return pika_true;
    }
}

void threading_Lock_release(PikaObj* self) {
    pika_platform_thread_mutex_t* m = obj_getPtr(self, "_mutex_");
    pika_platform_thread_mutex_unlock(m);
}
