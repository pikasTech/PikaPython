/* SPEC: PJ2026-050109 v0.3、PJ2026-0501 v0.47；字节码校验。 */
#include "pika_program.h"
#include "pika_builtin.h"
#include "pika_text_method.h"
#include "verifier_internal.h"

#include <limits.h>
#include <stddef.h>

static int slot_is_valid(const PikaFunction* function, uint8_t slot) {
    return (uint16_t)slot < function->slot_count;
}

static int target_is_valid(const PikaFunction* function, int32_t target) {
    uint32_t first = function->first_instruction;
    uint32_t end = first + function->instruction_count;
    return target >= 0 && (uint32_t)target >= first &&
           (uint32_t)target < end;
}

static int python_modules_are_valid(const PikaProgram* program) {
    uint16_t index;
    if (program->binding_module_count > 0u &&
        program->binding_modules == NULL) {
        return 0;
    }
    if (program->python_module_count == 0u) {
        return program->python_modules == NULL;
    }
    if (program->python_modules == NULL ||
        program->module_names == NULL ||
        program->module_name_bytes == 0u) {
        return 0;
    }
    for (index = 0u; index < program->python_module_count; ++index) {
        const PikaPythonModule* module =
            &program->python_modules[index];
        if (module->name_length == 0u ||
            module->name_offset > program->module_name_bytes ||
            module->name_length >
                program->module_name_bytes - module->name_offset ||
            (module->flags &
             (uint8_t)~PIKA_PYTHON_MODULE_PUBLIC) != 0u) {
            return 0;
        }
    }
    return 1;
}

#if PIKA_BINDING_RUNTIME_ENABLE

static int binding_result_requires_typed_values(uint8_t kind) {
    return kind == PIKA_BINDING_VALUE_FLOAT ||
           kind == PIKA_BINDING_VALUE_STRING ||
           kind == PIKA_BINDING_VALUE_BYTES ||
           kind == PIKA_BINDING_VALUE_BYTEARRAY ||
           kind == PIKA_BINDING_VALUE_LIST ||
           kind == PIKA_BINDING_VALUE_TUPLE ||
           kind == PIKA_BINDING_VALUE_DICT;
}

static int binding_call_result_is_valid(
    const PikaBindingCallSite* site) {
    if (site->kind != (uint8_t)site->callable->kind) {
        return 0;
    }
    if (site->kind == PIKA_BINDING_CALL_CONSTRUCTOR) {
        return site->result_kind ==
                   PIKA_BINDING_VALUE_OPAQUE &&
               site->class_descriptor != NULL;
    }
    if (site->result_kind != site->callable->result_kind) {
        return 0;
    }
#if PIKA_CAPABILITY_BINDING_OBJECT_RESULT_ENABLE
    if (site->result_kind == PIKA_BINDING_VALUE_OBJECT) {
        return site->module->classes != NULL &&
               site->callable->result_class_index <
                   site->module->class_count &&
               site->class_descriptor ==
                   &site->module->classes[
                       site->callable->result_class_index];
    }
#endif
    return site->result_kind <= PIKA_BINDING_VALUE_DICT;
}

