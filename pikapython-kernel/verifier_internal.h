/* SPEC: PJ2026-0501 kernel v0.8; internal verifier boundaries. */
#ifndef PIKA_VERIFIER_INTERNAL_H
#define PIKA_VERIFIER_INTERNAL_H

#include "pika_program.h"

PikaStatus pika_verify_object_instruction(
    const PikaProgram* program,
    const PikaFunction* function,
    const PikaInstruction* instruction);

#endif
