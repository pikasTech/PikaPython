/* SPEC: PJ2026-0501 V2 kernel v0.47, PJ2026-050111 v0.10; image exporter. */
#include "program-prebuild.h"

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "pika_program_image.h"

static const char* path_name(const char* path) {
    const char* slash;
    const char* backslash;
    if (path == NULL) return NULL;
    slash = strrchr(path, '/');
    backslash = strrchr(path, '\\');
    if (slash == NULL ||
        (backslash != NULL && backslash > slash)) {
        slash = backslash;
    }
    return slash == NULL ? path : slash + 1;
}

static int write_c_string(FILE* output, const char* value) {
    const unsigned char* cursor =
        (const unsigned char*)value;
    if (value == NULL) return fputs("NULL", output) >= 0;
    if (fputc('"', output) == EOF) return 0;
    while (*cursor != 0u) {
        unsigned char byte = *cursor++;
        if (byte == '"' || byte == '\\') {
            if (fputc('\\', output) == EOF ||
                fputc((int)byte, output) == EOF) {
                return 0;
            }
        } else if (byte >= 0x20u && byte <= 0x7eu) {
            if (fputc((int)byte, output) == EOF) return 0;
        } else if (fprintf(output, "\\%03o", byte) < 0) {
            return 0;
        }
    }
    return fputc('"', output) != EOF;
}

static int write_byte_array(FILE* output,
                            const char* type,
                            const char* name,
                            const uint8_t* bytes,
                            uint32_t length) {
    uint32_t index;
    if (length == 0u) return 1;
    if (fprintf(
            output, "static const %s %s[] = {\n",
            type, name) < 0) {
        return 0;
    }
    for (index = 0u; index < length; ++index) {
        if (index % 12u == 0u &&
            fputs("    ", output) < 0) {
            return 0;
        }
        if (fprintf(output, "0x%02" PRIx8 "u,", bytes[index]) < 0) {
            return 0;
        }
        if (index % 12u == 11u || index + 1u == length) {
            if (fputc('\n', output) == EOF) return 0;
        } else if (fputc(' ', output) == EOF) {
            return 0;
        }
    }
    return fputs("};\n\n", output) >= 0;
}

static int write_instructions(
    FILE* output, const PikaProgram* program) {
    uint32_t index;
    if (program->instruction_count == 0u) return 1;
    if (fputs(
            "static const PikaInstruction "
            "pika_image_instructions[] = {\n",
            output) < 0) {
        return 0;
    }
    for (index = 0u; index < program->instruction_count; ++index) {
        const PikaInstruction* instruction =
            pika_program_instruction_at(program, index);
        if (instruction == NULL) return 0;
        if (fprintf(
                output,
                "    {%uu, %uu, %uu, %uu, %" PRId32 "},\n",
                (unsigned)instruction->opcode,
                (unsigned)instruction->a,
                (unsigned)instruction->b,
                (unsigned)instruction->c,
                instruction->immediate) < 0) {
            return 0;
        }
    }
    return fputs("};\n\n", output) >= 0;
}

static int write_functions(
    FILE* output, const PikaProgram* program) {
    uint32_t index;
    if (program->function_count == 0u) return 1;
    if (fputs(
            "static const PikaFunction "
            "pika_image_functions[] = {\n",
            output) < 0) {
        return 0;
    }
    for (index = 0u; index < program->function_count; ++index) {
        const PikaFunction* function =
            &program->functions[index];
        if (fprintf(
                output,
                "    {%" PRIu32 "u, %" PRIu32
                "u, %uu, %uu, %uu},\n",
                function->first_instruction,
                function->instruction_count,
                (unsigned)function->slot_count,
                (unsigned)function->parameter_count,
                (unsigned)function->reserved) < 0) {
            return 0;
        }
    }
    return fputs("};\n\n", output) >= 0;
}

static int write_classes(
    FILE* output, const PikaProgram* program) {
    uint32_t index;
    if (program->class_count == 0u) return 1;
    if (fputs(
            "static const PikaClassLayout "
            "pika_image_classes[] = {\n",
            output) < 0) {
        return 0;
    }
    for (index = 0u; index < program->class_count; ++index) {
        const PikaClassLayout* binding_class =
            &program->classes[index];
        if (fprintf(
                output, "    {%uu, %uu},\n",
                (unsigned)binding_class->field_count,
                (unsigned)binding_class->reserved) < 0) {
            return 0;
        }
    }
    return fputs("};\n\n", output) >= 0;
}

static int write_binding_calls(
    FILE* output, const PikaProgram* program) {
    uint32_t index;
    uint32_t argument;
    if (program->binding_call_count == 0u) return 1;
    if (fputs(
            "static const PikaBindingCallSite "
            "pika_image_binding_calls[] = {\n",
            output) < 0) {
        return 0;
    }
    for (index = 0u;
         index < program->binding_call_count;
         ++index) {
        const PikaBindingCallSite* call =
            &program->binding_calls[index];
        if (fprintf(
                output,
                "    {%uu, %uu, %uu, %uu, %uu, "
                "NULL, NULL, NULL, {",
                (unsigned)call->kind,
                (unsigned)call->receiver_slot,
                (unsigned)call->argument_count,
                (unsigned)call->result_kind,
                (unsigned)call->flags) < 0) {
            return 0;
        }
        for (argument = 0u;
             argument < PIKA_BINDING_ARGUMENT_LIMIT;
             ++argument) {
            if (fprintf(
                    output, "%s%uu",
                    argument == 0u ? "" : ", ",
                    (unsigned)call->argument_kinds[argument]) < 0) {
                return 0;
            }
        }
        if (fputs("}},\n", output) < 0) return 0;
    }
    return fputs("};\n\n", output) >= 0;
}

static int write_builtin_calls(
    FILE* output, const PikaProgram* program) {
    uint32_t index;
    if (program->builtin_call_count == 0u) return 1;
    if (fputs(
            "static const PikaBuiltinCallSite "
            "pika_image_builtin_calls[] = {\n",
            output) < 0) {
        return 0;
    }
    for (index = 0u;
         index < program->builtin_call_count;
         ++index) {
        const PikaBuiltinCallSite* call =
            &program->builtin_calls[index];
        if (fprintf(
                output, "    {%uu, %uu, %uu},\n",
                (unsigned)call->builtin_id,
                (unsigned)call->argument_count,
                (unsigned)call->result_kind) < 0) {
            return 0;
        }
    }
    return fputs("};\n\n", output) >= 0;
}