static int binding_call_flags_are_valid(
    const PikaBindingCallSite* site) {
    uint8_t expected = site->flags &
        PIKA_BINDING_CALL_SITE_PREVALIDATED_ARGUMENTS;
    uint32_t index;
    if ((site->flags &
         (uint8_t)~(
             PIKA_BINDING_CALL_SITE_CONTAINER_ARGUMENT |
             PIKA_BINDING_CALL_SITE_PREVALIDATED_ARGUMENTS)) != 0u) {
        return 0;
    }
    if ((site->flags &
         PIKA_BINDING_CALL_SITE_PREVALIDATED_ARGUMENTS) != 0u &&
        (site->argument_count != site->callable->parameter_count ||
         (site->argument_count > 0u &&
          site->callable->parameters == NULL))) {
        return 0;
    }
    for (index = 0u; index < site->argument_count; ++index) {
        uint8_t kind = site->argument_kinds[index];
        if ((site->flags &
             PIKA_BINDING_CALL_SITE_PREVALIDATED_ARGUMENTS) != 0u) {
            PikaBindingValueKind parameter =
                site->callable->parameters[index];
            if (kind > PIKA_BINDING_VALUE_CALLABLE ||
                kind == PIKA_BINDING_VALUE_OBJECT ||
                (parameter != PIKA_BINDING_VALUE_ANY &&
                 kind != (uint8_t)parameter)) {
                return 0;
            }
        }
        if (kind == PIKA_BINDING_VALUE_ANY ||
            kind == PIKA_BINDING_VALUE_LIST ||
            kind == PIKA_BINDING_VALUE_TUPLE ||
            kind == PIKA_BINDING_VALUE_DICT) {
            expected |= PIKA_BINDING_CALL_SITE_CONTAINER_ARGUMENT;
            break;
        }
    }
    return site->flags == expected;
}

#endif

static int opcode_capability_state(const PikaProgram* program,
                                   uint8_t opcode) {
    switch ((PikaOpcode)opcode) {
        case PIKA_OP_CONSTANT:
        case PIKA_OP_MOVE:
        case PIKA_OP_RETURN:
            return PIKA_CAPABILITY_EXEC_MODULE_ENABLE;
        case PIKA_OP_ADD:
        case PIKA_OP_SUBTRACT:
        case PIKA_OP_MULTIPLY:
        case PIKA_OP_MODULO:
            return program->uses_typed_values != 0u
                       ? (PIKA_CAPABILITY_OP_NUMERIC_BASIC_ENABLE ||
                          PIKA_CAPABILITY_OP_INTEGER_ENABLE)
                       : PIKA_CAPABILITY_OP_INTEGER_ENABLE;
        case PIKA_OP_LESS_THAN:
        case PIKA_OP_EQUAL:
            return program->uses_typed_values != 0u
                       ? (PIKA_CAPABILITY_OP_COMPARE_BASIC_ENABLE ||
                          PIKA_CAPABILITY_OP_INTEGER_ENABLE)
                       : PIKA_CAPABILITY_OP_INTEGER_ENABLE;
        case PIKA_OP_JUMP:
        case PIKA_OP_BRANCH_FALSE:
        case PIKA_OP_SELECT:
            return PIKA_CAPABILITY_FLOW_BRANCH_ENABLE;
        case PIKA_OP_CALL:
        case PIKA_OP_CALL_DYNAMIC_METHOD:
            return PIKA_CAPABILITY_CALL_POSITIONAL_ENABLE;
        case PIKA_OP_PRINT:
            return PIKA_CAPABILITY_BUILTIN_PRINT_ENABLE;
        case PIKA_OP_BIND_MOVE:
        case PIKA_OP_BIND_CALL:
            return PIKA_CAPABILITY_BINDING_C_ENABLE;
        case PIKA_OP_DIVIDE:
        case PIKA_OP_FLOOR_DIVIDE:
        case PIKA_OP_POWER:
        case PIKA_OP_UNARY_PLUS:
        case PIKA_OP_UNARY_MINUS:
            return PIKA_CAPABILITY_OP_NUMERIC_BASIC_ENABLE;
        case PIKA_OP_LESS_EQUAL:
        case PIKA_OP_GREATER_THAN:
        case PIKA_OP_GREATER_EQUAL:
        case PIKA_OP_NOT_EQUAL:
        case PIKA_OP_IDENTITY:
            return PIKA_CAPABILITY_OP_COMPARE_BASIC_ENABLE;
        case PIKA_OP_BITWISE_AND:
        case PIKA_OP_BITWISE_XOR:
        case PIKA_OP_BITWISE_OR:
        case PIKA_OP_SHIFT_LEFT:
        case PIKA_OP_SHIFT_RIGHT:
        case PIKA_OP_BITWISE_NOT:
            return PIKA_CAPABILITY_OP_BITWISE_ENABLE;
        case PIKA_OP_BRANCH_TRUE:
        case PIKA_OP_LOGICAL_NOT:
            return PIKA_CAPABILITY_LOGIC_SHORT_CIRCUIT_ENABLE;
        case PIKA_OP_BUILD_LIST:
            return PIKA_CAPABILITY_CONTAINER_LIST_ENABLE;
        case PIKA_OP_BUILD_TUPLE:
            return PIKA_CAPABILITY_CONTAINER_TUPLE_ENABLE;
        case PIKA_OP_BUILD_DICT:
            return PIKA_CAPABILITY_CONTAINER_DICT_ENABLE;
        case PIKA_OP_BUILTIN_CALL:
            return PIKA_BUILTIN_RUNTIME_ENABLE;
        case PIKA_OP_SUBSCRIPT:
        case PIKA_OP_SET_SUBSCRIPT:
            return PIKA_CAPABILITY_OP_SUBSCRIPT_ENABLE;
        case PIKA_OP_SLICE:
            return PIKA_CAPABILITY_OP_SLICE_ENABLE;
        case PIKA_OP_CONTAINS:
            return PIKA_CAPABILITY_OP_MEMBERSHIP_ENABLE;
        case PIKA_OP_LOAD_CONSTANT:
            return PIKA_CAPABILITY_EXEC_MODULE_ENABLE;
        case PIKA_OP_MATRIX_MULTIPLY:
            return PIKA_CAPABILITY_OP_MATRIX_MULTIPLY_ENABLE;
        case PIKA_OP_APPEND:
        case PIKA_OP_POP:
            return PIKA_TYPED_MUTABLE_CONTAINER_RUNTIME_ENABLE;
        case PIKA_OP_ITERATE:
            return PIKA_TYPED_ITERATION_RUNTIME_ENABLE;
        case PIKA_OP_NEW_INSTANCE:
            return PIKA_CAPABILITY_OBJECT_USER_BASIC_ENABLE ||
                   PIKA_CAPABILITY_OBJECT_CLASS_ENABLE;
        case PIKA_OP_LOAD_ATTRIBUTE:
        case PIKA_OP_STORE_ATTRIBUTE:
            return PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE;
        case PIKA_OP_RAISE:
            return PIKA_CAPABILITY_EXCEPTION_BASIC_ENABLE;
        case PIKA_OP_LOAD_CALLABLE:
            return PIKA_CAPABILITY_CALL_REFERENCE_ENABLE;
        case PIKA_OP_LOAD_GLOBAL:
        case PIKA_OP_STORE_GLOBAL:
            return PIKA_CAPABILITY_NAME_GLOBAL_ENABLE;
        case PIKA_OP_UNPACK_SEQUENCE:
            return PIKA_CAPABILITY_ASSIGNMENT_UNPACK_ENABLE;
        default:
            return -1;
    }
}

