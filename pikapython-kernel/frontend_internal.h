/* SPEC: PJ2026-0501 V2 kernel v0.47; frontend internals. */
#ifndef PIKA_FRONTEND_INTERNAL_H
#define PIKA_FRONTEND_INTERNAL_H

#include "pika_frontend.h"
#include "pika_builtin.h"
#include "pika_kernel.h"

#if PIKA_FRONTEND_MEMORY_PROBE_ENABLE
void pika_frontend_memory_probe_checkpoint(const char* phase,
                                           uint32_t index);
#define PIKA_FRONTEND_MEMORY_CHECKPOINT(phase, index) \
    pika_frontend_memory_probe_checkpoint((phase), (index))
#else
#define PIKA_FRONTEND_MEMORY_CHECKPOINT(phase, index) ((void)0)
#endif

typedef enum {
    PIKA_VALUE_INTEGER = 0,
    PIKA_VALUE_BOOLEAN = 1,
    PIKA_VALUE_BINDING = 2,
    PIKA_VALUE_NATIVE_OBJECT = 3,
    PIKA_VALUE_NONE = 4,
    PIKA_VALUE_FLOAT = 5,
    PIKA_VALUE_STRING = 6,
    PIKA_VALUE_BYTES = 7,
    PIKA_VALUE_BYTEARRAY = 8,
    PIKA_VALUE_LIST = 9,
    PIKA_VALUE_TUPLE = 10,
    PIKA_VALUE_DICT = 11,
    PIKA_VALUE_SET = 12,
    PIKA_VALUE_RANGE = 13,
    PIKA_VALUE_INSTANCE = 14,
    PIKA_VALUE_UNKNOWN = 15,
    PIKA_VALUE_CALLABLE = 16,
    PIKA_VALUE_TYPE = 17,
    PIKA_VALUE_EXCEPTION = 18,
    PIKA_VALUE_MODULE = 19,
} PikaValueKind;

typedef struct {
    const PikaBindingClass* binding_class;
    uint16_t class_index;
    uint8_t kind;
    uint8_t binding_kind;
} PikaStaticShape;

typedef struct {
    uint32_t name_offset;
    uint16_t name_length;
    uint16_t first_parameter;
    uint8_t parameter_count;
    uint8_t required_parameter_count;
    uint16_t owner_class;
    PikaStaticShape return_shape;
    uint32_t first_return_element_shape;
    uint16_t return_element_shape_count;
    uint8_t has_return_shape;
    uint8_t variadic_parameter;
} PikaSemanticFunction;

typedef struct {
    uint32_t name_offset;
    uint16_t name_length;
    uint16_t default_constant;
} PikaSemanticParameter;

typedef struct {
    uint32_t name_offset;
    uint16_t name_length;
    uint8_t kind;
    uint8_t binding_kind;
    uint8_t allows_none;
    const PikaBindingClass* binding_class;
    uint16_t class_index;
} PikaSemanticField;

typedef struct {
    uint32_t name_offset;
    uint16_t name_length;
    uint16_t base_class;
    uint16_t first_field;
    uint16_t field_count;
    uint16_t member_count;
} PikaSemanticClass;

typedef struct {
    uint32_t name_offset;
    uint16_t name_length;
    uint8_t slot;
    uint8_t kind;
    uint8_t binding_kind;
    uint8_t reserved;
    const PikaBindingClass* binding_class;
    uint16_t class_index;
    uint32_t first_element_shape;
    uint16_t element_shape_count;
} PikaSemanticGlobal;

typedef struct {
    uint32_t name_offset;
    uint16_t name_length;
    uint8_t selected;
    uint8_t reserved;
    const PikaBindingModule* module;
} PikaSemanticImport;

typedef struct {
    uint32_t name_offset;
    uint16_t name_length;
    uint16_t entry_function;
    uint32_t first_dependency;
    uint16_t dependency_count;
    uint16_t first_binding_import;
    uint16_t binding_import_count;
    uint8_t module_index;
    uint8_t reserved;
} PikaSemanticProgramModule;

typedef struct {
    uint32_t name_offset;
    uint16_t name_length;
    uint8_t module_index;
    uint8_t reserved;
} PikaSemanticProgramBindingImport;

typedef struct {
    uint8_t slot;
    PikaValueKind kind;
    PikaBindingValueKind binding_kind;
    const PikaBindingClass* binding_class;
    uint16_t class_index;
    uint32_t first_element_shape;
    uint16_t element_shape_count;
} PikaExpression;

typedef struct {
    const char* name_source;
    uint32_t offset;
    uint32_t length;
    uint8_t slot;
    PikaValueKind kind;
    PikaBindingValueKind binding_kind;
    const PikaBindingClass* binding_class;
    uint16_t class_index;
    uint32_t first_element_shape;
    uint16_t element_shape_count;
    uint8_t is_global;
    uint8_t global_slot;
} PikaSymbol;

