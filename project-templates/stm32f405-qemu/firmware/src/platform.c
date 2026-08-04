/* SPEC: PJ2026-050106 REPL v0.5; QEMU semihosting byte transport. */
#include <stddef.h>
#include <stdint.h>

#include "pika_platform.h"

extern uint8_t __heap_start;
extern uint8_t __heap_end;
extern uint8_t __stack_bottom;
extern uint8_t __stack_top;
extern int pika_semihost_call(int operation, const void* argument);

static uint8_t* pika_heap_cursor = &__heap_start;
static uint32_t pika_heap_peak;
static uint32_t pika_qemu_time_ticks = 1000u;

void* _sbrk(ptrdiff_t increment) {
    uint8_t* previous = pika_heap_cursor;
    if (increment < 0) {
        uintptr_t decrease = (uintptr_t)(-increment);
        if (decrease > (uintptr_t)(pika_heap_cursor - &__heap_start)) {
            return (void*)-1;
        }
        pika_heap_cursor -= decrease;
        return previous;
    }
    if ((uintptr_t)increment >
        (uintptr_t)(&__heap_end - pika_heap_cursor)) {
        return (void*)-1;
    }
    pika_heap_cursor += (uintptr_t)increment;
    if ((uint32_t)(pika_heap_cursor - &__heap_start) >
        pika_heap_peak) {
        pika_heap_peak =
            (uint32_t)(pika_heap_cursor - &__heap_start);
    }
    return previous;
}

static void pika_qemu_write_u32(uint32_t value) {
    char digits[10];
    size_t count = 0u;
    do {
        digits[count++] = (char)('0' + value % 10u);
        value /= 10u;
    } while (value != 0u);
    while (count > 0u) {
        --count;
        (void)pika_platform_write(&digits[count], 1u);
    }
}

static uint32_t pika_qemu_stack_peak_bytes(void) {
    const uint32_t pattern = 0xa5a5a5a5u;
    const uint32_t* cursor =
        (const uint32_t*)(const void*)&__stack_bottom;
    const uint32_t* top =
        (const uint32_t*)(const void*)&__stack_top;
    while (cursor < top && *cursor == pattern) {
        ++cursor;
    }
    return (uint32_t)((uintptr_t)top - (uintptr_t)cursor);
}

void pika_qemu_report_resources(void) {
    static const char heap_prefix[] =
        "PIKA_QEMU_RESOURCE heap_peak_bytes=";
    static const char stack_prefix[] = " stack_peak_bytes=";
    static const char newline[] = "\n";
    (void)pika_platform_write(
        heap_prefix, sizeof(heap_prefix) - 1u);
    pika_qemu_write_u32(pika_heap_peak);
    (void)pika_platform_write(
        stack_prefix, sizeof(stack_prefix) - 1u);
    pika_qemu_write_u32(pika_qemu_stack_peak_bytes());
    (void)pika_platform_write(newline, sizeof(newline) - 1u);
}

PikaStatus pika_platform_write(const char* data, size_t size) {
    size_t index;
    if (data == NULL && size != 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0u; index < size; ++index) {
        (void)pika_semihost_call(0x03, &data[index]);
    }
    return PIKA_STATUS_OK;
}

PikaStatus pika_platform_repl_read(
    uint8_t* data,
    size_t capacity,
    size_t* received) {
    int value;
    if (data == NULL || received == NULL || capacity == 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    value = pika_semihost_call(0x07, NULL);
    if (value < 0) {
        *received = 0u;
        return PIKA_STATUS_IO_ERROR;
    }
    data[0] = (uint8_t)value;
    *received = 1u;
    return PIKA_STATUS_OK;
}

PikaStatus pika_platform_time_ticks_ms(uint32_t* ticks) {
    if (ticks == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *ticks = pika_qemu_time_ticks;
    pika_qemu_time_ticks += 5u;
    return PIKA_STATUS_OK;
}

PikaStatus pika_platform_time_sleep_ms(uint32_t duration) {
    pika_qemu_time_ticks += duration;
    return PIKA_STATUS_OK;
}

void pika_qemu_exit(int status) {
    uintptr_t arguments[2];
    arguments[0] = 0x20026u;
    arguments[1] = status == 0 ? 0u : 1u;
    (void)pika_semihost_call(0x20, arguments);
    for (;;) {
    }
}

void _exit(int status) {
    pika_qemu_exit(status);
}
