#include "threading_Lock.h"
#include "PikaPlatformEx.h"

void threading_RLock___del__(PikaObj* self) {
    pika_platform_thread_rtmutex_t* rm = obj_getPtr(self, "_rmutex_");
    pika_platform_thread_rtmutex_destroy(rm);
    pikaFree(rm, sizeof(pika_platform_thread_rtmutex_t));
}
void threading_RLock___init__(PikaObj* self) {
    pika_platform_thread_rtmutex_t* rm =
        pikaMalloc(sizeof(pika_platform_thread_rtmutex_t));
    pika_platform_thread_rtmutex_init(rm);
    obj_setPtr(self, "_rmutex_", rm);
}
pika_bool threading_RLock_acquire(PikaObj* self,
                                  pika_bool block,
                                  Arg* timeout) {
    pika_platform_thread_rtmutex_t* rm = obj_getPtr(self, "_rmutex_");

    int result = pika_platform_thread_rtmutex_lock(rm, block, timeout);

    return result == 0 ? pika_true : pika_false;
}
pika_bool threading_RLock_locked(PikaObj* self) {
    pika_platform_thread_rtmutex_t* rm = obj_getPtr(self, "_rmutex_");

    int result = pika_platform_thread_rtmutex_locked(rm);

    return result != 0 ? pika_true : pika_false;
}
void threading_RLock_release(PikaObj* self) {
    pika_platform_thread_rtmutex_t* rm = obj_getPtr(self, "_rmutex_");
    pika_platform_thread_rtmutex_unlock(rm);
}
