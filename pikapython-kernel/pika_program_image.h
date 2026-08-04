/* SPEC: PJ2026-050111 可移植包 v0.10; module Program Image. */
#ifndef PIKA_PROGRAM_IMAGE_H
#define PIKA_PROGRAM_IMAGE_H

#include <stdint.h>

#include "frontend_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PIKA_PROGRAM_IMAGE_MAGIC UINT32_C(0x50494b41)
#define PIKA_PROGRAM_IMAGE_SCHEMA_VERSION 4u
#define PIKA_PROGRAM_IMAGE_DIGEST_LENGTH 32u

typedef struct {
    const char* name;
    uint16_t name_length;
    uint16_t reserved;
} PikaProgramImageBindingImport;

typedef struct {
    const char* name;
    uint16_t name_length;
    uint8_t module_index;
    uint8_t reserved;
    uint32_t entry_function;
    const uint8_t* dependencies;
    uint16_t dependency_count;
    uint16_t reserved_count;
    const PikaProgramImageBindingImport* binding_imports;
    uint16_t binding_import_count;
    uint16_t source_digest_length;
    const uint8_t* source_digest;
} PikaProgramImageModule;

typedef enum {
    PIKA_PROGRAM_RELOCATE_BINDING_CALL = 0,
    PIKA_PROGRAM_RELOCATE_FUNCTION_RETURN = 1,
    PIKA_PROGRAM_RELOCATE_SHAPE = 2,
    PIKA_PROGRAM_RELOCATE_FIELD = 3,
    PIKA_PROGRAM_RELOCATE_GLOBAL = 4,
    PIKA_PROGRAM_RELOCATE_IMPORT = 5,
} PikaProgramRelocationKind;

typedef struct {
    uint8_t kind;
    uint8_t reserved[3];
    uint32_t index;
    const char* module_name;
    const char* owner_class_name;
    const char* callable_name;
    const char* result_class_name;
} PikaProgramRelocation;

typedef struct PikaProgramImage {
    uint32_t magic;
    uint32_t schema_version;
    PikaCapability enabled_capabilities;
    const PikaProgramImageModule* modules;
    uint16_t module_count;
    uint16_t entry_module;
    const uint8_t* configuration_digest;
    uint16_t configuration_digest_length;
    const uint8_t* capability_digest;
    uint16_t capability_digest_length;
    PikaProgram program;
    uint32_t object_slot_estimate;
    uint32_t object_byte_estimate;
    const char* semantic_names;
    uint32_t semantic_name_count;
    const PikaSemanticFunction* semantic_functions;
    uint16_t semantic_function_count;
    const PikaSemanticParameter* semantic_parameters;
    uint16_t semantic_parameter_count;
    const PikaStaticShape* semantic_shapes;
    uint32_t semantic_shape_count;
    const PikaSemanticClass* semantic_classes;
    uint16_t semantic_class_count;
    const PikaSemanticField* semantic_fields;
    uint16_t semantic_field_count;
    const PikaSemanticGlobal* semantic_globals;
    uint16_t semantic_global_count;
    const PikaSemanticImport* semantic_imports;
    uint16_t semantic_import_count;
    const PikaPythonModule* semantic_python_imports;
    uint16_t semantic_python_import_count;
    const PikaProgramRelocation* relocations;
    uint32_t relocation_count;
    uint8_t has_interactive_result;
    uint8_t reserved[3];
} PikaProgramImage;

#if PIKA_FROZEN_PROGRAM_ENABLE
PikaStatus pika_compiled_module_from_image(
    const PikaProgramImage* image,
    PikaCapability enabled_capabilities,
    const PikaBindingRegistry* bindings,
    PikaCompiledModule** module);
#endif

#ifdef __cplusplus
}
#endif

#endif
