/* SPEC: PJ2026-0501 V2 kernel v0.46; runtime internals. */
#ifndef PIKA_RUNTIME_INTERNAL_H
#define PIKA_RUNTIME_INTERNAL_H

#include <stdint.h>

#include "pika_kernel.h"
#include "pika_builtin.h"
#include "pika_text_method.h"

#ifndef PIKA_RUNTIME_VALUE_LIMIT
#define PIKA_RUNTIME_VALUE_LIMIT 128u
#endif
#define PIKA_RUNTIME_EXCEPTION_NO_MESSAGE UINT32_MAX

typedef struct {
    uint32_t function_index;
    uint32_t instruction_index;
    uint32_t value_base;
    uint16_t slot_count;
    uint16_t return_slot;
} PikaRuntimeFrame;

typedef struct {
    int64_t start;
    int64_t stop;
    int64_t step;
} PikaRangeData;

typedef struct {
    uint32_t source_index;
    uint32_t position;
} PikaIteratorData;

typedef struct {
    PikaRuntimeValue start;
    PikaRuntimeValue stop;
    PikaRuntimeValue step;
} PikaSliceData;

typedef struct {
    PikaRuntimeValue key;
    PikaRuntimeValue value;
} PikaMapEntry;

typedef struct {
    const PikaProgram* program;
    PikaRuntimeFrame* frames;
    int64_t* values;
    PikaRuntimeValue* typed_values;
    PikaObjectStorage* objects;
    PikaBindingStorage* binding_storage;
#if PIKA_CAPABILITY_EXCEPTION_BASIC_ENABLE
    PikaRuntimeValue pending_exception;
    uint32_t exception_instruction_index;
#endif
    uint16_t binding_object_count;
    uint32_t depth;
    uint32_t binding_callback_depth;
    uint8_t binding_callback_active;
#if PIKA_GC_ENABLE
    uint8_t object_collection_pending;
    uint8_t gc_root_active;
    uint16_t gc_retry_block_depth;
    PikaRuntimeValue gc_root;
#endif
#if PIKA_CAPABILITY_EXCEPTION_BASIC_ENABLE
    uint8_t has_pending_exception;
    uint8_t has_exception_instruction;
#endif
    uint32_t max_slots;
    uint32_t value_capacity;
    const PikaOutput* output;
    PikaResult* result;
    PikaMetrics* metrics;
    uintptr_t stack_origin;
} PikaRuntimeContext;

uint32_t pika_runtime_max_slots(const PikaProgram* program);
PikaStatus pika_runtime_analyze_value_count(
    const PikaProgram* program,
    uint32_t* value_count);
uint16_t pika_runtime_object_slot_capacity(
    const PikaProgram* program);
uint32_t pika_runtime_object_arena_capacity(
    const PikaProgram* program);
int pika_runtime_object_storage_accepts(
    const PikaObjectStorage* storage,
    uint16_t slot_capacity,
    uint32_t arena_capacity);
int pika_runtime_requires_binding_values(
    const PikaProgram* program);
PikaStatus pika_runtime_initialize(PikaRuntimeContext* context,
                                   const PikaProgram* program,
                                   const PikaArguments* arguments,
                                   PikaStorage* storage,
                                   PikaBindingStorage* binding_storage,
                                   const PikaOutput* output,
                                   PikaResult* result,
                                   PikaMetrics* metrics,
                                   uintptr_t stack_origin);
PikaStatus pika_runtime_initialize_stateful(
    PikaRuntimeContext* context,
    const PikaProgram* program,
    const PikaArguments* arguments,
    PikaStorage* storage,
    PikaBindingStorage* binding_storage,
    const PikaOutput* output,
    uint16_t persistent_slot_count,
    const PikaExecutionState* state,
    PikaResult* result,
    PikaMetrics* metrics,
    uintptr_t stack_origin);
PikaStatus pika_runtime_push_call(PikaRuntimeContext* context,
                                  uint16_t return_slot,
                                  uint16_t argument_base,
                                  uint16_t argument_count,
                                  uint16_t function_index);
PikaStatus pika_runtime_push_callable_call(
    PikaRuntimeContext* context,
    uint16_t return_slot,
    uint16_t callable_slot,
    uint16_t argument_base,
    uint16_t argument_count);
PikaStatus pika_runtime_push_dynamic_method_call(
    PikaRuntimeContext* context,
    uint16_t return_slot,
    uint16_t argument_base,
    uint16_t argument_count,
    uint16_t call_site_index);
PikaStatus pika_runtime_push_callback(
    PikaRuntimeContext* context,
    uint16_t function_index);
PikaStatus pika_runtime_return(PikaRuntimeContext* context,
                               uint16_t slot,
                               int* finished);
