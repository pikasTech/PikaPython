/* SPEC: PJ2026-050111 可移植包 v0.10; module image loader. */
#include "pika_program_image.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if PIKA_FROZEN_PROGRAM_ENABLE

static int counted_pointer_valid(const void* pointer, uint32_t count) {
    return count == 0u || pointer != NULL;
}

static int range_valid(uint32_t first,
                       uint32_t count,
                       uint32_t total) {
    return first <= total && count <= total - first;
}

static int semantic_name_valid(const PikaProgramImage* image,
                               uint32_t offset,
                               uint16_t length) {
    return image->semantic_names != NULL &&
           range_valid(
               offset, (uint32_t)length,
               image->semantic_name_count);
}

static const PikaProgramImageModule* find_program_module(
    const PikaProgramImage* image,
    uint8_t module_index) {
    uint16_t index;
    for (index = 0u; index < image->module_count; ++index) {
        if (image->modules[index].module_index == module_index) {
            return &image->modules[index];
        }
    }
    return NULL;
}

static PikaStatus validate_program_modules(
    const PikaProgramImage* image) {
    const PikaProgramImageModule* entry;
    uint16_t index;
    if (image->modules == NULL || image->module_count == 0u ||
        image->module_count > UINT8_MAX + 1u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (image->configuration_digest == NULL ||
        image->configuration_digest_length !=
            PIKA_PROGRAM_IMAGE_DIGEST_LENGTH ||
        image->capability_digest == NULL ||
        image->capability_digest_length !=
            PIKA_PROGRAM_IMAGE_DIGEST_LENGTH) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    entry = find_program_module(image, (uint8_t)image->entry_module);
    if (image->entry_module > UINT8_MAX || entry == NULL ||
        entry->name == NULL ||
        entry->name_length != 8u ||
        memcmp(entry->name, "__main__", 8u) != 0) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    for (index = 0u; index < image->module_count; ++index) {
        const PikaProgramImageModule* module = &image->modules[index];
        uint16_t other;
        uint16_t dependency;
        if (module->name == NULL || module->name_length == 0u ||
            module->reserved != 0u ||
            module->reserved_count != 0u ||
            module->source_digest == NULL ||
            module->source_digest_length !=
                PIKA_PROGRAM_IMAGE_DIGEST_LENGTH ||
            module->entry_function >= image->program.function_count ||
            !counted_pointer_valid(
                module->dependencies, module->dependency_count) ||
            !counted_pointer_valid(
                module->binding_imports,
                module->binding_import_count)) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        for (other = 0u; other < index; ++other) {
            const PikaProgramImageModule* previous =
                &image->modules[other];
            if (previous->module_index == module->module_index ||
                (previous->name_length == module->name_length &&
                 memcmp(
                     previous->name, module->name,
                     module->name_length) == 0)) {
                return PIKA_STATUS_MODULE_CONFLICT;
            }
        }
        for (dependency = 0u;
             dependency < module->dependency_count;
             ++dependency) {
            uint16_t previous_dependency;
            if (find_program_module(
                    image, module->dependencies[dependency]) == NULL) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            for (previous_dependency = 0u;
                 previous_dependency < dependency;
                 ++previous_dependency) {
                if (module->dependencies[previous_dependency] ==
                    module->dependencies[dependency]) {
                    return PIKA_STATUS_INVALID_OPERAND;
                }
            }
        }
        for (dependency = 0u;
             dependency < module->binding_import_count;
             ++dependency) {
            const PikaProgramImageBindingImport* imported =
                &module->binding_imports[dependency];
            uint16_t previous_import;
            if (imported->name == NULL ||
                imported->name_length == 0u ||
                imported->reserved != 0u) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            for (previous_import = 0u;
                 previous_import < dependency;
                 ++previous_import) {
                const PikaProgramImageBindingImport* previous =
                    &module->binding_imports[previous_import];
                if (previous->name_length == imported->name_length &&
                    memcmp(
                        previous->name, imported->name,
                        imported->name_length) == 0) {
                    return PIKA_STATUS_INVALID_OPERAND;
                }
            }
        }
    }
    return PIKA_STATUS_OK;
}

static int find_semantic_name_offset(
    const PikaProgramImage* image,
    const char* name,
    uint16_t length,
    uint32_t* offset) {
    uint32_t index;
    if (image->semantic_names == NULL || name == NULL ||
        length == 0u || length > image->semantic_name_count) {
        return 0;
    }
    for (index = 0u;
         index <= image->semantic_name_count - length;
         ++index) {
        if (memcmp(
                &image->semantic_names[index], name, length) == 0) {
            *offset = index;
            return 1;
        }
    }
    return 0;
}

static PikaStatus copy_program_metadata(
    const PikaProgramImage* image,
    PikaCompiledModule* module) {
    uint32_t dependency_count = 0u;
    uint32_t binding_count = 0u;
    uint16_t index;
    for (index = 0u; index < image->module_count; ++index) {
        if (image->modules[index].dependency_count >
                UINT32_MAX - dependency_count ||
            image->modules[index].binding_import_count >
                UINT32_MAX - binding_count) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
        dependency_count += image->modules[index].dependency_count;
        binding_count += image->modules[index].binding_import_count;
    }
    if (binding_count > UINT16_MAX) {
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    module->program_modules = (PikaSemanticProgramModule*)calloc(
        image->module_count, sizeof(*module->program_modules));
    if (dependency_count > 0u) {
        module->program_module_dependencies = (uint8_t*)malloc(
            dependency_count *
            sizeof(*module->program_module_dependencies));
    }
    if (binding_count > 0u) {
        module->program_binding_imports =
            (PikaSemanticProgramBindingImport*)calloc(
                binding_count,
                sizeof(*module->program_binding_imports));
    }
    if (module->program_modules == NULL ||
        (dependency_count > 0u &&
         module->program_module_dependencies == NULL) ||
        (binding_count > 0u &&
         module->program_binding_imports == NULL)) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    module->program_module_capacity = image->module_count;
    module->program_module_dependency_capacity = dependency_count;
    module->program_binding_import_capacity = binding_count;
    module->program_entry_module = (uint8_t)image->entry_module;
    for (index = 0u; index < image->module_count; ++index) {
        const PikaProgramImageModule* source =
            &image->modules[index];
        PikaSemanticProgramModule* target =
            &module->program_modules[index];
        uint16_t binding;
        if (!find_semantic_name_offset(
                image, source->name, source->name_length,
                &target->name_offset)) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        target->name_length = source->name_length;
        target->entry_function = source->entry_function;
        target->first_dependency =
            module->program_module_dependency_count;
        target->dependency_count = source->dependency_count;
        target->first_binding_import =
            (uint16_t)module->program_binding_import_count;
        target->binding_import_count = source->binding_import_count;
        target->module_index = source->module_index;
        if (source->dependency_count > 0u) {
            memcpy(
                &module->program_module_dependencies[
                    module->program_module_dependency_count],
                source->dependencies,
                source->dependency_count *
                    sizeof(*source->dependencies));
            module->program_module_dependency_count +=
                source->dependency_count;
        }
        for (binding = 0u;
             binding < source->binding_import_count;
             ++binding) {
            const PikaProgramImageBindingImport* imported =
                &source->binding_imports[binding];
            PikaSemanticProgramBindingImport* retained =
                &module->program_binding_imports[
                    module->program_binding_import_count++];
            if (!find_semantic_name_offset(
                    image, imported->name,
                    imported->name_length,
                    &retained->name_offset)) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            retained->name_length = imported->name_length;
            retained->module_index = source->module_index;
        }
        ++module->program_module_count;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus validate_semantic_tables(
    const PikaProgramImage* image) {
    uint32_t index;
    if (!counted_pointer_valid(
            image->semantic_names,
            image->semantic_name_count) ||
        !counted_pointer_valid(
            image->semantic_functions,
            image->semantic_function_count) ||
        !counted_pointer_valid(
            image->semantic_parameters,
            image->semantic_parameter_count) ||
        !counted_pointer_valid(
            image->semantic_shapes,
            image->semantic_shape_count) ||
        !counted_pointer_valid(
            image->semantic_classes,
            image->semantic_class_count) ||
        !counted_pointer_valid(
            image->semantic_fields,
            image->semantic_field_count) ||
        !counted_pointer_valid(
            image->semantic_globals,
            image->semantic_global_count) ||
        !counted_pointer_valid(
            image->semantic_imports,
            image->semantic_import_count) ||
        !counted_pointer_valid(
            image->semantic_python_imports,
            image->semantic_python_import_count) ||
        !counted_pointer_valid(
            image->relocations, image->relocation_count) ||
        image->semantic_function_count >
            image->program.function_count ||
        image->semantic_class_count > image->program.class_count) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0u;
         index < image->semantic_function_count;
         ++index) {
        const PikaSemanticFunction* function =
            &image->semantic_functions[index];
        if (!semantic_name_valid(
                image, function->name_offset,
                function->name_length) ||
            !range_valid(
                function->first_parameter,
                function->parameter_count,
                image->semantic_parameter_count) ||
            !range_valid(
                function->first_return_element_shape,
                function->return_element_shape_count,
                image->semantic_shape_count) ||
            function->required_parameter_count >
                function->parameter_count ||
            function->has_return_shape > 1u) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
    }
    for (index = 0u;
         index < image->semantic_parameter_count;
         ++index) {
        const PikaSemanticParameter* parameter =
            &image->semantic_parameters[index];
        if (!semantic_name_valid(
                image, parameter->name_offset,
                parameter->name_length) ||
            (parameter->default_constant != UINT16_MAX &&
             parameter->default_constant >=
                 image->program.constant_count)) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
    }
    for (index = 0u;
         index < image->semantic_class_count;
         ++index) {
        const PikaSemanticClass* binding_class =
            &image->semantic_classes[index];
        if (!semantic_name_valid(
                image, binding_class->name_offset,
                binding_class->name_length) ||
            !range_valid(
                binding_class->first_field,
                binding_class->field_count,
                image->semantic_field_count)) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
    }
    for (index = 0u;
         index < image->semantic_field_count;
         ++index) {
        const PikaSemanticField* field =
            &image->semantic_fields[index];
        if (!semantic_name_valid(
                image, field->name_offset,
                field->name_length)) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
    }
    for (index = 0u;
         index < image->semantic_global_count;
         ++index) {
        const PikaSemanticGlobal* global =
            &image->semantic_globals[index];
        if (!semantic_name_valid(
                image, global->name_offset,
                global->name_length) ||
            global->slot != index ||
            !range_valid(
                global->first_element_shape,
                global->element_shape_count,
                image->semantic_shape_count)) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
    }
    for (index = 0u;
         index < image->semantic_import_count;
         ++index) {
        const PikaSemanticImport* import_value =
            &image->semantic_imports[index];
        if (!semantic_name_valid(
                image, import_value->name_offset,
                import_value->name_length)) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
    }
    for (index = 0u;
         index < image->semantic_python_import_count;
         ++index) {
        const PikaPythonModule* import_value =
            &image->semantic_python_imports[index];
        if (!semantic_name_valid(
                image, import_value->name_offset,
                import_value->name_length) ||
            (import_value->flags &
             (uint8_t)~PIKA_PYTHON_MODULE_PUBLIC) != 0u ||
            import_value->target_module == 0u ||
            find_program_module(
                image, import_value->target_module) == NULL) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
    }
    return PIKA_STATUS_OK;
}

static PikaStatus validate_relocations(
    const PikaProgramImage* image) {
    uint32_t index;
    for (index = 0u; index < image->relocation_count; ++index) {
        const PikaProgramRelocation* relocation =
            &image->relocations[index];
        uint32_t limit;
        if (relocation->reserved[0] != 0u ||
            relocation->reserved[1] != 0u ||
            relocation->reserved[2] != 0u ||
            relocation->module_name == NULL) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        switch ((PikaProgramRelocationKind)relocation->kind) {
            case PIKA_PROGRAM_RELOCATE_BINDING_CALL:
                limit = image->program.binding_call_count;
                if (relocation->callable_name == NULL) {
                    return PIKA_STATUS_INVALID_OPERAND;
                }
                break;
            case PIKA_PROGRAM_RELOCATE_FUNCTION_RETURN:
                limit = image->semantic_function_count;
                if (relocation->result_class_name == NULL) {
                    return PIKA_STATUS_INVALID_OPERAND;
                }
                break;
            case PIKA_PROGRAM_RELOCATE_SHAPE:
                limit = image->semantic_shape_count;
                if (relocation->result_class_name == NULL) {
                    return PIKA_STATUS_INVALID_OPERAND;
                }
                break;
            case PIKA_PROGRAM_RELOCATE_FIELD:
                limit = image->semantic_field_count;
                if (relocation->result_class_name == NULL) {
                    return PIKA_STATUS_INVALID_OPERAND;
                }
                break;
            case PIKA_PROGRAM_RELOCATE_GLOBAL:
                limit = image->semantic_global_count;
                if (relocation->result_class_name == NULL) {
                    return PIKA_STATUS_INVALID_OPERAND;
                }
                break;
            case PIKA_PROGRAM_RELOCATE_IMPORT:
                limit = image->semantic_import_count;
                break;
            default:
                return PIKA_STATUS_INVALID_OPERAND;
        }
        if (relocation->index >= limit) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
    }
    return PIKA_STATUS_OK;
}

static void* copy_table(const void* source,
                        uint32_t count,
                        size_t item_size) {
    void* result;
    if (count == 0u) return NULL;
    if (source == NULL || item_size == 0u ||
        (size_t)count > SIZE_MAX / item_size) {
        return NULL;
    }
    result = malloc((size_t)count * item_size);
    if (result != NULL) {
        memcpy(result, source, (size_t)count * item_size);
    }
    return result;
}

static PikaStatus copy_image_tables(
    const PikaProgramImage* image,
    const PikaBindingRegistry* bindings,
    PikaCompiledModule* module) {
    uint32_t index;
    PikaStatus status;
    module->program = image->program;
    module->instructions =
        (PikaInstruction*)image->program.instructions;
    module->instruction_capacity =
        image->program.instruction_count;
    module->functions =
        (PikaFunction*)image->program.functions;
    module->function_capacity =
        image->program.function_count;
    module->classes =
        (PikaClassLayout*)image->program.classes;
    module->class_capacity = image->program.class_count;
    module->class_count = image->program.class_count;
    module->binding_calls = (PikaBindingCallSite*)copy_table(
        image->program.binding_calls,
        image->program.binding_call_count,
        sizeof(*module->binding_calls));
    module->binding_call_capacity =
        image->program.binding_call_count;
    module->binding_call_count =
        image->program.binding_call_count;
    module->builtin_calls =
        (PikaBuiltinCallSite*)image->program.builtin_calls;
    module->builtin_call_capacity =
        image->program.builtin_call_count;
    module->builtin_call_count =
        image->program.builtin_call_count;
    module->dynamic_method_calls =
        (PikaDynamicMethodCallSite*)copy_table(
            image->program.dynamic_method_calls,
            image->program.dynamic_method_call_count,
            sizeof(*module->dynamic_method_calls));
    module->dynamic_method_call_capacity =
        image->program.dynamic_method_call_count;
    module->dynamic_method_call_count =
        image->program.dynamic_method_call_count;
    module->dynamic_method_candidates =
        (PikaDynamicMethodCandidate*)copy_table(
            image->program.dynamic_method_candidates,
            image->program.dynamic_method_candidate_count,
            sizeof(*module->dynamic_method_candidates));
    module->dynamic_method_candidate_capacity =
        image->program.dynamic_method_candidate_count;
    module->dynamic_method_candidate_count =
        image->program.dynamic_method_candidate_count;
    module->constants =
        (PikaConstant*)image->program.constants;
    module->constant_capacity =
        image->program.constant_count;
    module->constant_count =
        image->program.constant_count;
    module->index_sites =
        (PikaIndexSite*)image->program.index_sites;
    module->index_site_capacity =
        image->program.index_site_count;
    module->index_site_count =
        image->program.index_site_count;
    module->exception_handlers =
        (PikaExceptionHandler*)image->program.exception_handlers;
    module->exception_handler_capacity =
        image->program.exception_handler_count;
    module->exception_handler_count =
        image->program.exception_handler_count;
    module->object_slot_estimate =
        image->object_slot_estimate;
    module->object_byte_estimate =
        image->object_byte_estimate;
    module->semantic_names = (char*)image->semantic_names;
    module->semantic_name_capacity =
        image->semantic_name_count;
    module->semantic_name_count =
        image->semantic_name_count;
    module->semantic_functions =
        (PikaSemanticFunction*)copy_table(
            image->semantic_functions,
            image->semantic_function_count,
            sizeof(*module->semantic_functions));
    module->semantic_function_capacity =
        image->semantic_function_count;
    module->semantic_function_count =
        image->semantic_function_count;
    module->semantic_parameters =
        (PikaSemanticParameter*)image->semantic_parameters;
    module->semantic_parameter_capacity =
        image->semantic_parameter_count;
    module->semantic_parameter_count =
        image->semantic_parameter_count;
    module->semantic_shapes =
        (PikaStaticShape*)copy_table(
            image->semantic_shapes,
            image->semantic_shape_count,
            sizeof(*module->semantic_shapes));
    module->semantic_shape_capacity =
        image->semantic_shape_count;
    module->semantic_shape_count =
        image->semantic_shape_count;
    module->semantic_classes =
        (PikaSemanticClass*)image->semantic_classes;
    module->semantic_class_capacity =
        image->semantic_class_count;
    module->semantic_class_count =
        image->semantic_class_count;
    module->semantic_fields =
        (PikaSemanticField*)copy_table(
            image->semantic_fields,
            image->semantic_field_count,
            sizeof(*module->semantic_fields));
    module->semantic_field_capacity =
        image->semantic_field_count;
    module->semantic_field_count =
        image->semantic_field_count;
    module->semantic_globals =
        (PikaSemanticGlobal*)copy_table(
            image->semantic_globals,
            image->semantic_global_count,
            sizeof(*module->semantic_globals));
    module->semantic_global_capacity =
        image->semantic_global_count;
    module->semantic_global_count =
        image->semantic_global_count;
    module->semantic_imports =
        (PikaSemanticImport*)copy_table(
            image->semantic_imports,
            image->semantic_import_count,
            sizeof(*module->semantic_imports));
    module->semantic_import_capacity =
        image->semantic_import_count;
    module->semantic_import_count =
        image->semantic_import_count;
    module->semantic_python_imports =
        (PikaPythonModule*)image->semantic_python_imports;
    module->semantic_python_import_capacity =
        image->semantic_python_import_count;
    module->semantic_python_import_count =
        image->semantic_python_import_count;
    module->program.module_names = module->semantic_names;
    module->program.module_name_bytes = module->semantic_name_count;
    module->program.python_modules =
        module->semantic_python_imports;
    module->program.python_module_count =
        module->semantic_python_import_count;
    status = copy_program_metadata(image, module);
    if (status != PIKA_STATUS_OK) return status;
    module->enabled_capabilities =
        image->enabled_capabilities;
    module->bindings = bindings;
    status = pika_frontend_capture_binding_modules(
        module, bindings);
    if (status != PIKA_STATUS_OK) return status;
    module->program.binding_modules =
        module->available_binding_modules;
    module->program.binding_module_count =
        module->available_binding_module_count;
    module->has_interactive_result =
        image->has_interactive_result;
    module->borrows_program_image = 1u;
    module->program.binding_calls = module->binding_calls;
    module->program.dynamic_method_calls =
        module->dynamic_method_calls;
    module->program.dynamic_method_candidates =
        module->dynamic_method_candidates;
    if ((module->binding_call_count > 0u &&
         module->binding_calls == NULL) ||
        (module->dynamic_method_call_count > 0u &&
         module->dynamic_method_calls == NULL) ||
        (module->dynamic_method_candidate_count > 0u &&
         module->dynamic_method_candidates == NULL) ||
        (module->semantic_function_count > 0u &&
         module->semantic_functions == NULL) ||
        (module->semantic_shape_count > 0u &&
         module->semantic_shapes == NULL) ||
        (module->semantic_field_count > 0u &&
         module->semantic_fields == NULL) ||
        (module->semantic_global_count > 0u &&
         module->semantic_globals == NULL) ||
        (module->semantic_import_count > 0u &&
         module->semantic_imports == NULL)) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    for (index = 0u; index < module->binding_call_count; ++index) {
        module->binding_calls[index].module = NULL;
        module->binding_calls[index].class_descriptor = NULL;
        module->binding_calls[index].callable = NULL;
    }
    for (index = 0u;
         index < module->semantic_function_count;
         ++index) {
        module->semantic_functions[index]
            .return_shape.binding_class = NULL;
    }
    for (index = 0u;
         index < module->semantic_shape_count;
         ++index) {
        module->semantic_shapes[index].binding_class = NULL;
    }
    for (index = 0u;
         index < module->semantic_field_count;
         ++index) {
        module->semantic_fields[index].binding_class = NULL;
    }
    for (index = 0u;
         index < module->semantic_global_count;
         ++index) {
        module->semantic_globals[index].binding_class = NULL;
    }
    for (index = 0u;
         index < module->semantic_import_count;
         ++index) {
        module->semantic_imports[index].module = NULL;
    }
    return PIKA_STATUS_OK;
}

#if PIKA_BINDING_RUNTIME_ENABLE

static PikaStatus find_class(
    const PikaBindingModule* module,
    const char* name,
    const PikaBindingClass** binding_class) {
    if (name == NULL) {
        *binding_class = NULL;
        return PIKA_STATUS_OK;
    }
    return pika_binding_find_class(
        module, name, binding_class);
}

static PikaStatus resolve_call(
    const PikaProgramRelocation* relocation,
    const PikaBindingModule* binding_module,
    PikaBindingCallSite* site) {
    const PikaBindingClass* owner = NULL;
    const PikaBindingClass* result_class = NULL;
    const PikaBindingCallable* callable = NULL;
    PikaStatus status = find_class(
        binding_module, relocation->owner_class_name, &owner);
    if (status != PIKA_STATUS_OK) return status;
    status = find_class(
        binding_module, relocation->result_class_name,
        &result_class);
    if (status != PIKA_STATUS_OK) return status;
    if (site->kind == PIKA_BINDING_CALL_FUNCTION) {
        status = pika_binding_find_function(
            binding_module, relocation->callable_name,
            &callable);
    } else if (site->kind ==
               PIKA_BINDING_CALL_CONSTRUCTOR) {
        if (owner == NULL || owner->constructor == NULL ||
            strcmp(
                owner->constructor->name,
                relocation->callable_name) != 0) {
            return PIKA_STATUS_MEMBER_NOT_FOUND;
        }
        callable = owner->constructor;
    } else if (site->kind == PIKA_BINDING_CALL_METHOD) {
        if (owner == NULL) return PIKA_STATUS_MEMBER_NOT_FOUND;
        status = pika_binding_find_method(
            owner, relocation->callable_name, &callable);
    } else {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (status != PIKA_STATUS_OK) return status;
    site->module = binding_module;
    site->class_descriptor = result_class;
    site->callable = callable;
    return PIKA_STATUS_OK;
}

static PikaStatus apply_relocations(
    const PikaProgramImage* image,
    const PikaBindingRegistry* bindings,
    PikaCompiledModule* module) {
    uint32_t index;
    for (index = 0u; index < image->relocation_count; ++index) {
        const PikaProgramRelocation* relocation =
            &image->relocations[index];
        const PikaBindingModule* binding_module = NULL;
        const PikaBindingClass* binding_class = NULL;
        PikaStatus status;
        if (bindings == NULL) return PIKA_STATUS_MODULE_NOT_FOUND;
        status = pika_binding_find_module(
            bindings, relocation->module_name,
            &binding_module);
        if (status != PIKA_STATUS_OK) return status;
        if (relocation->kind ==
            PIKA_PROGRAM_RELOCATE_BINDING_CALL) {
            status = resolve_call(
                relocation, binding_module,
                &module->binding_calls[relocation->index]);
        } else if (relocation->kind ==
                   PIKA_PROGRAM_RELOCATE_IMPORT) {
            module->semantic_imports[
                relocation->index].module = binding_module;
            status = PIKA_STATUS_OK;
        } else {
            status = find_class(
                binding_module,
                relocation->result_class_name,
                &binding_class);
            if (status == PIKA_STATUS_OK &&
                relocation->kind ==
                    PIKA_PROGRAM_RELOCATE_FUNCTION_RETURN) {
                module->semantic_functions[
                    relocation->index]
                    .return_shape.binding_class =
                    binding_class;
            } else if (
                status == PIKA_STATUS_OK &&
                relocation->kind ==
                    PIKA_PROGRAM_RELOCATE_SHAPE) {
                module->semantic_shapes[
                    relocation->index].binding_class =
                    binding_class;
            } else if (
                status == PIKA_STATUS_OK &&
                relocation->kind ==
                    PIKA_PROGRAM_RELOCATE_FIELD) {
                module->semantic_fields[
                    relocation->index].binding_class =
                    binding_class;
            } else if (
                status == PIKA_STATUS_OK &&
                relocation->kind ==
                    PIKA_PROGRAM_RELOCATE_GLOBAL) {
                module->semantic_globals[
                    relocation->index].binding_class =
                    binding_class;
            }
        }
        if (status != PIKA_STATUS_OK) return status;
    }
    for (index = 0u; index < module->binding_call_count; ++index) {
        if (module->binding_calls[index].module == NULL ||
            module->binding_calls[index].callable == NULL) {
            return PIKA_STATUS_BINDING_SCHEMA;
        }
    }
    for (index = 0u;
         index < module->semantic_import_count;
         ++index) {
        if (module->semantic_imports[index].module == NULL) {
            return PIKA_STATUS_BINDING_SCHEMA;
        }
    }
    return PIKA_STATUS_OK;
}

#else

static PikaStatus apply_relocations(
    const PikaProgramImage* image,
    const PikaBindingRegistry* bindings,
    PikaCompiledModule* module) {
    (void)bindings;
    (void)module;
    return image->relocation_count == 0u
               ? PIKA_STATUS_OK
               : PIKA_STATUS_UNSUPPORTED_CAPABILITY;
}

#endif

PikaStatus pika_compiled_module_from_image(
    const PikaProgramImage* image,
    PikaCapability enabled_capabilities,
    const PikaBindingRegistry* bindings,
    PikaCompiledModule** module) {
    PikaCompiledModule* loaded;
    PikaStatus status;
    if (module == NULL) return PIKA_STATUS_INVALID_ARGUMENT;
    *module = NULL;
    if (image == NULL ||
        image->magic != PIKA_PROGRAM_IMAGE_MAGIC ||
        image->schema_version !=
            PIKA_PROGRAM_IMAGE_SCHEMA_VERSION) {
        return PIKA_STATUS_UNSUPPORTED_FORMAT;
    }
    if ((image->enabled_capabilities &
         ~enabled_capabilities) != 0u) {
        return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
    }
    status = validate_program_modules(image);
    if (status == PIKA_STATUS_OK) {
        status = validate_semantic_tables(image);
    }
    if (status == PIKA_STATUS_OK) {
        status = validate_relocations(image);
    }
    if (status != PIKA_STATUS_OK) return status;
    loaded = (PikaCompiledModule*)calloc(
        1u, sizeof(PikaCompiledModule));
    if (loaded == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
    loaded->borrows_program_image = 1u;
    status = copy_image_tables(image, bindings, loaded);
    if (status == PIKA_STATUS_OK) {
        status = apply_relocations(image, bindings, loaded);
    }
    if (status == PIKA_STATUS_OK) {
        status = pika_program_verify(&loaded->program);
    }
    if (status != PIKA_STATUS_OK) {
        pika_compiled_module_destroy(loaded);
        return status;
    }
    *module = loaded;
    return PIKA_STATUS_OK;
}

#endif
