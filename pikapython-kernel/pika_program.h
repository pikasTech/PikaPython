/* SPEC: PJ2026-0501 V2 kernel v0.47; program contract. */
#ifndef PIKA_PROGRAM_H
#define PIKA_PROGRAM_H

#include <stddef.h>
#include <stdint.h>

#include "pika_capability.h"
#include "pika_ir.h"
#include "pika_binding.h"
#include "pika_status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t kind;
    uint8_t receiver_slot;
    uint8_t argument_count;
    uint8_t result_kind;
    uint8_t flags;
    const PikaBindingModule* module;
    const PikaBindingClass* class_descriptor;
    const PikaBindingCallable* callable;
    uint8_t argument_kinds[PIKA_BINDING_ARGUMENT_LIMIT];
} PikaBindingCallSite;

#define PIKA_BINDING_CALL_SITE_CONTAINER_ARGUMENT 0x01u
#define PIKA_BINDING_CALL_SITE_PREVALIDATED_ARGUMENTS 0x02u

typedef enum {
    PIKA_CONSTANT_INTEGER = 0,
    PIKA_CONSTANT_FLOAT = 1,
    PIKA_CONSTANT_STRING = 2,
    PIKA_CONSTANT_BYTES = 3,
    PIKA_CONSTANT_BOOLEAN = 4,
    PIKA_CONSTANT_NONE = 5,
    PIKA_CONSTANT_TYPE = 6,
    PIKA_CONSTANT_MODULE = 7,
} PikaConstantKind;

typedef struct {
    uint8_t kind;
    uint8_t reserved;
    uint16_t length;
    union {
        int64_t integer;
        double floating;
        const char* bytes;
    } as;
} PikaConstant;

typedef struct {
    uint16_t builtin_id;
    uint8_t argument_count;
    uint8_t result_kind;
} PikaBuiltinCallSite;

typedef struct {
    uint16_t class_index;
    uint16_t function_index;
} PikaDynamicMethodCandidate;

typedef struct {
    uint16_t first_candidate;
    uint8_t candidate_count;
    uint8_t argument_count;
} PikaDynamicMethodCallSite;

typedef struct {
    uint8_t container_slot;
    uint8_t index_slot;
    uint8_t start_slot;
    uint8_t stop_slot;
    uint8_t step_slot;
    uint8_t flags;
} PikaIndexSite;

typedef struct {
    uint16_t function_index;
    uint8_t status_slot;
    uint8_t catch_kind;
    uint32_t first_instruction;
    uint32_t instruction_count;
    uint32_t handler_instruction;
} PikaExceptionHandler;

typedef struct {
    uint32_t name_offset;
    uint16_t name_length;
    uint8_t target_module;
    uint8_t flags;
} PikaPythonModule;

#define PIKA_PYTHON_MODULE_PUBLIC 0x01u

typedef struct {
    uint32_t format_version;
    const PikaInstruction* instructions;
    const PikaInstruction* instruction_prefix;
    uint32_t instruction_count;
    uint32_t instruction_prefix_count;
    const PikaFunction* functions;
    uint16_t function_count;
    uint16_t entry_function;
    uint16_t max_call_depth;
    const PikaBindingCallSite* binding_calls;
    uint16_t binding_call_count;
    uint16_t binding_object_count;
    const PikaBuiltinCallSite* builtin_calls;
    uint16_t builtin_call_count;
    const PikaDynamicMethodCallSite* dynamic_method_calls;
    uint16_t dynamic_method_call_count;
    const PikaDynamicMethodCandidate* dynamic_method_candidates;
    uint16_t dynamic_method_candidate_count;
    const PikaConstant* constants;
    uint16_t constant_count;
    const PikaIndexSite* index_sites;
    uint16_t index_site_count;
    uint8_t uses_typed_values;
    uint8_t allows_binding_callback;
    uint16_t object_slot_count;
    uint32_t object_bytes;
    uint32_t execution_value_count;
    const PikaClassLayout* classes;
    uint16_t class_count;
    uint16_t reserved_class;
    const PikaExceptionHandler* exception_handlers;
    uint16_t exception_handler_count;
    uint16_t reserved_exception;
    const char* module_names;
    uint32_t module_name_bytes;
    const PikaPythonModule* python_modules;
    uint16_t python_module_count;
    uint16_t reserved_module;
    const PikaBindingModule* const* binding_modules;
    uint16_t binding_module_count;
    uint16_t reserved_binding_module;
} PikaProgram;

typedef struct {
    uint32_t frame_bytes;
    uint32_t value_count;
    uint32_t binding_value_count;
    uint16_t binding_object_count;
    uint32_t typed_value_count;
    uint16_t object_slot_count;
    uint32_t object_bytes;
} PikaProgramRequirements;

static inline int pika_program_instruction_storage_valid(
    const PikaProgram* program) {
    if (program == NULL || program->instruction_count == 0u ||
        program->instruction_prefix_count >
            program->instruction_count) {
        return 0;
    }
    if (program->instruction_prefix_count > 0u &&
        program->instruction_prefix == NULL) {
        return 0;
    }
    return program->instruction_count ==
                   program->instruction_prefix_count ||
               program->instructions != NULL;
}

static inline const PikaInstruction* pika_program_instruction_at(
    const PikaProgram* program,
    uint32_t index) {
    if (!pika_program_instruction_storage_valid(program) ||
        index >= program->instruction_count) {
        return NULL;
    }
    if (program->instruction_prefix_count > 0u &&
        index < program->instruction_prefix_count) {
        return &program->instruction_prefix[index];
    }
    if (program->instructions == NULL) {
        return NULL;
    }
    return &program->instructions[
        index - program->instruction_prefix_count];
}

static inline const PikaInstruction*
pika_program_function_instructions(
    const PikaProgram* program,
    const PikaFunction* function,
    uint32_t* storage_base) {
    uint32_t first;
    if (!pika_program_instruction_storage_valid(program) ||
        function == NULL || storage_base == NULL) {
        return NULL;
    }
    first = function->first_instruction;
    if (first > program->instruction_count ||
        function->instruction_count >
            program->instruction_count - first) {
        return NULL;
    }
    if (program->instruction_prefix_count > 0u &&
        first < program->instruction_prefix_count) {
        if (function->instruction_count >
            program->instruction_prefix_count - first) {
            return NULL;
        }
        *storage_base = 0u;
        return program->instruction_prefix;
    }
    *storage_base = program->instruction_prefix_count;
    return program->instructions;
}

PikaStatus pika_program_verify(const PikaProgram* program);
PikaStatus pika_program_storage_requirements(
    const PikaProgram* program,
    PikaProgramRequirements* requirements);
#ifdef __cplusplus
}
#endif

#endif