void pika_runtime_record_stack(PikaRuntimeContext* context,
                               const void* marker);
void pika_runtime_update_storage_peak(PikaRuntimeContext* context);
#if PIKA_GC_ENABLE
int pika_runtime_object_collection_needed(
    const PikaRuntimeContext* context);
void pika_runtime_note_object_pressure(
    PikaRuntimeContext* context);
void pika_runtime_collect_objects(PikaRuntimeContext* context);
#endif
void pika_runtime_set_error(PikaRuntimeContext* context,
                            PikaStatus status,
                            uint32_t instruction_index);
void pika_runtime_set_instruction_error(
    PikaRuntimeContext* context,
    PikaStatus status,
    uint32_t instruction_index,
    const PikaInstruction* instruction);
void pika_runtime_finalize_error(PikaRuntimeContext* context,
                                 PikaStatus status);
PikaStatus pika_runtime_raise_status(
    PikaRuntimeContext* context,
    const PikaRuntimeFrame* frame,
    const PikaInstruction* instruction,
    PikaStatus* status);
PikaStatus pika_runtime_execute_fallback_opcode(
    PikaRuntimeContext* context,
    const PikaInstruction* instruction);
int pika_runtime_handle_exception(PikaRuntimeContext* context,
                                  PikaStatus status,
                                  uint32_t instruction_index);
#if PIKA_RUNTIME_MULTIPLY_HELPER_ENABLE
PikaStatus pika_runtime_integer_multiply_checked(
    int64_t left,
    int64_t right,
    int64_t* result);
#endif
#if PIKA_RUNTIME_POWER_HELPER_ENABLE
PikaStatus pika_runtime_integer_power(
    int64_t base,
    int64_t exponent,
    int64_t* result);
#endif
#if PIKA_CAPABILITY_BUILTIN_PRINT_ENABLE
PikaStatus pika_runtime_write_register_value(
    PikaRuntimeContext* context,
    const PikaInstruction* instruction,
    int64_t value);
#endif
PikaStatus pika_runtime_execute_register(PikaRuntimeContext* context);
PikaStatus pika_runtime_execute_register_segmented(
    PikaRuntimeContext* context);
PikaStatus pika_binding_invoke_validated(
    const PikaBindingModule* module,
    const PikaBindingCallable* callable,
    void* instance,
    const PikaBindingValue* arguments,
    uint8_t argument_count,
    PikaBindingValue* result);
PikaStatus pika_binding_invoke_call_site(
    const PikaBindingModule* module,
    const PikaBindingCallable* callable,
    void* instance,
    const PikaBindingValue* arguments,
    uint8_t argument_count,
    PikaBindingValue* result,
    uint8_t call_site_flags);
PikaStatus pika_binding_construct_validated(
    const PikaBindingModule* module,
    const PikaBindingClass* class_descriptor,
    const PikaBindingValue* arguments,
    uint8_t argument_count,
    PikaBindingObject* object);
PikaStatus pika_binding_construct_call_site(
    const PikaBindingModule* module,
    const PikaBindingClass* class_descriptor,
    const PikaBindingValue* arguments,
    uint8_t argument_count,
    PikaBindingObject* object,
    uint8_t call_site_flags);
PikaStatus pika_binding_invoke_method_validated(
    const PikaBindingModule* module,
    const PikaBindingObject* object,
    const PikaBindingCallable* method,
    const PikaBindingValue* arguments,
    uint8_t argument_count,
    PikaBindingValue* result);
PikaStatus pika_binding_invoke_method_call_site(
    const PikaBindingModule* module,
    const PikaBindingObject* object,
    const PikaBindingCallable* method,
    const PikaBindingValue* arguments,
    uint8_t argument_count,
    PikaBindingValue* result,
    uint8_t call_site_flags);
PikaStatus pika_runtime_binding_call(
    PikaRuntimeContext* context,
    const PikaInstruction* instruction);
PikaStatus pika_runtime_execute_binding_opcode(
    PikaRuntimeContext* context,
    const PikaInstruction* instruction);
#if PIKA_CAPABILITY_PROTOCOL_TEXT_NETWORK_ENABLE
PikaStatus pika_runtime_execute_text_method(
    PikaRuntimeContext* context,
    const PikaRuntimeFrame* frame,
    uint16_t method_id,
    uint8_t argument_base,
    uint8_t argument_count,
    PikaRuntimeValue* result);
#endif
PikaRuntimeValue pika_runtime_read_value(
    const PikaRuntimeContext* context,
    uint32_t index);
PikaStatus pika_runtime_write_value(
    PikaRuntimeContext* context,
    uint32_t index,
    PikaRuntimeValue value);
