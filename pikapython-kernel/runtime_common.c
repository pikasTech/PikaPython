/* SPEC: PJ2026-0501 V2 kernel v0.47; shared runtime state. */
#include "runtime_internal.h"

#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned char padding;
    PikaRuntimeFrame frame;
} PikaRuntimeFrameAlignment;

static uintptr_t runtime_frame_alignment(void) {
    return (uintptr_t)offsetof(PikaRuntimeFrameAlignment, frame);
}

int pika_runtime_object_storage_accepts(
    const PikaObjectStorage* storage,
    uint16_t slot_capacity,
    uint32_t arena_capacity) {
    uint16_t slot_limit;
    uint32_t arena_limit;
    int physical_slots;
    int physical_arena;
    if (storage == NULL) {
        return 0;
    }
    slot_limit = storage->slot_limit != 0u
                     ? storage->slot_limit
                     : storage->capacity;
    arena_limit = storage->arena_limit != 0u
                      ? storage->arena_limit
                      : storage->arena_capacity;
    if (slot_capacity > slot_limit ||
        (storage->reserve != NULL &&
         arena_capacity > arena_limit) ||
        storage->count > slot_limit ||
        storage->arena_used > arena_limit) {
        return 0;
    }
    physical_slots =
        slot_capacity == 0u ||
        (storage->slots != NULL &&
         storage->capacity >= slot_capacity);
    physical_arena =
        arena_capacity == 0u ||
        storage->arena != NULL;
    return (physical_slots && physical_arena) ||
           storage->reserve != NULL;
}

#if PIKA_RUNTIME_MULTIPLY_HELPER_ENABLE

PikaStatus pika_runtime_integer_multiply_checked(
    int64_t left,
    int64_t right,
    int64_t* result) {
    if (result == NULL) return PIKA_STATUS_INVALID_ARGUMENT;
    if (left == 0 || right == 0) {
        *result = 0;
        return PIKA_STATUS_OK;
    }
    if (left > 0) {
        if (right > 0) {
            if (left > INT64_MAX / right) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
        } else if (right != -1 && left > INT64_MIN / right) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
    } else if (right > 0) {
        if (left < INT64_MIN / right) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
    } else if (right < INT64_MAX / left) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    *result = left * right;
    return PIKA_STATUS_OK;
}

#endif

#if PIKA_RUNTIME_POWER_HELPER_ENABLE

PikaStatus pika_runtime_integer_power(
    int64_t base,
    int64_t exponent,
    int64_t* result) {
    int64_t value = 1;
    int64_t factor = base;
    PikaStatus status;
    if (result == NULL) return PIKA_STATUS_INVALID_ARGUMENT;
    if (exponent < 0) return PIKA_STATUS_INVALID_OPERAND;
    while (exponent != 0) {
        if ((exponent & 1) != 0) {
            status = pika_runtime_integer_multiply_checked(
                value, factor, &value);
            if (status != PIKA_STATUS_OK) return status;
        }
        exponent >>= 1;
        if (exponent != 0) {
            status = pika_runtime_integer_multiply_checked(
                factor, factor, &factor);
            if (status != PIKA_STATUS_OK) return status;
        }
    }
    *result = value;
    return PIKA_STATUS_OK;
}

#endif

uint32_t pika_runtime_max_slots(const PikaProgram* program) {
    uint32_t maximum = 0u;
    for (uint32_t index = 0u; index < program->function_count; ++index) {
        if (program->functions[index].slot_count > maximum) {
            maximum = program->functions[index].slot_count;
        }
    }
    return maximum;
}

static uint32_t conservative_value_count(
    const PikaProgram* program) {
    uint32_t maximum = pika_runtime_max_slots(program);
    uint32_t entry =
        program->functions[program->entry_function].slot_count;
    return entry +
           (program->max_call_depth - 1u) * maximum;
}

static int program_has_dynamic_callable(
    const PikaProgram* program) {
    uint32_t index;
    if (program->allows_binding_callback != 0u) return 1;
    for (index = 0u; index < program->instruction_count; ++index) {
        const PikaInstruction* instruction =
            pika_program_instruction_at(program, index);
        if (instruction == NULL ||
            instruction->opcode == PIKA_OP_LOAD_CALLABLE) {
            return 1;
        }
    }
    return 0;
}

