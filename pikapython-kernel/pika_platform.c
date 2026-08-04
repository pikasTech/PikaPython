/* SPEC: PJ2026-0501 kernel v0.34; weak platform defaults. */
#include "pika_platform.h"

PIKA_WEAK PikaStatus pika_platform_write(
    const char* data,
    size_t size) {
    (void)data;
    (void)size;
    return PIKA_STATUS_OK;
}

PIKA_WEAK PikaStatus pika_platform_repl_read(
    uint8_t* data,
    size_t capacity,
    size_t* received) {
    (void)data;
    (void)capacity;
    if (received != NULL) *received = 0u;
    return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
}

PIKA_WEAK PikaStatus pika_platform_time_ticks_ms(uint32_t* ticks) {
    if (ticks == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
}

PIKA_WEAK PikaStatus pika_platform_time_sleep_ms(uint32_t duration) {
    (void)duration;
    return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
}
