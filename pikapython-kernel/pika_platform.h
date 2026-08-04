/* SPEC: PJ2026-0501 kernel v0.34; weak platform defaults. */
#ifndef PIKA_PLATFORM_H
#define PIKA_PLATFORM_H

#include <stddef.h>
#include <stdint.h>

#include "pika_status.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__) || defined(__clang__)
#define PIKA_WEAK __attribute__((weak))
#elif defined(__CC_ARM)
#define PIKA_WEAK __weak
#elif defined(__ICCARM__)
#define PIKA_WEAK __weak
#else
#define PIKA_WEAK
#endif

PikaStatus pika_platform_write(const char* data, size_t size);
PikaStatus pika_platform_repl_read(
    uint8_t* data,
    size_t capacity,
    size_t* received);
PikaStatus pika_platform_time_ticks_ms(uint32_t* ticks);
PikaStatus pika_platform_time_sleep_ms(uint32_t duration);

#ifdef __cplusplus
}
#endif

#endif
