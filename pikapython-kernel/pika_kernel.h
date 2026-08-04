/* SPEC: PJ2026-0501 V2 kernel v0.47; execution API. */
#ifndef PIKA_KERNEL_H
#define PIKA_KERNEL_H

#include <stddef.h>
#include <stdint.h>

#include "pika_program.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const int64_t* values;
    uint16_t count;
} PikaArguments;

typedef enum {
    PIKA_RUNTIME_VALUE_NONE = 0,
    PIKA_RUNTIME_VALUE_BOOLEAN = 1,
    PIKA_RUNTIME_VALUE_INTEGER = 2,
    PIKA_RUNTIME_VALUE_FLOAT = 3,
    PIKA_RUNTIME_VALUE_STRING = 4,
    PIKA_RUNTIME_VALUE_BYTES = 5,
    PIKA_RUNTIME_VALUE_LIST = 6,
    PIKA_RUNTIME_VALUE_TUPLE = 7,
    PIKA_RUNTIME_VALUE_DICT = 8,
    PIKA_RUNTIME_VALUE_SET = 9,
    PIKA_RUNTIME_VALUE_RANGE = 10,
    PIKA_RUNTIME_VALUE_ITERATOR = 11,
    PIKA_RUNTIME_VALUE_SLICE = 12,
    PIKA_RUNTIME_VALUE_TYPE = 13,
    PIKA_RUNTIME_VALUE_BYTEARRAY = 14,
    PIKA_RUNTIME_VALUE_INSTANCE = 15,
    PIKA_RUNTIME_VALUE_CALLABLE = 16,
    PIKA_RUNTIME_VALUE_NATIVE_OBJECT = 17,
    PIKA_RUNTIME_VALUE_EXCEPTION = 18,
    PIKA_RUNTIME_VALUE_MODULE = 19,
} PikaRuntimeValueKind;

typedef struct {
    PikaRuntimeValueKind kind;
    uint8_t reserved;
    uint16_t flags;
    union {
        int64_t integer;
        double floating;
        uint32_t object_index;
    } as;
} PikaRuntimeValue;

typedef struct {
    uint8_t kind;
    uint8_t active;
    uint8_t owned;
    uint8_t reserved;
    uint32_t length;
    uint32_t capacity;
    void* data;
} PikaObjectSlot;

typedef void* (*PikaObjectAllocate)(void* context, uint32_t size);
typedef void (*PikaObjectRelease)(void* context, void* pointer);
typedef PikaStatus (*PikaObjectReserve)(void* context,
                                       uint16_t slot_capacity,
                                       uint32_t arena_capacity);

typedef struct {
    PikaObjectSlot* slots;
    uint16_t capacity;
    uint16_t count;
    uint8_t* arena;
    uint32_t arena_capacity;
    uint32_t arena_used;
    PikaObjectAllocate allocate;
    PikaObjectRelease release;
    void* context;
    uint16_t slot_limit;
    uint32_t arena_limit;
    PikaObjectReserve reserve;
} PikaObjectStorage;

typedef struct {
    void* frames;
    uint32_t frame_bytes;
    int64_t* values;
    uint32_t value_capacity;
    PikaRuntimeValue* typed_values;
    uint32_t typed_value_capacity;
    PikaObjectStorage* objects;
} PikaStorage;

typedef struct {
    PikaBindingValue* values;
    uint32_t value_capacity;
    PikaBindingObject* objects;
    uint16_t object_capacity;
} PikaBindingStorage;

typedef struct {
    uint16_t persistent_slot_count;
    uint16_t binding_object_count;
    uint8_t initialized;
    uint8_t uses_typed_values;
    uint8_t uses_binding_values;
#if PIKA_GC_ENABLE
    uint8_t object_collection_pending;
#endif
} PikaExecutionState;

typedef struct {
    PikaStatus status;
    int64_t value;
    uint32_t instruction_index;
    PikaRuntimeValue typed_value;
} PikaResult;

typedef struct {
    uint64_t instruction_count;
    uint64_t call_count;
    uint32_t frame_peak;
    uint32_t value_peak;
    uint32_t execution_storage_peak_bytes;
    uint32_t host_stack_peak_bytes;
    uint32_t binding_call_count;
    uint16_t binding_object_peak;
    uint32_t builtin_call_count;
    uint16_t object_peak;
#if PIKA_GC_ENABLE
    uint32_t gc_collection_count;
    uint32_t gc_reclaimed_arena_bytes;
    uint32_t gc_reclaimed_object_count;
    uint32_t gc_reclaimed_binding_object_count;
#endif
} PikaMetrics;

typedef PikaStatus (*PikaOutputWrite)(void* context,
                                     const char* bytes,
                                     uint32_t length);

typedef struct {
    PikaOutputWrite write;
    void* context;
} PikaOutput;

PikaStatus pika_program_execute(const PikaProgram* program,
                                const PikaArguments* arguments,
                                PikaStorage* storage,
                                const PikaOutput* output,
                                PikaResult* result,
                                PikaMetrics* metrics);
PikaStatus pika_program_execute_bound(const PikaProgram* program,
                                      const PikaArguments* arguments,
                                      PikaStorage* storage,
                                      PikaBindingStorage* binding_storage,
                                      const PikaOutput* output,
                                      PikaResult* result,
                                      PikaMetrics* metrics);
PikaStatus pika_program_execute_stateful(
    const PikaProgram* program,
    const PikaArguments* arguments,
    PikaStorage* storage,
    PikaBindingStorage* binding_storage,
    const PikaOutput* output,
    uint16_t persistent_slot_count,
    PikaExecutionState* state,
    PikaResult* result,
    PikaMetrics* metrics);
void pika_execution_state_reset(
    PikaExecutionState* state,
    PikaStorage* storage,
    PikaBindingStorage* binding_storage);
#if PIKA_TYPED_RUNTIME_ENABLE
void pika_object_storage_reset(PikaObjectStorage* storage);
void pika_object_storage_release(PikaObjectStorage* storage);
#endif

#ifdef __cplusplus
}
#endif

#endif
