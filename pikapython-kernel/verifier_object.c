/* SPEC: PJ2026-0501 kernel v0.8; user object bytecode verification. */
#include "verifier_internal.h"

#include <limits.h>
#include <stdint.h>

#if PIKA_OBJECT_BYTECODE_RUNTIME_ENABLE

static int object_slot_is_valid(const PikaFunction* function,
                                uint8_t slot) {
    return (uint16_t)slot < function->slot_count;
}

PikaStatus pika_verify_object_instruction(
    const PikaProgram* program,
    const PikaFunction* function,
    const PikaInstruction* instruction) {
    switch ((PikaOpcode)instruction->opcode) {
        case PIKA_OP_NEW_INSTANCE:
            return object_slot_is_valid(function, instruction->a) &&
                           instruction->immediate >= 0 &&
                           (uint32_t)instruction->immediate <
                               program->class_count
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_OPERAND;
        case PIKA_OP_LOAD_ATTRIBUTE:
        case PIKA_OP_STORE_ATTRIBUTE:
            return object_slot_is_valid(function, instruction->a) &&
                           object_slot_is_valid(function, instruction->b) &&
                           instruction->immediate >= 0 &&
                           instruction->immediate <= UINT16_MAX
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_OPERAND;
        default:
            return PIKA_STATUS_UNKNOWN_OPCODE;
    }
}

#endif
