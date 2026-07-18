#include "threading_Lock.h"
#include "PikaPlatformEx.h"

typedef struct {
    pika_platform_thread_lock_t lock;
} ThreadingLockState;

static ThreadingLockState* _lock_state(PikaObj* self) {
    return obj_getPtr(self, "_mutex_");
}

static void _lock_release_error(PikaObj* self) {
    obj_setErrorCode(self, PIKA_RES_ERR_RUNTIME_ERROR);
    obj_setSysOut(self, "RuntimeError: release unlocked lock");
}

void threading_Lock___del__(PikaObj* self) {
    ThreadingLockState* state = _lock_state(self);
    pika_platform_thread_lock_destroy(&state->lock);
    pikaFree(state, sizeof(ThreadingLockState));
}

void threading_Lock___init__(PikaObj* self) {
    ThreadingLockState* state = pikaMalloc(sizeof(ThreadingLockState));
    pika_platform_thread_lock_init(&state->lock);
    obj_setPtr(self, "_mutex_", state);
}

pika_bool threading_Lock_acquire(PikaObj* self, pika_bool block, Arg* timeout) {
    ThreadingLockState* state = _lock_state(self);
    int result =
        pika_platform_thread_lock_acquire(&state->lock, block, timeout);
    if (result == PIKA_RES_ERR_INVALID_PARAM) {
        obj_setErrorCode(self, PIKA_RES_ERR_INVALID_PARAM);
        obj_setSysOut(self, "invalid param!");
    }
    if (result != 0) {
        return pika_false;
    }
    return pika_true;
}

pika_bool threading_Lock_locked(PikaObj* self) {
    ThreadingLockState* state = _lock_state(self);
    return pika_platform_thread_lock_locked(&state->lock) ? pika_true
                                                           : pika_false;
}

void threading_Lock_release(PikaObj* self) {
    ThreadingLockState* state = _lock_state(self);
    if (pika_platform_thread_lock_release(&state->lock) != 0) {
        _lock_release_error(self);
    }
}