static int write_dynamic_method_calls(
    FILE* output, const PikaProgram* program) {
    uint32_t index;
    if (program->dynamic_method_call_count == 0u) return 1;
    if (fputs(
            "static const PikaDynamicMethodCallSite "
            "pika_image_dynamic_method_calls[] = {\n",
            output) < 0) {
        return 0;
    }
    for (index = 0u;
         index < program->dynamic_method_call_count;
         ++index) {
        const PikaDynamicMethodCallSite* site =
            &program->dynamic_method_calls[index];
        if (fprintf(
                output, "    {%uu, %uu, %uu},\n",
                (unsigned)site->first_candidate,
                (unsigned)site->candidate_count,
                (unsigned)site->argument_count) < 0) {
            return 0;
        }
    }
    return fputs("};\n\n", output) >= 0;
}

static int write_dynamic_method_candidates(
    FILE* output, const PikaProgram* program) {
    uint32_t index;
    if (program->dynamic_method_candidate_count == 0u) return 1;
    if (fputs(
            "static const PikaDynamicMethodCandidate "
            "pika_image_dynamic_method_candidates[] = {\n",
            output) < 0) {
        return 0;
    }
    for (index = 0u;
         index < program->dynamic_method_candidate_count;
         ++index) {
        const PikaDynamicMethodCandidate* candidate =
            &program->dynamic_method_candidates[index];
        if (fprintf(
                output, "    {%uu, %uu},\n",
                (unsigned)candidate->class_index,
                (unsigned)candidate->function_index) < 0) {
            return 0;
        }
    }
    return fputs("};\n\n", output) >= 0;
}

static int write_integer(FILE* output, int64_t value) {
    if (value == INT64_MIN) {
        return fputs(
            "(-INT64_C(9223372036854775807) - INT64_C(1))",
            output) >= 0;
    }
    if (value < 0) {
        return fprintf(
                   output, "-INT64_C(%" PRIu64 ")",
                   (uint64_t)(-value)) >= 0;
    }
    return fprintf(
               output, "INT64_C(%" PRIu64 ")",
               (uint64_t)value) >= 0;
}

static int write_constants(
    FILE* output, const PikaProgram* program) {
    uint32_t index;
    char name[64];
    for (index = 0u; index < program->constant_count; ++index) {
        const PikaConstant* constant =
            &program->constants[index];
        if ((constant->kind == PIKA_CONSTANT_STRING ||
             constant->kind == PIKA_CONSTANT_BYTES ||
             constant->kind == PIKA_CONSTANT_TYPE ||
             constant->kind == PIKA_CONSTANT_MODULE) &&
            constant->length > 0u) {
            if (snprintf(
                    name, sizeof(name),
                    "pika_image_constant_%" PRIu32,
                    index) < 0 ||
                !write_byte_array(
                    output, "char", name,
                    (const uint8_t*)constant->as.bytes,
                    constant->length)) {
                return 0;
            }
        }
    }
    if (program->constant_count == 0u) return 1;
    if (fputs(
            "static const PikaConstant "
            "pika_image_constants[] = {\n",
            output) < 0) {
        return 0;
    }
    for (index = 0u; index < program->constant_count; ++index) {
        const PikaConstant* constant =
            &program->constants[index];
        if (fprintf(
                output, "    {%uu, %uu, %uu, {",
                (unsigned)constant->kind,
                (unsigned)constant->reserved,
                (unsigned)constant->length) < 0) {
            return 0;
        }
        if (constant->kind == PIKA_CONSTANT_FLOAT) {
            if (fprintf(
                    output, ".floating = %.17g",
                    constant->as.floating) < 0) {
                return 0;
            }
        } else if (
            constant->kind == PIKA_CONSTANT_STRING ||
            constant->kind == PIKA_CONSTANT_BYTES ||
            constant->kind == PIKA_CONSTANT_TYPE ||
            constant->kind == PIKA_CONSTANT_MODULE) {
            if (constant->length == 0u) {
                if (fputs(".bytes = NULL", output) < 0) return 0;
            } else if (fprintf(
                           output,
                           ".bytes = pika_image_constant_%" PRIu32,
                           index) < 0) {
                return 0;
            }
        } else {
            if (fputs(".integer = ", output) < 0 ||
                !write_integer(output, constant->as.integer)) {
                return 0;
            }
        }
        if (fputs("}},\n", output) < 0) return 0;
    }
    return fputs("};\n\n", output) >= 0;
}

static int write_index_sites(
    FILE* output, const PikaProgram* program) {
    uint32_t index;
    if (program->index_site_count == 0u) return 1;
    if (fputs(
            "static const PikaIndexSite "
            "pika_image_index_sites[] = {\n",
            output) < 0) {
        return 0;
    }
    for (index = 0u; index < program->index_site_count; ++index) {
        const PikaIndexSite* site =
            &program->index_sites[index];
        if (fprintf(
                output,
                "    {%uu, %uu, %uu, %uu, %uu, %uu},\n",
                (unsigned)site->container_slot,
                (unsigned)site->index_slot,
                (unsigned)site->start_slot,
                (unsigned)site->stop_slot,
                (unsigned)site->step_slot,
                (unsigned)site->flags) < 0) {
            return 0;
        }
    }
    return fputs("};\n\n", output) >= 0;
}

static int write_exception_handlers(
    FILE* output, const PikaProgram* program) {
    uint32_t index;
    if (program->exception_handler_count == 0u) return 1;
    if (fputs(
            "static const PikaExceptionHandler "
            "pika_image_exception_handlers[] = {\n",
            output) < 0) {
        return 0;
    }
    for (index = 0u;
         index < program->exception_handler_count;
         ++index) {
        const PikaExceptionHandler* handler =
            &program->exception_handlers[index];
        if (fprintf(
                output,
                "    {%uu, %uu, %uu, %" PRIu32
                "u, %" PRIu32 "u, %" PRIu32 "u},\n",
                (unsigned)handler->function_index,
                (unsigned)handler->status_slot,
                (unsigned)handler->catch_kind,
                handler->first_instruction,
                handler->instruction_count,
                handler->handler_instruction) < 0) {
            return 0;
        }
    }
    return fputs("};\n\n", output) >= 0;
}

static int write_static_shape(
    FILE* output, const PikaStaticShape* shape) {
    return fprintf(
               output, "{NULL, %uu, %uu, %uu}",
               (unsigned)shape->class_index,
               (unsigned)shape->kind,
               (unsigned)shape->binding_kind) >= 0;
}

static int write_semantic_names(
    FILE* output, const PikaCompiledModule* module) {
    return write_byte_array(
        output, "char", "pika_image_semantic_names",
        (const uint8_t*)module->semantic_names,
        module->semantic_name_count);
}