PikaStatus pika_runtime_analyze_value_count(
    const PikaProgram* program,
    uint32_t* value_count) {
    uint32_t* path_values;
    uint32_t* current;
    uint32_t* next;
    uint32_t peak;
    uint16_t depth;
    uint16_t source;
    if (program == NULL || value_count == NULL ||
        program->functions == NULL ||
        !pika_program_instruction_storage_valid(program) ||
        program->entry_function >= program->function_count ||
        program->max_call_depth == 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (program_has_dynamic_callable(program)) {
        *value_count = conservative_value_count(program);
        return PIKA_STATUS_OK;
    }
    path_values = (uint32_t*)calloc(
        (size_t)program->function_count * 2u,
        sizeof(*path_values));
    if (path_values == NULL) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    current = path_values;
    next = &path_values[program->function_count];
    peak = program->functions[
        program->entry_function].slot_count;
    current[program->entry_function] = peak;
    for (depth = 1u; depth < program->max_call_depth; ++depth) {
        int has_next = 0;
        memset(next, 0,
               (size_t)program->function_count * sizeof(*next));
        for (source = 0u; source < program->function_count; ++source) {
            const PikaFunction* function;
            const PikaInstruction* instruction_storage;
            uint32_t instruction_storage_base;
            uint32_t offset;
            if (current[source] == 0u) continue;
            function = &program->functions[source];
            if (function->first_instruction >
                    program->instruction_count ||
                function->instruction_count >
                    program->instruction_count -
                        function->first_instruction) {
                free(path_values);
                return PIKA_STATUS_INVALID_CONTROL_FLOW;
            }
            instruction_storage =
                pika_program_function_instructions(
                    program, function,
                    &instruction_storage_base);
            if (instruction_storage == NULL) {
                free(path_values);
                return PIKA_STATUS_INVALID_CONTROL_FLOW;
            }
            for (offset = 0u;
                 offset < function->instruction_count;
                 ++offset) {
                const PikaInstruction* instruction =
                    &instruction_storage[
                        function->first_instruction + offset -
                        instruction_storage_base];
                uint32_t edge;
                uint32_t edge_count;
                uint32_t first_edge;
                if (instruction->opcode == PIKA_OP_CALL) {
                    if (instruction->immediate < 0 ||
                        (uint32_t)instruction->immediate >=
                            program->function_count) {
                        free(path_values);
                        return PIKA_STATUS_INVALID_FUNCTION;
                    }
                    first_edge = (uint32_t)instruction->immediate;
                    edge_count = 1u;
                } else if (
                    instruction->opcode ==
                    PIKA_OP_CALL_DYNAMIC_METHOD) {
                    const PikaDynamicMethodCallSite* site;
                    if (instruction->immediate < 0 ||
                        (uint32_t)instruction->immediate >=
                            program->dynamic_method_call_count) {
                        free(path_values);
                        return PIKA_STATUS_INVALID_FUNCTION;
                    }
                    site = &program->dynamic_method_calls[
                        instruction->immediate];
                    first_edge = site->first_candidate;
                    edge_count = site->candidate_count;
                } else {
                    continue;
                }
                for (edge = 0u; edge < edge_count; ++edge) {
                    uint16_t target =
                        instruction->opcode == PIKA_OP_CALL
                            ? (uint16_t)first_edge
                            : program->dynamic_method_candidates[
                                  first_edge + edge]
                                  .function_index;
                    uint32_t candidate =
                        current[source] +
                        program->functions[target].slot_count;
                    if (candidate > next[target]) {
                        next[target] = candidate;
                    }
                    if (candidate > peak) peak = candidate;
                    has_next = 1;
                }
            }
        }
        if (!has_next) break;
        {
            uint32_t* swap = current;
            current = next;
            next = swap;
        }
    }
    free(path_values);
    *value_count = peak;
    return PIKA_STATUS_OK;
}

uint16_t pika_runtime_object_slot_capacity(
    const PikaProgram* program) {
    uint16_t capacity = program->object_slot_count;
#if PIKA_GC_ENABLE
    if (capacity > PIKA_GC_OBJECT_SLOT_CAPACITY) {
        capacity = (uint16_t)PIKA_GC_OBJECT_SLOT_CAPACITY;
    }
#endif
    return capacity;
}

uint32_t pika_runtime_object_arena_capacity(
    const PikaProgram* program) {
    uint32_t capacity = program->object_bytes;
#if PIKA_GC_ENABLE
    if (capacity > PIKA_GC_ARENA_BYTES) {
        capacity = PIKA_GC_ARENA_BYTES;
    }
#endif
    return capacity;
}

int pika_runtime_requires_binding_values(
    const PikaProgram* program) {
    return program != NULL &&
           program->binding_call_count > 0u &&
           (program->uses_typed_values == 0u ||
            program->allows_binding_callback != 0u);
}

void pika_runtime_update_storage_peak(PikaRuntimeContext* context) {
    uint32_t frame_bytes =
        context->metrics->frame_peak * (uint32_t)sizeof(PikaRuntimeFrame);
    uint32_t value_bytes =
        context->metrics->value_peak *
        (uint32_t)(context->typed_values != NULL
                       ? sizeof(PikaRuntimeValue)
                       : sizeof(int64_t));
    uint32_t binding_bytes = 0u;
    uint32_t object_bytes = 0u;
    if (context->binding_storage != NULL) {
        if (context->binding_storage->values != NULL) {
            binding_bytes =
                context->metrics->value_peak *
                (uint32_t)sizeof(PikaBindingValue);
        }
        object_bytes =
            context->metrics->binding_object_peak *
            (uint32_t)sizeof(PikaBindingObject);
    }
    if (context->objects != NULL) {
        object_bytes +=
            context->metrics->object_peak *
            (uint32_t)sizeof(PikaObjectSlot);
        object_bytes += context->objects->arena_used;
    }
    context->metrics->execution_storage_peak_bytes =
        frame_bytes + value_bytes + binding_bytes + object_bytes;
}

void pika_runtime_record_stack(PikaRuntimeContext* context,
                               const void* marker) {
    uintptr_t current = (uintptr_t)marker;
    uintptr_t difference = current > context->stack_origin
                               ? current - context->stack_origin
                               : context->stack_origin - current;
    if (difference > context->metrics->host_stack_peak_bytes) {
        context->metrics->host_stack_peak_bytes = (uint32_t)difference;
    }
}

void pika_runtime_set_error(PikaRuntimeContext* context,
                            PikaStatus status,
                            uint32_t instruction_index) {
    context->result->status = status;
    context->result->instruction_index = instruction_index;
#if PIKA_CAPABILITY_EXCEPTION_BASIC_ENABLE
    context->exception_instruction_index = instruction_index;
    context->has_exception_instruction = 1u;
#endif
}

void pika_runtime_set_instruction_error(
    PikaRuntimeContext* context,
    PikaStatus status,
    uint32_t instruction_index,
    const PikaInstruction* instruction) {
    if (instruction != NULL &&
        instruction->opcode == PIKA_OP_RAISE &&
        instruction->immediate == 0 &&
        context->result->status == status) {
        context->result->status = status;
        context->result->instruction_index = instruction_index;
        return;
    }
    pika_runtime_set_error(context, status, instruction_index);
}

void pika_runtime_finalize_error(PikaRuntimeContext* context,
                                 PikaStatus status) {
#if PIKA_CAPABILITY_EXCEPTION_BASIC_ENABLE
    if (status != PIKA_STATUS_OK &&
        context->has_exception_instruction != 0u &&
        context->result->status == status) {
        context->result->instruction_index =
            context->exception_instruction_index;
    }
#else
    (void)context;
    (void)status;
#endif
}

#if PIKA_CAPABILITY_EXCEPTION_BASIC_ENABLE

static PikaStatus set_pending_exception(
    PikaRuntimeContext* context,
    PikaStatus status,
    uint32_t message_index) {
    if (!pika_status_is_known(status)) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    memset(&context->pending_exception, 0,
           sizeof(context->pending_exception));
    context->pending_exception.kind =
        PIKA_RUNTIME_VALUE_EXCEPTION;
    context->pending_exception.reserved =
        (uint8_t)status;
    context->pending_exception.as.object_index =
        message_index;
    context->has_pending_exception = 1u;
    return PIKA_STATUS_OK;
}

PikaStatus pika_runtime_raise_status(
    PikaRuntimeContext* context,
    const PikaRuntimeFrame* frame,
    const PikaInstruction* instruction,
    PikaStatus* status) {
    int64_t raw;
    if (context == NULL || frame == NULL || instruction == NULL ||
        status == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (instruction->immediate != 0) {
        PikaStatus explicit_status =
            (PikaStatus)instruction->immediate;
        uint32_t message_index =
            PIKA_RUNTIME_EXCEPTION_NO_MESSAGE;
        if (!pika_status_can_raise(explicit_status)) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        if (instruction->c > 1u) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        if (instruction->c != 0u) {
            PikaRuntimeValue message;
            PikaObjectSlot* object;
            if (context->typed_values == NULL ||
                instruction->b >= frame->slot_count) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            message = context->typed_values[
                frame->value_base + instruction->b];
            object = pika_runtime_object(context, message);
            if (message.kind != PIKA_RUNTIME_VALUE_STRING ||
                object == NULL ||
                object->kind != PIKA_RUNTIME_VALUE_STRING) {
                return PIKA_STATUS_TYPE_MISMATCH;
            }
            message_index = message.as.object_index;
        }
        (void)set_pending_exception(
            context, explicit_status, message_index);
        *status = explicit_status;
        return PIKA_STATUS_OK;
    }
    if (context->typed_values != NULL) {
        PikaRuntimeValue value = context->typed_values[
            frame->value_base + instruction->a];
        if (value.kind == PIKA_RUNTIME_VALUE_EXCEPTION) {
            PikaStatus saved_status =
                (PikaStatus)value.reserved;
            if (saved_status == PIKA_STATUS_OK ||
                !pika_status_is_known(saved_status)) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            context->pending_exception = value;
            context->has_pending_exception = 1u;
            *status = saved_status;
            return PIKA_STATUS_OK;
        }
        if (value.kind != PIKA_RUNTIME_VALUE_INTEGER) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        raw = value.as.integer;
    } else {
        raw = context->values[frame->value_base + instruction->a];
    }
    if (raw == 0) {
        memset(&context->pending_exception, 0,
               sizeof(context->pending_exception));
        context->has_pending_exception = 0u;
        *status = PIKA_STATUS_OK;
        return PIKA_STATUS_OK;
    }
    if (raw < PIKA_STATUS_OK || raw > UINT8_MAX ||
        !pika_status_is_known((PikaStatus)raw)) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    *status = (PikaStatus)raw;
    return set_pending_exception(
        context, *status,
        PIKA_RUNTIME_EXCEPTION_NO_MESSAGE);
}

PikaStatus pika_runtime_execute_fallback_opcode(
    PikaRuntimeContext* context,
    const PikaInstruction* instruction) {
    PikaRuntimeFrame* frame;
    PikaStatus raised = PIKA_STATUS_OK;
    PikaStatus status;
    if (context == NULL || instruction == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (instruction->opcode != PIKA_OP_RAISE) {
        return PIKA_STATUS_UNKNOWN_OPCODE;
    }
    frame = &context->frames[context->depth];
    status = pika_runtime_raise_status(
        context, frame, instruction, &raised);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (raised != PIKA_STATUS_OK) {
        return raised;
    }
    ++frame->instruction_index;
    return PIKA_STATUS_OK;
}

int pika_runtime_handle_exception(PikaRuntimeContext* context,
                                  PikaStatus status,
                                  uint32_t instruction_index) {
    uint32_t search_instruction = instruction_index;
    if (context == NULL || status == PIKA_STATUS_OK) {
        return 0;
    }
    if (context->has_pending_exception == 0u ||
        context->pending_exception.reserved !=
            (uint8_t)status) {
        if (set_pending_exception(
                context, status,
                PIKA_RUNTIME_EXCEPTION_NO_MESSAGE) !=
            PIKA_STATUS_OK) {
            return 0;
        }
    }
    for (;;) {
        const PikaExceptionHandler* selected = NULL;
        PikaRuntimeFrame* frame = &context->frames[context->depth];
        uint16_t index;
        for (index = 0u;
             index < context->program->exception_handler_count;
             ++index) {
            const PikaExceptionHandler* handler =
                &context->program->exception_handlers[index];
            uint32_t handler_end;
            if (handler->function_index != frame->function_index ||
                search_instruction < handler->first_instruction) {
                continue;
            }
            handler_end = handler->first_instruction +
                          handler->instruction_count;
            if (search_instruction >= handler_end ||
                !pika_status_matches_exception(
                    status,
                    (PikaExceptionKind)handler->catch_kind)) {
                continue;
            }
            if (selected == NULL ||
                handler->instruction_count <
                    selected->instruction_count) {
                selected = handler;
            }
        }
        if (selected != NULL) {
            uint32_t slot_index =
                frame->value_base + selected->status_slot;
            if (context->typed_values != NULL) {
                context->typed_values[slot_index] =
                    context->pending_exception;
            } else {
                context->values[slot_index] = status;
            }
            memset(&context->pending_exception, 0,
                   sizeof(context->pending_exception));
            context->has_pending_exception = 0u;
            frame->instruction_index =
                selected->handler_instruction;
#if PIKA_TYPED_RUNTIME_ENABLE && PIKA_GC_ENABLE
            if (context->typed_values != NULL &&
                pika_runtime_object_collection_needed(
                    context)) {
                pika_runtime_collect_objects(context);
            }
#endif
            return 1;
        }
        if (context->depth == 0u) {
            return 0;
        }
        --context->depth;
#if PIKA_TYPED_RUNTIME_ENABLE && PIKA_GC_ENABLE
        if (context->typed_values != NULL) {
            pika_runtime_note_object_pressure(context);
        }
#endif
        frame = &context->frames[context->depth];
        if (frame->instruction_index == 0u) {
            return 0;
        }
        search_instruction = frame->instruction_index - 1u;
    }
}

#else

PikaStatus pika_runtime_execute_fallback_opcode(
    PikaRuntimeContext* context,
    const PikaInstruction* instruction) {
    if (context == NULL || instruction == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_UNKNOWN_OPCODE;
}

int pika_runtime_handle_exception(PikaRuntimeContext* context,
                                  PikaStatus status,
                                  uint32_t instruction_index) {
    (void)context;
    (void)status;
    (void)instruction_index;
    return 0;
}

#endif

PikaStatus pika_runtime_initialize(PikaRuntimeContext* context,
                                   const PikaProgram* program,
                                   const PikaArguments* arguments,
                                   PikaStorage* storage,
                                   PikaBindingStorage* binding_storage,
                                   const PikaOutput* output,
                                   PikaResult* result,
                                   PikaMetrics* metrics,
                                   uintptr_t stack_origin) {
    if (context == NULL || program == NULL || arguments == NULL ||
        storage == NULL || result == NULL || metrics == NULL ||
        program->functions == NULL ||
        !pika_program_instruction_storage_valid(program) ||
        program->entry_function >= program->function_count ||
        (arguments->count > 0u && arguments->values == NULL)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    const PikaFunction* entry =
        &program->functions[program->entry_function];
    if (arguments->count != entry->parameter_count) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }

    uint32_t max_slots = pika_runtime_max_slots(program);
    uint32_t required_frame_bytes =
        program->max_call_depth * (uint32_t)sizeof(PikaRuntimeFrame);
    uint32_t required_values = program->execution_value_count;
    int uses_typed_values = program->uses_typed_values != 0u;
    int uses_binding = program->binding_call_count > 0u;
    int uses_binding_values =
        pika_runtime_requires_binding_values(program);
    if (required_values == 0u) {
        PikaStatus analysis_status =
            pika_runtime_analyze_value_count(
                program, &required_values);
        if (analysis_status != PIKA_STATUS_OK) {
            return analysis_status;
        }
    }
    if (storage->frames == NULL ||
        storage->frame_bytes < required_frame_bytes ||
        ((uintptr_t)storage->frames % runtime_frame_alignment()) != 0u) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    if (!uses_typed_values &&
        (storage->values == NULL ||
         storage->value_capacity < required_values)) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    if (uses_typed_values &&
        (storage->typed_values == NULL ||
         storage->typed_value_capacity < required_values ||
         (pika_runtime_object_slot_capacity(program) > 0u &&
          !pika_runtime_object_storage_accepts(
              storage->objects,
              pika_runtime_object_slot_capacity(program),
              pika_runtime_object_arena_capacity(program))))) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    if (uses_binding_values &&
        (binding_storage == NULL || binding_storage->values == NULL ||
         binding_storage->value_capacity < required_values)) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    if (program->binding_object_count > 0u &&
        (binding_storage == NULL || binding_storage->objects == NULL ||
         binding_storage->object_capacity <
             program->binding_object_count)) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }

    memset(storage->frames, 0, required_frame_bytes);
    if (!uses_typed_values) {
        memset(storage->values, 0, required_values * sizeof(int64_t));
    }
    if (uses_typed_values) {
        memset(storage->typed_values, 0,
               required_values * sizeof(PikaRuntimeValue));
    }
#if PIKA_TYPED_RUNTIME_ENABLE
    if (uses_typed_values && storage->objects != NULL) {
        pika_object_storage_reset(storage->objects);
    }
#endif
    if (uses_binding_values) {
        memset(binding_storage->values, 0,
               required_values * sizeof(PikaBindingValue));
    }
    if (uses_binding &&
        binding_storage != NULL &&
        binding_storage->objects != NULL &&
        program->binding_object_count > 0u) {
        memset(binding_storage->objects, 0,
               program->binding_object_count *
                   sizeof(PikaBindingObject));
    }
    memset(metrics, 0, sizeof(*metrics));
    result->status = PIKA_STATUS_OK;
    result->value = 0;
    memset(&result->typed_value, 0, sizeof(result->typed_value));
    result->typed_value.kind = PIKA_RUNTIME_VALUE_NONE;
    result->instruction_index = entry->first_instruction;

    context->program = program;
    context->frames = (PikaRuntimeFrame*)storage->frames;
    context->values = uses_typed_values ? NULL : storage->values;
    context->typed_values = uses_typed_values
                                ? storage->typed_values
                                : NULL;
    context->objects = uses_typed_values ? storage->objects : NULL;
    context->binding_storage = uses_binding ? binding_storage : NULL;
    context->binding_object_count = 0u;
    context->depth = 0u;
    context->binding_callback_depth = 0u;
    context->binding_callback_active = 0u;
#if PIKA_GC_ENABLE
    context->object_collection_pending = 0u;
    context->gc_root_active = 0u;
    context->gc_retry_block_depth = 0u;
    memset(&context->gc_root, 0, sizeof(context->gc_root));
#endif
#if PIKA_CAPABILITY_EXCEPTION_BASIC_ENABLE
    memset(&context->pending_exception, 0,
           sizeof(context->pending_exception));
    context->has_pending_exception = 0u;
    context->exception_instruction_index = 0u;
    context->has_exception_instruction = 0u;
#endif
    context->max_slots = max_slots;
    context->value_capacity =
        uses_typed_values
            ? storage->typed_value_capacity
            : storage->value_capacity;
    context->output = output;
    context->result = result;
    context->metrics = metrics;
    context->stack_origin = stack_origin;

    PikaRuntimeFrame* frame = &context->frames[0];
    frame->function_index = program->entry_function;
    frame->instruction_index = entry->first_instruction;
    frame->value_base = 0u;
    frame->slot_count = entry->slot_count;
    frame->return_slot = 0u;
    if (uses_typed_values) {
        for (uint32_t index = 0u; index < arguments->count; ++index) {
            PikaRuntimeValue* value = &context->typed_values[index];
            value->kind = PIKA_RUNTIME_VALUE_INTEGER;
            value->as.integer = arguments->values[index];
        }
    } else if (arguments->count > 0u) {
        memcpy(context->values, arguments->values,
               arguments->count * sizeof(int64_t));
    }
    metrics->frame_peak = 1u;
    metrics->value_peak = entry->slot_count;
    pika_runtime_update_storage_peak(context);
    return PIKA_STATUS_OK;
}

static int runtime_value_range_fits(
    const PikaRuntimeContext* context,
    uint32_t base,
    uint32_t count) {
    uint32_t end;
    if (base > UINT32_MAX - count) return 0;
    end = base + count;
    if ((context->values == NULL &&
         context->typed_values == NULL) ||
        end > context->value_capacity) {
        return 0;
    }
    if (context->binding_storage != NULL &&
        context->binding_storage->values != NULL &&
        end > context->binding_storage->value_capacity) {
        return 0;
    }
    return 1;
}

#if PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE
static PikaStatus validate_method_receiver(
    PikaRuntimeContext* context,
    const PikaRuntimeFrame* caller,
    uint16_t argument_base,
    const PikaFunction* function) {
    PikaRuntimeValue receiver;
    PikaObjectSlot* object;
    uint16_t actual_class;
    uint16_t expected_class;
    uint16_t steps = 0u;
    if (function->parameter_count == 0u ||
        context->typed_values == NULL) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    receiver = context->typed_values[
        caller->value_base + argument_base];
    if (receiver.kind != PIKA_RUNTIME_VALUE_INSTANCE) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    object = pika_runtime_object(context, receiver);
    if (object == NULL || object->reserved == 0u) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    actual_class = (uint16_t)(object->reserved - 1u);
    expected_class = (uint16_t)(function->reserved - 1u);
    while (actual_class != expected_class) {
        uint16_t encoded_base;
        if (actual_class >= context->program->class_count ||
            steps++ >= context->program->class_count) {
            return PIKA_STATUS_TYPE_MISMATCH;
        }
        encoded_base =
            context->program->classes[actual_class].reserved;
        if (encoded_base == 0u) {
            return PIKA_STATUS_TYPE_MISMATCH;
        }
        actual_class = (uint16_t)(encoded_base - 1u);
    }
    return PIKA_STATUS_OK;
}
#endif

PikaStatus pika_runtime_push_call(PikaRuntimeContext* context,
                                  uint16_t return_slot,
                                  uint16_t argument_base,
                                  uint16_t argument_count,
                                  uint16_t function_index) {
    if (context == NULL || context->program == NULL ||
        context->frames == NULL ||
        function_index >= context->program->function_count) {
        return PIKA_STATUS_INVALID_FUNCTION;
    }
    if (context->depth + 1u >= context->program->max_call_depth) {
        return PIKA_STATUS_CALL_DEPTH_EXCEEDED;
    }
    PikaRuntimeFrame* caller = &context->frames[context->depth];
    const PikaFunction* function =
        &context->program->functions[function_index];
    if ((uint32_t)argument_base + argument_count > caller->slot_count ||
        argument_count != function->parameter_count ||
        return_slot >= caller->slot_count) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
#if PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE
    if (function->reserved != 0u) {
        PikaStatus status = validate_method_receiver(
            context, caller, argument_base, function);
        if (status != PIKA_STATUS_OK) return status;
    }
#endif

    uint32_t next_depth = context->depth + 1u;
    uint32_t value_base;
    if (caller->value_base >
        UINT32_MAX - caller->slot_count) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    value_base = caller->value_base + caller->slot_count;
    if (!runtime_value_range_fits(
            context, value_base, function->slot_count)) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    PikaRuntimeFrame* callee = &context->frames[next_depth];
    callee->function_index = function_index;
    callee->instruction_index = function->first_instruction;
    callee->value_base = value_base;
    callee->slot_count = function->slot_count;
    callee->return_slot = return_slot;
    if (context->typed_values == NULL) {
        memset(&context->values[callee->value_base], 0,
               function->slot_count * sizeof(int64_t));
        for (uint32_t index = 0u; index < argument_count; ++index) {
            context->values[callee->value_base + index] =
                context->values[
                    caller->value_base + argument_base + index];
        }
    } else {
        memset(&context->typed_values[callee->value_base], 0,
               function->slot_count * sizeof(PikaRuntimeValue));
        for (uint32_t index = 0u; index < argument_count; ++index) {
            context->typed_values[callee->value_base + index] =
                context->typed_values[
                    caller->value_base + argument_base + index];
        }
    }
    if (context->binding_storage != NULL &&
        context->binding_storage->values != NULL) {
        memset(
            &context->binding_storage->values[callee->value_base], 0,
            function->slot_count * sizeof(PikaBindingValue));
        for (uint32_t index = 0u; index < argument_count; ++index) {
            context->binding_storage
                ->values[callee->value_base + index] =
                context->binding_storage
                    ->values[caller->value_base +
                             argument_base + index];
        }
    }
    context->depth = next_depth;
    context->metrics->call_count++;
    if (next_depth + 1u > context->metrics->frame_peak) {
        context->metrics->frame_peak = next_depth + 1u;
    }
    uint32_t value_peak = callee->value_base + function->slot_count;
    if (value_peak > context->metrics->value_peak) {
        context->metrics->value_peak = value_peak;
    }
    pika_runtime_update_storage_peak(context);
    return PIKA_STATUS_OK;
}

PikaStatus pika_runtime_push_dynamic_method_call(
    PikaRuntimeContext* context,
    uint16_t return_slot,
    uint16_t argument_base,
    uint16_t argument_count,
    uint16_t call_site_index) {
#if PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE
    const PikaDynamicMethodCallSite* site;
    const PikaRuntimeFrame* caller;
    PikaRuntimeValue receiver;
    PikaObjectSlot* object;
    uint16_t actual_class;
    uint16_t steps = 0u;
    if (context == NULL || context->program == NULL ||
        context->typed_values == NULL ||
        call_site_index >=
            context->program->dynamic_method_call_count) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    site = &context->program->dynamic_method_calls[call_site_index];
    caller = &context->frames[context->depth];
    if (argument_count != site->argument_count ||
        (uint32_t)argument_base + argument_count >
            caller->slot_count) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    receiver = context->typed_values[
        caller->value_base + argument_base];
    if (receiver.kind != PIKA_RUNTIME_VALUE_INSTANCE) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    object = pika_runtime_object(context, receiver);
    if (object == NULL || object->reserved == 0u) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    actual_class = (uint16_t)(object->reserved - 1u);
    while (actual_class < context->program->class_count &&
           steps++ < context->program->class_count) {
        uint32_t index;
        for (index = 0u; index < site->candidate_count; ++index) {
            const PikaDynamicMethodCandidate* candidate =
                &context->program->dynamic_method_candidates[
                    site->first_candidate + index];
            if (candidate->class_index == actual_class) {
                return pika_runtime_push_call(
                    context, return_slot, argument_base,
                    argument_count, candidate->function_index);
            }
        }
        if (context->program->classes[actual_class].reserved == 0u) {
            break;
        }
        actual_class = (uint16_t)(
            context->program->classes[actual_class].reserved - 1u);
    }
    return PIKA_STATUS_MEMBER_NOT_FOUND;
#else
    (void)context;
    (void)return_slot;
    (void)argument_base;
    (void)argument_count;
    (void)call_site_index;
    return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
#endif
}

PikaStatus pika_runtime_push_callback(
    PikaRuntimeContext* context,
    uint16_t function_index) {
    const PikaFunction* function;
    PikaRuntimeFrame* callee;
    uint32_t next_depth;
    uint32_t value_peak;
    if (context == NULL ||
        function_index >= context->program->function_count) {
        return PIKA_STATUS_INVALID_FUNCTION;
    }
    if (context->depth + 1u >= context->program->max_call_depth) {
        return PIKA_STATUS_CALL_DEPTH_EXCEEDED;
    }
    function = &context->program->functions[function_index];
    next_depth = context->depth + 1u;
    if (context->frames[context->depth].value_base >
        UINT32_MAX -
            context->frames[context->depth].slot_count) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    value_peak =
        context->frames[context->depth].value_base +
        context->frames[context->depth].slot_count;
    if (!runtime_value_range_fits(
            context, value_peak, function->slot_count)) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    callee = &context->frames[next_depth];
    callee->function_index = function_index;
    callee->instruction_index = function->first_instruction;
    callee->value_base = value_peak;
    callee->slot_count = function->slot_count;
    callee->return_slot = UINT16_MAX;
    if (context->typed_values != NULL) {
        memset(&context->typed_values[callee->value_base], 0,
               function->slot_count * sizeof(PikaRuntimeValue));
    } else {
        memset(&context->values[callee->value_base], 0,
               function->slot_count * sizeof(int64_t));
    }
    if (context->binding_storage != NULL &&
        context->binding_storage->values != NULL) {
        memset(
            &context->binding_storage->values[callee->value_base], 0,
            function->slot_count * sizeof(PikaBindingValue));
    }
    context->depth = next_depth;
    context->metrics->call_count++;
    if (next_depth + 1u > context->metrics->frame_peak) {
        context->metrics->frame_peak = next_depth + 1u;
    }
    value_peak = callee->value_base + function->slot_count;
    if (value_peak > context->metrics->value_peak) {
        context->metrics->value_peak = value_peak;
    }
    pika_runtime_update_storage_peak(context);
    return PIKA_STATUS_OK;
}

PikaStatus pika_runtime_return(PikaRuntimeContext* context,
                               uint16_t slot,
                               int* finished) {
    PikaRuntimeFrame* frame = &context->frames[context->depth];
    if (slot >= frame->slot_count || finished == NULL) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    uint32_t value_index = frame->value_base + slot;
    if (context->depth == 0u) {
        if (context->typed_values != NULL) {
            PikaRuntimeValue value = context->typed_values[value_index];
            context->result->typed_value = value;
            context->result->value =
                value.kind == PIKA_RUNTIME_VALUE_INTEGER ||
                        value.kind == PIKA_RUNTIME_VALUE_BOOLEAN
                    ? value.as.integer
                    : 0;
        } else {
            int64_t value = context->values[value_index];
            context->result->value = value;
            context->result->typed_value.kind =
                PIKA_RUNTIME_VALUE_INTEGER;
            context->result->typed_value.reserved = 0u;
            context->result->typed_value.flags = 0u;
            context->result->typed_value.as.integer = value;
        }
        context->result->status = PIKA_STATUS_OK;
        *finished = 1;
        return PIKA_STATUS_OK;
    }
    uint16_t return_slot = frame->return_slot;
    PikaRuntimeFrame* caller =
        &context->frames[context->depth - 1u];
    if (return_slot != UINT16_MAX &&
        return_slot >= caller->slot_count) {
        return PIKA_STATUS_INTERNAL_ERROR;
    }
    --context->depth;
    if (return_slot == UINT16_MAX) {
#if PIKA_TYPED_RUNTIME_ENABLE && PIKA_GC_ENABLE
        if (context->typed_values != NULL) {
            pika_runtime_note_object_pressure(context);
            if (pika_runtime_object_collection_needed(context)) {
                pika_runtime_collect_objects(context);
            }
        }
#endif
        *finished = 0;
        return PIKA_STATUS_OK;
    }
    uint32_t return_index = caller->value_base + return_slot;
    if (context->typed_values != NULL) {
        context->typed_values[return_index] =
            context->typed_values[value_index];
    } else {
        context->values[return_index] = context->values[value_index];
    }
    if (context->binding_storage != NULL &&
        context->binding_storage->values != NULL) {
        context->binding_storage
            ->values[caller->value_base + return_slot] =
            context->binding_storage
                ->values[frame->value_base + slot];
    }
#if PIKA_TYPED_RUNTIME_ENABLE && PIKA_GC_ENABLE
    if (context->typed_values != NULL) {
        pika_runtime_note_object_pressure(context);
        if (pika_runtime_object_collection_needed(context)) {
            pika_runtime_collect_objects(context);
        }
    }
#endif
    *finished = 0;
    return PIKA_STATUS_OK;
}
