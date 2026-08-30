/* SPEC: PJ2026-0501 V2 kernel v0.47, PJ2026-050106 REPL v0.9; module lifetime. */
#include "frontend_internal.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int constant_owns_bytes(uint8_t kind) {
    return kind == PIKA_CONSTANT_STRING ||
           kind == PIKA_CONSTANT_BYTES ||
           kind == PIKA_CONSTANT_MODULE;
}

static void release_module(PikaCompiledModule* module) {
    uint32_t index;
    if (module != NULL) {
        if (module->borrows_program_image != 0u) {
            free(module->available_binding_modules);
            free(module->program_binding_imports);
            free(module->program_module_dependencies);
            free(module->program_modules);
            free(module->semantic_imports);
            free(module->semantic_globals);
            free(module->semantic_fields);
            free(module->semantic_shapes);
            free(module->semantic_functions);
            free(module->dynamic_method_calls);
            free(module->dynamic_method_candidates);
            free(module->binding_calls);
            free(module);
            return;
        }
        for (index = 0u; index < module->constant_count; ++index) {
            if (index < module->borrowed_constant_count) {
                continue;
            }
            if (constant_owns_bytes(module->constants[index].kind)) {
                free((void*)module->constants[index].as.bytes);
            }
        }
        free(module->index_sites);
        free(module->available_binding_modules);
        free(module->exception_handlers);
        free(module->semantic_shapes);
        free(module->semantic_parameters);
        free(module->semantic_fields);
        free(module->program_binding_imports);
        free(module->program_module_dependencies);
        free(module->program_modules);
        free(module->semantic_python_imports);
        free(module->semantic_imports);
        free(module->semantic_globals);
        free(module->semantic_classes);
        free(module->semantic_functions);
        free(module->semantic_names);
        free(module->constants);
        free(module->builtin_calls);
        free(module->dynamic_method_calls);
        free(module->dynamic_method_candidates);
        free(module->classes);
        free(module->functions);
        free(module->binding_calls);
        free(module->instructions);
        free(module);
    }
}