static int write_semantic_functions(
    FILE* output, const PikaCompiledModule* module) {
    uint32_t index;
    if (module->semantic_function_count == 0u) return 1;
    if (fputs(
            "static const PikaSemanticFunction "
            "pika_image_semantic_functions[] = {\n",
            output) < 0) {
        return 0;
    }
    for (index = 0u;
         index < module->semantic_function_count;
         ++index) {
        const PikaSemanticFunction* function =
            &module->semantic_functions[index];
        if (fprintf(
                output,
                "    {%" PRIu32 "u, %uu, %uu, %uu, %uu, %uu, ",
                function->name_offset,
                (unsigned)function->name_length,
                (unsigned)function->first_parameter,
                (unsigned)function->parameter_count,
                (unsigned)function->required_parameter_count,
                (unsigned)function->owner_class) < 0 ||
            !write_static_shape(output, &function->return_shape) ||
            fprintf(
                output,
                ", %" PRIu32 "u, %uu, %uu, %uu},\n",
                function->first_return_element_shape,
                (unsigned)function->return_element_shape_count,
                (unsigned)function->has_return_shape,
                (unsigned)function->variadic_parameter) < 0) {
            return 0;
        }
    }
    return fputs("};\n\n", output) >= 0;
}

static int write_semantic_parameters(
    FILE* output, const PikaCompiledModule* module) {
    uint32_t index;
    if (module->semantic_parameter_count == 0u) return 1;
    if (fputs(
            "static const PikaSemanticParameter "
            "pika_image_semantic_parameters[] = {\n",
            output) < 0) {
        return 0;
    }
    for (index = 0u;
         index < module->semantic_parameter_count;
         ++index) {
        const PikaSemanticParameter* parameter =
            &module->semantic_parameters[index];
        if (fprintf(
                output,
                "    {%" PRIu32 "u, %uu, %uu},\n",
                parameter->name_offset,
                (unsigned)parameter->name_length,
                (unsigned)parameter->default_constant) < 0) {
            return 0;
        }
    }
    return fputs("};\n\n", output) >= 0;
}

static int write_semantic_shapes(
    FILE* output, const PikaCompiledModule* module) {
    uint32_t index;
    if (module->semantic_shape_count == 0u) return 1;
    if (fputs(
            "static const PikaStaticShape "
            "pika_image_semantic_shapes[] = {\n",
            output) < 0) {
        return 0;
    }
    for (index = 0u;
         index < module->semantic_shape_count;
         ++index) {
        if (fputs("    ", output) < 0 ||
            !write_static_shape(
                output, &module->semantic_shapes[index]) ||
            fputs(",\n", output) < 0) {
            return 0;
        }
    }
    return fputs("};\n\n", output) >= 0;
}

static int write_semantic_classes(
    FILE* output, const PikaCompiledModule* module) {
    uint32_t index;
    if (module->semantic_class_count == 0u) return 1;
    if (fputs(
            "static const PikaSemanticClass "
            "pika_image_semantic_classes[] = {\n",
            output) < 0) {
        return 0;
    }
    for (index = 0u;
         index < module->semantic_class_count;
         ++index) {
        const PikaSemanticClass* binding_class =
            &module->semantic_classes[index];
        if (fprintf(
                output,
                "    {%" PRIu32
                "u, %uu, %uu, %uu, %uu, %uu},\n",
                binding_class->name_offset,
                (unsigned)binding_class->name_length,
                (unsigned)binding_class->base_class,
                (unsigned)binding_class->first_field,
                (unsigned)binding_class->field_count,
                (unsigned)binding_class->member_count) < 0) {
            return 0;
        }
    }
    return fputs("};\n\n", output) >= 0;
}

static int write_semantic_fields(
    FILE* output, const PikaCompiledModule* module) {
    uint32_t index;
    if (module->semantic_field_count == 0u) return 1;
    if (fputs(
            "static const PikaSemanticField "
            "pika_image_semantic_fields[] = {\n",
            output) < 0) {
        return 0;
    }
    for (index = 0u;
         index < module->semantic_field_count;
         ++index) {
        const PikaSemanticField* field =
            &module->semantic_fields[index];
        if (fprintf(
                output,
                "    {%" PRIu32
                "u, %uu, %uu, %uu, %uu, NULL, %uu},\n",
                field->name_offset,
                (unsigned)field->name_length,
                (unsigned)field->kind,
                (unsigned)field->binding_kind,
                (unsigned)field->allows_none,
                (unsigned)field->class_index) < 0) {
            return 0;
        }
    }
    return fputs("};\n\n", output) >= 0;
}

static int write_semantic_globals(
    FILE* output, const PikaCompiledModule* module) {
    uint32_t index;
    if (module->semantic_global_count == 0u) return 1;
    if (fputs(
            "static const PikaSemanticGlobal "
            "pika_image_semantic_globals[] = {\n",
            output) < 0) {
        return 0;
    }
    for (index = 0u;
         index < module->semantic_global_count;
         ++index) {
        const PikaSemanticGlobal* global =
            &module->semantic_globals[index];
        if (fprintf(
                output,
                "    {%" PRIu32
                "u, %uu, %uu, %uu, %uu, %uu, NULL, "
                "%uu, %" PRIu32 "u, %uu},\n",
                global->name_offset,
                (unsigned)global->name_length,
                (unsigned)global->slot,
                (unsigned)global->kind,
                (unsigned)global->binding_kind,
                (unsigned)global->reserved,
                (unsigned)global->class_index,
                global->first_element_shape,
                (unsigned)global->element_shape_count) < 0) {
            return 0;
        }
    }
    return fputs("};\n\n", output) >= 0;
}

static int write_semantic_imports(
    FILE* output, const PikaCompiledModule* module) {
    uint32_t index;
    if (module->semantic_import_count == 0u) return 1;
    if (fputs(
            "static const PikaSemanticImport "
            "pika_image_semantic_imports[] = {\n",
            output) < 0) {
        return 0;
    }
    for (index = 0u;
         index < module->semantic_import_count;
         ++index) {
        const PikaSemanticImport* import_value =
            &module->semantic_imports[index];
        if (fprintf(
                output,
                "    {%" PRIu32
                "u, %uu, %uu, %uu, NULL},\n",
                import_value->name_offset,
                (unsigned)import_value->name_length,
                (unsigned)import_value->selected,
                (unsigned)import_value->reserved) < 0) {
            return 0;
        }
    }
    return fputs("};\n\n", output) >= 0;
}

