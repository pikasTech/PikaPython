/* SPEC: PJ2026-050109 network-ready v0.1; binding registry and calls. */
#include "runtime_internal.h"

#include <stddef.h>
#include <string.h>

#if PIKA_BINDING_RUNTIME_ENABLE

static int name_is_valid(const char* name) {
    return name != NULL && name[0] != '\0';
}

static int names_are_equal(const char* left, const char* right) {
    return left[0] == right[0] && strcmp(left, right) == 0;
}

static int value_kind_is_valid(PikaBindingValueKind kind,
                               int descriptor) {
    if ((uint32_t)kind >
        (uint32_t)PIKA_BINDING_VALUE_OBJECT) {
        return 0;
    }
    if (descriptor) {
        return kind != PIKA_BINDING_VALUE_OPAQUE &&
               kind != PIKA_BINDING_VALUE_CALLABLE &&
               kind != PIKA_BINDING_VALUE_OBJECT;
    }
    return kind != PIKA_BINDING_VALUE_ANY;
}

static int result_descriptor_is_valid(
    const PikaBindingModule* module,
    const PikaBindingCallable* callable) {
    if (callable->result_kind != PIKA_BINDING_VALUE_OBJECT) {
        return value_kind_is_valid(callable->result_kind, 1);
    }
#if PIKA_CAPABILITY_BINDING_OBJECT_RESULT_ENABLE
    return module != NULL && module->classes != NULL &&
           callable->result_class_index < module->class_count;
#else
    (void)module;
    return 0;
#endif
}

static int simple_value_is_valid(const PikaBindingValue* value) {
    if (!value_kind_is_valid(value->kind, 0) ||
        value->kind == PIKA_BINDING_VALUE_OPAQUE ||
        value->kind == PIKA_BINDING_VALUE_OBJECT) {
        return 0;
    }
    if ((value->kind == PIKA_BINDING_VALUE_STRING ||
         value->kind == PIKA_BINDING_VALUE_BYTES) &&
        value->as.buffer.length > 0u &&
        value->as.buffer.data == NULL) {
        return 0;
    }
    if (value->kind == PIKA_BINDING_VALUE_BYTEARRAY &&
        value->as.mutable_buffer.length > 0u &&
        value->as.mutable_buffer.data == NULL) {
        return 0;
    }
    if (value->kind == PIKA_BINDING_VALUE_CALLABLE &&
        (value->as.callable.context == NULL ||
         value->as.callable.active == 0u ||
         value->as.callable.parameter_count >
             PIKA_BINDING_ARGUMENT_LIMIT)) {
        return 0;
    }
    return 1;
}

static int container_element_is_valid(
    const PikaBindingValue* value) {
    return simple_value_is_valid(value) &&
           (value->kind <= PIKA_BINDING_VALUE_BYTES ||
            value->kind == PIKA_BINDING_VALUE_BYTEARRAY);
}

static int value_is_valid(const PikaBindingValue* value) {
    uint32_t index;
    if (!simple_value_is_valid(value)) {
        return 0;
    }
    if (value->kind == PIKA_BINDING_VALUE_LIST ||
        value->kind == PIKA_BINDING_VALUE_TUPLE) {
        if (value->as.sequence.length >
                PIKA_BINDING_CONTAINER_ELEMENT_LIMIT ||
            (value->as.sequence.length > 0u &&
             value->as.sequence.values == NULL)) {
            return 0;
        }
        for (index = 0u;
             index < value->as.sequence.length;
             ++index) {
            if (!container_element_is_valid(
                    &value->as.sequence.values[index])) {
                return 0;
            }
        }
    } else if (value->kind == PIKA_BINDING_VALUE_DICT) {
        if (value->as.mapping.length >
                PIKA_BINDING_CONTAINER_ELEMENT_LIMIT ||
            (value->as.mapping.length > 0u &&
             value->as.mapping.entries == NULL)) {
            return 0;
        }
        for (index = 0u;
             index < value->as.mapping.length;
             ++index) {
            const PikaBindingMapEntry* entry =
                &value->as.mapping.entries[index];
            if (!container_element_is_valid(&entry->key) ||
                !container_element_is_valid(&entry->value)) {
                return 0;
            }
        }
    }
    return 1;
}

