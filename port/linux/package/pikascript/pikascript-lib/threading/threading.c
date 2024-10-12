#include "threading.h"

int64_t threading_get_ident(PikaObj* self) {
    return (int64_t)pika_platform_thread_self();
}