PikaStatus pika_frontend_capture_binding_modules(
    PikaCompiledModule* module,
    const PikaBindingRegistry* bindings) {
    const PikaBindingModule** modules;
    if (module == NULL) return PIKA_STATUS_INVALID_ARGUMENT;
    if (bindings == NULL || bindings->count == 0u) {
        module->available_binding_modules = NULL;
        module->available_binding_module_count = 0u;
        return PIKA_STATUS_OK;
    }
    if (bindings->modules == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    modules = (const PikaBindingModule**)malloc(
        (size_t)bindings->count * sizeof(*modules));
    if (modules == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
    memcpy(
        modules, bindings->modules,
        (size_t)bindings->count * sizeof(*modules));
    module->available_binding_modules = modules;
    module->available_binding_module_count = bindings->count;
    return PIKA_STATUS_OK;
}

static void* shrink_array(void* data,
                          uint32_t count,
                          size_t element_size) {
    void* resized;
    if (count == 0u) {
        free(data);
        return NULL;
    }
    resized = realloc(data, (size_t)count * element_size);
    return resized == NULL ? data : resized;
}

static void shrink_module_instructions(
    PikaCompiledModule* module) {
    uint32_t storage_count = 0u;
    if (module->program.instruction_count >=
        module->instruction_storage_base) {
        storage_count =
            module->program.instruction_count -
            module->instruction_storage_base;
    }
    module->instructions = (PikaInstruction*)shrink_array(
        module->instructions, storage_count,
        sizeof(*module->instructions));
    module->instruction_capacity = storage_count;
    module->program.instructions = module->instructions;
}

static void shrink_module_storage(PikaCompiledModule* module,
                                  int include_instructions) {
    if (include_instructions) {
        shrink_module_instructions(module);
    }
    module->functions = (PikaFunction*)shrink_array(
        module->functions, module->program.function_count,
        sizeof(*module->functions));
    module->function_capacity = module->program.function_count;
    module->classes = (PikaClassLayout*)shrink_array(
        module->classes, module->class_count,
        sizeof(*module->classes));
    module->class_capacity = module->class_count;
    module->binding_calls = (PikaBindingCallSite*)shrink_array(
        module->binding_calls, module->binding_call_count,
        sizeof(*module->binding_calls));
    module->binding_call_capacity = module->binding_call_count;
    module->builtin_calls = (PikaBuiltinCallSite*)shrink_array(
        module->builtin_calls, module->builtin_call_count,
        sizeof(*module->builtin_calls));
    module->builtin_call_capacity = module->builtin_call_count;
    module->dynamic_method_calls =
        (PikaDynamicMethodCallSite*)shrink_array(
            module->dynamic_method_calls,
            module->dynamic_method_call_count,
            sizeof(*module->dynamic_method_calls));
    module->dynamic_method_call_capacity =
        module->dynamic_method_call_count;
    module->dynamic_method_candidates =
        (PikaDynamicMethodCandidate*)shrink_array(
            module->dynamic_method_candidates,
            module->dynamic_method_candidate_count,
            sizeof(*module->dynamic_method_candidates));
    module->dynamic_method_candidate_capacity =
        module->dynamic_method_candidate_count;
    module->constants = (PikaConstant*)shrink_array(
        module->constants, module->constant_count,
        sizeof(*module->constants));
    module->constant_capacity = module->constant_count;
    module->index_sites = (PikaIndexSite*)shrink_array(
        module->index_sites, module->index_site_count,
        sizeof(*module->index_sites));
    module->index_site_capacity = module->index_site_count;
    module->exception_handlers = (PikaExceptionHandler*)shrink_array(
        module->exception_handlers,
        module->exception_handler_count,
        sizeof(*module->exception_handlers));
    module->exception_handler_capacity =
        module->exception_handler_count;
    module->semantic_names = (char*)shrink_array(
        module->semantic_names, module->semantic_name_count,
        sizeof(*module->semantic_names));
    module->semantic_name_capacity = module->semantic_name_count;
    module->semantic_parameters = (PikaSemanticParameter*)shrink_array(
        module->semantic_parameters,
        module->semantic_parameter_count,
        sizeof(*module->semantic_parameters));
    module->semantic_parameter_capacity =
        module->semantic_parameter_count;
    module->semantic_shapes = (PikaStaticShape*)shrink_array(
        module->semantic_shapes, module->semantic_shape_count,
        sizeof(*module->semantic_shapes));
    module->semantic_shape_capacity = module->semantic_shape_count;
    module->semantic_fields = (PikaSemanticField*)shrink_array(
        module->semantic_fields, module->semantic_field_count,
        sizeof(*module->semantic_fields));
    module->semantic_field_capacity = module->semantic_field_count;
    module->semantic_functions = (PikaSemanticFunction*)shrink_array(
        module->semantic_functions,
        module->semantic_function_count,
        sizeof(*module->semantic_functions));
    module->semantic_function_capacity =
        module->semantic_function_count;
    module->semantic_classes = (PikaSemanticClass*)shrink_array(
        module->semantic_classes, module->semantic_class_count,
        sizeof(*module->semantic_classes));
    module->semantic_class_capacity =
        module->semantic_class_count;
    module->semantic_globals = (PikaSemanticGlobal*)shrink_array(
        module->semantic_globals, module->semantic_global_count,
        sizeof(*module->semantic_globals));
    module->semantic_global_capacity =
        module->semantic_global_count;
    module->semantic_imports = (PikaSemanticImport*)shrink_array(
        module->semantic_imports, module->semantic_import_count,
        sizeof(*module->semantic_imports));
    module->semantic_import_capacity =
        module->semantic_import_count;
    module->semantic_python_imports =
        (PikaPythonModule*)shrink_array(
            module->semantic_python_imports,
            module->semantic_python_import_count,
            sizeof(*module->semantic_python_imports));
    module->semantic_python_import_capacity =
        module->semantic_python_import_count;
    module->program.instructions = module->instructions;
    module->program.functions = module->functions;
    module->program.classes = module->classes;
    module->program.binding_calls = module->binding_calls;
    module->program.builtin_calls = module->builtin_calls;
    module->program.dynamic_method_calls =
        module->dynamic_method_calls;
    module->program.dynamic_method_call_count =
        (uint16_t)module->dynamic_method_call_count;
    module->program.dynamic_method_candidates =
        module->dynamic_method_candidates;
    module->program.dynamic_method_candidate_count =
        (uint16_t)module->dynamic_method_candidate_count;
    module->program.constants = module->constants;
    module->program.dynamic_method_calls = module->dynamic_method_calls;
    module->program.dynamic_method_call_count =
        (uint16_t)module->dynamic_method_call_count;
    module->program.dynamic_method_candidates =
        module->dynamic_method_candidates;
    module->program.dynamic_method_candidate_count =
        (uint16_t)module->dynamic_method_candidate_count;
    module->program.index_sites = module->index_sites;
    module->program.exception_handlers = module->exception_handlers;
    module->program.module_names = module->semantic_names;
    module->program.module_name_bytes = module->semantic_name_count;
    module->program.python_modules = module->semantic_python_imports;
    module->program.python_module_count =
        module->semantic_python_import_count;
    module->program.binding_modules =
        module->available_binding_modules;
    module->program.binding_module_count =
        module->available_binding_module_count;
}

static PikaStatus finalize_execution_requirements(
    PikaCompiledModule* module) {
    PikaProgramRequirements requirements;
    PikaStatus status = pika_program_storage_requirements(
        &module->program, &requirements);
    if (status != PIKA_STATUS_OK) return status;
    module->program.execution_value_count =
        module->program.uses_typed_values != 0u
            ? requirements.typed_value_count
            : requirements.value_count;
    return PIKA_STATUS_OK;
}

static uint32_t count_tokens(const PikaTokenBuffer* tokens,
                             PikaTokenKind kind,
                             uint32_t maximum) {
    uint32_t index;
    uint32_t count = 0u;
    for (index = 0u; index < tokens->count; ++index) {
        if (tokens->tokens[index].kind == kind && count < maximum) {
            ++count;
        }
    }
    return count;
}

static uint32_t semantic_name_storage_capacity(
    const PikaTokenBuffer* tokens,
    uint32_t retained_bytes) {
    uint32_t index;
    uint32_t capacity = retained_bytes;
    for (index = 0u; index < tokens->count; ++index) {
        const PikaToken* token = &tokens->tokens[index];
        if (token->kind != PIKA_TOKEN_NAME) continue;
        if (token->length > UINT32_MAX - capacity) return 0u;
        capacity += token->length;
    }
    return capacity == 0u ? 1u : capacity;
}

static uint32_t initial_instruction_capacity(
    uint32_t token_count) {
    uint32_t target = token_count / 2u + 8u;
    return target < 8u ? 8u : target;
}

static int capability_dependencies_present(PikaCapability enabled,
                                           PikaCapability capability,
                                           PikaCapability dependencies) {
    return (enabled & capability) == 0u ||
           (enabled & dependencies) == dependencies;
}

static PikaStatus validate_capabilities(PikaCapability enabled) {
    if ((enabled & ~PIKA_CAPABILITY_ALL) != 0u) {
        return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
    }
    if (!capability_dependencies_present(
            enabled, PIKA_CAPABILITY_VALUE_INTEGER,
            PIKA_CAPABILITY_EXEC_MODULE) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_VALUE_BOOLEAN,
            PIKA_CAPABILITY_EXEC_MODULE) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_NAME_LOCAL,
            PIKA_CAPABILITY_EXEC_MODULE) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_INTEGER_OPERATOR,
            PIKA_CAPABILITY_VALUE_INTEGER) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_TRUTH_PROTOCOL,
            PIKA_CAPABILITY_VALUE_BOOLEAN) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_BRANCH_FLOW,
            PIKA_CAPABILITY_TRUTH_PROTOCOL) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_LOOP_FLOW,
            PIKA_CAPABILITY_BRANCH_FLOW) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_POSITIONAL_CALL,
            PIKA_CAPABILITY_NAME_LOCAL) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_RANGE_ITERATION,
            PIKA_CAPABILITY_LOOP_FLOW |
                PIKA_CAPABILITY_VALUE_INTEGER) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_PRINT_BUILTIN,
            PIKA_CAPABILITY_EXEC_MODULE) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_VALUE_NONE,
            PIKA_CAPABILITY_EXEC_MODULE) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_VALUE_FLOAT,
            PIKA_CAPABILITY_EXEC_MODULE) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_VALUE_STRING,
            PIKA_CAPABILITY_EXEC_MODULE) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_VALUE_BYTES,
            PIKA_CAPABILITY_EXEC_MODULE) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_NAME_GLOBAL,
            PIKA_CAPABILITY_EXEC_MODULE) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_MODULE_IMPORT,
            PIKA_CAPABILITY_NAME_GLOBAL) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_C_BINDING,
            PIKA_CAPABILITY_MODULE_IMPORT |
                PIKA_CAPABILITY_POSITIONAL_CALL) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_OBJECT_ATTRIBUTE,
            PIKA_CAPABILITY_NAME_GLOBAL |
                PIKA_CAPABILITY_POSITIONAL_CALL) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_C_OBJECT_BINDING,
            PIKA_CAPABILITY_C_BINDING |
                PIKA_CAPABILITY_OBJECT_ATTRIBUTE) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_BINDING_OBJECT_RESULT,
            PIKA_CAPABILITY_C_OBJECT_BINDING) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_ASSIGNMENT_UNPACK,
            PIKA_CAPABILITY_NAME_LOCAL) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_COMPLETE_OPERATOR,
            PIKA_CAPABILITY_VALUE_INTEGER |
                PIKA_CAPABILITY_TRUTH_PROTOCOL) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_EMBEDDED_BUILTIN,
            PIKA_CAPABILITY_EXEC_MODULE |
                PIKA_CAPABILITY_TRUTH_PROTOCOL) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_VALUE_CONTAINER,
            PIKA_CAPABILITY_EXEC_MODULE) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_BUILTIN_ITERATION,
            PIKA_CAPABILITY_LOOP_FLOW) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_PYTHON_MODULE,
            PIKA_CAPABILITY_MODULE_IMPORT |
                PIKA_CAPABILITY_OBJECT_ATTRIBUTE) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_OBJECT_CLASS,
            PIKA_CAPABILITY_OBJECT_ATTRIBUTE) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_EMBEDDED_INTEGER,
            PIKA_CAPABILITY_VALUE_INTEGER) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_DEFAULT_CALL,
            PIKA_CAPABILITY_POSITIONAL_CALL) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_KEYWORD_CALL,
            PIKA_CAPABILITY_POSITIONAL_CALL) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_FROM_IMPORT,
            PIKA_CAPABILITY_MODULE_IMPORT) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_BINDING_CONSTANT,
            PIKA_CAPABILITY_C_BINDING |
                PIKA_CAPABILITY_OBJECT_ATTRIBUTE) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_EXCEPTION_BASIC,
            PIKA_CAPABILITY_EXEC_MODULE) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_EXCEPTION_FINALLY,
            PIKA_CAPABILITY_EXCEPTION_BASIC) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_CALL_REFERENCE,
            PIKA_CAPABILITY_POSITIONAL_CALL) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_VARIADIC_CALL,
            PIKA_CAPABILITY_POSITIONAL_CALL |
                PIKA_CAPABILITY_CONTAINER_TUPLE) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_CONTAINER_TUPLE,
            PIKA_CAPABILITY_EXEC_MODULE) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_CONTAINER_LIST,
            PIKA_CAPABILITY_EXEC_MODULE) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_CONTAINER_DICT,
            PIKA_CAPABILITY_EXEC_MODULE) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_CONTAINER_BYTEARRAY,
            PIKA_CAPABILITY_VALUE_BYTES) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_NUMERIC_OPERATOR,
            PIKA_CAPABILITY_VALUE_INTEGER) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_COMPARISON_OPERATOR,
            PIKA_CAPABILITY_VALUE_BOOLEAN) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_BITWISE_OPERATOR,
            PIKA_CAPABILITY_VALUE_INTEGER) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_MEMBERSHIP_OPERATOR,
            PIKA_CAPABILITY_VALUE_BOOLEAN) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_SUBSCRIPT_OPERATOR,
            PIKA_CAPABILITY_VALUE_INTEGER) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_SLICE_OPERATOR,
            PIKA_CAPABILITY_VALUE_INTEGER) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_AUGMENTED_OPERATOR,
            PIKA_CAPABILITY_NUMERIC_OPERATOR) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_MATRIX_OPERATOR,
            PIKA_CAPABILITY_CONTAINER_LIST |
                PIKA_CAPABILITY_NUMERIC_OPERATOR) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_SHORT_CIRCUIT_LOGIC,
            PIKA_CAPABILITY_BRANCH_FLOW) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_PASS_FLOW,
            PIKA_CAPABILITY_EXEC_MODULE) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_GLOBAL_STATEMENT,
            PIKA_CAPABILITY_NAME_GLOBAL |
                PIKA_CAPABILITY_POSITIONAL_CALL) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_LEN_BUILTIN,
            PIKA_CAPABILITY_VALUE_INTEGER) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_SCALAR_CONVERT_BUILTIN,
            PIKA_CAPABILITY_VALUE_INTEGER |
                PIKA_CAPABILITY_VALUE_BOOLEAN |
                PIKA_CAPABILITY_VALUE_STRING |
                PIKA_CAPABILITY_VALUE_BYTES) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_CONTAINER_CONSTRUCT_BUILTIN,
            PIKA_CAPABILITY_CONTAINER_TUPLE |
                PIKA_CAPABILITY_CONTAINER_LIST |
                PIKA_CAPABILITY_CONTAINER_DICT |
                PIKA_CAPABILITY_CONTAINER_BYTEARRAY) ||
        !capability_dependencies_present(
            enabled, PIKA_CAPABILITY_PYTHON_CALLBACK,
            PIKA_CAPABILITY_C_BINDING |
                PIKA_CAPABILITY_NAME_GLOBAL |
                PIKA_CAPABILITY_POSITIONAL_CALL)) {
        return PIKA_STATUS_CAPABILITY_DEPENDENCY;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus copy_constants(
    const PikaCompiledModule* source,
    PikaCompiledModule* target,
    int borrow_strings) {
    uint32_t index;
    target->borrowed_constant_count =
        borrow_strings != 0 ? source->constant_count : 0u;
    for (index = 0u;
         index < source->constant_count;
         ++index) {
        PikaConstant constant = source->constants[index];
        if (constant_owns_bytes(constant.kind)) {
            if (borrow_strings != 0) {
                target->constants[target->constant_count++] =
                    constant;
                continue;
            }
            char* bytes = (char*)malloc(
                (size_t)constant.length + 1u);
            if (bytes == NULL) {
                return PIKA_STATUS_FRONTEND_LIMIT;
            }
            if (constant.length > 0u) {
                memcpy(
                    bytes, constant.as.bytes,
                    constant.length);
            }
            bytes[constant.length] = '\0';
            constant.as.bytes = bytes;
        }
        target->constants[target->constant_count++] =
            constant;
    }
    return PIKA_STATUS_OK;
}

static void* copy_array(const void* source,
                        uint32_t count,
                        size_t element_size) {
    void* target;
    if (count == 0u) return NULL;
    if ((size_t)count > SIZE_MAX / element_size) return NULL;
    target = malloc((size_t)count * element_size);
    if (target != NULL) {
        memcpy(target, source, (size_t)count * element_size);
    }
    return target;
}

typedef struct {
    PikaCompiledModule* base;
    PikaSemanticGlobal* global_snapshot;
    PikaFunction entry_snapshot;
    uint32_t class_count;
    uint32_t binding_call_count;
    uint32_t builtin_call_count;
    uint32_t dynamic_method_call_count;
    uint32_t dynamic_method_candidate_count;
    uint32_t constant_count;
    uint32_t index_site_count;
    uint32_t semantic_name_count;
    uint16_t semantic_function_count;
    uint16_t semantic_parameter_count;
    uint32_t semantic_shape_count;
    uint16_t semantic_class_count;
    uint16_t semantic_field_count;
    uint16_t semantic_global_count;
    uint16_t semantic_import_count;
    uint16_t semantic_python_import_count;
    uint16_t program_module_count;
    uint32_t program_module_dependency_count;
    uint32_t program_binding_import_count;
    uint16_t function_count;
    uint16_t entry_function;
    uint8_t entry_snapshot_valid;
    uint8_t active;
} PikaStorageTransfer;

static void alias_reusable_storage(
    PikaCompiledModule* target,
    const PikaCompiledModule* source) {
    target->available_binding_modules =
        source->available_binding_modules;
    target->available_binding_module_count =
        source->available_binding_module_count;
    target->functions = source->functions;
    target->function_capacity = source->function_capacity;
    target->classes = source->classes;
    target->class_capacity = source->class_capacity;
    target->class_count = source->class_count;
    target->binding_calls = source->binding_calls;
    target->binding_call_capacity = source->binding_call_capacity;
    target->binding_call_count = source->binding_call_count;
    target->builtin_calls = source->builtin_calls;
    target->builtin_call_capacity = source->builtin_call_capacity;
    target->builtin_call_count = source->builtin_call_count;
    target->dynamic_method_calls = source->dynamic_method_calls;
    target->dynamic_method_call_capacity =
        source->dynamic_method_call_capacity;
    target->dynamic_method_call_count =
        source->dynamic_method_call_count;
    target->dynamic_method_candidates =
        source->dynamic_method_candidates;
    target->dynamic_method_candidate_capacity =
        source->dynamic_method_candidate_capacity;
    target->dynamic_method_candidate_count =
        source->dynamic_method_candidate_count;
    target->constants = source->constants;
    target->constant_capacity = source->constant_capacity;
    target->constant_count = source->constant_count;
    target->index_sites = source->index_sites;
    target->index_site_capacity = source->index_site_capacity;
    target->index_site_count = source->index_site_count;
    target->semantic_names = source->semantic_names;
    target->semantic_name_capacity = source->semantic_name_capacity;
    target->semantic_name_count = source->semantic_name_count;
    target->semantic_functions = source->semantic_functions;
    target->semantic_function_capacity =
        source->semantic_function_capacity;
    target->semantic_function_count =
        source->semantic_function_count;
    target->semantic_parameters = source->semantic_parameters;
    target->semantic_parameter_capacity =
        source->semantic_parameter_capacity;
    target->semantic_parameter_count =
        source->semantic_parameter_count;
    target->semantic_shapes = source->semantic_shapes;
    target->semantic_shape_capacity = source->semantic_shape_capacity;
    target->semantic_shape_count = source->semantic_shape_count;
    target->semantic_classes = source->semantic_classes;
    target->semantic_class_capacity = source->semantic_class_capacity;
    target->semantic_class_count = source->semantic_class_count;
    target->semantic_fields = source->semantic_fields;
    target->semantic_field_capacity = source->semantic_field_capacity;
    target->semantic_field_count = source->semantic_field_count;
    target->semantic_globals = source->semantic_globals;
    target->semantic_global_capacity = source->semantic_global_capacity;
    target->semantic_global_count = source->semantic_global_count;
    target->semantic_imports = source->semantic_imports;
    target->semantic_import_capacity = source->semantic_import_capacity;
    target->semantic_import_count = source->semantic_import_count;
    target->semantic_python_imports = source->semantic_python_imports;
    target->semantic_python_import_capacity =
        source->semantic_python_import_capacity;
    target->semantic_python_import_count =
        source->semantic_python_import_count;
    target->program_modules = source->program_modules;
    target->program_module_capacity = source->program_module_capacity;
    target->program_module_count = source->program_module_count;
    target->program_module_dependencies =
        source->program_module_dependencies;
    target->program_module_dependency_capacity =
        source->program_module_dependency_capacity;
    target->program_module_dependency_count =
        source->program_module_dependency_count;
    target->program_binding_imports = source->program_binding_imports;
    target->program_binding_import_capacity =
        source->program_binding_import_capacity;
    target->program_binding_import_count =
        source->program_binding_import_count;
    target->program.classes = target->classes;
    target->program.functions = target->functions;
    target->program.binding_calls = target->binding_calls;
    target->program.builtin_calls = target->builtin_calls;
    target->program.dynamic_method_calls = target->dynamic_method_calls;
    target->program.dynamic_method_call_count =
        (uint16_t)target->dynamic_method_call_count;
    target->program.dynamic_method_candidates =
        target->dynamic_method_candidates;
    target->program.dynamic_method_candidate_count =
        (uint16_t)target->dynamic_method_candidate_count;
    target->program.constants = target->constants;
    target->program.index_sites = target->index_sites;
    target->borrowed_constant_count = 0u;
}

static void detach_reusable_storage(PikaCompiledModule* module) {
    module->available_binding_modules = NULL;
    module->available_binding_module_count = 0u;
    module->functions = NULL;
    module->function_capacity = 0u;
    module->classes = NULL;
    module->class_capacity = 0u;
    module->class_count = 0u;
    module->binding_calls = NULL;
    module->binding_call_capacity = 0u;
    module->binding_call_count = 0u;
    module->builtin_calls = NULL;
    module->builtin_call_capacity = 0u;
    module->builtin_call_count = 0u;
    module->dynamic_method_calls = NULL;
    module->dynamic_method_call_capacity = 0u;
    module->dynamic_method_call_count = 0u;
    module->dynamic_method_candidates = NULL;
    module->dynamic_method_candidate_capacity = 0u;
    module->dynamic_method_candidate_count = 0u;
    module->constants = NULL;
    module->constant_capacity = 0u;
    module->constant_count = 0u;
    module->index_sites = NULL;
    module->index_site_capacity = 0u;
    module->index_site_count = 0u;
    module->semantic_names = NULL;
    module->semantic_name_capacity = 0u;
    module->semantic_name_count = 0u;
    module->semantic_functions = NULL;
    module->semantic_function_capacity = 0u;
    module->semantic_function_count = 0u;
    module->semantic_parameters = NULL;
    module->semantic_parameter_capacity = 0u;
    module->semantic_parameter_count = 0u;
    module->semantic_shapes = NULL;
    module->semantic_shape_capacity = 0u;
    module->semantic_shape_count = 0u;
    module->semantic_classes = NULL;
    module->semantic_class_capacity = 0u;
    module->semantic_class_count = 0u;
    module->semantic_fields = NULL;
    module->semantic_field_capacity = 0u;
    module->semantic_field_count = 0u;
    module->semantic_globals = NULL;
    module->semantic_global_capacity = 0u;
    module->semantic_global_count = 0u;
    module->semantic_imports = NULL;
    module->semantic_import_capacity = 0u;
    module->semantic_import_count = 0u;
    module->semantic_python_imports = NULL;
    module->semantic_python_import_capacity = 0u;
    module->semantic_python_import_count = 0u;
    module->program_modules = NULL;
    module->program_module_capacity = 0u;
    module->program_module_count = 0u;
    module->program_module_dependencies = NULL;
    module->program_module_dependency_capacity = 0u;
    module->program_module_dependency_count = 0u;
    module->program_binding_imports = NULL;
    module->program_binding_import_capacity = 0u;
    module->program_binding_import_count = 0u;
    module->program.classes = NULL;
    module->program.functions = NULL;
    module->program.binding_calls = NULL;
    module->program.builtin_calls = NULL;
    module->program.dynamic_method_calls = NULL;
    module->program.dynamic_method_call_count = 0u;
    module->program.dynamic_method_candidates = NULL;
    module->program.dynamic_method_candidate_count = 0u;
    module->program.constants = NULL;
    module->program.index_sites = NULL;
    module->borrowed_constant_count = 0u;
}

static void release_incremental_preallocation(
    PikaCompiledModule* module) {
    free(module->functions);
    free(module->classes);
    free(module->binding_calls);
    free(module->builtin_calls);
    free(module->dynamic_method_calls);
    free(module->dynamic_method_candidates);
    free(module->constants);
    free(module->index_sites);
    free(module->semantic_names);
    free(module->semantic_fields);
    module->functions = NULL;
    module->classes = NULL;
    module->binding_calls = NULL;
    module->builtin_calls = NULL;
    module->dynamic_method_calls = NULL;
    module->dynamic_method_candidates = NULL;
    module->constants = NULL;
    module->index_sites = NULL;
    module->semantic_names = NULL;
    module->semantic_fields = NULL;
}

static PikaStatus begin_storage_transfer(
    PikaCompiledModule* base,
    PikaCompiledModule* module,
    PikaStorageTransfer* transfer) {
    uint32_t requested_function_capacity =
        module->function_capacity;
    uint32_t requested_class_capacity = module->class_capacity;
    uint32_t requested_name_capacity =
        module->semantic_name_capacity;
    uint32_t old_class_capacity;
    if (base == NULL || module == NULL || transfer == NULL ||
        base->borrows_program_image != 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (base->functions == NULL ||
        base->program.entry_function !=
            base->semantic_function_count ||
        base->program.entry_function >=
            base->program.function_count) {
        return PIKA_STATUS_INTERNAL_ERROR;
    }
    if (base->semantic_global_count > 0u) {
        if (base->semantic_globals == NULL) {
            return PIKA_STATUS_INTERNAL_ERROR;
        }
        transfer->global_snapshot =
            (PikaSemanticGlobal*)malloc(
                (size_t)base->semantic_global_count *
                sizeof(*transfer->global_snapshot));
        if (transfer->global_snapshot == NULL) {
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
        memcpy(
            transfer->global_snapshot,
            base->semantic_globals,
            (size_t)base->semantic_global_count *
                sizeof(*transfer->global_snapshot));
    }
    transfer->base = base;
    transfer->class_count = base->class_count;
    transfer->binding_call_count = base->binding_call_count;
    transfer->builtin_call_count = base->builtin_call_count;
    transfer->dynamic_method_call_count =
        base->dynamic_method_call_count;
    transfer->dynamic_method_candidate_count =
        base->dynamic_method_candidate_count;
    transfer->constant_count = base->constant_count;
    transfer->index_site_count = base->index_site_count;
    transfer->semantic_name_count = base->semantic_name_count;
    transfer->semantic_function_count =
        base->semantic_function_count;
    transfer->semantic_parameter_count =
        base->semantic_parameter_count;
    transfer->semantic_shape_count = base->semantic_shape_count;
    transfer->semantic_class_count = base->semantic_class_count;
    transfer->semantic_field_count = base->semantic_field_count;
    transfer->semantic_global_count = base->semantic_global_count;
    transfer->semantic_import_count = base->semantic_import_count;
    transfer->semantic_python_import_count =
        base->semantic_python_import_count;
    transfer->program_module_count = base->program_module_count;
    transfer->program_module_dependency_count =
        base->program_module_dependency_count;
    transfer->program_binding_import_count =
        base->program_binding_import_count;
    transfer->entry_snapshot =
        base->functions[base->program.entry_function];
    transfer->function_count = base->program.function_count;
    transfer->entry_function = base->program.entry_function;
    transfer->entry_snapshot_valid = 1u;
    release_incremental_preallocation(module);
    alias_reusable_storage(module, base);
    transfer->active = 1u;
    if (requested_function_capacity >
        module->function_capacity) {
        PikaFunction* functions = (PikaFunction*)realloc(
            module->functions,
            (size_t)requested_function_capacity *
                sizeof(*functions));
        if (functions == NULL) {
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
        module->functions = functions;
        module->function_capacity =
            requested_function_capacity;
        module->program.functions = functions;
    }
    if (requested_name_capacity > module->semantic_name_capacity) {
        char* names = (char*)realloc(
            module->semantic_names, requested_name_capacity);
        if (names == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
        module->semantic_names = names;
        module->semantic_name_capacity = requested_name_capacity;
    }
    old_class_capacity = module->class_capacity;
    if (requested_class_capacity > old_class_capacity) {
        PikaClassLayout* classes = (PikaClassLayout*)realloc(
            module->classes,
            (size_t)requested_class_capacity * sizeof(*classes));
        if (classes == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
        memset(
            &classes[old_class_capacity], 0,
            (size_t)(requested_class_capacity - old_class_capacity) *
                sizeof(*classes));
        module->classes = classes;
        module->class_capacity = requested_class_capacity;
    }
    alias_reusable_storage(base, module);
    return PIKA_STATUS_OK;
}

static void rollback_storage_transfer(
    PikaStorageTransfer* transfer,
    PikaCompiledModule* module) {
    PikaCompiledModule* base;
    uint32_t index;
    if (transfer->active == 0u) return;
    base = transfer->base;
    for (index = transfer->constant_count;
         index < module->constant_count;
         ++index) {
        if (constant_owns_bytes(module->constants[index].kind)) {
            free((void*)module->constants[index].as.bytes);
        }
    }
    if (transfer->semantic_global_count > 0u) {
        memcpy(
            module->semantic_globals,
            transfer->global_snapshot,
            (size_t)transfer->semantic_global_count *
                sizeof(*transfer->global_snapshot));
    }
    alias_reusable_storage(base, module);
    if (transfer->entry_snapshot_valid != 0u) {
        base->functions[transfer->entry_function] =
            transfer->entry_snapshot;
    }
    base->program.function_count = transfer->function_count;
    base->program.entry_function = transfer->entry_function;
    base->class_count = transfer->class_count;
    base->binding_call_count = transfer->binding_call_count;
    base->builtin_call_count = transfer->builtin_call_count;
    base->dynamic_method_call_count =
        transfer->dynamic_method_call_count;
    base->dynamic_method_candidate_count =
        transfer->dynamic_method_candidate_count;
    base->constant_count = transfer->constant_count;
    base->index_site_count = transfer->index_site_count;
    base->semantic_name_count = transfer->semantic_name_count;
    base->semantic_function_count =
        transfer->semantic_function_count;
    base->semantic_parameter_count =
        transfer->semantic_parameter_count;
    base->semantic_shape_count = transfer->semantic_shape_count;
    base->semantic_class_count = transfer->semantic_class_count;
    base->semantic_field_count = transfer->semantic_field_count;
    base->semantic_global_count = transfer->semantic_global_count;
    base->semantic_import_count = transfer->semantic_import_count;
    base->semantic_python_import_count =
        transfer->semantic_python_import_count;
    base->program_module_count = transfer->program_module_count;
    base->program_module_dependency_count =
        transfer->program_module_dependency_count;
    base->program_binding_import_count =
        transfer->program_binding_import_count;
    detach_reusable_storage(module);
    free(transfer->global_snapshot);
    transfer->global_snapshot = NULL;
    transfer->entry_snapshot_valid = 0u;
    transfer->active = 0u;
}

static void commit_storage_transfer(
    PikaStorageTransfer* transfer) {
    if (transfer->active == 0u) return;
    detach_reusable_storage(transfer->base);
    free(transfer->global_snapshot);
    transfer->global_snapshot = NULL;
    transfer->entry_snapshot_valid = 0u;
    transfer->active = 0u;
}

static PikaCompiledModule* allocate_incremental_module(
    const PikaCompiledModule* base,
    const PikaTokenBuffer* tokens) {
    PikaCompiledModule* module =
        (PikaCompiledModule*)calloc(
            1u, sizeof(PikaCompiledModule));
    uint32_t retained_instructions =
        base->program.functions[
            base->program.entry_function]
            .first_instruction;
    uint32_t added_functions;
    uint32_t added_classes;
    if (module == NULL) return NULL;
    added_functions = count_tokens(
        tokens, PIKA_TOKEN_DEF, PIKA_FRONTEND_FUNCTION_LIMIT);
    added_functions += count_tokens(
        tokens, PIKA_TOKEN_LAMBDA, PIKA_FRONTEND_FUNCTION_LIMIT);
    if (added_functions > PIKA_FRONTEND_FUNCTION_LIMIT) {
        added_functions = PIKA_FRONTEND_FUNCTION_LIMIT;
    }
    added_classes = count_tokens(
        tokens, PIKA_TOKEN_CLASS, PIKA_FRONTEND_CLASS_LIMIT);
    module->instruction_storage_base = retained_instructions;
    module->instruction_capacity =
        initial_instruction_capacity(tokens->count);
    module->function_capacity =
        base->semantic_function_count + added_functions + 1u;
    if (module->function_capacity >
        PIKA_FRONTEND_FUNCTION_LIMIT + 1u) {
        module->function_capacity =
            PIKA_FRONTEND_FUNCTION_LIMIT + 1u;
    }
    module->class_capacity = base->class_count + added_classes;
    if (module->class_capacity > PIKA_FRONTEND_CLASS_LIMIT) {
        module->class_capacity = PIKA_FRONTEND_CLASS_LIMIT;
    }
    module->binding_call_capacity = base->binding_call_count;
    module->builtin_call_capacity = base->builtin_call_count;
    module->dynamic_method_call_capacity =
        base->dynamic_method_call_count;
    module->dynamic_method_candidate_capacity =
        base->dynamic_method_candidate_count;
    module->constant_capacity = base->constant_count;
    module->index_site_capacity = base->index_site_count;
    module->exception_handler_capacity =
        base->exception_handler_count;
    module->semantic_name_capacity =
        semantic_name_storage_capacity(
            tokens, base->semantic_name_count);
    module->semantic_field_capacity =
        base->semantic_field_count;
    if (module->semantic_name_capacity == 0u) {
        release_module(module);
        return NULL;
    }
    module->instructions = (PikaInstruction*)calloc(
        module->instruction_capacity,
        sizeof(PikaInstruction));
    module->functions = (PikaFunction*)calloc(
        module->function_capacity, sizeof(PikaFunction));
    if (module->class_capacity > 0u) {
        module->classes = (PikaClassLayout*)calloc(
            module->class_capacity, sizeof(PikaClassLayout));
    }
    if (module->binding_call_capacity > 0u) {
        module->binding_calls = (PikaBindingCallSite*)calloc(
            module->binding_call_capacity,
            sizeof(PikaBindingCallSite));
    }
    if (module->builtin_call_capacity > 0u) {
        module->builtin_calls = (PikaBuiltinCallSite*)calloc(
            module->builtin_call_capacity,
            sizeof(PikaBuiltinCallSite));
    }
    if (module->dynamic_method_call_capacity > 0u) {
        module->dynamic_method_calls =
            (PikaDynamicMethodCallSite*)calloc(
                module->dynamic_method_call_capacity,
                sizeof(*module->dynamic_method_calls));
    }
    if (module->dynamic_method_candidate_capacity > 0u) {
        module->dynamic_method_candidates =
            (PikaDynamicMethodCandidate*)calloc(
                module->dynamic_method_candidate_capacity,
                sizeof(*module->dynamic_method_candidates));
    }
    if (module->constant_capacity > 0u) {
        module->constants = (PikaConstant*)calloc(
            module->constant_capacity, sizeof(PikaConstant));
    }
    if (module->index_site_capacity > 0u) {
        module->index_sites = (PikaIndexSite*)calloc(
            module->index_site_capacity, sizeof(PikaIndexSite));
    }
    if (module->exception_handler_capacity > 0u) {
        module->exception_handlers =
            (PikaExceptionHandler*)calloc(
                module->exception_handler_capacity,
                sizeof(PikaExceptionHandler));
    }
    module->semantic_names = (char*)malloc(
        module->semantic_name_capacity);
    if (module->semantic_field_capacity > 0u) {
        module->semantic_fields = (PikaSemanticField*)calloc(
            module->semantic_field_capacity,
            sizeof(*module->semantic_fields));
    }
    if (module->instructions == NULL ||
        module->functions == NULL ||
        module->semantic_names == NULL ||
        (module->class_capacity > 0u &&
         module->classes == NULL) ||
        (module->binding_call_capacity > 0u &&
         module->binding_calls == NULL) ||
        (module->builtin_call_capacity > 0u &&
         module->builtin_calls == NULL) ||
        (module->dynamic_method_call_capacity > 0u &&
         module->dynamic_method_calls == NULL) ||
        (module->dynamic_method_candidate_capacity > 0u &&
         module->dynamic_method_candidates == NULL) ||
        (module->constant_capacity > 0u &&
         module->constants == NULL) ||
        (module->index_site_capacity > 0u &&
         module->index_sites == NULL) ||
        (module->exception_handler_capacity > 0u &&
         module->exception_handlers == NULL) ||
        (module->semantic_field_capacity > 0u &&
         module->semantic_fields == NULL)) {
        release_module(module);
        return NULL;
    }
    return module;
}

static PikaStatus copy_incremental_state(
    const PikaCompiledModule* base,
    PikaCompiledModule* module,
    int borrow_constants,
    int reuse_storage) {
    uint32_t retained_instructions =
        base->program.functions[
            base->program.entry_function]
            .first_instruction;
    uint32_t index;
    module->program = base->program;
    module->program.instruction_count =
        retained_instructions;
    module->program.instruction_prefix = NULL;
    module->program.instruction_prefix_count = 0u;
    if (borrow_constants != 0) {
        if (base->program.instruction_prefix_count > 0u) {
            module->program.instruction_prefix =
                base->program.instruction_prefix;
            module->program.instruction_prefix_count =
                base->program.instruction_prefix_count;
        } else if (base->borrows_program_image != 0u) {
            module->program.instruction_prefix =
                base->program.instructions;
            module->program.instruction_prefix_count =
                retained_instructions;
        }
    }
    if (reuse_storage == 0 &&
        base->semantic_function_count > 0u) {
        memcpy(
            module->functions, base->functions,
            base->semantic_function_count *
                sizeof(PikaFunction));
    }
    module->program.function_count =
        base->semantic_function_count;
    module->program.entry_function =
        base->semantic_function_count;
    module->class_count = base->class_count;
    if (reuse_storage == 0 && base->class_count > 0u) {
        memcpy(
            module->classes, base->classes,
            base->class_count * sizeof(PikaClassLayout));
    }
    module->binding_call_count =
        base->binding_call_count;
    if (reuse_storage == 0 &&
        base->binding_call_count > 0u) {
        memcpy(
            module->binding_calls, base->binding_calls,
            base->binding_call_count *
                sizeof(PikaBindingCallSite));
    }
    module->builtin_call_count =
        base->builtin_call_count;
    if (reuse_storage == 0 &&
        base->builtin_call_count > 0u) {
        memcpy(
            module->builtin_calls, base->builtin_calls,
            base->builtin_call_count *
                sizeof(PikaBuiltinCallSite));
    }
    module->dynamic_method_call_count =
        base->dynamic_method_call_count;
    if (reuse_storage == 0 &&
        base->dynamic_method_call_count > 0u) {
        memcpy(
            module->dynamic_method_calls,
            base->dynamic_method_calls,
            base->dynamic_method_call_count *
                sizeof(*module->dynamic_method_calls));
    }
    module->dynamic_method_candidate_count =
        base->dynamic_method_candidate_count;
    if (reuse_storage == 0 &&
        base->dynamic_method_candidate_count > 0u) {
        memcpy(
            module->dynamic_method_candidates,
            base->dynamic_method_candidates,
            base->dynamic_method_candidate_count *
                sizeof(*module->dynamic_method_candidates));
    }
    module->index_site_count = base->index_site_count;
    if (reuse_storage == 0 &&
        base->index_site_count > 0u) {
        memcpy(
            module->index_sites, base->index_sites,
            base->index_site_count *
                sizeof(PikaIndexSite));
    }
    for (index = 0u;
         index < base->exception_handler_count;
         ++index) {
        if (base->exception_handlers[index].function_index <
            base->semantic_function_count) {
            module->exception_handlers[
                module->exception_handler_count++] =
                base->exception_handlers[index];
        }
    }
    module->object_slot_estimate =
        base->object_slot_estimate;
    module->object_byte_estimate =
        base->object_byte_estimate;
    module->semantic_name_count =
        base->semantic_name_count;
    if (reuse_storage == 0 &&
        base->semantic_name_count > 0u) {
        memcpy(
            module->semantic_names,
            base->semantic_names,
            base->semantic_name_count);
    }
    if (reuse_storage == 0) {
        module->semantic_functions =
            (PikaSemanticFunction*)copy_array(
                base->semantic_functions,
                base->semantic_function_count,
                sizeof(*module->semantic_functions));
        if (base->semantic_function_count > 0u &&
            module->semantic_functions == NULL) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
    }
    module->semantic_function_capacity =
        base->semantic_function_count;
    module->semantic_function_count =
        base->semantic_function_count;
    if (reuse_storage == 0) {
        module->semantic_classes =
            (PikaSemanticClass*)copy_array(
                base->semantic_classes,
                base->semantic_class_count,
                sizeof(*module->semantic_classes));
        if (base->semantic_class_count > 0u &&
            module->semantic_classes == NULL) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
    }
    module->semantic_class_capacity =
        base->semantic_class_count;
    module->semantic_class_count =
        base->semantic_class_count;
    if (reuse_storage == 0 &&
        base->semantic_field_count > 0u) {
        memcpy(
            module->semantic_fields,
            base->semantic_fields,
            (size_t)base->semantic_field_count *
                sizeof(*module->semantic_fields));
    }
    module->semantic_field_count =
        base->semantic_field_count;
    if (reuse_storage == 0) {
        module->semantic_globals =
            (PikaSemanticGlobal*)copy_array(
                base->semantic_globals,
                base->semantic_global_count,
                sizeof(*module->semantic_globals));
        if (base->semantic_global_count > 0u &&
            module->semantic_globals == NULL) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
    }
    module->semantic_global_capacity =
        base->semantic_global_count;
    module->semantic_global_count =
        base->semantic_global_count;
    if (reuse_storage == 0) {
        module->semantic_imports =
            (PikaSemanticImport*)copy_array(
                base->semantic_imports,
                base->semantic_import_count,
                sizeof(*module->semantic_imports));
        if (base->semantic_import_count > 0u &&
            module->semantic_imports == NULL) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
    }
    module->semantic_import_capacity =
        base->semantic_import_count;
    module->semantic_import_count =
        base->semantic_import_count;
    if (reuse_storage == 0) {
        module->semantic_python_imports =
            (PikaPythonModule*)copy_array(
                base->semantic_python_imports,
                base->semantic_python_import_count,
                sizeof(*module->semantic_python_imports));
        if (base->semantic_python_import_count > 0u &&
            module->semantic_python_imports == NULL) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
    }
    module->semantic_python_import_capacity =
        base->semantic_python_import_count;
    module->semantic_python_import_count =
        base->semantic_python_import_count;
    if (reuse_storage == 0) {
        module->program_modules =
            (PikaSemanticProgramModule*)copy_array(
                base->program_modules,
                base->program_module_count,
                sizeof(*module->program_modules));
        if (base->program_module_count > 0u &&
            module->program_modules == NULL) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
    }
    module->program_module_capacity =
        base->program_module_count;
    module->program_module_count =
        base->program_module_count;
    if (reuse_storage == 0) {
        module->program_module_dependencies = (uint8_t*)copy_array(
            base->program_module_dependencies,
            base->program_module_dependency_count,
            sizeof(*module->program_module_dependencies));
        if (base->program_module_dependency_count > 0u &&
            module->program_module_dependencies == NULL) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
    }
    module->program_module_dependency_capacity =
        base->program_module_dependency_count;
    module->program_module_dependency_count =
        base->program_module_dependency_count;
    if (reuse_storage == 0) {
        module->program_binding_imports =
            (PikaSemanticProgramBindingImport*)copy_array(
                base->program_binding_imports,
                base->program_binding_import_count,
                sizeof(*module->program_binding_imports));
        if (base->program_binding_import_count > 0u &&
            module->program_binding_imports == NULL) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
    }
    module->program_binding_import_capacity =
        base->program_binding_import_count;
    module->program_binding_import_count =
        base->program_binding_import_count;
    module->program_entry_module = base->program_entry_module;
    module->semantic_parameter_count =
        base->semantic_parameter_count;
    module->semantic_parameter_capacity =
        base->semantic_parameter_count;
    if (reuse_storage == 0 &&
        base->semantic_parameter_count > 0u) {
        module->semantic_parameters =
            (PikaSemanticParameter*)malloc(
                base->semantic_parameter_count *
                sizeof(PikaSemanticParameter));
        if (module->semantic_parameters == NULL) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
        memcpy(
            module->semantic_parameters,
            base->semantic_parameters,
            base->semantic_parameter_count *
                sizeof(PikaSemanticParameter));
    }
    module->semantic_shape_count =
        base->semantic_shape_count;
    module->semantic_shape_capacity =
        base->semantic_shape_count;
    if (reuse_storage == 0 &&
        base->semantic_shape_count > 0u) {
        module->semantic_shapes =
            (PikaStaticShape*)malloc(
                base->semantic_shape_count *
                sizeof(PikaStaticShape));
        if (module->semantic_shapes == NULL) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
        memcpy(
            module->semantic_shapes,
            base->semantic_shapes,
            base->semantic_shape_count *
                sizeof(PikaStaticShape));
    }
    module->enabled_capabilities =
        base->enabled_capabilities;
    module->bindings = base->bindings;
    if (reuse_storage == 0) {
        PikaStatus status = pika_frontend_capture_binding_modules(
            module, base->bindings);
        if (status != PIKA_STATUS_OK) return status;
    }
    module->program.classes = module->classes;
    module->program.binding_calls = module->binding_calls;
    module->program.builtin_calls = module->builtin_calls;
    module->program.dynamic_method_calls =
        module->dynamic_method_calls;
    module->program.dynamic_method_candidates =
        module->dynamic_method_candidates;
    module->program.constants = module->constants;
    module->program.index_sites = module->index_sites;
    if (reuse_storage != 0) {
        return PIKA_STATUS_OK;
    }
    return copy_constants(base, module, borrow_constants);
}

static void commit_constant_transfer(
    PikaCompiledModule* base,
    PikaCompiledModule* module) {
    uint32_t inherited_borrowed_count;
    uint32_t index;
    uint32_t borrowed_count;
    if (base == NULL || module == NULL) return;
    borrowed_count = module->borrowed_constant_count;
    if (borrowed_count == 0u ||
        borrowed_count > base->constant_count ||
        borrowed_count > module->constant_count) {
        return;
    }
    inherited_borrowed_count =
        base->borrows_program_image != 0u
            ? base->constant_count
            : base->borrowed_constant_count;
    if (inherited_borrowed_count > borrowed_count) {
        return;
    }
    for (index = inherited_borrowed_count;
         index < borrowed_count;
         ++index) {
        if (constant_owns_bytes(base->constants[index].kind)) {
            base->constants[index].as.bytes = NULL;
        }
    }
    module->borrowed_constant_count =
        inherited_borrowed_count;
}

static PikaStatus materialize_incremental_instructions(
    const PikaCompiledModule* base,
    PikaCompiledModule* module) {
    uint32_t prefix_count = module->instruction_storage_base;
    uint32_t instruction_count =
        module->program.instruction_count;
    uint32_t suffix_count;
    uint32_t index;
    PikaInstruction* instructions;
    if (prefix_count == 0u) {
        module->program.instructions = module->instructions;
        return PIKA_STATUS_OK;
    }
    if (base == NULL ||
        prefix_count > base->program.instruction_count ||
        prefix_count > instruction_count) {
        return PIKA_STATUS_INTERNAL_ERROR;
    }
    suffix_count = instruction_count - prefix_count;
    if (suffix_count > module->instruction_capacity ||
        (suffix_count > 0u && module->instructions == NULL)) {
        return PIKA_STATUS_INTERNAL_ERROR;
    }
#if SIZE_MAX <= UINT32_MAX
    if (instruction_count >
        SIZE_MAX / sizeof(PikaInstruction)) {
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
#endif
    instructions = (PikaInstruction*)malloc(
        (size_t)instruction_count * sizeof(*instructions));
    if (instructions == NULL) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    for (index = 0u; index < prefix_count; ++index) {
        const PikaInstruction* source =
            pika_program_instruction_at(&base->program, index);
        if (source == NULL) {
            free(instructions);
            return PIKA_STATUS_INTERNAL_ERROR;
        }
        instructions[index] = *source;
    }
    if (suffix_count > 0u) {
        memcpy(
            &instructions[prefix_count],
            module->instructions,
            (size_t)suffix_count * sizeof(*instructions));
    }
    free(module->instructions);
    module->instructions = instructions;
    module->instruction_capacity = instruction_count;
    module->instruction_storage_base = 0u;
    module->program.instruction_prefix = NULL;
    module->program.instruction_prefix_count = 0u;
    module->program.instructions = instructions;
    return PIKA_STATUS_OK;
}

typedef struct {
    PikaCompiledModule* base;
    PikaInstruction* previous_entry;
    uint32_t storage_offset;
    uint32_t overwritten_count;
    uint32_t storage_capacity;
    uint8_t active;
} PikaInstructionTransfer;

static PikaStatus materialize_owned_incremental_instructions(
    PikaCompiledModule* base,
    PikaCompiledModule* module,
    PikaInstructionTransfer* transfer) {
    uint32_t prefix_count = module->instruction_storage_base;
    uint32_t previous_count;
    uint32_t instruction_count =
        module->program.instruction_count;
    uint32_t storage_base;
    uint32_t previous_storage_count;
    uint32_t prefix_storage_count;
    uint32_t requested_storage_count;
    uint32_t suffix_count;
    uint32_t storage_capacity;
    PikaInstruction* previous_entry = NULL;
    PikaInstruction* instructions;
    if (base == NULL || prefix_count == 0u) {
        return materialize_incremental_instructions(
            base, module);
    }
    if (base->borrows_program_image != 0u) {
        if (module->program.instruction_prefix == NULL ||
            module->program.instruction_prefix_count !=
                prefix_count) {
            return PIKA_STATUS_INTERNAL_ERROR;
        }
        module->instruction_storage_base = prefix_count;
        module->program.instructions = module->instructions;
        return PIKA_STATUS_OK;
    }
    previous_count = base->program.instruction_count;
    storage_base =
        base->program.instruction_prefix_count;
    if (prefix_count > previous_count ||
        prefix_count > instruction_count ||
        prefix_count < storage_base ||
        base->instruction_storage_base != storage_base) {
        return PIKA_STATUS_INTERNAL_ERROR;
    }
    previous_storage_count = previous_count - storage_base;
    prefix_storage_count = prefix_count - storage_base;
    requested_storage_count = instruction_count - storage_base;
    suffix_count = instruction_count - prefix_count;
    if (suffix_count > module->instruction_capacity ||
        (previous_storage_count > 0u &&
         base->instructions == NULL) ||
        (suffix_count > 0u && module->instructions == NULL)) {
        return PIKA_STATUS_INTERNAL_ERROR;
    }
    transfer->overwritten_count =
        suffix_count < previous_count - prefix_count
            ? suffix_count
            : previous_count - prefix_count;
    if (transfer->overwritten_count > 0u) {
        previous_entry = (PikaInstruction*)malloc(
            (size_t)transfer->overwritten_count *
            sizeof(*previous_entry));
        if (previous_entry == NULL) {
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
        memcpy(
            previous_entry,
            &base->instructions[prefix_storage_count],
            (size_t)transfer->overwritten_count *
                sizeof(*previous_entry));
    }
    storage_capacity =
        previous_storage_count > requested_storage_count
            ? previous_storage_count
            : requested_storage_count;
#if SIZE_MAX <= UINT32_MAX
    if (storage_capacity >
        SIZE_MAX / sizeof(PikaInstruction)) {
        free(previous_entry);
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
#endif
    instructions = base->instructions;
    if (base->instruction_capacity < storage_capacity) {
        PikaInstruction* grown =
            (PikaInstruction*)realloc(
                instructions,
                (size_t)storage_capacity *
                    sizeof(*instructions));
        if (grown == NULL) {
            free(previous_entry);
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
        instructions = grown;
    }
    base->instructions = instructions;
    base->instruction_capacity = storage_capacity;
    base->program.instructions = instructions;
    if (suffix_count > 0u) {
        memcpy(
            &instructions[prefix_storage_count],
            module->instructions,
            (size_t)suffix_count * sizeof(*instructions));
    }
    free(module->instructions);
    module->instructions = instructions;
    module->instruction_capacity = storage_capacity;
    module->instruction_storage_base = storage_base;
    module->program.instructions = instructions;
    base->instructions = NULL;
    base->instruction_capacity = 0u;
    base->program.instructions = NULL;
    transfer->base = base;
    transfer->previous_entry = previous_entry;
    transfer->storage_offset = prefix_storage_count;
    transfer->storage_capacity = storage_capacity;
    transfer->active = 1u;
    return PIKA_STATUS_OK;
}

static void rollback_instruction_transfer(
    PikaInstructionTransfer* transfer,
    PikaCompiledModule* module) {
    PikaCompiledModule* base;
    if (transfer->active == 0u) return;
    base = transfer->base;
    base->instructions = module->instructions;
    base->instruction_capacity =
        transfer->storage_capacity;
    base->program.instructions = base->instructions;
    if (transfer->overwritten_count > 0u) {
        memcpy(
            &base->instructions[transfer->storage_offset],
            transfer->previous_entry,
            (size_t)transfer->overwritten_count *
                sizeof(*transfer->previous_entry));
    }
    module->instructions = NULL;
    module->instruction_capacity = 0u;
    module->program.instructions = NULL;
    free(transfer->previous_entry);
    transfer->previous_entry = NULL;
    transfer->active = 0u;
}

static int commit_instruction_transfer(
    PikaInstructionTransfer* transfer) {
    int transferred = transfer->active != 0u;
    free(transfer->previous_entry);
    transfer->previous_entry = NULL;
    transfer->active = 0u;
    return transferred;
}

static PikaStatus compile_configured_mode(
    const char* source,
    size_t length,
    const PikaFrontendConfig* config,
    PikaModulePrepare seed,
    void* seed_context,
    PikaCompiledModule** module,
    PikaDiagnostic* diagnostic,
    int interactive) {
    PikaTokenBuffer tokens;
    PikaCompiledModule* compiled;
    PikaStatus status;
    if (module == NULL || source == NULL || config == NULL) {
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_INVALID_ARGUMENT, 1u, 1u);
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *module = NULL;
    status = validate_capabilities(config->enabled);
    if (status != PIKA_STATUS_OK) {
        pika_frontend_set_diagnostic(
            diagnostic, status, 1u, 1u);
        return status;
    }
    if ((config->enabled & PIKA_CAPABILITY_EXEC_MODULE) == 0u) {
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_UNSUPPORTED_CAPABILITY, 1u, 1u);
        return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
    }
    if (length > PIKA_FRONTEND_SOURCE_BYTE_LIMIT) {
        pika_frontend_set_limit_diagnostic(
            diagnostic, PIKA_FRONTEND_BOUND_SOURCE_BYTES,
            PIKA_FRONTEND_SOURCE_BYTE_LIMIT,
            length > UINT32_MAX ? UINT32_MAX : (uint32_t)length,
            1u, 1u, 0u);
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    status = pika_frontend_tokenize_allocated(
        source, length, &tokens, diagnostic);
    if (status != PIKA_STATUS_OK) return status;
    PIKA_FRONTEND_MEMORY_CHECKPOINT(
        "frontend-tokenized", tokens.count);
    if (tokens.count > (uint32_t)(INT32_MAX - 16) / 4u) {
        free(tokens.tokens);
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_FRONTEND_LIMIT, 1u, 1u);
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    compiled = (PikaCompiledModule*)calloc(
        1u, sizeof(PikaCompiledModule));
    if (compiled == NULL) {
        free(tokens.tokens);
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_FRONTEND_LIMIT, 1u, 1u);
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    compiled->semantic_name_capacity =
        semantic_name_storage_capacity(&tokens, 0u);
    if (compiled->semantic_name_capacity == 0u) {
        free(tokens.tokens);
        release_module(compiled);
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_FRONTEND_LIMIT, 1u, 1u);
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    compiled->instruction_capacity =
        initial_instruction_capacity(tokens.count);
    compiled->instructions = (PikaInstruction*)calloc(
        compiled->instruction_capacity,
        sizeof(*compiled->instructions));
    compiled->semantic_names = (char*)malloc(
        compiled->semantic_name_capacity);
    compiled->enabled_capabilities = config->enabled;
    compiled->bindings = config->bindings;
    status = pika_frontend_capture_binding_modules(
        compiled, config->bindings);
    if (status != PIKA_STATUS_OK) {
        free(tokens.tokens);
        release_module(compiled);
        pika_frontend_set_diagnostic(
            diagnostic, status, 1u, 1u);
        return status;
    }
    compiled->function_capacity = 1u + count_tokens(
        &tokens, PIKA_TOKEN_DEF, PIKA_FRONTEND_FUNCTION_LIMIT);
    compiled->function_capacity += count_tokens(
        &tokens, PIKA_TOKEN_LAMBDA, PIKA_FRONTEND_FUNCTION_LIMIT);
    compiled->functions = (PikaFunction*)calloc(
        compiled->function_capacity, sizeof(PikaFunction));
    compiled->class_capacity = count_tokens(
        &tokens, PIKA_TOKEN_CLASS, PIKA_FRONTEND_CLASS_LIMIT);
    if (compiled->class_capacity > 0u) {
        compiled->classes = (PikaClassLayout*)calloc(
            compiled->class_capacity, sizeof(PikaClassLayout));
    }
    if (compiled->instructions == NULL ||
        compiled->semantic_names == NULL ||
        compiled->functions == NULL ||
        (compiled->class_capacity > 0u &&
         compiled->classes == NULL)) {
        free(tokens.tokens);
        release_module(compiled);
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_STORAGE_TOO_SMALL, 1u, 1u);
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    PIKA_FRONTEND_MEMORY_CHECKPOINT(
        "frontend-emit-storage", tokens.count);
    memset(&compiled->program, 0, sizeof(compiled->program));
    if (seed != NULL) {
        status = seed(seed_context, compiled);
        if (status != PIKA_STATUS_OK) {
            free(tokens.tokens);
            release_module(compiled);
            pika_frontend_set_diagnostic(
                diagnostic, status, 1u, 1u);
            return status;
        }
    }
    PIKA_FRONTEND_MEMORY_CHECKPOINT(
        "frontend-parser-emit-start", tokens.count);
    status = pika_frontend_parse(
        source, tokens.tokens, tokens.count, config->enabled,
        config->bindings, interactive, compiled, diagnostic);
    PIKA_FRONTEND_MEMORY_CHECKPOINT(
        "frontend-parser-emit-finished", tokens.count);
    free(tokens.tokens);
    PIKA_FRONTEND_MEMORY_CHECKPOINT(
        "frontend-tokens-released", 0u);
    if (status == PIKA_STATUS_OK) {
        shrink_module_storage(compiled, 1);
        status = finalize_execution_requirements(compiled);
        if (status != PIKA_STATUS_OK) {
            pika_frontend_set_diagnostic(
                diagnostic, status, 1u, 1u);
        }
        PIKA_FRONTEND_MEMORY_CHECKPOINT(
            "frontend-program-committed",
            compiled->program.instruction_count);
    }
    if (status != PIKA_STATUS_OK) {
        release_module(compiled);
        return status;
    }
    *module = compiled;
    return PIKA_STATUS_OK;
}

PikaStatus pika_module_compile_configured(
    const char* source,
    size_t length,
    const PikaFrontendConfig* config,
    PikaCompiledModule** module,
    PikaDiagnostic* diagnostic) {
    return compile_configured_mode(
        source, length, config, NULL, NULL,
        module, diagnostic, 0);
}

PikaStatus pika_module_compile_seeded_configured(
    const char* source,
    size_t length,
    const PikaFrontendConfig* config,
    PikaModulePrepare seed,
    void* seed_context,
    PikaCompiledModule** module,
    PikaDiagnostic* diagnostic) {
    return compile_configured_mode(
        source, length, config, seed, seed_context,
        module, diagnostic, 0);
}

static PikaStatus compile_incremental_mode(
    const PikaCompiledModule* base,
    PikaCompiledModule* transferable_base,
    const char* source,
    size_t length,
    const PikaFrontendConfig* config,
    PikaModulePrepare prepare,
    void* prepare_context,
    PikaModuleCommitCheck commit_check,
    void* commit_context,
    PikaCompiledModule** module,
    PikaDiagnostic* diagnostic,
    int interactive) {
    PikaTokenBuffer tokens;
    PikaCompiledModule* compiled = NULL;
    PikaInstructionTransfer transfer;
    PikaStorageTransfer storage_transfer;
    uint32_t retained_instructions;
    PikaStatus status;
    memset(&transfer, 0, sizeof(transfer));
    memset(&storage_transfer, 0, sizeof(storage_transfer));
    if (module == NULL || source == NULL || config == NULL) {
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_INVALID_ARGUMENT, 1u, 1u);
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *module = NULL;
    if (transferable_base != NULL &&
        transferable_base != base) {
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_INVALID_ARGUMENT, 1u, 1u);
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (base == NULL) {
        status = compile_configured_mode(
            source, length, config, NULL, NULL,
            &compiled, diagnostic, interactive);
        if (status == PIKA_STATUS_OK && prepare != NULL) {
            status = prepare(prepare_context, compiled);
        }
        if (status == PIKA_STATUS_OK &&
            commit_check != NULL) {
            status = commit_check(
                commit_context, compiled);
        }
        if (status != PIKA_STATUS_OK) {
            if (compiled != NULL) {
                release_module(compiled);
            }
            if (diagnostic != NULL &&
                diagnostic->status == PIKA_STATUS_OK) {
                pika_frontend_set_diagnostic(
                    diagnostic, status, 1u, 1u);
            }
            return status;
        }
        *module = compiled;
        return PIKA_STATUS_OK;
    }
    status = validate_capabilities(config->enabled);
    if (status != PIKA_STATUS_OK ||
        config->enabled != base->enabled_capabilities ||
        config->bindings != base->bindings) {
        if (status == PIKA_STATUS_OK) {
            status = PIKA_STATUS_INVALID_ARGUMENT;
        }
        pika_frontend_set_diagnostic(
            diagnostic, status, 1u, 1u);
        return status;
    }
    if (length > PIKA_FRONTEND_SOURCE_BYTE_LIMIT ||
        length > UINT32_MAX - base->semantic_name_count) {
        pika_frontend_set_limit_diagnostic(
            diagnostic, PIKA_FRONTEND_BOUND_SOURCE_BYTES,
            PIKA_FRONTEND_SOURCE_BYTE_LIMIT,
            length > UINT32_MAX ? UINT32_MAX : (uint32_t)length,
            1u, 1u, 0u);
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    status = pika_frontend_tokenize_allocated(
        source, length, &tokens, diagnostic);
    if (status != PIKA_STATUS_OK) return status;
    PIKA_FRONTEND_MEMORY_CHECKPOINT(
        "frontend-incremental-tokenized", tokens.count);
    status = pika_program_verify(&base->program);
    retained_instructions =
        base->program.functions[
            base->program.entry_function]
            .first_instruction;
    if (status != PIKA_STATUS_OK ||
        tokens.count >
            (UINT32_MAX - retained_instructions - 16u) / 4u ||
        base->binding_call_count > UINT32_MAX - tokens.count ||
        base->builtin_call_count > UINT32_MAX - tokens.count ||
        base->constant_count > UINT32_MAX - tokens.count ||
        base->index_site_count > UINT32_MAX - tokens.count) {
        free(tokens.tokens);
        pika_frontend_set_diagnostic(
            diagnostic, status == PIKA_STATUS_OK
                            ? PIKA_STATUS_FRONTEND_LIMIT
                            : status,
            1u, 1u);
        return status == PIKA_STATUS_OK
                   ? PIKA_STATUS_FRONTEND_LIMIT
                   : status;
    }
    compiled = allocate_incremental_module(base, &tokens);
    if (compiled == NULL) {
        free(tokens.tokens);
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_FRONTEND_LIMIT, 1u, 1u);
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    PIKA_FRONTEND_MEMORY_CHECKPOINT(
        "frontend-incremental-storage", tokens.count);
    status = PIKA_STATUS_OK;
    if (transferable_base != NULL &&
        base->borrows_program_image == 0u) {
        status = begin_storage_transfer(
            transferable_base, compiled,
            &storage_transfer);
    }
    if (status == PIKA_STATUS_OK) {
        status = copy_incremental_state(
            base, compiled,
            transferable_base != NULL,
            storage_transfer.active != 0u);
    }
    if (status == PIKA_STATUS_OK) {
        PIKA_FRONTEND_MEMORY_CHECKPOINT(
            "frontend-incremental-parser-emit-start",
            tokens.count);
        status = pika_frontend_parse(
            source, tokens.tokens, tokens.count,
            config->enabled, config->bindings,
            interactive, compiled, diagnostic);
        PIKA_FRONTEND_MEMORY_CHECKPOINT(
            "frontend-incremental-parser-emit-finished",
            tokens.count);
    }
    free(tokens.tokens);
    PIKA_FRONTEND_MEMORY_CHECKPOINT(
        "frontend-incremental-tokens-released", 0u);
    if (status == PIKA_STATUS_OK && prepare != NULL) {
        status = prepare(prepare_context, compiled);
        if (status != PIKA_STATUS_OK) {
            pika_frontend_set_diagnostic(
                diagnostic, status, 1u, 1u);
        }
    }
    if (status == PIKA_STATUS_OK) {
        status = transferable_base == NULL
                     ? materialize_incremental_instructions(
                           base, compiled)
                     : materialize_owned_incremental_instructions(
                           transferable_base, compiled,
                           &transfer);
        if (status != PIKA_STATUS_OK) {
            pika_frontend_set_diagnostic(
                diagnostic, status, 1u, 1u);
        } else {
            PIKA_FRONTEND_MEMORY_CHECKPOINT(
                "frontend-incremental-instructions-materialized",
                compiled->program.instruction_count);
        }
    }
    if (status == PIKA_STATUS_OK) {
        status = finalize_execution_requirements(compiled);
        if (status != PIKA_STATUS_OK) {
            pika_frontend_set_diagnostic(
                diagnostic, status, 1u, 1u);
        }
    }
    if (status == PIKA_STATUS_OK) {
        shrink_module_storage(
            compiled, transfer.active == 0u);
        PIKA_FRONTEND_MEMORY_CHECKPOINT(
            "frontend-incremental-storage-shrunk",
            compiled->program.instruction_count);
    }
    if (status == PIKA_STATUS_OK &&
        commit_check != NULL) {
        status = commit_check(
            commit_context, compiled);
        if (status != PIKA_STATUS_OK) {
            pika_frontend_set_diagnostic(
                diagnostic, status, 1u, 1u);
        }
    }
    if (status == PIKA_STATUS_OK) {
        if (commit_instruction_transfer(&transfer)) {
            shrink_module_instructions(compiled);
        }
        commit_storage_transfer(&storage_transfer);
        commit_constant_transfer(transferable_base, compiled);
        PIKA_FRONTEND_MEMORY_CHECKPOINT(
            "frontend-incremental-committed",
            compiled->program.instruction_count);
    }
    if (status != PIKA_STATUS_OK) {
        rollback_instruction_transfer(
            &transfer, compiled);
        rollback_storage_transfer(
            &storage_transfer, compiled);
        if (diagnostic != NULL &&
            diagnostic->status == PIKA_STATUS_OK) {
            pika_frontend_set_diagnostic(
                diagnostic, status, 1u, 1u);
        }
        release_module(compiled);
        return status;
    }
    *module = compiled;
    return PIKA_STATUS_OK;
}

PikaStatus pika_module_compile_incremental(
    const PikaCompiledModule* base,
    const char* source,
    size_t length,
    const PikaFrontendConfig* config,
    PikaCompiledModule** module,
    PikaDiagnostic* diagnostic) {
    return compile_incremental_mode(
        base, NULL, source, length, config,
        NULL, NULL, NULL, NULL,
        module, diagnostic, 0);
}

PikaStatus pika_module_compile_incremental_prepared_interactive(
    const PikaCompiledModule* base,
    PikaCompiledModule* transferable_base,
    const char* source,
    size_t length,
    const PikaFrontendConfig* config,
    PikaModulePrepare prepare,
    void* prepare_context,
    PikaModuleCommitCheck commit_check,
    void* commit_context,
    PikaCompiledModule** module,
    PikaInteractiveCompileState* state,
    PikaDiagnostic* diagnostic) {
    PikaStatus status;
    if (state == NULL) {
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_INVALID_ARGUMENT, 1u, 1u);
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *state = PIKA_INTERACTIVE_COMPILE_INVALID;
    status = compile_incremental_mode(
        base, transferable_base, source, length, config,
        prepare, prepare_context,
        commit_check, commit_context,
        module, diagnostic, 1);
    if (status == PIKA_STATUS_OK) {
        *state = PIKA_INTERACTIVE_COMPILE_COMPLETE;
        return PIKA_STATUS_OK;
    }
    if (diagnostic != NULL &&
        diagnostic->reason ==
            PIKA_DIAGNOSTIC_REASON_INCOMPLETE_INPUT) {
        *state = PIKA_INTERACTIVE_COMPILE_INCOMPLETE;
        return PIKA_STATUS_OK;
    }
    return status;
}

PikaStatus pika_module_compile_incremental_interactive(
    const PikaCompiledModule* base,
    const char* source,
    size_t length,
    const PikaFrontendConfig* config,
    PikaCompiledModule** module,
    PikaInteractiveCompileState* state,
    PikaDiagnostic* diagnostic) {
    return pika_module_compile_incremental_prepared_interactive(
        base, NULL, source, length, config,
        NULL, NULL, NULL, NULL,
        module, state, diagnostic);
}

PikaStatus pika_module_compile_incremental_owned_interactive(
    PikaCompiledModule* base,
    const char* source,
    size_t length,
    const PikaFrontendConfig* config,
    PikaModuleCommitCheck commit_check,
    void* commit_context,
    PikaCompiledModule** module,
    PikaInteractiveCompileState* state,
    PikaDiagnostic* diagnostic) {
    return pika_module_compile_incremental_prepared_interactive(
        base, base, source, length, config,
        NULL, NULL, commit_check, commit_context,
        module, state, diagnostic);
}

PikaStatus pika_module_compile(const char* source,
                               size_t length,
                               PikaCompiledModule** module,
                               PikaDiagnostic* diagnostic) {
    const PikaFrontendConfig config = {
        PIKA_CAPABILITY_APPLICATION_DEFAULT,
        NULL,
    };
    return pika_module_compile_configured(
        source, length, &config, module, diagnostic);
}

PikaStatus pika_compiled_module_release_interactive_entry(
    PikaCompiledModule* module) {
    PikaFunction* entry;
    uint32_t entry_storage_index;
    uint32_t index;
    uint32_t entry_slots;
    if (module == NULL ||
        module->borrows_program_image != 0u ||
        module->functions == NULL ||
        module->program.entry_function >=
            module->program.function_count ||
        module->persistent_binding_call_count >
            module->binding_call_count ||
        module->persistent_builtin_call_count >
            module->builtin_call_count ||
        module->persistent_dynamic_method_call_count >
            module->dynamic_method_call_count ||
        module->persistent_dynamic_method_candidate_count >
            module->dynamic_method_candidate_count ||
        module->persistent_constant_count >
            module->constant_count ||
        module->persistent_constant_count <
            module->borrowed_constant_count ||
        module->persistent_index_site_count >
            module->index_site_count ||
        module->persistent_exception_handler_count >
            module->exception_handler_count) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    entry =
        &module->functions[module->program.entry_function];
    if (entry->first_instruction <
            module->instruction_storage_base ||
        entry->first_instruction >=
            module->program.instruction_count) {
        return PIKA_STATUS_INTERNAL_ERROR;
    }
    entry_storage_index =
        entry->first_instruction -
        module->instruction_storage_base;
    if (module->instructions == NULL ||
        entry_storage_index >= module->instruction_capacity ||
        module->semantic_global_count > UINT8_MAX) {
        return PIKA_STATUS_INTERNAL_ERROR;
    }

    for (index = module->persistent_constant_count;
         index < module->constant_count;
         ++index) {
        if (index < module->borrowed_constant_count) {
            continue;
        }
        if (constant_owns_bytes(module->constants[index].kind)) {
            free((void*)module->constants[index].as.bytes);
        }
    }

    module->binding_call_count =
        module->persistent_binding_call_count;
    module->builtin_call_count =
        module->persistent_builtin_call_count;
    module->dynamic_method_call_count =
        module->persistent_dynamic_method_call_count;
    module->dynamic_method_candidate_count =
        module->persistent_dynamic_method_candidate_count;
    module->constant_count =
        module->persistent_constant_count;
    module->index_site_count =
        module->persistent_index_site_count;
    module->exception_handler_count =
        module->persistent_exception_handler_count;
    module->program.binding_call_count =
        (uint16_t)module->binding_call_count;
    module->program.builtin_call_count =
        (uint16_t)module->builtin_call_count;
    module->program.dynamic_method_call_count =
        (uint16_t)module->dynamic_method_call_count;
    module->program.dynamic_method_candidate_count =
        (uint16_t)module->dynamic_method_candidate_count;
    module->program.constant_count =
        (uint16_t)module->constant_count;
    module->program.index_site_count =
        (uint16_t)module->index_site_count;
    module->program.exception_handler_count =
        (uint16_t)module->exception_handler_count;

    module->instructions[entry_storage_index] =
        (PikaInstruction){
            PIKA_OP_RETURN, 0u, 0u, 0u, 0
        };
    module->program.instruction_count =
        entry->first_instruction + 1u;
    entry_slots = module->semantic_global_count;
    if (entry_slots == 0u) entry_slots = 1u;
    entry->instruction_count = 1u;
    entry->slot_count = (uint16_t)entry_slots;
    entry->parameter_count = 0u;
    entry->reserved = 0u;
    module->has_interactive_result = 0u;

    shrink_module_instructions(module);
    module->binding_calls =
        (PikaBindingCallSite*)shrink_array(
            module->binding_calls,
            module->binding_call_count,
            sizeof(*module->binding_calls));
    module->binding_call_capacity =
        module->binding_call_count;
    module->builtin_calls =
        (PikaBuiltinCallSite*)shrink_array(
            module->builtin_calls,
            module->builtin_call_count,
            sizeof(*module->builtin_calls));
    module->builtin_call_capacity =
        module->builtin_call_count;
    module->dynamic_method_calls =
        (PikaDynamicMethodCallSite*)shrink_array(
            module->dynamic_method_calls,
            module->dynamic_method_call_count,
            sizeof(*module->dynamic_method_calls));
    module->dynamic_method_call_capacity =
        module->dynamic_method_call_count;
    module->dynamic_method_candidates =
        (PikaDynamicMethodCandidate*)shrink_array(
            module->dynamic_method_candidates,
            module->dynamic_method_candidate_count,
            sizeof(*module->dynamic_method_candidates));
    module->dynamic_method_candidate_capacity =
        module->dynamic_method_candidate_count;
    module->constants = (PikaConstant*)shrink_array(
        module->constants, module->constant_count,
        sizeof(*module->constants));
    module->constant_capacity = module->constant_count;
    module->index_sites = (PikaIndexSite*)shrink_array(
        module->index_sites, module->index_site_count,
        sizeof(*module->index_sites));
    module->index_site_capacity = module->index_site_count;
    module->exception_handlers =
        (PikaExceptionHandler*)shrink_array(
            module->exception_handlers,
            module->exception_handler_count,
            sizeof(*module->exception_handlers));
    module->exception_handler_capacity =
        module->exception_handler_count;
    module->program.instructions = module->instructions;
    module->program.binding_calls = module->binding_calls;
    module->program.builtin_calls = module->builtin_calls;
    module->program.constants = module->constants;
    module->program.index_sites = module->index_sites;
    module->program.exception_handlers =
        module->exception_handlers;
    PIKA_FRONTEND_MEMORY_CHECKPOINT(
        "frontend-interactive-entry-released",
        module->program.instruction_count);
    return PIKA_STATUS_OK;
}

const PikaProgram* pika_compiled_module_program(
    const PikaCompiledModule* module) {
    return module == NULL ? NULL : &module->program;
}

uint16_t pika_compiled_module_global_count(
    const PikaCompiledModule* module) {
    return module == NULL
               ? 0u
               : module->semantic_global_count;
}

int pika_compiled_module_has_interactive_result(
    const PikaCompiledModule* module) {
    return module != NULL &&
           module->has_interactive_result != 0u;
}

void pika_compiled_module_destroy(PikaCompiledModule* module) {
    release_module(module);
}
