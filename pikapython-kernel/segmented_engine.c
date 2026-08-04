/* SPEC: PJ2026-0501 kernel; segmented register execution. */
#include "runtime_internal.h"

#include <stdint.h>

#if PIKA_CAPABILITY_OP_INTEGER_ENABLE

static PikaStatus segmented_apply_binary(PikaOpcode opcode,
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

PikaStatus pika_runtime_execute_register_segmented(
    PikaRuntimeContext* context) {
    uint64_t instruction_count =
        context->metrics->instruction_count;

    for (;;) {
        PikaRuntimeFrame* frame = &context->frames[context->depth];
        const PikaFunction* function =
            &context->program->functions[frame->function_index];
        const PikaInstruction* instruction_storage;
        uint32_t instruction_storage_base;
        uint32_t end =
            function->first_instruction + function->instruction_count;
        int64_t* slots = &context->values[frame->value_base];
        instruction_storage =
            pika_program_function_instructions(
                context->program, function,
                &instruction_storage_base);
        if (instruction_storage == NULL) {
            context->metrics->instruction_count =
                instruction_count;
            pika_runtime_set_error(
                context, PIKA_STATUS_INVALID_CONTROL_FLOW,
                frame->instruction_index);
            return PIKA_STATUS_INVALID_CONTROL_FLOW;
        }

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
                &instruction_storage[
                    current_index - instruction_storage_base];
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
                    status = segmented_apply_binary(
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
                    status = pika_runtime_push_call(
                        context, instruction->a,
                        instruction->b, instruction->c,
                        (uint16_t)instruction->immediate);
                    if (status == PIKA_STATUS_OK) {
                        goto refresh_frame;
                    }
                    break;
                case PIKA_OP_CALL_DYNAMIC_METHOD:
                    frame->instruction_index++;
                    status = pika_runtime_push_dynamic_method_call(
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
                    status = pika_runtime_return(
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