static int write_semantic_python_imports(
    FILE* output, const PikaCompiledModule* module) {
    uint32_t index;
    if (module->semantic_python_import_count == 0u) return 1;
    if (fputs(
            "static const PikaPythonModule "
            "pika_image_semantic_python_imports[] = {\n",
            output) < 0) {
        return 0;
    }
    for (index = 0u;
         index < module->semantic_python_import_count;
         ++index) {
        const PikaPythonModule* import_value =
            &module->semantic_python_imports[index];
        if (fprintf(
                output,
                "    {%" PRIu32 "u, %uu, %uu, %uu},\n",
                import_value->name_offset,
                (unsigned)import_value->name_length,
                (unsigned)import_value->target_module,
                (unsigned)import_value->flags) < 0) {
            return 0;
        }
    }
    return fputs("};\n\n", output) >= 0;
}

static const PikaProgramImageSourceIdentity* find_source_identity(
    const PikaProgramImageExportIdentity* identity,
    const char* name,
    uint16_t name_length) {
    uint16_t index;
    if (identity == NULL || name == NULL) return NULL;
    for (index = 0u; index < identity->module_count; ++index) {
        const PikaProgramImageSourceIdentity* source =
            &identity->modules[index];
        if (source->name != NULL &&
            strlen(source->name) == name_length &&
            memcmp(source->name, name, name_length) == 0) {
            return source;
        }
    }
    return NULL;
}

static const PikaSemanticProgramModule* find_program_module(
    const PikaCompiledModule* module,
    uint8_t module_index) {
    uint16_t index;
    for (index = 0u; index < module->program_module_count; ++index) {
        if (module->program_modules[index].module_index ==
            module_index) {
            return &module->program_modules[index];
        }
    }
    return NULL;
}

static int semantic_range_valid(
    const PikaCompiledModule* module,
    uint32_t offset,
    uint16_t length) {
    return module->semantic_names != NULL && length > 0u &&
           offset <= module->semantic_name_count &&
           length <= module->semantic_name_count - offset;
}

static PikaStatus validate_source_identity_catalog(
    const PikaProgramImageExportIdentity* identity) {
    uint16_t index;
    for (index = 0u; index < identity->module_count; ++index) {
        const PikaProgramImageSourceIdentity* source =
            &identity->modules[index];
        uint16_t previous;
        if (source->name == NULL || source->name[0] == '\0' ||
            source->source_digest == NULL ||
            source->source_digest_length !=
                PIKA_PROGRAM_IMAGE_DIGEST_LENGTH) {
            return PIKA_STATUS_INVALID_ARGUMENT;
        }
        for (previous = 0u; previous < index; ++previous) {
            if (strcmp(identity->modules[previous].name,
                       source->name) == 0) {
                return PIKA_STATUS_MODULE_CONFLICT;
            }
        }
    }
    return PIKA_STATUS_OK;
}

static PikaStatus validate_export_metadata(
    const PikaCompiledModule* module,
    const PikaProgramImageExportIdentity* identity) {
    const PikaSemanticProgramModule* entry;
    PikaStatus status;
    uint16_t index;
    if (module == NULL || identity == NULL ||
        module->program_modules == NULL ||
        module->program_module_count == 0u ||
        identity->modules == NULL ||
        identity->module_count < module->program_module_count ||
        identity->configuration_digest == NULL ||
        identity->configuration_digest_length !=
            PIKA_PROGRAM_IMAGE_DIGEST_LENGTH ||
        identity->capability_digest == NULL ||
        identity->capability_digest_length !=
            PIKA_PROGRAM_IMAGE_DIGEST_LENGTH) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = validate_source_identity_catalog(identity);
    if (status != PIKA_STATUS_OK) return status;
    entry = find_program_module(
        module, module->program_entry_module);
    if (entry == NULL) return PIKA_STATUS_INVALID_OPERAND;
    for (index = 0u;
         index < module->program_module_count;
         ++index) {
        const PikaSemanticProgramModule* program_module =
            &module->program_modules[index];
        const PikaProgramImageSourceIdentity* source;
        uint16_t other;
        uint16_t dependency;
        uint16_t binding;
        if (!semantic_range_valid(
                module, program_module->name_offset,
                program_module->name_length) ||
            program_module->entry_function >=
                module->program.function_count ||
            program_module->first_dependency >
                module->program_module_dependency_count ||
            program_module->dependency_count >
                module->program_module_dependency_count -
                    program_module->first_dependency ||
            program_module->first_binding_import >
                module->program_binding_import_count ||
            program_module->binding_import_count >
                module->program_binding_import_count -
                    program_module->first_binding_import) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        source = find_source_identity(
            identity,
            &module->semantic_names[program_module->name_offset],
            program_module->name_length);
        if (source == NULL || source->source_digest == NULL ||
            source->source_digest_length !=
                PIKA_PROGRAM_IMAGE_DIGEST_LENGTH) {
            return PIKA_STATUS_INVALID_ARGUMENT;
        }
        for (other = 0u; other < index; ++other) {
            const PikaSemanticProgramModule* previous =
                &module->program_modules[other];
            if (previous->module_index ==
                    program_module->module_index ||
                (previous->name_length ==
                     program_module->name_length &&
                 memcmp(
                     &module->semantic_names[previous->name_offset],
                     &module->semantic_names[
                         program_module->name_offset],
                     program_module->name_length) == 0)) {
                return PIKA_STATUS_MODULE_CONFLICT;
            }
        }
        for (dependency = 0u;
             dependency < program_module->dependency_count;
             ++dependency) {
            uint8_t dependency_index =
                module->program_module_dependencies[
                    program_module->first_dependency + dependency];
            if (find_program_module(module, dependency_index) == NULL) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
        }
        for (binding = 0u;
             binding < program_module->binding_import_count;
             ++binding) {
            const PikaSemanticProgramBindingImport* imported =
                &module->program_binding_imports[
                    program_module->first_binding_import + binding];
            if (imported->module_index !=
                    program_module->module_index ||
                !semantic_range_valid(
                    module, imported->name_offset,
                    imported->name_length)) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
        }
    }
    return PIKA_STATUS_OK;
}

static int write_program_identity_tables(
    FILE* output,
    const PikaCompiledModule* module,
    const PikaProgramImageExportIdentity* identity) {
    uint16_t index;
    char symbol[96];
    if (!write_byte_array(
            output, "uint8_t", "pika_image_configuration_digest",
            identity->configuration_digest,
            identity->configuration_digest_length) ||
        !write_byte_array(
            output, "uint8_t", "pika_image_capability_digest",
            identity->capability_digest,
            identity->capability_digest_length)) {
        return 0;
    }
    for (index = 0u; index < module->program_module_count; ++index) {
        const PikaSemanticProgramModule* program_module =
            &module->program_modules[index];
        const PikaProgramImageSourceIdentity* source =
            find_source_identity(
                identity,
                &module->semantic_names[program_module->name_offset],
                program_module->name_length);
        int written = snprintf(
            symbol, sizeof(symbol),
            "pika_image_module_%u_source_digest",
            (unsigned)index);
        if (source == NULL || written < 0 ||
            (size_t)written >= sizeof(symbol) ||
            !write_byte_array(
                output, "uint8_t", symbol,
                source->source_digest,
                source->source_digest_length)) {
            return 0;
        }
    }
    return 1;
}