static int constant_capability_state(uint8_t kind) {
    switch ((PikaConstantKind)kind) {
        case PIKA_CONSTANT_INTEGER:
            return PIKA_CAPABILITY_VALUE_INT_ENABLE;
        case PIKA_CONSTANT_FLOAT:
            return PIKA_CAPABILITY_VALUE_FLOAT_ENABLE;
        case PIKA_CONSTANT_STRING:
            return PIKA_CAPABILITY_VALUE_STRING_ENABLE;
        case PIKA_CONSTANT_BYTES:
            return PIKA_CAPABILITY_VALUE_BYTES_ENABLE;
        case PIKA_CONSTANT_BOOLEAN:
            return PIKA_CAPABILITY_VALUE_BOOL_ENABLE;
        case PIKA_CONSTANT_NONE:
            return PIKA_CAPABILITY_VALUE_NONE_ENABLE;
        case PIKA_CONSTANT_TYPE:
            return PIKA_CAPABILITY_BUILTIN_TYPE_ENABLE;
        case PIKA_CONSTANT_MODULE:
            return PIKA_CAPABILITY_MODULE_IMPORT_ENABLE;
        default:
            return -1;
    }
}

static PikaStatus verify_instruction(const PikaProgram* program,
                                     const PikaFunction* function,
                                     const PikaInstruction* instruction) {
    int capability_state =
        opcode_capability_state(program, instruction->opcode);
    if (capability_state < 0) {
        return PIKA_STATUS_UNKNOWN_OPCODE;
    }
    if (capability_state == 0) {
        return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
    }
    switch ((PikaOpcode)instruction->opcode) {
        case PIKA_OP_CONSTANT:
            return slot_is_valid(function, instruction->a)
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_OPERAND;
        case PIKA_OP_LOAD_CONSTANT:
            if (!slot_is_valid(function, instruction->a) ||
                instruction->immediate < 0 ||
                (uint32_t)instruction->immediate >= program->constant_count) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            return PIKA_STATUS_OK;
        case PIKA_OP_LOAD_CALLABLE:
            return program->uses_typed_values != 0u &&
                           slot_is_valid(
                               function, instruction->a) &&
                           instruction->immediate >= 0 &&
                           (uint32_t)instruction->immediate <
                               program->function_count
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_OPERAND;
        case PIKA_OP_LOAD_GLOBAL:
        case PIKA_OP_STORE_GLOBAL:
            return program->uses_typed_values != 0u &&
                           slot_is_valid(
                               function, instruction->a) &&
                           instruction->immediate >= 0 &&
                           (uint32_t)instruction->immediate <
                               program->functions[
                                   program->entry_function]
                                   .slot_count
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_OPERAND;
        case PIKA_OP_MOVE:
            return slot_is_valid(function, instruction->a) &&
                           slot_is_valid(function, instruction->b)
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_OPERAND;
        case PIKA_OP_UNPACK_SEQUENCE:
            return program->uses_typed_values != 0u &&
                           slot_is_valid(function, instruction->a) &&
                           slot_is_valid(function, instruction->b) &&
                           instruction->c != 0u &&
                           instruction->immediate >= 0 &&
                           (uint32_t)instruction->immediate <
                               instruction->c
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_OPERAND;
        case PIKA_OP_ADD:
        case PIKA_OP_SUBTRACT:
        case PIKA_OP_MULTIPLY:
        case PIKA_OP_MODULO:
        case PIKA_OP_LESS_THAN:
        case PIKA_OP_EQUAL:
        case PIKA_OP_DIVIDE:
        case PIKA_OP_FLOOR_DIVIDE:
        case PIKA_OP_POWER:
        case PIKA_OP_LESS_EQUAL:
        case PIKA_OP_GREATER_THAN:
        case PIKA_OP_GREATER_EQUAL:
        case PIKA_OP_NOT_EQUAL:
        case PIKA_OP_BITWISE_AND:
        case PIKA_OP_BITWISE_XOR:
        case PIKA_OP_BITWISE_OR:
        case PIKA_OP_SHIFT_LEFT:
        case PIKA_OP_SHIFT_RIGHT:
        case PIKA_OP_MATRIX_MULTIPLY:
        case PIKA_OP_CONTAINS:
        case PIKA_OP_IDENTITY:
            return slot_is_valid(function, instruction->a) &&
                           slot_is_valid(function, instruction->b) &&
                           slot_is_valid(function, instruction->c)
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_OPERAND;
        case PIKA_OP_JUMP:
            return target_is_valid(function, instruction->immediate)
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_CONTROL_FLOW;
        case PIKA_OP_BRANCH_FALSE:
            if (!slot_is_valid(function, instruction->a)) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            return target_is_valid(function, instruction->immediate)
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_CONTROL_FLOW;
        case PIKA_OP_BRANCH_TRUE:
            if (!slot_is_valid(function, instruction->a)) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            return target_is_valid(function, instruction->immediate)
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_CONTROL_FLOW;
        case PIKA_OP_LOGICAL_NOT:
        case PIKA_OP_UNARY_PLUS:
        case PIKA_OP_UNARY_MINUS:
        case PIKA_OP_BITWISE_NOT:
            return slot_is_valid(function, instruction->a) &&
                           slot_is_valid(function, instruction->b)
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_OPERAND;
        case PIKA_OP_BUILD_LIST:
        case PIKA_OP_BUILD_TUPLE:
            return slot_is_valid(function, instruction->a) &&
                           (uint16_t)instruction->b + instruction->c <=
                               function->slot_count
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_OPERAND;
        case PIKA_OP_BUILD_DICT:
            return slot_is_valid(function, instruction->a) &&
                           (uint16_t)instruction->b +
                                   (uint16_t)instruction->c * 2u <=
                               function->slot_count
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_OPERAND;
#if PIKA_BUILTIN_RUNTIME_ENABLE
        case PIKA_OP_BUILTIN_CALL: {
            const PikaBuiltinCallSite* site;
            const PikaBuiltinDescriptor* descriptor;
            if (!slot_is_valid(function, instruction->a) ||
                instruction->immediate < 0 ||
                (uint32_t)instruction->immediate >=
                    program->builtin_call_count ||
                (uint16_t)instruction->b + instruction->c >
                    function->slot_count) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            site = &program->builtin_calls[instruction->immediate];
            if (site->builtin_id >= PIKA_BUILTIN_CATALOG_COUNT) {
#if PIKA_CAPABILITY_PROTOCOL_TEXT_NETWORK_ENABLE
                const PikaTextMethodDescriptor* text_descriptor =
                    pika_text_method_descriptor(site->builtin_id);
                if (text_descriptor == NULL ||
                    site->argument_count != instruction->c ||
                    site->argument_count <
                        text_descriptor->minimum_arguments ||
                    site->argument_count >
                        text_descriptor->maximum_arguments) {
                    return PIKA_STATUS_INVALID_OPERAND;
                }
                return PIKA_STATUS_OK;
#else
                return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
#endif
            }
            descriptor = pika_builtin_descriptor(
                (PikaBuiltinId)site->builtin_id);
            if (descriptor == NULL) {
                return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
            }
            if (descriptor->implemented == 0u ||
                site->argument_count != instruction->c ||
                site->argument_count < descriptor->minimum_arguments ||
                site->argument_count > descriptor->maximum_arguments) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            return PIKA_STATUS_OK;
        }
#endif
        case PIKA_OP_SUBSCRIPT: {
            const PikaIndexSite* site;
            if (!slot_is_valid(function, instruction->a) ||
                instruction->immediate < 0 ||
                (uint32_t)instruction->immediate >=
                    program->index_site_count) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            site = &program->index_sites[instruction->immediate];
            return slot_is_valid(function, site->container_slot) &&
                           slot_is_valid(function, site->index_slot)
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_OPERAND;
        }
        case PIKA_OP_SLICE: {
            const PikaIndexSite* site;
            if (!slot_is_valid(function, instruction->a) ||
                instruction->immediate < 0 ||
                (uint32_t)instruction->immediate >=
                    program->index_site_count) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            site = &program->index_sites[instruction->immediate];
            if (!slot_is_valid(function, site->container_slot)) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            if ((site->flags & 1u) != 0u &&
                !slot_is_valid(function, site->start_slot)) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            if ((site->flags & 2u) != 0u &&
                !slot_is_valid(function, site->stop_slot)) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            if ((site->flags & 4u) != 0u &&
                !slot_is_valid(function, site->step_slot)) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            return PIKA_STATUS_OK;
        }
        case PIKA_OP_SELECT:
            return slot_is_valid(function, instruction->a) &&
                           slot_is_valid(function, instruction->b) &&
                           slot_is_valid(function, instruction->c) &&
                           instruction->immediate >= 0 &&
                           slot_is_valid(function,
                                         (uint8_t)instruction->immediate)
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_OPERAND;
        case PIKA_OP_SET_SUBSCRIPT:
            return slot_is_valid(function, instruction->a) &&
                           slot_is_valid(function, instruction->b) &&
                           slot_is_valid(function, instruction->c)
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_OPERAND;
        case PIKA_OP_APPEND:
            return slot_is_valid(function, instruction->a) &&
                           slot_is_valid(function, instruction->b) &&
                           slot_is_valid(function, instruction->c)
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_OPERAND;
        case PIKA_OP_POP:
            if (!slot_is_valid(function, instruction->a) ||
                !slot_is_valid(function, instruction->b)) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            if (instruction->c != UINT8_MAX &&
                !slot_is_valid(function, instruction->c)) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            if (instruction->immediate >= 0 &&
                !slot_is_valid(function, (uint8_t)instruction->immediate)) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            return PIKA_STATUS_OK;
        case PIKA_OP_ITERATE:
            if (!slot_is_valid(function, instruction->a) ||
                !slot_is_valid(function, instruction->b) ||
                !slot_is_valid(function, instruction->c)) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            return target_is_valid(function, instruction->immediate)
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_CONTROL_FLOW;
#if PIKA_OBJECT_BYTECODE_RUNTIME_ENABLE
        case PIKA_OP_NEW_INSTANCE:
        case PIKA_OP_LOAD_ATTRIBUTE:
        case PIKA_OP_STORE_ATTRIBUTE:
            return pika_verify_object_instruction(
                program, function, instruction);
#endif
        case PIKA_OP_CALL: {
            if (!slot_is_valid(function, instruction->a)) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            if (instruction->immediate < 0 ||
                (uint32_t)instruction->immediate >=
                    program->function_count) {
                return PIKA_STATUS_INVALID_FUNCTION;
            }
            const PikaFunction* callee =
                &program->functions[instruction->immediate];
            if (instruction->c != callee->parameter_count ||
                (uint16_t)instruction->b + instruction->c >
                    function->slot_count) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            return PIKA_STATUS_OK;
        }
        case PIKA_OP_CALL_DYNAMIC_METHOD: {
            const PikaDynamicMethodCallSite* site;
            uint32_t end;
            if (program->uses_typed_values == 0u ||
                !slot_is_valid(function, instruction->a) ||
                instruction->immediate < 0 ||
                (uint32_t)instruction->immediate >=
                    program->dynamic_method_call_count) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            site = &program->dynamic_method_calls[
                instruction->immediate];
            end = (uint32_t)site->first_candidate +
                  site->candidate_count;
            if (site->candidate_count == 0u ||
                site->argument_count != instruction->c ||
                (uint16_t)instruction->b + instruction->c >
                    function->slot_count ||
                end > program->dynamic_method_candidate_count) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            for (uint32_t index = site->first_candidate;
                 index < end; ++index) {
                const PikaDynamicMethodCandidate* candidate =
                    &program->dynamic_method_candidates[index];
                if (candidate->class_index >= program->class_count ||
                    candidate->function_index >=
                        program->function_count ||
                    program->functions[candidate->function_index]
                            .parameter_count != instruction->c ||
                    program->functions[candidate->function_index]
                            .reserved != candidate->class_index + 1u) {
                    return PIKA_STATUS_INVALID_OPERAND;
                }
            }
            return PIKA_STATUS_OK;
        }
        case PIKA_OP_RETURN:
            return slot_is_valid(function, instruction->a)
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_OPERAND;
        case PIKA_OP_PRINT:
            return slot_is_valid(function, instruction->a) &&
                           instruction->b <= PIKA_PRINT_VALUE
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_OPERAND;
#if PIKA_BINDING_RUNTIME_ENABLE
        case PIKA_OP_BIND_MOVE:
            return slot_is_valid(function, instruction->a) &&
                           slot_is_valid(function, instruction->b)
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_OPERAND;
        case PIKA_OP_BIND_CALL: {
            const PikaBindingCallSite* site;
            if (!slot_is_valid(function, instruction->a) ||
                instruction->immediate < 0 ||
                (uint32_t)instruction->immediate >=
                    program->binding_call_count) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            site = &program->binding_calls[instruction->immediate];
            if (site->callable == NULL ||
                site->module == NULL ||
                site->argument_count > PIKA_BINDING_ARGUMENT_LIMIT ||
                site->argument_count != instruction->c ||
                !binding_call_flags_are_valid(site) ||
                !binding_call_result_is_valid(site) ||
                (program->uses_typed_values == 0u &&
                 binding_result_requires_typed_values(
                     site->result_kind)) ||
                (uint16_t)instruction->b + instruction->c >
                    function->slot_count ||
                (site->kind == PIKA_BINDING_CALL_METHOD &&
                 !slot_is_valid(function, site->receiver_slot))) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            return PIKA_STATUS_OK;
        }
#endif
#if PIKA_CAPABILITY_EXCEPTION_BASIC_ENABLE
        case PIKA_OP_RAISE:
            if (!slot_is_valid(function, instruction->a) ||
                instruction->c > 1u) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            if (instruction->immediate == 0) {
                return instruction->b == 0u &&
                               instruction->c == 0u
                           ? PIKA_STATUS_OK
                           : PIKA_STATUS_INVALID_OPERAND;
            }
            if ((instruction->c == 0u &&
                 instruction->b != 0u) ||
                (instruction->c != 0u &&
                 (program->uses_typed_values == 0u ||
                  !slot_is_valid(
                      function, instruction->b)))) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            return instruction->immediate > 0 &&
                           pika_status_can_raise(
                               (PikaStatus)instruction->immediate)
                       ? PIKA_STATUS_OK
                       : PIKA_STATUS_INVALID_OPERAND;
#endif
        default:
            return PIKA_STATUS_UNKNOWN_OPCODE;
    }
}

