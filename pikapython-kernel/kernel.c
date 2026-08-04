/* SPEC: PJ2026-0501 V2 kernel v0.46; execution entry. */
#include "pika_kernel.h"

#include <limits.h>
#include <stddef.h>

#include "runtime_internal.h"

#if PIKA_CAPABILITY_EXCEPTION_BASIC_ENABLE

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

#endif

PikaStatus pika_program_storage_requirements(
    const PikaProgram* program,
    PikaProgramRequirements* requirements) {
    if (requirements == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    PikaStatus status = pika_program_verify(program);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    uint64_t frame_bytes =
        (uint64_t)program->max_call_depth * sizeof(PikaRuntimeFrame);
    uint32_t value_count;
    status = pika_runtime_analyze_value_count(
        program, &value_count);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    requirements->frame_bytes = (uint32_t)frame_bytes;
    requirements->value_count =
        program->uses_typed_values == 0u
            ? value_count
            : 0u;
    requirements->binding_value_count =
        pika_runtime_requires_binding_values(program)
            ? value_count
            : 0u;
    requirements->binding_object_count =
        program->binding_object_count;
    requirements->typed_value_count =
        program->uses_typed_values != 0u
            ? value_count
            : 0u;
    requirements->object_slot_count =
        program->uses_typed_values != 0u
            ? pika_runtime_object_slot_capacity(program)
            : 0u;
    requirements->object_bytes =
        program->uses_typed_values != 0u
            ? pika_runtime_object_arena_capacity(program)
            : 0u;
    return PIKA_STATUS_OK;
}

PikaStatus pika_program_execute(const PikaProgram* program,
                                const PikaArguments* arguments,
                                PikaStorage* storage,
                                const PikaOutput* output,
                                PikaResult* result,
                                PikaMetrics* metrics) {
    return pika_program_execute_bound(
        program, arguments, storage, NULL, output, result, metrics);
}

PikaStatus pika_program_execute_bound(const PikaProgram* program,
                                      const PikaArguments* arguments,
                                      PikaStorage* storage,
                                      PikaBindingStorage* binding_storage,
                                      const PikaOutput* output,
                                      PikaResult* result,
                                      PikaMetrics* metrics) {
    uint8_t stack_origin = 0u;
    PikaRuntimeContext context;
    PikaStatus status = pika_runtime_initialize(
        &context, program, arguments, storage, binding_storage, output,
        result, metrics, (uintptr_t)&stack_origin);
    if (status != PIKA_STATUS_OK) {
        if (result != NULL) {
            result->status = status;
        }
        return status;
    }
    pika_runtime_record_stack(&context, &context);

    if (program->exception_handler_count == 0u) {
        status = pika_runtime_execute_register(&context);
#if PIKA_CAPABILITY_EXCEPTION_BASIC_ENABLE
    } else {
        status = execute_with_exception_handlers(&context);
#else
    } else {
        status = PIKA_STATUS_UNSUPPORTED_CAPABILITY;
#endif
    }
    pika_runtime_finalize_error(&context, status);
#if PIKA_BINDING_RUNTIME_ENABLE
    if (binding_storage != NULL &&
        binding_storage->objects != NULL) {
        uint16_t index;
        for (index = 0u;
             index < context.binding_object_count;
             ++index) {
            pika_binding_object_release(
                &binding_storage->objects[index]);
        }
    }
#endif
    result->status = status;
    return status;
}