static int write_program_modules(
    FILE* output,
    const PikaCompiledModule* module,
    const PikaProgramImageExportIdentity* identity) {
    uint16_t index;
    for (index = 0u;
         index < module->program_module_count;
         ++index) {
        const PikaSemanticProgramModule* program_module =
            &module->program_modules[index];
        uint16_t dependency;
        if (program_module->dependency_count > 0u) {
            if (fprintf(
                    output,
                    "static const uint8_t "
                    "pika_image_module_%u_dependencies[] = {\n    ",
                    (unsigned)index) < 0) {
                return 0;
            }
            for (dependency = 0u;
                 dependency < program_module->dependency_count;
                 ++dependency) {
                if (fprintf(
                        output, "%s%uu",
                        dependency == 0u ? "" : ", ",
                        (unsigned)
                            module->program_module_dependencies[
                                program_module->first_dependency +
                                dependency]) < 0) {
                    return 0;
                }
            }
            if (fputs("\n};\n\n", output) < 0) return 0;
        }
        if (program_module->binding_import_count > 0u) {
            uint16_t binding;
            if (fprintf(
                    output,
                    "static const PikaProgramImageBindingImport "
                    "pika_image_module_%u_binding_imports[] = {\n",
                    (unsigned)index) < 0) {
                return 0;
            }
            for (binding = 0u;
                 binding < program_module->binding_import_count;
                 ++binding) {
                const PikaSemanticProgramBindingImport* imported =
                    &module->program_binding_imports[
                        program_module->first_binding_import + binding];
                if (fprintf(
                        output,
                        "    {&pika_image_semantic_names[%" PRIu32
                        "], %uu, 0u},\n",
                        imported->name_offset,
                        (unsigned)imported->name_length) < 0) {
                    return 0;
                }
            }
            if (fputs("};\n\n", output) < 0) return 0;
        }
    }
    if (fputs(
            "static const PikaProgramImageModule "
            "pika_image_modules[] = {\n",
            output) < 0) {
        return 0;
    }
    for (index = 0u; index < module->program_module_count; ++index) {
        const PikaSemanticProgramModule* program_module =
            &module->program_modules[index];
        const PikaProgramImageSourceIdentity* source =
            find_source_identity(
                identity,
                &module->semantic_names[program_module->name_offset],
                program_module->name_length);
        char dependency_symbol[96];
        char binding_symbol[96];
        const char* dependency_table = "NULL";
        const char* binding_table = "NULL";
        if (program_module->dependency_count > 0u) {
            int written = snprintf(
                    dependency_symbol, sizeof(dependency_symbol),
                    "pika_image_module_%u_dependencies",
                    (unsigned)index);
            if (written < 0 ||
                (size_t)written >= sizeof(dependency_symbol)) {
                return 0;
            }
            dependency_table = dependency_symbol;
        }
        if (program_module->binding_import_count > 0u) {
            int written = snprintf(
                    binding_symbol, sizeof(binding_symbol),
                    "pika_image_module_%u_binding_imports",
                    (unsigned)index);
            if (written < 0 ||
                (size_t)written >= sizeof(binding_symbol)) {
                return 0;
            }
            binding_table = binding_symbol;
        }
        if (source == NULL ||
            fprintf(
                output,
                "    {\n"
                "        .name = &pika_image_semantic_names[%" PRIu32
                "],\n"
                "        .name_length = %uu,\n"
                "        .module_index = %uu,\n"
                "        .reserved = 0u,\n"
                "        .entry_function = %uu,\n"
                "        .dependencies = %s,\n"
                "        .dependency_count = %uu,\n"
                "        .reserved_count = 0u,\n"
                "        .binding_imports = %s,\n"
                "        .binding_import_count = %uu,\n"
                "        .source_digest_length = %uu,\n"
                "        .source_digest = "
                "pika_image_module_%u_source_digest,\n"
                "    },\n",
                program_module->name_offset,
                (unsigned)program_module->name_length,
                (unsigned)program_module->module_index,
                (unsigned)program_module->entry_function,
                dependency_table,
                (unsigned)program_module->dependency_count,
                binding_table,
                (unsigned)program_module->binding_import_count,
                (unsigned)source->source_digest_length,
                (unsigned)index) < 0) {
            return 0;
        }
    }
    return fputs("};\n\n", output) >= 0;
}

static int write_json_string(
    FILE* output, const char* value, uint16_t length) {
    uint16_t index;
    if (value == NULL || fputc('"', output) == EOF) return 0;
    for (index = 0u; index < length; ++index) {
        unsigned char byte = (unsigned char)value[index];
        if (byte == '"' || byte == '\\') {
            if (fputc('\\', output) == EOF ||
                fputc((int)byte, output) == EOF) {
                return 0;
            }
        } else if (byte < 0x20u) {
            if (fprintf(output, "\\u%04x", (unsigned)byte) < 0) {
                return 0;
            }
        } else if (fputc((int)byte, output) == EOF) {
            return 0;
        }
    }
    return fputc('"', output) != EOF;
}

static int write_metadata_module_name(
    FILE* output,
    const PikaCompiledModule* module,
    uint8_t module_index) {
    const PikaSemanticProgramModule* dependency =
        find_program_module(module, module_index);
    return dependency != NULL &&
           write_json_string(
               output,
               &module->semantic_names[dependency->name_offset],
               dependency->name_length);
}

