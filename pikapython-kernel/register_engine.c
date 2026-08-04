/* SPEC: PJ2026-0501 V2 kernel v0.47; register execution. */
#include "runtime_internal.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#if PIKA_CAPABILITY_OP_INTEGER_ENABLE

static PikaStatus apply_binary(PikaOpcode opcode,
                               int64_t left,
                               int64_t right,
                               int64_t* value) {
    if (opcode == PIKA_OP_MULTIPLY) {
        if (left >= INT32_MIN && left <= INT32_MAX &&
            right >= INT32_MIN && right <= INT32_MAX) {
            *value = left * right;
            return PIKA_STATUS_OK;
        }
        return pika_runtime_integer_multiply_checked(
            left, right, value);
    }
    *value = left == right;
    return PIKA_STATUS_OK;
}

#endif

#if PIKA_CAPABILITY_CALL_POSITIONAL_ENABLE

static int register_value_range_fits(
    const PikaRuntimeContext* context,
    uint32_t base,
    uint32_t count) {
    uint32_t end;
    if (base > UINT32_MAX - count) return 0;
    end = base + count;
    if (context->values == NULL ||
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

static inline PikaStatus register_push_call(
    PikaRuntimeContext* context,
    uint16_t return_slot,
    uint16_t argument_base,
    uint16_t argument_count,
    uint16_t function_index) {
    PikaRuntimeFrame* caller;
    PikaRuntimeFrame* callee;
    const PikaFunction* function;
    uint32_t next_depth;
    uint32_t value_base;
    uint32_t value_peak;
    uint32_t index;
    if (context == NULL || context->program == NULL ||
        context->frames == NULL ||
        function_index >= context->program->function_count) {
        return PIKA_STATUS_INVALID_FUNCTION;
    }
    next_depth = context->depth + 1u;
    if (next_depth >= context->program->max_call_depth) {
        return PIKA_STATUS_CALL_DEPTH_EXCEEDED;
    }
    caller = &context->frames[context->depth];
    function = &context->program->functions[function_index];
    if ((uint32_t)argument_base + argument_count >
            caller->slot_count ||
        argument_count != function->parameter_count ||
        return_slot >= caller->slot_count) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
#if PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE
    if (function->reserved != 0u) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
#endif
    if (caller->value_base >
        UINT32_MAX - caller->slot_count) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    value_base = caller->value_base + caller->slot_count;
    if (!register_value_range_fits(
            context, value_base, function->slot_count)) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    callee = &context->frames[next_depth];
    callee->function_index = function_index;
    callee->instruction_index = function->first_instruction;
    callee->value_base = value_base;
    callee->slot_count = function->slot_count;
    callee->return_slot = return_slot;
    memset(&context->values[value_base], 0,
           function->slot_count * sizeof(int64_t));
    for (index = 0u; index < argument_count; ++index) {
        context->values[value_base + index] =
            context->values[
                caller->value_base + argument_base + index];
    }
    if (context->binding_storage != NULL &&
        context->binding_storage->values != NULL) {
        memset(&context->binding_storage->values[value_base], 0,
               function->slot_count * sizeof(PikaBindingValue));
        for (index = 0u; index < argument_count; ++index) {
            context->binding_storage->values[value_base + index] =
                context->binding_storage->values[
                    caller->value_base + argument_base + index];
        }
    }
    context->depth = next_depth;
    context->metrics->call_count++;
    if (next_depth + 1u > context->metrics->frame_peak) {
        context->metrics->frame_peak = next_depth + 1u;
    }
    value_peak = value_base + function->slot_count;
    if (value_peak > context->metrics->value_peak) {
        context->metrics->value_peak = value_peak;
    }
    pika_runtime_update_storage_peak(context);
    return PIKA_STATUS_OK;
}

static inline PikaStatus register_return(
    PikaRuntimeContext* context,
    uint16_t slot,
    int* finished) {
    PikaRuntimeFrame* frame =
        &context->frames[context->depth];
    PikaRuntimeFrame* caller;
    uint16_t return_slot;
    uint32_t value_index;
    uint32_t return_index;
    int64_t value;
    if (slot >= frame->slot_count || finished == NULL) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    value_index = frame->value_base + slot;
    if (context->depth == 0u) {
        value = context->values[value_index];
        context->result->value = value;
        context->result->typed_value.kind =
            PIKA_RUNTIME_VALUE_INTEGER;
        context->result->typed_value.reserved = 0u;
        context->result->typed_value.flags = 0u;
        context->result->typed_value.as.integer = value;
        context->result->status = PIKA_STATUS_OK;
        *finished = 1;
        return PIKA_STATUS_OK;
    }
    return_slot = frame->return_slot;
    caller = &context->frames[context->depth - 1u];
    if (return_slot != UINT16_MAX &&
        return_slot >= caller->slot_count) {
        return PIKA_STATUS_INTERNAL_ERROR;
    }
    --context->depth;
    if (return_slot == UINT16_MAX) {
        *finished = 0;
        return PIKA_STATUS_OK;
    }
    return_index = caller->value_base + return_slot;
    context->values[return_index] =
        context->values[value_index];
    if (context->binding_storage != NULL &&
        context->binding_storage->values != NULL) {
        context->binding_storage->values[return_index] =
            context->binding_storage->values[value_index];
    }
    *finished = 0;
    return PIKA_STATUS_OK;
}

#endif

PikaStatus pika_runtime_execute_register(PikaRuntimeContext* context) {
    uint8_t stack_marker = 0u;
    uint64_t instruction_count;
    pika_runtime_record_stack(context, &stack_marker);

    if (context->typed_values != NULL) {
#if PIKA_TYPED_RUNTIME_ENABLE
        return pika_runtime_execute_typed(context);
#else
        return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
#endif
    }
    if (context->program->instruction_prefix_count != 0u) {
        return pika_runtime_execute_register_segmented(context);
    }
    instruction_count = context->metrics->instruction_count;

    for (;;) {
        PikaRuntimeFrame* frame = &context->frames[context->depth];
        const PikaFunction* function =
            &context->program->functions[frame->function_index];
        uint32_t end =
            function->first_instruction + function->instruction_count;
        int64_t* slots = &context->values[frame->value_base];

        for (;;) {
            uint32_t current_index;
            const PikaInstruction* instruction;
            PikaStatus status = PIKA_STATUS_OK;
#if PIKA_CAPABILITY_OP_INTEGER_ENABLE
            int64_t binary_value = 0;
#endif

            if (frame->instruction_index <
                    function->first_instruction ||
                frame->instruction_index >= end) {
                context->metrics->instruction_count =
                    instruction_count;
                pika_runtime_set_error(
                    context, PIKA_STATUS_INVALID_CONTROL_FLOW,
                    frame->instruction_index);
                return PIKA_STATUS_INVALID_CONTROL_FLOW;
            }

            current_index = frame->instruction_index;
            instruction =
                &context->program->instructions[current_index];
            ++instruction_count;

            switch ((PikaOpcode)instruction->opcode) {
                case PIKA_OP_CONSTANT:
                    slots[instruction->a] =
                        instruction->immediate;
                    frame->instruction_index++;
                    break;
                case PIKA_OP_MOVE:
                    slots[instruction->a] =
                        slots[instruction->b];
                    frame->instruction_index++;
                    break;
#if PIKA_CAPABILITY_OP_INTEGER_ENABLE
                case PIKA_OP_ADD:
                    binary_value =
                        slots[instruction->b] + slots[instruction->c];
                    goto store_binary_value;
                case PIKA_OP_SUBTRACT:
                    binary_value =
                        slots[instruction->b] - slots[instruction->c];
                    goto store_binary_value;
                case PIKA_OP_LESS_THAN:
                    binary_value =
                        slots[instruction->b] < slots[instruction->c];
                    goto store_binary_value;
                case PIKA_OP_MULTIPLY:
                case PIKA_OP_EQUAL:
                    status = apply_binary(
                        (PikaOpcode)instruction->opcode,
                        slots[instruction->b],
                        slots[instruction->c],
                        &slots[instruction->a]);
                    if (status == PIKA_STATUS_OK) {
                        frame->instruction_index++;
                    }
                    break;
                case PIKA_OP_MODULO: {
                    int64_t left = slots[instruction->b];
                    int64_t right = slots[instruction->c];
                    int64_t remainder;
                    int64_t needs_adjustment;
                    if (right == 0) {
                        status = PIKA_STATUS_DIVISION_BY_ZERO;
                        break;
                    }
                    remainder = left % right;
                    needs_adjustment = (int64_t)(
                        ((left < 0) != (right < 0)) &
                        (remainder != 0));
                    slots[instruction->a] =
                        remainder + needs_adjustment * right;
                    frame->instruction_index++;
                    break;
                }
            store_binary_value:
                slots[instruction->a] = binary_value;
                frame->instruction_index++;
                break;
#endif
#if PIKA_CAPABILITY_FLOW_BRANCH_ENABLE
                case PIKA_OP_JUMP:
                    frame->instruction_index =
                        (uint32_t)instruction->immediate;
                    break;
                case PIKA_OP_BRANCH_FALSE:
                    frame->instruction_index =
                        slots[instruction->a]
                            ? frame->instruction_index + 1u
                            : (uint32_t)instruction->immediate;
                    break;
#endif
#if PIKA_CAPABILITY_CALL_POSITIONAL_ENABLE
                case PIKA_OP_CALL:
                    frame->instruction_index++;
                    status = register_push_call(
                        context, instruction->a,
                        instruction->b, instruction->c,
                        (uint16_t)instruction->immediate);
                    if (status == PIKA_STATUS_OK) {
                        goto refresh_frame;
                    }
                    break;
#endif
                case PIKA_OP_RETURN: {
                    int finished = 0;
                    status = register_return(
                        context, instruction->a, &finished);
                    if (status == PIKA_STATUS_OK && finished) {
                        context->metrics->instruction_count =
                            instruction_count;
                        return PIKA_STATUS_OK;
                    }
                    if (status == PIKA_STATUS_OK) {
                        goto refresh_frame;
                    }
                    break;
                }
#if PIKA_CAPABILITY_BUILTIN_PRINT_ENABLE
                case PIKA_OP_PRINT:
                    status = pika_runtime_write_register_value(
                        context, instruction,
                        slots[instruction->a]);
                    if (status == PIKA_STATUS_OK) {
                        frame->instruction_index++;
                    }
                    break;
#endif
                default:
#if PIKA_CAPABILITY_FLOW_BRANCH_ENABLE
                    /* Keep sparse optional opcodes out of the core jump table. */
                    if (instruction->opcode == PIKA_OP_SELECT) {
                        slots[instruction->a] =
                            slots[instruction->b]
                                ? slots[instruction->c]
                                : slots[(uint8_t)instruction->immediate];
                        frame->instruction_index++;
                        break;
                    }
#endif
#if PIKA_BINDING_RUNTIME_ENABLE
                    status =
                        pika_runtime_execute_binding_opcode(
                            context, instruction);
#else
                    status = PIKA_STATUS_UNSUPPORTED_CAPABILITY;
#endif
                    break;
            }
            if (status != PIKA_STATUS_OK) {
                context->metrics->instruction_count =
                    instruction_count;
                pika_runtime_set_instruction_error(
                    context, status, current_index, instruction);
                return status;
            }
        }
    refresh_frame:
        ;
    }
}