PikaStatus pika_runtime_create_object(
    PikaRuntimeContext* context,
    PikaRuntimeValueKind kind,
    uint32_t length,
    uint32_t element_size,
    uint32_t* object_index,
    void** data);
PikaStatus pika_runtime_create_sequence(
    PikaRuntimeContext* context,
    PikaRuntimeValueKind kind,
    const PikaRuntimeValue* values,
    uint32_t length,
    PikaRuntimeValue* result);
PikaStatus pika_runtime_create_text(
    PikaRuntimeContext* context,
    PikaRuntimeValueKind kind,
    const char* bytes,
    uint32_t length,
    PikaRuntimeValue* result);
PikaStatus pika_runtime_create_text_buffer(
    PikaRuntimeContext* context,
    PikaRuntimeValueKind kind,
    uint32_t length,
    PikaRuntimeValue* result,
    unsigned char** bytes);
PikaStatus pika_runtime_create_instance(
    PikaRuntimeContext* context,
    uint16_t class_index,
    PikaRuntimeValue* result);
PikaStatus pika_runtime_load_attribute(
    PikaRuntimeContext* context,
    PikaRuntimeValue instance,
    uint16_t field_index,
    PikaRuntimeValue* result);
PikaStatus pika_runtime_store_attribute(
    PikaRuntimeContext* context,
    PikaRuntimeValue instance,
    uint16_t field_index,
    PikaRuntimeValue value);
PikaStatus pika_runtime_create_range(
    PikaRuntimeContext* context,
    int64_t start,
    int64_t stop,
    int64_t step,
    PikaRuntimeValue* result);
PikaStatus pika_runtime_range_length(
    PikaRuntimeContext* context,
    PikaRuntimeValue value,
    uint32_t* length);
PikaStatus pika_runtime_create_iterator(
    PikaRuntimeContext* context,
    PikaRuntimeValue source,
    PikaRuntimeValue* result);
PikaStatus pika_runtime_create_slice(
    PikaRuntimeContext* context,
    PikaRuntimeValue start,
    PikaRuntimeValue stop,
    PikaRuntimeValue step,
    PikaRuntimeValue* result);
PikaObjectSlot* pika_runtime_object(
    PikaRuntimeContext* context,
    PikaRuntimeValue value);
int pika_runtime_value_truthy(
    const PikaRuntimeContext* context,
    PikaRuntimeValue value);
PikaStatus pika_runtime_value_equal(
    PikaRuntimeContext* context,
    PikaRuntimeValue left,
    PikaRuntimeValue right,
    int* equal);
PikaStatus pika_runtime_sequence_item(
    PikaRuntimeContext* context,
    PikaRuntimeValue value,
    int64_t index,
    PikaRuntimeValue* result);
PikaStatus pika_runtime_iteration_item(
    PikaRuntimeContext* context,
    PikaRuntimeValue value,
    uint32_t index,
    PikaRuntimeValue* result,
    int* available);
#if PIKA_CAPABILITY_ASSIGNMENT_UNPACK_ENABLE
PikaStatus pika_runtime_iterable_length(
    PikaRuntimeContext* context,
    PikaRuntimeValue value,
    uint32_t* length);
#endif
PikaStatus pika_runtime_set_item(
    PikaRuntimeContext* context,
    PikaRuntimeValue container,
    PikaRuntimeValue index,
    PikaRuntimeValue value);
PikaStatus pika_runtime_append(
    PikaRuntimeContext* context,
    PikaRuntimeValue container,
    PikaRuntimeValue value,
    PikaRuntimeValue* result);
PikaStatus pika_runtime_pop(
    PikaRuntimeContext* context,
    PikaRuntimeValue container,
    PikaRuntimeValue index,
    int has_index,
    PikaRuntimeValue default_value,
    int has_default,
    PikaRuntimeValue* result);
PikaStatus pika_runtime_dict_lookup(
    PikaRuntimeContext* context,
    PikaRuntimeValue dictionary,
    PikaRuntimeValue key,
    PikaRuntimeValue* result,
    int* found);
PikaStatus pika_runtime_sequence_slice(
    PikaRuntimeContext* context,
    PikaRuntimeValue value,
    PikaRuntimeValue start,
    PikaRuntimeValue stop,
    PikaRuntimeValue step,
    PikaRuntimeValue* result);
PikaStatus pika_runtime_write_value_text(
    PikaRuntimeContext* context,
    PikaRuntimeValue value,
    int representation,
    int depth);
PikaStatus pika_runtime_execute_typed(PikaRuntimeContext* context);
PikaStatus pika_runtime_execute_builtin(
    PikaRuntimeContext* context,
    const PikaRuntimeFrame* frame,
    uint16_t builtin_id,
    uint8_t argument_base,
    uint8_t argument_count,
    uint8_t result_slot);

#endif