static int write_frontend_metadata(
    FILE* output, const PikaCompiledModule* module) {
    const PikaSemanticProgramModule* entry =
        find_program_module(module, module->program_entry_module);
    uint16_t index;
    if (entry == NULL ||
        fputs(
            "{\n"
            "  \"entryModule\": ",
            output) < 0 ||
        !write_json_string(
            output, &module->semantic_names[entry->name_offset],
            entry->name_length) ||
        fprintf(
            output,
            ",\n  \"programFormatVersion\": %u,\n"
            "  \"schema\": "
            "\"pika.program-image/frontend-metadata-v1\",\n"
            "  \"modules\": [\n",
            (unsigned)module->program.format_version) < 0) {
        return 0;
    }
    for (index = 0u;
         index < module->program_module_count;
         ++index) {
        const PikaSemanticProgramModule* program_module =
            &module->program_modules[index];
        uint16_t dependency;
        uint16_t binding;
        if (fprintf(
                output,
                "%s    {\n"
                "      \"bindingRelocations\": [",
                index == 0u ? "" : ",\n") < 0) {
            return 0;
        }
        for (binding = 0u;
             binding < program_module->binding_import_count;
             ++binding) {
            const PikaSemanticProgramBindingImport* imported =
                &module->program_binding_imports[
                    program_module->first_binding_import + binding];
            if (fputs(binding == 0u ? "" : ", ", output) < 0 ||
                !write_json_string(
                    output,
                    &module->semantic_names[imported->name_offset],
                    imported->name_length)) {
                return 0;
            }
        }
        if (fputs("],\n      \"dependencies\": [", output) < 0) {
            return 0;
        }
        for (dependency = 0u;
             dependency < program_module->dependency_count;
             ++dependency) {
            if (fputs(dependency == 0u ? "" : ", ", output) < 0 ||
                !write_metadata_module_name(
                    output, module,
                    module->program_module_dependencies[
                        program_module->first_dependency +
                        dependency])) {
                return 0;
            }
        }
        for (binding = 0u;
             binding < program_module->binding_import_count;
             ++binding) {
            const PikaSemanticProgramBindingImport* imported =
                &module->program_binding_imports[
                    program_module->first_binding_import + binding];
            if (fputs(
                    program_module->dependency_count == 0u &&
                            binding == 0u
                        ? ""
                        : ", ",
                    output) < 0 ||
                !write_json_string(
                    output,
                    &module->semantic_names[imported->name_offset],
                    imported->name_length)) {
                return 0;
            }
        }
        if (fprintf(
                output,
                "],\n      \"entryFunction\": %u,\n"
                "      \"name\": ",
                (unsigned)program_module->entry_function) < 0 ||
            !write_json_string(
                output,
                &module->semantic_names[program_module->name_offset],
                program_module->name_length) ||
            fputs(",\n      \"pythonImportRelocations\": [", output) < 0) {
            return 0;
        }
        for (dependency = 0u;
             dependency < program_module->dependency_count;
             ++dependency) {
            if (fputs(dependency == 0u ? "" : ", ", output) < 0 ||
                !write_metadata_module_name(
                    output, module,
                    module->program_module_dependencies[
                        program_module->first_dependency +
                        dependency])) {
                return 0;
            }
        }
        if (fputs("]\n    }", output) < 0) return 0;
    }
    return fputs("\n  ]\n}\n", output) >= 0;
}

static const PikaBindingModule* class_module(
    const PikaBindingRegistry* registry,
    const PikaBindingClass* binding_class) {
    uint16_t module_index;
    if (registry == NULL || binding_class == NULL) return NULL;
    for (module_index = 0u;
         module_index < registry->count;
         ++module_index) {
        const PikaBindingModule* module =
            registry->modules[module_index];
        uint16_t class_index;
        for (class_index = 0u;
             class_index < module->class_count;
             ++class_index) {
            if (&module->classes[class_index] ==
                binding_class) {
                return module;
            }
        }
    }
    return NULL;
}

static const PikaBindingClass* callable_owner(
    const PikaBindingModule* module,
    const PikaBindingCallable* callable) {
    uint16_t class_index;
    if (module == NULL || callable == NULL) return NULL;
    for (class_index = 0u;
         class_index < module->class_count;
         ++class_index) {
        const PikaBindingClass* binding_class =
            &module->classes[class_index];
        uint16_t method_index;
        if (binding_class->constructor == callable) {
            return binding_class;
        }
        for (method_index = 0u;
             method_index < binding_class->method_count;
             ++method_index) {
            if (&binding_class->methods[method_index] ==
                callable) {
                return binding_class;
            }
        }
    }
    return NULL;
}

static uint32_t relocation_count(
    const PikaCompiledModule* module) {
    uint32_t count = module->binding_call_count +
                     module->semantic_import_count;
    uint32_t index;
    for (index = 0u;
         index < module->semantic_function_count;
         ++index) {
        if (module->semantic_functions[index]
                .return_shape.binding_class != NULL) {
            ++count;
        }
    }
    for (index = 0u;
         index < module->semantic_shape_count;
         ++index) {
        if (module->semantic_shapes[index].binding_class != NULL) {
            ++count;
        }
    }
    for (index = 0u;
         index < module->semantic_field_count;
         ++index) {
        if (module->semantic_fields[index].binding_class != NULL) {
            ++count;
        }
    }
    for (index = 0u;
         index < module->semantic_global_count;
         ++index) {
        if (module->semantic_globals[index].binding_class != NULL) {
            ++count;
        }
    }
    return count;
}

static int write_relocation(
    FILE* output,
    PikaProgramRelocationKind kind,
    uint32_t index,
    const char* module_name,
    const char* owner_class_name,
    const char* callable_name,
    const char* result_class_name) {
    if (fprintf(
            output, "    {%uu, {0u, 0u, 0u}, %" PRIu32 "u, ",
            (unsigned)kind, index) < 0 ||
        !write_c_string(output, module_name) ||
        fputs(", ", output) < 0 ||
        !write_c_string(output, owner_class_name) ||
        fputs(", ", output) < 0 ||
        !write_c_string(output, callable_name) ||
        fputs(", ", output) < 0 ||
        !write_c_string(output, result_class_name) ||
        fputs("},\n", output) < 0) {
        return 0;
    }
    return 1;
}

static int write_class_relocation(
    FILE* output,
    const PikaBindingRegistry* registry,
    PikaProgramRelocationKind kind,
    uint32_t index,
    const PikaBindingClass* binding_class) {
    const PikaBindingModule* module =
        class_module(registry, binding_class);
    return module != NULL &&
           write_relocation(
               output, kind, index, module->name,
               NULL, NULL, binding_class->name);
}