static int argument_matches(
    const PikaBindingValue* value,
    PikaBindingValueKind expected) {
    PikaBindingValueKind kind;
    kind = value->kind;
    if ((uint32_t)kind >
            (uint32_t)PIKA_BINDING_VALUE_OBJECT ||
        kind == PIKA_BINDING_VALUE_ANY ||
        kind == PIKA_BINDING_VALUE_OBJECT ||
        (expected != PIKA_BINDING_VALUE_ANY && expected != kind)) {
        return 0;
    }
    if (kind <= PIKA_BINDING_VALUE_FLOAT) {
        return 1;
    }
    if (kind == PIKA_BINDING_VALUE_STRING ||
        kind == PIKA_BINDING_VALUE_BYTES) {
        return value->as.buffer.length == 0u ||
               value->as.buffer.data != NULL;
    }
    if (kind == PIKA_BINDING_VALUE_OPAQUE) {
        return value->as.opaque != NULL;
    }
    if (kind == PIKA_BINDING_VALUE_BYTEARRAY) {
        return value->as.mutable_buffer.length == 0u ||
               value->as.mutable_buffer.data != NULL;
    }
    return value_is_valid(value);
}

static PikaStatus validate_constant(
    const PikaBindingConstant* constant) {
    if (constant->record_size < sizeof(*constant) ||
        !name_is_valid(constant->name) ||
        !value_is_valid(&constant->value) ||
        constant->value.kind > PIKA_BINDING_VALUE_BYTES) {
        return PIKA_STATUS_BINDING_SCHEMA;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus validate_callable(
    const PikaBindingModule* module,
    const PikaBindingCallable* callable,
    PikaBindingCallableKind expected) {
    uint32_t index;
    uint32_t other;
    uint16_t unsupported_flags = (uint16_t)(
        callable != NULL
            ? callable->flags &
                  (uint16_t)~(uint16_t)
                      PIKA_BINDING_CALLABLE_DEINITIALIZER
            : 0u);
    if (callable == NULL ||
        callable->record_size < sizeof(*callable) ||
        !name_is_valid(callable->name) ||
        callable->kind != expected ||
        callable->parameter_count > PIKA_BINDING_ARGUMENT_LIMIT ||
        (callable->parameter_count > 0u &&
         callable->parameters == NULL) ||
        !result_descriptor_is_valid(module, callable) ||
        callable->callback == NULL ||
        unsupported_flags != 0u) {
        return PIKA_STATUS_BINDING_SCHEMA;
    }
    if ((callable->flags &
         PIKA_BINDING_CALLABLE_DEINITIALIZER) != 0u &&
        (expected != PIKA_BINDING_CALL_METHOD ||
         callable->parameter_count != 0u ||
         callable->result_kind != PIKA_BINDING_VALUE_NONE)) {
        return PIKA_STATUS_BINDING_SCHEMA;
    }
    if (callable->parameter_names == NULL) {
        if (callable->defaults != NULL ||
            callable->required_parameter_count != 0u) {
            return PIKA_STATUS_BINDING_SCHEMA;
        }
    } else if (
        callable->required_parameter_count >
                callable->parameter_count ||
        (callable->required_parameter_count <
                 callable->parameter_count &&
             callable->defaults == NULL) ||
        (callable->required_parameter_count ==
                 callable->parameter_count &&
             callable->defaults != NULL)) {
        return PIKA_STATUS_BINDING_SCHEMA;
    }
    for (index = 0u; index < callable->parameter_count; ++index) {
        PikaBindingValueKind expected_kind =
            callable->parameters[index];
        if (!value_kind_is_valid(expected_kind, 1) ||
            expected_kind == PIKA_BINDING_VALUE_NONE) {
            return PIKA_STATUS_BINDING_SCHEMA;
        }
        if (callable->parameter_names == NULL) {
            continue;
        }
        if (!name_is_valid(callable->parameter_names[index])) {
            return PIKA_STATUS_BINDING_SCHEMA;
        }
        for (other = 0u; other < index; ++other) {
            if (names_are_equal(
                    callable->parameter_names[index],
                    callable->parameter_names[other])) {
                return PIKA_STATUS_BINDING_SCHEMA;
            }
        }
        if (index >= callable->required_parameter_count) {
            const PikaBindingValue* value =
                &callable->defaults[
                    index - callable->required_parameter_count];
            if (!value_is_valid(value) ||
                (expected_kind != PIKA_BINDING_VALUE_ANY &&
                 expected_kind != value->kind)) {
                return PIKA_STATUS_BINDING_SCHEMA;
            }
        }
    }
    return PIKA_STATUS_OK;
}

static PikaStatus validate_class(
    const PikaBindingModule* module,
    const PikaBindingClass* class_descriptor) {
    uint32_t index;
    uint32_t other;
    uint32_t member;
    PikaStatus status;
    if (class_descriptor == NULL ||
        class_descriptor->record_size < sizeof(*class_descriptor) ||
        !name_is_valid(class_descriptor->name) ||
        class_descriptor->constructor == NULL ||
        (class_descriptor->method_count > 0u &&
         class_descriptor->methods == NULL) ||
        (class_descriptor->constant_count > 0u &&
         class_descriptor->constants == NULL)) {
        return PIKA_STATUS_BINDING_SCHEMA;
    }
    status = validate_callable(
        module,
        class_descriptor->constructor,
        PIKA_BINDING_CALL_CONSTRUCTOR);
    if (status != PIKA_STATUS_OK ||
        class_descriptor->constructor->result_kind !=
            PIKA_BINDING_VALUE_NONE) {
        return PIKA_STATUS_BINDING_SCHEMA;
    }
    for (index = 0u; index < class_descriptor->method_count; ++index) {
        status = validate_callable(
            module,
            &class_descriptor->methods[index],
            PIKA_BINDING_CALL_METHOD);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        for (other = 0u; other < index; ++other) {
            if (names_are_equal(
                    class_descriptor->methods[index].name,
                    class_descriptor->methods[other].name)) {
                return PIKA_STATUS_BINDING_SCHEMA;
            }
        }
    }
    for (index = 0u;
         index < class_descriptor->constant_count;
         ++index) {
        status = validate_constant(
            &class_descriptor->constants[index]);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        for (other = 0u; other < index; ++other) {
            if (names_are_equal(
                    class_descriptor->constants[index].name,
                    class_descriptor->constants[other].name)) {
                return PIKA_STATUS_BINDING_SCHEMA;
            }
        }
        for (member = 0u;
             member < class_descriptor->method_count;
             ++member) {
            if (names_are_equal(
                    class_descriptor->constants[index].name,
                    class_descriptor->methods[member].name)) {
                return PIKA_STATUS_BINDING_SCHEMA;
            }
        }
    }
    return PIKA_STATUS_OK;
}

static PikaStatus validate_module(
    const PikaBindingModule* module) {
    uint32_t index;
    uint32_t other;
    uint32_t member;
    PikaStatus status;
    if (module == NULL ||
        module->schema_version != PIKA_BINDING_SCHEMA_VERSION ||
        module->record_size < sizeof(*module) ||
        !name_is_valid(module->name) ||
        (module->function_count > 0u &&
         module->functions == NULL) ||
        (module->class_count > 0u && module->classes == NULL) ||
        (module->constant_count > 0u &&
         module->constants == NULL)) {
        return PIKA_STATUS_BINDING_SCHEMA;
    }
    for (index = 0u; index < module->function_count; ++index) {
        status = validate_callable(
            module,
            &module->functions[index],
            PIKA_BINDING_CALL_FUNCTION);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        for (other = 0u; other < index; ++other) {
            if (names_are_equal(
                    module->functions[index].name,
                    module->functions[other].name)) {
                return PIKA_STATUS_BINDING_SCHEMA;
            }
        }
    }
    for (index = 0u; index < module->class_count; ++index) {
        status = validate_class(module, &module->classes[index]);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        for (other = 0u; other < index; ++other) {
            if (names_are_equal(
                    module->classes[index].name,
                    module->classes[other].name)) {
                return PIKA_STATUS_BINDING_SCHEMA;
            }
        }
        for (other = 0u; other < module->function_count; ++other) {
            if (names_are_equal(
                    module->classes[index].name,
                    module->functions[other].name)) {
                return PIKA_STATUS_BINDING_SCHEMA;
            }
        }
    }
    for (index = 0u; index < module->constant_count; ++index) {
        status = validate_constant(&module->constants[index]);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        for (other = 0u; other < index; ++other) {
            if (names_are_equal(
                    module->constants[index].name,
                    module->constants[other].name)) {
                return PIKA_STATUS_BINDING_SCHEMA;
            }
        }
        for (member = 0u;
             member < module->function_count;
             ++member) {
            if (names_are_equal(
                    module->constants[index].name,
                    module->functions[member].name)) {
                return PIKA_STATUS_BINDING_SCHEMA;
            }
        }
        for (member = 0u; member < module->class_count; ++member) {
            if (names_are_equal(
                    module->constants[index].name,
                    module->classes[member].name)) {
                return PIKA_STATUS_BINDING_SCHEMA;
            }
        }
    }
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding_registry_initialize(
    PikaBindingRegistry* registry,
    const PikaBindingModule** storage,
    uint16_t capacity) {
    if (registry == NULL ||
        (capacity > 0u && storage == NULL)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    registry->modules = storage;
    registry->capacity = capacity;
    registry->count = 0u;
    if (capacity > 0u) {
        memset(storage, 0, capacity * sizeof(*storage));
    }
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding_registry_register(
    PikaBindingRegistry* registry,
    const PikaBindingModule* module) {
    uint32_t index;
    PikaStatus status;
    if (registry == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = validate_module(module);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    for (index = 0u; index < registry->count; ++index) {
        if (names_are_equal(
                registry->modules[index]->name, module->name)) {
            return PIKA_STATUS_DUPLICATE_BINDING;
        }
    }
    if (registry->count >= registry->capacity) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    registry->modules[registry->count++] = module;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding_find_module(
    const PikaBindingRegistry* registry,
    const char* name,
    const PikaBindingModule** module) {
    uint32_t index;
    if (registry == NULL || !name_is_valid(name) || module == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *module = NULL;
    for (index = 0u; index < registry->count; ++index) {
        if (strcmp(registry->modules[index]->name, name) == 0) {
            *module = registry->modules[index];
            return PIKA_STATUS_OK;
        }
    }
    return PIKA_STATUS_MODULE_NOT_FOUND;
}

PikaStatus pika_binding_find_function(
    const PikaBindingModule* module,
    const char* name,
    const PikaBindingCallable** callable) {
    uint32_t index;
    if (module == NULL || !name_is_valid(name) || callable == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *callable = NULL;
    for (index = 0u; index < module->function_count; ++index) {
        if (strcmp(module->functions[index].name, name) == 0) {
            *callable = &module->functions[index];
            return PIKA_STATUS_OK;
        }
    }
    return PIKA_STATUS_MEMBER_NOT_FOUND;
}

PikaStatus pika_binding_find_class(
    const PikaBindingModule* module,
    const char* name,
    const PikaBindingClass** class_descriptor) {
    uint32_t index;
    if (module == NULL || !name_is_valid(name) ||
        class_descriptor == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *class_descriptor = NULL;
    for (index = 0u; index < module->class_count; ++index) {
        if (strcmp(module->classes[index].name, name) == 0) {
            *class_descriptor = &module->classes[index];
            return PIKA_STATUS_OK;
        }
    }
    return PIKA_STATUS_MEMBER_NOT_FOUND;
}

PikaStatus pika_binding_find_method(
    const PikaBindingClass* class_descriptor,
    const char* name,
    const PikaBindingCallable** callable) {
    uint32_t index;
    if (class_descriptor == NULL || !name_is_valid(name) ||
        callable == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *callable = NULL;
    for (index = 0u; index < class_descriptor->method_count; ++index) {
        if (strcmp(class_descriptor->methods[index].name, name) == 0) {
            *callable = &class_descriptor->methods[index];
            return PIKA_STATUS_OK;
        }
    }
    return PIKA_STATUS_MEMBER_NOT_FOUND;
}

PikaStatus pika_binding_find_module_constant(
    const PikaBindingModule* module,
    const char* name,
    const PikaBindingConstant** constant) {
    uint32_t index;
    if (module == NULL || !name_is_valid(name) ||
        constant == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *constant = NULL;
    for (index = 0u; index < module->constant_count; ++index) {
        if (strcmp(module->constants[index].name, name) == 0) {
            *constant = &module->constants[index];
            return PIKA_STATUS_OK;
        }
    }
    return PIKA_STATUS_MEMBER_NOT_FOUND;
}

PikaStatus pika_binding_find_class_constant(
    const PikaBindingClass* class_descriptor,
    const char* name,
    const PikaBindingConstant** constant) {
    uint32_t index;
    if (class_descriptor == NULL || !name_is_valid(name) ||
        constant == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *constant = NULL;
    for (index = 0u;
         index < class_descriptor->constant_count;
         ++index) {
        if (strcmp(class_descriptor->constants[index].name,
                   name) == 0) {
            *constant = &class_descriptor->constants[index];
            return PIKA_STATUS_OK;
        }
    }
    return PIKA_STATUS_MEMBER_NOT_FOUND;
}

static PikaStatus validate_arguments(
    const PikaBindingCallable* callable,
    const PikaBindingValue* arguments,
    uint8_t argument_count) {
    uint32_t index;
    if (argument_count != callable->parameter_count) {
        return PIKA_STATUS_ARGUMENT_COUNT;
    }
    if (argument_count > 0u && arguments == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0u; index < argument_count; ++index) {
        PikaBindingValueKind expected = callable->parameters[index];
        if (!argument_matches(&arguments[index], expected)) {
            return PIKA_STATUS_TYPE_MISMATCH;
        }
    }
    return PIKA_STATUS_OK;
}

#if PIKA_CAPABILITY_BINDING_OBJECT_RESULT_ENABLE
static void discard_object_result(
    const PikaBindingModule* module,
    const PikaBindingCallable* callable,
    PikaBindingValue* result) {
    const PikaBindingClass* class_descriptor;
    if (module->classes == NULL ||
        callable->result_kind != PIKA_BINDING_VALUE_OBJECT ||
        result->kind != PIKA_BINDING_VALUE_OBJECT ||
        result->as.opaque == NULL ||
        callable->result_class_index >= module->class_count) {
        return;
    }
    class_descriptor =
        &module->classes[callable->result_class_index];
    if (class_descriptor->destroy != NULL) {
        class_descriptor->destroy(
            module->context, result->as.opaque);
    }
    memset(result, 0, sizeof(*result));
}
#endif

static PikaStatus invoke_callback(
    const PikaBindingModule* module,
    const PikaBindingCallable* callable,
    void* instance,
    const PikaBindingObject* receiver,
    const PikaBindingValue* arguments,
    uint8_t argument_count,
    PikaBindingValue* result,
    int constructor,
    int arguments_prevalidated) {
    PikaBindingCall call;
    PikaStatus status;
    if (module == NULL || callable == NULL || result == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (argument_count > 0u && arguments == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (!arguments_prevalidated) {
        status = validate_arguments(
            callable, arguments, argument_count);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
    }
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_BINDING_VALUE_NONE;
    call.arguments = arguments;
    call.argument_count = argument_count;
    call.instance = instance;
    call.module_context = module->context;
    call.receiver = receiver;
    status = callable->callback(&call, result);
    if (status != PIKA_STATUS_OK) {
#if PIKA_CAPABILITY_BINDING_OBJECT_RESULT_ENABLE
        discard_object_result(module, callable, result);
#endif
        return status;
    }
    if (constructor) {
        return result->kind == PIKA_BINDING_VALUE_OPAQUE &&
                       result->as.opaque != NULL
                   ? PIKA_STATUS_OK
                   : PIKA_STATUS_TYPE_MISMATCH;
    }
    if (callable->result_kind != PIKA_BINDING_VALUE_ANY &&
        callable->result_kind != result->kind) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
#if PIKA_CAPABILITY_BINDING_OBJECT_RESULT_ENABLE
    if (callable->result_kind == PIKA_BINDING_VALUE_OBJECT) {
        return result->as.opaque != NULL
                   ? PIKA_STATUS_OK
                   : PIKA_STATUS_TYPE_MISMATCH;
    }
#endif
    if ((uint32_t)result->kind <=
        (uint32_t)PIKA_BINDING_VALUE_FLOAT) {
        return PIKA_STATUS_OK;
    }
    if (!value_is_valid(result)) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding_invoke(
    const PikaBindingModule* module,
    const PikaBindingCallable* callable,
    void* instance,
    const PikaBindingValue* arguments,
    uint8_t argument_count,
    PikaBindingValue* result) {
    PikaStatus status = validate_callable(
        module,
        callable,
        instance == NULL ? PIKA_BINDING_CALL_FUNCTION
                         : PIKA_BINDING_CALL_METHOD);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    return pika_binding_invoke_validated(
        module, callable, instance, arguments, argument_count,
        result);
}

PikaStatus pika_binding_invoke_validated(
    const PikaBindingModule* module,
    const PikaBindingCallable* callable,
    void* instance,
    const PikaBindingValue* arguments,
    uint8_t argument_count,
    PikaBindingValue* result) {
    return pika_binding_invoke_call_site(
        module, callable, instance, arguments, argument_count,
        result, 0u);
}

PikaStatus pika_binding_invoke_call_site(
    const PikaBindingModule* module,
    const PikaBindingCallable* callable,
    void* instance,
    const PikaBindingValue* arguments,
    uint8_t argument_count,
    PikaBindingValue* result,
    uint8_t call_site_flags) {
    return invoke_callback(
        module, callable, instance, NULL, arguments, argument_count,
        result, 0,
        (call_site_flags &
         PIKA_BINDING_CALL_SITE_PREVALIDATED_ARGUMENTS) != 0u);
}

PikaStatus pika_binding_construct(
    const PikaBindingModule* module,
    const PikaBindingClass* class_descriptor,
    const PikaBindingValue* arguments,
    uint8_t argument_count,
    PikaBindingObject* object) {
    PikaStatus status;
    if (module == NULL || object == NULL || object->active != 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = validate_class(module, class_descriptor);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    return pika_binding_construct_validated(
        module, class_descriptor, arguments, argument_count,
        object);
}

PikaStatus pika_binding_construct_validated(
    const PikaBindingModule* module,
    const PikaBindingClass* class_descriptor,
    const PikaBindingValue* arguments,
    uint8_t argument_count,
    PikaBindingObject* object) {
    return pika_binding_construct_call_site(
        module, class_descriptor, arguments, argument_count,
        object, 0u);
}

PikaStatus pika_binding_construct_call_site(
    const PikaBindingModule* module,
    const PikaBindingClass* class_descriptor,
    const PikaBindingValue* arguments,
    uint8_t argument_count,
    PikaBindingObject* object,
    uint8_t call_site_flags) {
    PikaBindingValue result;
    PikaStatus status;
    if (module == NULL || class_descriptor == NULL || object == NULL ||
        object->active != 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = invoke_callback(
        module, class_descriptor->constructor, NULL, NULL, arguments,
        argument_count, &result, 1,
        (call_site_flags &
         PIKA_BINDING_CALL_SITE_PREVALIDATED_ARGUMENTS) != 0u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    object->type = class_descriptor;
    object->module_context = module->context;
    object->instance = result.as.opaque;
    object->active = 1u;
    return PIKA_STATUS_OK;
}

static int method_belongs_to_object(
    const PikaBindingObject* object,
    const PikaBindingCallable* method) {
    uint32_t index;
    for (index = 0u; index < object->type->method_count; ++index) {
        if (&object->type->methods[index] == method) {
            return 1;
        }
    }
    return 0;
}

PikaStatus pika_binding_invoke_method(
    const PikaBindingModule* module,
    const PikaBindingObject* object,
    const PikaBindingCallable* method,
    const PikaBindingValue* arguments,
    uint8_t argument_count,
    PikaBindingValue* result) {
    if (module == NULL || object == NULL || object->active == 0u ||
        object->instance == NULL || object->type == NULL ||
        !method_belongs_to_object(object, method)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    {
        PikaStatus status = validate_callable(
            module,
            method, PIKA_BINDING_CALL_METHOD);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
    }
    return invoke_callback(
        module, method, object->instance, object, arguments,
        argument_count, result, 0, 0);
}

PikaStatus pika_binding_invoke_method_validated(
    const PikaBindingModule* module,
    const PikaBindingObject* object,
    const PikaBindingCallable* method,
    const PikaBindingValue* arguments,
    uint8_t argument_count,
    PikaBindingValue* result) {
    return pika_binding_invoke_method_call_site(
        module, object, method, arguments, argument_count,
        result, 0u);
}

PikaStatus pika_binding_invoke_method_call_site(
    const PikaBindingModule* module,
    const PikaBindingObject* object,
    const PikaBindingCallable* method,
    const PikaBindingValue* arguments,
    uint8_t argument_count,
    PikaBindingValue* result,
    uint8_t call_site_flags) {
    if (module == NULL || object == NULL || object->active == 0u ||
        object->instance == NULL || object->type == NULL ||
        !method_belongs_to_object(object, method)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return invoke_callback(
        module, method, object->instance, object, arguments,
        argument_count, result, 0,
        (call_site_flags &
         PIKA_BINDING_CALL_SITE_PREVALIDATED_ARGUMENTS) != 0u);
}

#if PIKA_CAPABILITY_BINDING_OBJECT_RESULT_ENABLE
PikaStatus pika_binding_object_adopt_result(
    const PikaBindingModule* module,
    const PikaBindingCallable* callable,
    PikaBindingValue* result,
    PikaBindingObject* object) {
    if (module == NULL || callable == NULL || result == NULL ||
        object == NULL || object->active != 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (!result_descriptor_is_valid(module, callable)) {
        return PIKA_STATUS_BINDING_SCHEMA;
    }
    if (callable->result_kind != PIKA_BINDING_VALUE_OBJECT ||
        result->kind != PIKA_BINDING_VALUE_OBJECT ||
        result->as.opaque == NULL) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    object->type =
        &module->classes[callable->result_class_index];
    object->module_context = module->context;
    object->instance = result->as.opaque;
    object->active = 1u;
    memset(result, 0, sizeof(*result));
    return PIKA_STATUS_OK;
}
#endif

void pika_binding_callable_release(
    PikaBindingPythonCallable* callable) {
    if (callable != NULL) {
        memset(callable, 0, sizeof(*callable));
    }
}

void pika_binding_object_release(PikaBindingObject* object) {
    if (object == NULL || object->active == 0u) {
        return;
    }
    if (object->type != NULL && object->type->destroy != NULL) {
        object->type->destroy(
            object->module_context, object->instance);
    }
    object->type = NULL;
    object->module_context = NULL;
    object->instance = NULL;
    object->active = 0u;
}

#endif