PikaStatus pika_program_verify(const PikaProgram* program) {
    if (!pika_program_instruction_storage_valid(program) ||
        program->functions == NULL || program->instruction_count == 0u ||
        program->function_count == 0u ||
        program->entry_function >= program->function_count ||
        program->max_call_depth == 0u ||
        (program->binding_call_count > 0u &&
         program->binding_calls == NULL) ||
        (program->builtin_call_count > 0u &&
         program->builtin_calls == NULL) ||
        (program->dynamic_method_call_count > 0u &&
         program->dynamic_method_calls == NULL) ||
        (program->dynamic_method_candidate_count > 0u &&
         program->dynamic_method_candidates == NULL) ||
        (program->class_count > 0u && program->classes == NULL) ||
        (program->constant_count > 0u && program->constants == NULL) ||
        (program->index_site_count > 0u && program->index_sites == NULL) ||
        (program->exception_handler_count > 0u &&
         program->exception_handlers == NULL) ||
        !python_modules_are_valid(program)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (program->format_version != PIKA_PROGRAM_FORMAT_VERSION) {
        return PIKA_STATUS_UNSUPPORTED_FORMAT;
    }
    if ((!PIKA_TYPED_RUNTIME_ENABLE &&
         program->uses_typed_values != 0u) ||
        (!PIKA_BINDING_RUNTIME_ENABLE &&
         (program->binding_call_count > 0u ||
          program->binding_object_count > 0u ||
          program->allows_binding_callback != 0u)) ||
        (!PIKA_BUILTIN_RUNTIME_ENABLE &&
         program->builtin_call_count > 0u) ||
        (!PIKA_OBJECT_RUNTIME_ENABLE &&
         (program->class_count > 0u ||
          program->dynamic_method_call_count > 0u ||
          program->dynamic_method_candidate_count > 0u ||
          program->object_slot_count > 0u ||
          program->object_bytes > 0u)) ||
        (!PIKA_CAPABILITY_EXCEPTION_BASIC_ENABLE &&
         program->exception_handler_count > 0u)) {
        return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
    }
    for (uint32_t class_index = 0u;
         class_index < program->class_count; ++class_index) {
        uint16_t encoded_base =
            program->classes[class_index].reserved;
        if (encoded_base > class_index) {
            return PIKA_STATUS_INVALID_FUNCTION;
        }
    }
    for (uint32_t constant_index = 0u;
         constant_index < program->constant_count; ++constant_index) {
        const PikaConstant* constant = &program->constants[constant_index];
        int capability_state =
            constant_capability_state(constant->kind);
        if (capability_state < 0 ||
            ((constant->kind == PIKA_CONSTANT_STRING ||
              constant->kind == PIKA_CONSTANT_BYTES ||
              constant->kind == PIKA_CONSTANT_TYPE ||
              constant->kind == PIKA_CONSTANT_MODULE) &&
             constant->length > 0u && constant->as.bytes == NULL)) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        if (capability_state == 0) {
            return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
        }
    }
    uint32_t expected_first = 0u;
    for (uint32_t function_index = 0u;
         function_index < program->function_count; ++function_index) {
        const PikaFunction* function = &program->functions[function_index];
        const PikaInstruction* instruction_storage;
        uint32_t instruction_storage_base;
        if (function->first_instruction != expected_first ||
            function->instruction_count == 0u ||
            function->slot_count == 0u ||
            function->slot_count > (uint16_t)UINT8_MAX ||
            function->parameter_count > function->slot_count ||
            function->instruction_count >
                program->instruction_count - expected_first) {
            return PIKA_STATUS_INVALID_FUNCTION;
        }
        instruction_storage =
            pika_program_function_instructions(
                program, function, &instruction_storage_base);
        if (instruction_storage == NULL) {
            return PIKA_STATUS_INVALID_FUNCTION;
        }
        if (function->reserved > program->class_count ||
            (function->reserved != 0u &&
             (function->parameter_count == 0u ||
              program->uses_typed_values == 0u))) {
            return PIKA_STATUS_INVALID_FUNCTION;
        }
        uint32_t end = expected_first + function->instruction_count;
        for (uint32_t index = expected_first; index < end; ++index) {
            PikaStatus status = verify_instruction(
                program, function,
                &instruction_storage[
                    index - instruction_storage_base]);
            if (status != PIKA_STATUS_OK) {
                return status;
            }
        }
        if (instruction_storage[
                end - 1u - instruction_storage_base].opcode !=
            PIKA_OP_RETURN) {
            return PIKA_STATUS_INVALID_CONTROL_FLOW;
        }
        expected_first = end;
    }
    if (expected_first != program->instruction_count) {
        return PIKA_STATUS_INVALID_FUNCTION;
    }
    for (uint16_t index = 0u;
         index < program->exception_handler_count; ++index) {
        const PikaExceptionHandler* handler =
            &program->exception_handlers[index];
        const PikaFunction* function;
        uint32_t function_end;
        uint32_t handler_end;
        if (handler->function_index >= program->function_count) {
            return PIKA_STATUS_INVALID_FUNCTION;
        }
        function = &program->functions[handler->function_index];
        if (!slot_is_valid(function, handler->status_slot) ||
            handler->catch_kind <= PIKA_EXCEPTION_NONE ||
            handler->catch_kind > PIKA_EXCEPTION_OVERFLOW_ERROR) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        function_end = function->first_instruction +
                       function->instruction_count;
        if (handler->instruction_count == 0u ||
            handler->first_instruction <
                function->first_instruction ||
            handler->first_instruction >= function_end ||
            handler->instruction_count >
                function_end - handler->first_instruction ||
            !target_is_valid(
                function, (int32_t)handler->handler_instruction)) {
            return PIKA_STATUS_INVALID_CONTROL_FLOW;
        }
        handler_end = handler->first_instruction +
                      handler->instruction_count;
        if (handler->handler_instruction >=
                handler->first_instruction &&
            handler->handler_instruction < handler_end) {
            return PIKA_STATUS_INVALID_CONTROL_FLOW;
        }
    }
    return PIKA_STATUS_OK;
}