static int write_relocations(
    FILE* output, const PikaCompiledModule* module) {
    uint32_t index;
    uint32_t count = relocation_count(module);
    if (count == 0u) return 1;
    if (fputs(
            "static const PikaProgramRelocation "
            "pika_image_relocations[] = {\n",
            output) < 0) {
        return 0;
    }
    for (index = 0u;
         index < module->binding_call_count;
         ++index) {
        const PikaBindingCallSite* call =
            &module->binding_calls[index];
        const PikaBindingClass* owner =
            callable_owner(call->module, call->callable);
        if (call->module == NULL || call->callable == NULL ||
            (call->kind != PIKA_BINDING_CALL_FUNCTION &&
             owner == NULL) ||
            !write_relocation(
                output, PIKA_PROGRAM_RELOCATE_BINDING_CALL,
                index, call->module->name,
                owner == NULL ? NULL : owner->name,
                call->callable->name,
                call->class_descriptor == NULL
                    ? NULL
                    : call->class_descriptor->name)) {
            return 0;
        }
    }
    for (index = 0u;
         index < module->semantic_function_count;
         ++index) {
        const PikaBindingClass* binding_class =
            module->semantic_functions[index]
                .return_shape.binding_class;
        if (binding_class != NULL &&
            !write_class_relocation(
                output, module->bindings,
                PIKA_PROGRAM_RELOCATE_FUNCTION_RETURN,
                index, binding_class)) {
            return 0;
        }
    }
    for (index = 0u;
         index < module->semantic_shape_count;
         ++index) {
        const PikaBindingClass* binding_class =
            module->semantic_shapes[index].binding_class;
        if (binding_class != NULL &&
            !write_class_relocation(
                output, module->bindings,
                PIKA_PROGRAM_RELOCATE_SHAPE,
                index, binding_class)) {
            return 0;
        }
    }
    for (index = 0u;
         index < module->semantic_field_count;
         ++index) {
        const PikaBindingClass* binding_class =
            module->semantic_fields[index].binding_class;
        if (binding_class != NULL &&
            !write_class_relocation(
                output, module->bindings,
                PIKA_PROGRAM_RELOCATE_FIELD,
                index, binding_class)) {
            return 0;
        }
    }
    for (index = 0u;
         index < module->semantic_global_count;
         ++index) {
        const PikaBindingClass* binding_class =
            module->semantic_globals[index].binding_class;
        if (binding_class != NULL &&
            !write_class_relocation(
                output, module->bindings,
                PIKA_PROGRAM_RELOCATE_GLOBAL,
                index, binding_class)) {
            return 0;
        }
    }
    for (index = 0u;
         index < module->semantic_import_count;
         ++index) {
        const PikaBindingModule* import_module =
            module->semantic_imports[index].module;
        if (import_module == NULL ||
            !write_relocation(
                output, PIKA_PROGRAM_RELOCATE_IMPORT,
                index, import_module->name,
                NULL, NULL, NULL)) {
            return 0;
        }
    }
    return fputs("};\n\n", output) >= 0;
}

static const char* table_name(uint32_t count, const char* name) {
    return count == 0u ? "NULL" : name;
}

static int write_image(
    FILE* output,
    const PikaCompiledModule* module,
    const char* symbol,
    const PikaProgramImageExportIdentity* identity) {
    const PikaProgram* program = &module->program;
    uint32_t relocations = relocation_count(module);
    if (fprintf(output, "const PikaProgramImage %s = {\n", symbol) < 0 ||
        fprintf(
            output,
            "    .magic = PIKA_PROGRAM_IMAGE_MAGIC,\n"
            "    .schema_version = "
            "PIKA_PROGRAM_IMAGE_SCHEMA_VERSION,\n"
            "    .enabled_capabilities = "
            "UINT64_C(0x%016" PRIx64 "),\n"
            "    .modules = pika_image_modules,\n"
            "    .module_count = %uu,\n"
            "    .entry_module = %uu,\n"
            "    .configuration_digest = "
            "pika_image_configuration_digest,\n"
            "    .configuration_digest_length = %uu,\n"
            "    .capability_digest = "
            "pika_image_capability_digest,\n"
            "    .capability_digest_length = %uu,\n"
            "    .program = {\n"
            "        .format_version = %uu,\n"
            "        .instructions = %s,\n"
            "        .instruction_count = %" PRIu32 "u,\n"
            "        .functions = %s,\n"
            "        .function_count = %uu,\n"
            "        .entry_function = %uu,\n"
            "        .max_call_depth = %uu,\n"
            "        .binding_calls = %s,\n"
            "        .binding_call_count = %uu,\n"
            "        .binding_object_count = %uu,\n"
            "        .builtin_calls = %s,\n"
            "        .builtin_call_count = %uu,\n"
            "        .dynamic_method_calls = %s,\n"
            "        .dynamic_method_call_count = %uu,\n"
            "        .dynamic_method_candidates = %s,\n"
            "        .dynamic_method_candidate_count = %uu,\n"
            "        .constants = %s,\n"
            "        .constant_count = %uu,\n"
            "        .index_sites = %s,\n"
            "        .index_site_count = %uu,\n"
            "        .uses_typed_values = %uu,\n"
            "        .allows_binding_callback = %uu,\n"
            "        .object_slot_count = %uu,\n"
            "        .object_bytes = %" PRIu32 "u,\n"
            "        .execution_value_count = %" PRIu32 "u,\n"
            "        .classes = %s,\n"
            "        .class_count = %uu,\n"
            "        .reserved_class = %uu,\n"
            "        .exception_handlers = %s,\n"
            "        .exception_handler_count = %uu,\n"
            "        .reserved_exception = %uu,\n"
            "    },\n",
            (uint64_t)module->enabled_capabilities,
            (unsigned)module->program_module_count,
            (unsigned)module->program_entry_module,
            (unsigned)identity->configuration_digest_length,
            (unsigned)identity->capability_digest_length,
            (unsigned)program->format_version,
            table_name(
                program->instruction_count,
                "pika_image_instructions"),
            program->instruction_count,
            table_name(
                program->function_count,
                "pika_image_functions"),
            (unsigned)program->function_count,
            (unsigned)program->entry_function,
            (unsigned)program->max_call_depth,
            table_name(
                program->binding_call_count,
                "pika_image_binding_calls"),
            (unsigned)program->binding_call_count,
            (unsigned)program->binding_object_count,
            table_name(
                program->builtin_call_count,
                "pika_image_builtin_calls"),
            (unsigned)program->builtin_call_count,
            table_name(
                program->dynamic_method_call_count,
                "pika_image_dynamic_method_calls"),
            (unsigned)program->dynamic_method_call_count,
            table_name(
                program->dynamic_method_candidate_count,
                "pika_image_dynamic_method_candidates"),
            (unsigned)program->dynamic_method_candidate_count,
            table_name(
                program->constant_count,
                "pika_image_constants"),
            (unsigned)program->constant_count,
            table_name(
                program->index_site_count,
                "pika_image_index_sites"),
            (unsigned)program->index_site_count,
            (unsigned)program->uses_typed_values,
            (unsigned)program->allows_binding_callback,
            (unsigned)program->object_slot_count,
            program->object_bytes,
            program->execution_value_count,
            table_name(
                program->class_count,
                "pika_image_classes"),
            (unsigned)program->class_count,
            (unsigned)program->reserved_class,
            table_name(
                program->exception_handler_count,
                "pika_image_exception_handlers"),
            (unsigned)program->exception_handler_count,
            (unsigned)program->reserved_exception) < 0 ||
        fprintf(
            output,
            "    .object_slot_estimate = %" PRIu32 "u,\n"
            "    .object_byte_estimate = %" PRIu32 "u,\n"
            "    .semantic_names = %s,\n"
            "    .semantic_name_count = %" PRIu32 "u,\n"
            "    .semantic_functions = %s,\n"
            "    .semantic_function_count = %uu,\n"
            "    .semantic_parameters = %s,\n"
            "    .semantic_parameter_count = %uu,\n"
            "    .semantic_shapes = %s,\n"
            "    .semantic_shape_count = %" PRIu32 "u,\n"
            "    .semantic_classes = %s,\n"
            "    .semantic_class_count = %uu,\n"
            "    .semantic_fields = %s,\n"
            "    .semantic_field_count = %uu,\n"
            "    .semantic_globals = %s,\n"
            "    .semantic_global_count = %uu,\n"
            "    .semantic_imports = %s,\n"
            "    .semantic_import_count = %uu,\n"
            "    .semantic_python_imports = %s,\n"
            "    .semantic_python_import_count = %uu,\n"
            "    .relocations = %s,\n"
            "    .relocation_count = %" PRIu32 "u,\n"
            "    .has_interactive_result = %uu,\n"
            "    .reserved = {0u, 0u, 0u},\n"
            "};\n",
            module->object_slot_estimate,
            module->object_byte_estimate,
            table_name(
                module->semantic_name_count,
                "pika_image_semantic_names"),
            module->semantic_name_count,
            table_name(
                module->semantic_function_count,
                "pika_image_semantic_functions"),
            (unsigned)module->semantic_function_count,
            table_name(
                module->semantic_parameter_count,
                "pika_image_semantic_parameters"),
            (unsigned)module->semantic_parameter_count,
            table_name(
                module->semantic_shape_count,
                "pika_image_semantic_shapes"),
            module->semantic_shape_count,
            table_name(
                module->semantic_class_count,
                "pika_image_semantic_classes"),
            (unsigned)module->semantic_class_count,
            table_name(
                module->semantic_field_count,
                "pika_image_semantic_fields"),
            (unsigned)module->semantic_field_count,
            table_name(
                module->semantic_global_count,
                "pika_image_semantic_globals"),
            (unsigned)module->semantic_global_count,
            table_name(
                module->semantic_import_count,
                "pika_image_semantic_imports"),
            (unsigned)module->semantic_import_count,
            table_name(
                module->semantic_python_import_count,
                "pika_image_semantic_python_imports"),
            (unsigned)module->semantic_python_import_count,
            table_name(relocations, "pika_image_relocations"),
            relocations,
            (unsigned)module->has_interactive_result) < 0) {
        return 0;
    }
    return 1;
}

