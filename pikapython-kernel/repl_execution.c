/* SPEC: PJ2026-050106 REPL v0.9; persistent execution state. */
#include "pika_repl.h"

#if !PIKA_REPL_ENABLE
typedef unsigned char PikaReplDisabledTranslationUnit;
#endif

#if PIKA_REPL_ENABLE

#include "runtime_internal.h"

#include <stddef.h>
#include <string.h>

typedef struct {
    unsigned char padding;
    PikaRuntimeFrame frame;
} PikaReplFrameAlignment;

static uintptr_t repl_frame_alignment(void) {
    return (uintptr_t)offsetof(PikaReplFrameAlignment, frame);
}

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
    uintptr_t stack_origin) {
    const PikaFunction* entry;
    PikaRuntimeFrame* frame;
    uint32_t max_slots;
    uint32_t required_frame_bytes;
    uint32_t required_values;
    uint32_t required_scalar_values;
    uint32_t preserved_slots;
    int uses_typed_values;
    int uses_binding;
    int uses_binding_values;
    int resumes;
    if (context == NULL || program == NULL || arguments == NULL ||
        storage == NULL || state == NULL || result == NULL ||
        metrics == NULL || program->functions == NULL ||
        !pika_program_instruction_storage_valid(program) ||
        program->entry_function >= program->function_count ||
        (arguments->count > 0u && arguments->values == NULL)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    entry = &program->functions[program->entry_function];
    if (arguments->count != entry->parameter_count) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }

    max_slots = pika_runtime_max_slots(program);
    required_frame_bytes =
        program->max_call_depth * (uint32_t)sizeof(PikaRuntimeFrame);
    required_values = program->execution_value_count;
    if (required_values == 0u) {
        PikaStatus analysis_status =
            pika_runtime_analyze_value_count(
                program, &required_values);
        if (analysis_status != PIKA_STATUS_OK) {
            return analysis_status;
        }
    }
    uses_typed_values = program->uses_typed_values != 0u;
    uses_binding = program->binding_call_count > 0u;
    uses_binding_values =
        pika_runtime_requires_binding_values(program);
    resumes = state->initialized != 0u;
    preserved_slots =
        resumes ? state->persistent_slot_count : 0u;
    required_scalar_values = 0u;
    if (!uses_typed_values) {
        required_scalar_values = required_values;
    } else if (resumes && state->uses_typed_values == 0u) {
        required_scalar_values = preserved_slots;
    }
    if (persistent_slot_count > entry->slot_count ||
        preserved_slots > persistent_slot_count ||
        (resumes && state->uses_typed_values != 0u &&
         !uses_typed_values) ||
        (resumes && state->uses_binding_values != 0u &&
         !uses_binding_values)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (storage->frames == NULL ||
        storage->frame_bytes < required_frame_bytes ||
        ((uintptr_t)storage->frames % repl_frame_alignment()) != 0u) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    if (required_scalar_values > 0u &&
        (storage->values == NULL ||
         storage->value_capacity < required_scalar_values)) {
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
    if (resumes && state->binding_object_count > 0u &&
        (binding_storage == NULL ||
         state->binding_object_count >
             binding_storage->object_capacity)) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }

    memset(storage->frames, 0, required_frame_bytes);
    if (!uses_typed_values) {
        memset(
            &storage->values[preserved_slots], 0,
            (required_values - preserved_slots) *
                sizeof(int64_t));
    }
    if (uses_typed_values) {
        if (!resumes || state->uses_typed_values == 0u) {
            uint32_t index;
            memset(storage->typed_values, 0,
                   required_values *
                       sizeof(PikaRuntimeValue));
            for (index = 0u;
                 index < preserved_slots;
                 ++index) {
                storage->typed_values[index].kind =
                    PIKA_RUNTIME_VALUE_INTEGER;
                storage->typed_values[index].as.integer =
                    storage->values[index];
            }
        } else {
            memset(
                &storage->typed_values[preserved_slots], 0,
                (required_values - preserved_slots) *
                    sizeof(PikaRuntimeValue));
        }
    }
    if (uses_typed_values &&
        (!resumes || state->uses_typed_values == 0u) &&
        storage->objects != NULL) {
        pika_object_storage_reset(storage->objects);
    }
    if (uses_binding_values) {
        uint32_t first =
            resumes &&
                    state->uses_binding_values != 0u
                ? preserved_slots
                : 0u;
        memset(
            &binding_storage->values[first], 0,
            (required_values - first) *
                sizeof(PikaBindingValue));
    }
    if (uses_binding &&
        (!resumes || state->binding_object_count == 0u) &&
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
    context->binding_object_count =
        resumes ? state->binding_object_count : 0u;
    context->depth = 0u;
    context->binding_callback_depth = 0u;
    context->binding_callback_active = 0u;
#if PIKA_GC_ENABLE
    context->object_collection_pending =
        resumes ? state->object_collection_pending : 0u;
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

    frame = &context->frames[0];
    frame->function_index = program->entry_function;
    frame->instruction_index = entry->first_instruction;
    frame->value_base = 0u;
    frame->slot_count = entry->slot_count;
    frame->return_slot = 0u;
    if (uses_typed_values) {
        uint32_t index;
        for (index = 0u; index < arguments->count; ++index) {
            PikaRuntimeValue* value = &context->typed_values[index];
            value->kind = PIKA_RUNTIME_VALUE_INTEGER;
            value->as.integer = arguments->values[index];
        }
    } else if (arguments->count > 0u) {
        memcpy(context->values, arguments->values,
               arguments->count * sizeof(int64_t));
    }
#if PIKA_GC_ENABLE
    if (context->object_collection_pending != 0u) {
        pika_runtime_collect_objects(context);
    }
#endif
    metrics->frame_peak = 1u;
    metrics->value_peak = entry->slot_count;
    metrics->binding_object_peak =
        context->binding_object_count;
    metrics->object_peak =
        context->objects != NULL
            ? context->objects->count
            : 0u;
    pika_runtime_update_storage_peak(context);
    return PIKA_STATUS_OK;
}

static PikaStatus execute_with_exception_handlers(
    PikaRuntimeContext* context) {
    PikaStatus status;
    do {
        status = pika_runtime_execute_register(context);
    } while (status != PIKA_STATUS_OK &&
             pika_runtime_handle_exception(
                 context, status,
                 context->result->instruction_index));
    return status;
}

PikaStatus pika_program_execute_stateful(
    const PikaProgram* program,
    const PikaArguments* arguments,
    PikaStorage* storage,
    PikaBindingStorage* binding_storage,
    const PikaOutput* output,
    uint16_t persistent_slot_count,
    PikaExecutionState* state,
    PikaResult* result,
    PikaMetrics* metrics) {
    uint8_t stack_origin = 0u;
    PikaRuntimeContext context;
    PikaStatus status = pika_runtime_initialize_stateful(
        &context, program, arguments, storage,
        binding_storage, output, persistent_slot_count,
        state, result, metrics, (uintptr_t)&stack_origin);
    if (status != PIKA_STATUS_OK) {
        if (result != NULL) result->status = status;
        return status;
    }
    pika_runtime_record_stack(&context, &context);
    if (program->exception_handler_count == 0u) {
        status = pika_runtime_execute_register(&context);
    } else {
        status = execute_with_exception_handlers(&context);
    }
    pika_runtime_finalize_error(&context, status);
#if PIKA_GC_ENABLE
    if (context.typed_values != NULL &&
        context.objects != NULL) {
        context.depth = 0u;
        context.frames[0].slot_count = persistent_slot_count;
        pika_runtime_collect_objects(&context);
    }
#endif
    state->persistent_slot_count = persistent_slot_count;
    state->binding_object_count =
        context.binding_object_count;
    state->initialized = 1u;
    state->uses_typed_values =
        program->uses_typed_values != 0u;
    state->uses_binding_values =
        pika_runtime_requires_binding_values(program) != 0;
#if PIKA_GC_ENABLE
    state->object_collection_pending =
        context.object_collection_pending;
#endif
    result->status = status;
    return status;
}

void pika_execution_state_reset(
    PikaExecutionState* state,
    PikaStorage* storage,
    PikaBindingStorage* binding_storage) {
#if PIKA_BINDING_RUNTIME_ENABLE
    uint16_t index;
#endif
    if (state == NULL) return;
#if PIKA_BINDING_RUNTIME_ENABLE
    if (binding_storage != NULL &&
        binding_storage->objects != NULL) {
        uint16_t count = state->binding_object_count;
        if (count > binding_storage->object_capacity) {
            count = binding_storage->object_capacity;
        }
        for (index = 0u; index < count; ++index) {
            pika_binding_object_release(
                &binding_storage->objects[index]);
        }
    }
#else
    (void)binding_storage;
#endif
    if (storage != NULL && storage->objects != NULL) {
        pika_object_storage_reset(storage->objects);
    }
    *state = (PikaExecutionState){0};
}

#endif
