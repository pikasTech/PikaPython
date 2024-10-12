#include "threading_Condition.h"
#include "PikaPlatformEx.h"

void threading_Condition___del__(PikaObj* self) {
    pika_platform_thread_cond_t* cond = obj_getPtr(self, "_cond_");
    pika_platform_thread_cond_destroy(cond);
    pikaFree(cond, sizeof(pika_platform_thread_cond_t));
}

void threading_Condition___init__(PikaObj* self) {
    pika_platform_thread_cond_t* cond =
        pikaMalloc(sizeof(pika_platform_thread_cond_t));
    pika_platform_thread_cond_init(cond);
    obj_setPtr(self, "_cond_", cond);
}

pika_bool threading_Condition__is_owned(PikaObj* self) {
    pika_platform_thread_cond_t* cond = obj_getPtr(self, "_cond_");
    if (cond->owner != 0) {
        return pika_true;
    } else {
        return pika_false;
    }
}

void threading_Condition_notify(PikaObj* self, int n) {
    pika_platform_thread_cond_t* cond = obj_getPtr(self, "_cond_");
    pika_platform_thread_cond_signal(cond);
}

void threading_Condition_notify_all(PikaObj* self) {
    pika_platform_thread_cond_t* cond = obj_getPtr(self, "_cond_");
    pika_platform_thread_cond_broadcast(cond);
}

pika_bool threading_Condition_wait(PikaObj* self, Arg* timeout) {
    // pika_platform_thread_cond_t * cond = obj_getPtr(self, "_cond_");
    return pika_false;
}
pika_bool threading_Condition_acquire(PikaObj* self,
                                      pika_bool block,
                                      Arg* timeout) {
    return pika_false;
}
void threading_Condition_release(PikaObj* self) {}