static int write_header(
    FILE* output, const char* symbol) {
    return fputs(
               "/* Generated by pikapython-cli. Do not edit. */\n"
               "#ifndef PIKAPYTHON_PROGRAM_H\n"
               "#define PIKAPYTHON_PROGRAM_H\n"
               "#include \"pika_program_image.h\"\n"
               "extern const PikaProgramImage ",
               output) >= 0 &&
           fputs(symbol, output) >= 0 &&
           fputs(";\n#endif\n", output) >= 0;
}

static int write_source(
    FILE* output,
    const char* header_name,
    const PikaCompiledModule* module,
    const char* symbol,
    const PikaProgramImageExportIdentity* identity) {
    if (fputs(
            "/* Generated by pikapython-cli. Do not edit. */\n"
            "#include <stdint.h>\n"
            "#include \"",
            output) < 0 ||
        fputs(header_name, output) < 0 ||
        fputs("\"\n\n", output) < 0) {
        return 0;
    }
    return write_instructions(output, &module->program) &&
           write_functions(output, &module->program) &&
           write_classes(output, &module->program) &&
           write_binding_calls(output, &module->program) &&
           write_builtin_calls(output, &module->program) &&
           write_dynamic_method_calls(output, &module->program) &&
           write_dynamic_method_candidates(output, &module->program) &&
           write_constants(output, &module->program) &&
           write_index_sites(output, &module->program) &&
           write_exception_handlers(output, &module->program) &&
           write_semantic_names(output, module) &&
           write_semantic_functions(output, module) &&
           write_semantic_parameters(output, module) &&
           write_semantic_shapes(output, module) &&
           write_semantic_classes(output, module) &&
           write_semantic_fields(output, module) &&
           write_semantic_globals(output, module) &&
           write_semantic_imports(output, module) &&
           write_semantic_python_imports(output, module) &&
           write_program_identity_tables(output, module, identity) &&
           write_program_modules(output, module, identity) &&
           write_relocations(output, module) &&
           write_image(output, module, symbol, identity);
}

PikaStatus pika_program_image_export(
    const PikaCompiledModule* module,
    const char* source_path,
    const char* header_path,
    const char* metadata_path,
    const char* symbol,
    const PikaProgramImageExportIdentity* identity) {
    const char* header_name = path_name(header_path);
    FILE* header;
    FILE* source;
    FILE* metadata;
    PikaStatus status;
    int successful;
    if (module == NULL || source_path == NULL ||
        header_path == NULL || header_name == NULL ||
        metadata_path == NULL || symbol == NULL ||
        identity == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = pika_program_verify(&module->program);
    if (status != PIKA_STATUS_OK) return status;
    status = validate_export_metadata(module, identity);
    if (status != PIKA_STATUS_OK) return status;
    header = fopen(header_path, "wb");
    if (header == NULL) return PIKA_STATUS_IO_ERROR;
    successful = write_header(header, symbol) &&
                 fflush(header) == 0 &&
                 ferror(header) == 0;
    if (fclose(header) != 0) successful = 0;
    if (!successful) return PIKA_STATUS_IO_ERROR;
    source = fopen(source_path, "wb");
    if (source == NULL) return PIKA_STATUS_IO_ERROR;
    successful =
        write_source(
            source, header_name, module, symbol, identity) &&
        fflush(source) == 0 && ferror(source) == 0;
    if (fclose(source) != 0) successful = 0;
    if (!successful) return PIKA_STATUS_IO_ERROR;
    metadata = fopen(metadata_path, "wb");
    if (metadata == NULL) return PIKA_STATUS_IO_ERROR;
    successful = write_frontend_metadata(metadata, module) &&
                 fflush(metadata) == 0 &&
                 ferror(metadata) == 0;
    if (fclose(metadata) != 0) successful = 0;
    return successful ? PIKA_STATUS_OK
                      : PIKA_STATUS_IO_ERROR;
}