struct PikaCompiledModule {
    PikaInstruction* instructions;
    uint32_t instruction_capacity;
    uint32_t instruction_storage_base;
    PikaFunction* functions;
    uint32_t function_capacity;
    PikaClassLayout* classes;
    uint32_t class_capacity;
    uint32_t class_count;
    PikaBindingCallSite* binding_calls;
    uint32_t binding_call_capacity;
    uint32_t binding_call_count;
    PikaBuiltinCallSite* builtin_calls;
    uint32_t builtin_call_capacity;
    uint32_t builtin_call_count;
    PikaDynamicMethodCallSite* dynamic_method_calls;
    uint32_t dynamic_method_call_capacity;
    uint32_t dynamic_method_call_count;
    PikaDynamicMethodCandidate* dynamic_method_candidates;
    uint32_t dynamic_method_candidate_capacity;
    uint32_t dynamic_method_candidate_count;
    PikaConstant* constants;
    uint32_t constant_capacity;
    uint32_t constant_count;
    PikaIndexSite* index_sites;
    uint32_t index_site_capacity;
    uint32_t index_site_count;
    PikaExceptionHandler* exception_handlers;
    uint32_t exception_handler_capacity;
    uint32_t exception_handler_count;
    uint32_t object_slot_estimate;
    uint32_t object_byte_estimate;
    char* semantic_names;
    uint32_t semantic_name_capacity;
    uint32_t semantic_name_count;
    PikaSemanticFunction* semantic_functions;
    uint32_t semantic_function_capacity;
    uint16_t semantic_function_count;
    PikaSemanticParameter* semantic_parameters;
    uint16_t semantic_parameter_capacity;
    uint16_t semantic_parameter_count;
    PikaStaticShape* semantic_shapes;
    uint32_t semantic_shape_capacity;
    uint32_t semantic_shape_count;
    PikaSemanticClass* semantic_classes;
    uint32_t semantic_class_capacity;
    uint16_t semantic_class_count;
    PikaSemanticField* semantic_fields;
    uint32_t semantic_field_capacity;
    uint16_t semantic_field_count;
    PikaSemanticGlobal* semantic_globals;
    uint32_t semantic_global_capacity;
    uint16_t semantic_global_count;
    PikaSemanticImport* semantic_imports;
    uint32_t semantic_import_capacity;
    uint16_t semantic_import_count;
    PikaPythonModule* semantic_python_imports;
    uint32_t semantic_python_import_capacity;
    uint16_t semantic_python_import_count;
    PikaSemanticProgramModule* program_modules;
    uint16_t program_module_capacity;
    uint16_t program_module_count;
    uint8_t* program_module_dependencies;
    uint32_t program_module_dependency_capacity;
    uint32_t program_module_dependency_count;
    PikaSemanticProgramBindingImport* program_binding_imports;
    uint32_t program_binding_import_capacity;
    uint32_t program_binding_import_count;
    const PikaBindingModule** available_binding_modules;
    uint16_t available_binding_module_count;
    uint8_t program_entry_module;
    PikaCapability enabled_capabilities;
    const PikaBindingRegistry* bindings;
    uint8_t has_interactive_result;
    uint8_t borrows_program_image;
    uint32_t borrowed_constant_count;
    uint32_t persistent_binding_call_count;
    uint32_t persistent_builtin_call_count;
    uint32_t persistent_dynamic_method_call_count;
    uint32_t persistent_dynamic_method_candidate_count;
    uint32_t persistent_constant_count;
    uint32_t persistent_index_site_count;
    uint32_t persistent_exception_handler_count;
    PikaProgram program;
};

PikaStatus pika_frontend_capture_binding_modules(
    PikaCompiledModule* module,
    const PikaBindingRegistry* bindings);

PikaStatus pika_frontend_tokenize_allocated(
    const char* source,
    size_t length,
    PikaTokenBuffer* buffer,
    PikaDiagnostic* diagnostic);

void pika_frontend_set_diagnostic(PikaDiagnostic* diagnostic,
                                  PikaStatus status,
                                  uint32_t line,
                                  uint32_t column);
void pika_frontend_source_location(const char* source,
                                   uint32_t offset,
                                   uint32_t* line,
                                   uint32_t* column);
void pika_frontend_set_reason_diagnostic(
    PikaDiagnostic* diagnostic,
    PikaStatus status,
    PikaDiagnosticReason reason,
    uint32_t line,
    uint32_t column,
    uint32_t offset);
void pika_frontend_set_limit_diagnostic(
    PikaDiagnostic* diagnostic,
    PikaFrontendBound bound,
    uint32_t configured,
    uint32_t observed,
    uint32_t line,
    uint32_t column,
    uint32_t offset);
PikaStatus pika_frontend_parse(const char* source,
                               const PikaToken* tokens,
                               uint32_t token_count,
                               PikaCapability enabled_capabilities,
                               const PikaBindingRegistry* bindings,
                               int interactive,
                               PikaCompiledModule* module,
                               PikaDiagnostic* diagnostic);

typedef PikaStatus (*PikaModulePrepare)(void* context,
                                        PikaCompiledModule* module);

PikaStatus pika_module_compile_seeded_configured(
    const char* source,
    size_t length,
    const PikaFrontendConfig* config,
    PikaModulePrepare seed,
    void* seed_context,
    PikaCompiledModule** module,
    PikaDiagnostic* diagnostic);

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
    PikaDiagnostic* diagnostic);

#endif
