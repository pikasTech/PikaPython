/* SPEC: PJ2026-050109 network-ready v0.1; VM binding boundary. */
#include "runtime_internal.h"

#include <stddef.h>
#include <string.h>

#if PIKA_BINDING_RUNTIME_ENABLE

#if PIKA_CAPABILITY_BINDING_CONTAINER_ARGUMENT_ENABLE
typedef struct {
    PikaBindingValue values[PIKA_BINDING_CONTAINER_ELEMENT_LIMIT];
    PikaBindingMapEntry entries[PIKA_BINDING_CONTAINER_ELEMENT_LIMIT];
    uint32_t value_count;
    uint32_t entry_count;
} PikaBindingArgumentScratch;
#endif

static PikaBindingValueKind binding_kind_from_runtime(
    PikaRuntimeValueKind kind) {
    switch (kind) {
        case PIKA_RUNTIME_VALUE_NONE: return PIKA_BINDING_VALUE_NONE;
        case PIKA_RUNTIME_VALUE_BOOLEAN: return PIKA_BINDING_VALUE_BOOLEAN;
        case PIKA_RUNTIME_VALUE_INTEGER: return PIKA_BINDING_VALUE_INTEGER;
        case PIKA_RUNTIME_VALUE_FLOAT: return PIKA_BINDING_VALUE_FLOAT;
        case PIKA_RUNTIME_VALUE_STRING: return PIKA_BINDING_VALUE_STRING;
        case PIKA_RUNTIME_VALUE_BYTES: return PIKA_BINDING_VALUE_BYTES;
        case PIKA_RUNTIME_VALUE_BYTEARRAY:
            return PIKA_BINDING_VALUE_BYTEARRAY;
        case PIKA_RUNTIME_VALUE_CALLABLE:
            return PIKA_BINDING_VALUE_CALLABLE;
        case PIKA_RUNTIME_VALUE_NATIVE_OBJECT:
            return PIKA_BINDING_VALUE_OPAQUE;
#if PIKA_CAPABILITY_BINDING_CONTAINER_ARGUMENT_ENABLE
        case PIKA_RUNTIME_VALUE_LIST: return PIKA_BINDING_VALUE_LIST;
        case PIKA_RUNTIME_VALUE_TUPLE: return PIKA_BINDING_VALUE_TUPLE;
        case PIKA_RUNTIME_VALUE_DICT: return PIKA_BINDING_VALUE_DICT;
#endif
        default: return PIKA_BINDING_VALUE_ANY;
    }
}

static PikaStatus read_typed_scalar(
    PikaRuntimeContext* context,
    PikaRuntimeValue runtime_value,
    PikaBindingValueKind expected,
    PikaBindingValue* value) {
    PikaObjectSlot* object = NULL;
    PikaBindingValueKind actual = expected;
    if (expected == PIKA_BINDING_VALUE_ANY) {
        actual = binding_kind_from_runtime(runtime_value.kind);
        if (actual == PIKA_BINDING_VALUE_ANY ||
            actual == PIKA_BINDING_VALUE_LIST ||
            actual == PIKA_BINDING_VALUE_TUPLE ||
            actual == PIKA_BINDING_VALUE_DICT) {
            return PIKA_STATUS_TYPE_MISMATCH;
        }
    }
    memset(value, 0, sizeof(*value));
    value->kind = actual;
    if (actual == PIKA_BINDING_VALUE_NONE) {
        return runtime_value.kind == PIKA_RUNTIME_VALUE_NONE
                   ? PIKA_STATUS_OK
                   : PIKA_STATUS_TYPE_MISMATCH;
    }
    if (actual == PIKA_BINDING_VALUE_BOOLEAN) {
        if (runtime_value.kind != PIKA_RUNTIME_VALUE_BOOLEAN) {
            return PIKA_STATUS_TYPE_MISMATCH;
        }
        value->as.boolean =
            runtime_value.as.integer != 0 ? 1u : 0u;
        return PIKA_STATUS_OK;
    }
    if (actual == PIKA_BINDING_VALUE_INTEGER) {
        if (runtime_value.kind != PIKA_RUNTIME_VALUE_INTEGER) {
            return PIKA_STATUS_TYPE_MISMATCH;
        }
        value->as.integer = runtime_value.as.integer;
        return PIKA_STATUS_OK;
    }
    if (actual == PIKA_BINDING_VALUE_FLOAT) {
        if (runtime_value.kind != PIKA_RUNTIME_VALUE_FLOAT) {
            return PIKA_STATUS_TYPE_MISMATCH;
        }
        value->as.floating = runtime_value.as.floating;
        return PIKA_STATUS_OK;
    }
    if (actual == PIKA_BINDING_VALUE_STRING ||
        actual == PIKA_BINDING_VALUE_BYTES ||
        actual == PIKA_BINDING_VALUE_BYTEARRAY) {
        PikaRuntimeValueKind required =
            actual == PIKA_BINDING_VALUE_STRING
                ? PIKA_RUNTIME_VALUE_STRING
                : actual == PIKA_BINDING_VALUE_BYTES
                      ? PIKA_RUNTIME_VALUE_BYTES
                      : PIKA_RUNTIME_VALUE_BYTEARRAY;
        if (runtime_value.kind != required) {
            return PIKA_STATUS_TYPE_MISMATCH;
        }
        object = pika_runtime_object(context, runtime_value);
        if (object == NULL) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        if (actual == PIKA_BINDING_VALUE_BYTEARRAY) {
            value->as.mutable_buffer.data = (uint8_t*)object->data;
            value->as.mutable_buffer.length = object->length;
        } else {
            value->as.buffer.data = (const uint8_t*)object->data;
            value->as.buffer.length = object->length;
        }
        return PIKA_STATUS_OK;
    }
    if (actual == PIKA_BINDING_VALUE_CALLABLE) {
        uint32_t function_index;
        if (runtime_value.kind != PIKA_RUNTIME_VALUE_CALLABLE ||
            runtime_value.as.integer < 0 ||
            (uint64_t)runtime_value.as.integer >=
                context->program->function_count) {
            return PIKA_STATUS_TYPE_MISMATCH;
        }
        function_index = (uint32_t)runtime_value.as.integer;
        value->as.callable.context = context;
        value->as.callable.function_index = (uint16_t)function_index;
        value->as.callable.parameter_count =
            context->program->functions[function_index].parameter_count;
        value->as.callable.active = 1u;
        return PIKA_STATUS_OK;
    }
    if (actual == PIKA_BINDING_VALUE_OPAQUE) {
        uint32_t object_index = runtime_value.as.object_index;
        PikaBindingObject* binding_object;
        if (runtime_value.kind != PIKA_RUNTIME_VALUE_NATIVE_OBJECT ||
            context->binding_storage == NULL ||
            context->binding_storage->objects == NULL ||
            object_index >= context->binding_object_count ||
            object_index >= context->binding_storage->object_capacity) {
            return PIKA_STATUS_TYPE_MISMATCH;
        }
        binding_object =
            &context->binding_storage->objects[object_index];
        if (binding_object->active == 0u) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        value->as.opaque = binding_object;
        return PIKA_STATUS_OK;
    }
    return PIKA_STATUS_TYPE_MISMATCH;
}

#if PIKA_CAPABILITY_BINDING_CONTAINER_ARGUMENT_ENABLE
static PikaStatus read_typed_container(
    PikaRuntimeContext* context,
    PikaRuntimeValue runtime_value,
    PikaBindingValueKind expected,
    PikaBindingValue* value,
    PikaBindingArgumentScratch* scratch) {
    PikaObjectSlot* object = pika_runtime_object(context, runtime_value);
    uint32_t index;
    if (object == NULL || scratch == NULL ||
        object->length > PIKA_BINDING_CONTAINER_ELEMENT_LIMIT ||
        (object->length > 0u && object->data == NULL)) {
        return object != NULL ? PIKA_STATUS_STORAGE_TOO_SMALL
                              : PIKA_STATUS_INVALID_OPERAND;
    }
    memset(value, 0, sizeof(*value));
    value->kind = expected;
    if (expected == PIKA_BINDING_VALUE_LIST ||
        expected == PIKA_BINDING_VALUE_TUPLE) {
        PikaRuntimeValueKind required =
            expected == PIKA_BINDING_VALUE_LIST
                ? PIKA_RUNTIME_VALUE_LIST
                : PIKA_RUNTIME_VALUE_TUPLE;
        PikaBindingValue* destination;
        if (runtime_value.kind != required ||
            scratch->value_count + object->length >
                PIKA_BINDING_CONTAINER_ELEMENT_LIMIT) {
            return runtime_value.kind != required
                       ? PIKA_STATUS_TYPE_MISMATCH
                       : PIKA_STATUS_STORAGE_TOO_SMALL;
        }
        destination = &scratch->values[scratch->value_count];
        scratch->value_count += object->length;
        for (index = 0u; index < object->length; ++index) {
            PikaStatus status = read_typed_scalar(
                context, ((PikaRuntimeValue*)object->data)[index],
                PIKA_BINDING_VALUE_ANY, &destination[index]);
            if (status != PIKA_STATUS_OK) return status;
        }
        value->as.sequence.values = destination;
        value->as.sequence.length = object->length;
        return PIKA_STATUS_OK;
    }
    if (expected == PIKA_BINDING_VALUE_DICT) {
        PikaBindingMapEntry* destination;
        if (runtime_value.kind != PIKA_RUNTIME_VALUE_DICT ||
            scratch->entry_count + object->length >
                PIKA_BINDING_CONTAINER_ELEMENT_LIMIT) {
            return runtime_value.kind != PIKA_RUNTIME_VALUE_DICT
                       ? PIKA_STATUS_TYPE_MISMATCH
                       : PIKA_STATUS_STORAGE_TOO_SMALL;
        }
        destination = &scratch->entries[scratch->entry_count];
        scratch->entry_count += object->length;
        for (index = 0u; index < object->length; ++index) {
            PikaMapEntry* source =
                &((PikaMapEntry*)object->data)[index];
            PikaStatus status = read_typed_scalar(
                context, source->key, PIKA_BINDING_VALUE_ANY,
                &destination[index].key);
            if (status == PIKA_STATUS_OK) {
                status = read_typed_scalar(
                    context, source->value, PIKA_BINDING_VALUE_ANY,
                    &destination[index].value);
            }
            if (status != PIKA_STATUS_OK) return status;
        }
        value->as.mapping.entries = destination;
        value->as.mapping.length = object->length;
        return PIKA_STATUS_OK;
    }
    return PIKA_STATUS_TYPE_MISMATCH;
}
#endif

static PikaStatus read_typed_argument(
    PikaRuntimeContext* context,
    uint32_t index,
    PikaBindingValueKind expected,
    PikaBindingValue* value
#if PIKA_CAPABILITY_BINDING_CONTAINER_ARGUMENT_ENABLE
    , PikaBindingArgumentScratch* scratch
#endif
    ) {
    PikaRuntimeValue runtime_value =
        pika_runtime_read_value(context, index);
    PikaBindingValueKind actual = expected;
    if (expected == PIKA_BINDING_VALUE_ANY) {
        actual = binding_kind_from_runtime(runtime_value.kind);
    }
#if PIKA_CAPABILITY_BINDING_CONTAINER_ARGUMENT_ENABLE
    if (actual == PIKA_BINDING_VALUE_LIST ||
        actual == PIKA_BINDING_VALUE_TUPLE ||
        actual == PIKA_BINDING_VALUE_DICT) {
        return read_typed_container(
            context, runtime_value, actual, value, scratch);
    }
#endif
    return read_typed_scalar(
        context, runtime_value, actual, value);
}

static PikaStatus read_argument(
    PikaRuntimeContext* context,
    const PikaRuntimeFrame* frame,
    uint8_t slot,
    PikaBindingValueKind kind,
    PikaBindingValue* value
#if PIKA_CAPABILITY_BINDING_CONTAINER_ARGUMENT_ENABLE
    , PikaBindingArgumentScratch* scratch
#endif
    ) {
    uint32_t index = frame->value_base + slot;
    if (slot >= frame->slot_count) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (context->typed_values != NULL &&
        kind != PIKA_BINDING_VALUE_OPAQUE) {
        return read_typed_argument(
            context, index, kind, value
#if PIKA_CAPABILITY_BINDING_CONTAINER_ARGUMENT_ENABLE
            , scratch
#endif
            );
    }
    if (context->typed_values != NULL &&
        kind == PIKA_BINDING_VALUE_OPAQUE) {
        PikaRuntimeValue runtime_value =
            pika_runtime_read_value(context, index);
        if (runtime_value.kind ==
            PIKA_RUNTIME_VALUE_NATIVE_OBJECT) {
            return read_typed_argument(
                context, index, kind, value
#if PIKA_CAPABILITY_BINDING_CONTAINER_ARGUMENT_ENABLE
                , scratch
#endif
                );
        }
    }
    value->kind = kind;
    if (kind == PIKA_BINDING_VALUE_INTEGER) {
        value->as.integer = context->values[index];
        return PIKA_STATUS_OK;
    }
    if (kind == PIKA_BINDING_VALUE_BOOLEAN) {
        value->as.boolean =
            context->values[index] != 0 ? 1u : 0u;
        return PIKA_STATUS_OK;
    }
    if (context->binding_storage == NULL ||
        context->binding_storage->values == NULL) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    *value = context->binding_storage->values[index];
    return value->kind == kind
               ? PIKA_STATUS_OK
               : PIKA_STATUS_TYPE_MISMATCH;
}

static PikaStatus binding_object_to_runtime(
    PikaRuntimeContext* context,
    const PikaBindingValue* value,
    PikaRuntimeValue* result) {
    uint32_t index;
    if (context->binding_storage == NULL ||
        context->binding_storage->objects == NULL) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    if (context->binding_object_count >
        context->binding_storage->object_capacity) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    for (index = 0u;
         index < context->binding_object_count;
         ++index) {
        PikaBindingObject* object =
            &context->binding_storage->objects[index];
        if ((void*)object == value->as.opaque &&
            object->active != 0u) {
            memset(result, 0, sizeof(*result));
            result->kind =
                PIKA_RUNTIME_VALUE_NATIVE_OBJECT;
            result->as.object_index = index;
            return PIKA_STATUS_OK;
        }
    }
    return PIKA_STATUS_INVALID_OPERAND;
}

static PikaStatus binding_scalar_to_runtime(
    PikaRuntimeContext* context,
    const PikaBindingValue* value,
    PikaRuntimeValue* result) {
    memset(result, 0, sizeof(*result));
    if (value->kind == PIKA_BINDING_VALUE_NONE) {
        result->kind = PIKA_RUNTIME_VALUE_NONE;
        return PIKA_STATUS_OK;
    }
    if (value->kind == PIKA_BINDING_VALUE_BOOLEAN) {
        result->kind = PIKA_RUNTIME_VALUE_BOOLEAN;
        result->as.integer = value->as.boolean != 0u ? 1 : 0;
        return PIKA_STATUS_OK;
    }
    if (value->kind == PIKA_BINDING_VALUE_INTEGER) {
        result->kind = PIKA_RUNTIME_VALUE_INTEGER;
        result->as.integer = value->as.integer;
        return PIKA_STATUS_OK;
    }
    if (value->kind == PIKA_BINDING_VALUE_FLOAT) {
        result->kind = PIKA_RUNTIME_VALUE_FLOAT;
        result->as.floating = value->as.floating;
        return PIKA_STATUS_OK;
    }
    if (value->kind == PIKA_BINDING_VALUE_STRING ||
        value->kind == PIKA_BINDING_VALUE_BYTES) {
        if (value->as.buffer.length > PIKA_BINDING_RESULT_BYTE_LIMIT) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
        return pika_runtime_create_text(
            context,
            value->kind == PIKA_BINDING_VALUE_STRING
                ? PIKA_RUNTIME_VALUE_STRING
                : PIKA_RUNTIME_VALUE_BYTES,
            (const char*)value->as.buffer.data,
            value->as.buffer.length, result);
    }
    if (value->kind == PIKA_BINDING_VALUE_BYTEARRAY) {
        if (value->as.mutable_buffer.length >
            PIKA_BINDING_RESULT_BYTE_LIMIT) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
        return pika_runtime_create_text(
            context, PIKA_RUNTIME_VALUE_BYTEARRAY,
            (const char*)value->as.mutable_buffer.data,
            value->as.mutable_buffer.length, result);
    }
    if (value->kind == PIKA_BINDING_VALUE_CALLABLE) {
        if (value->as.callable.context != context ||
            value->as.callable.function_index >=
                context->program->function_count) {
            return PIKA_STATUS_INVALID_ARGUMENT;
        }
        result->kind = PIKA_RUNTIME_VALUE_CALLABLE;
        result->as.integer =
            value->as.callable.function_index;
        return PIKA_STATUS_OK;
    }
    return PIKA_STATUS_TYPE_MISMATCH;
}

static PikaStatus binding_sequence_to_runtime(
    PikaRuntimeContext* context,
    const PikaBindingValue* value,
    PikaRuntimeValueKind kind,
    PikaRuntimeValue* result) {
    uint32_t object_index;
    void* data = NULL;
    uint32_t item;
    PikaStatus status;
#if PIKA_GC_ENABLE
    ++context->gc_retry_block_depth;
#endif
    status = pika_runtime_create_object(
        context, kind, value->as.sequence.length,
        (uint32_t)sizeof(PikaRuntimeValue), &object_index, &data);
    for (item = 0u;
         status == PIKA_STATUS_OK &&
         item < value->as.sequence.length;
         ++item) {
        status = binding_scalar_to_runtime(
            context,
            &value->as.sequence.values[item],
            &((PikaRuntimeValue*)data)[item]);
    }
    if (status == PIKA_STATUS_OK) {
        memset(result, 0, sizeof(*result));
        result->kind = kind;
        result->as.object_index = object_index;
    }
#if PIKA_GC_ENABLE
    --context->gc_retry_block_depth;
#endif
    return status;
}

static PikaStatus binding_mapping_to_runtime(
    PikaRuntimeContext* context,
    const PikaBindingValue* value,
    PikaRuntimeValue* result) {
    uint32_t object_index;
    void* data = NULL;
    uint32_t item;
    PikaStatus status;
#if PIKA_GC_ENABLE
    ++context->gc_retry_block_depth;
#endif
    status = pika_runtime_create_object(
        context, PIKA_RUNTIME_VALUE_DICT,
        value->as.mapping.length,
        (uint32_t)sizeof(PikaMapEntry), &object_index, &data);
    for (item = 0u;
         status == PIKA_STATUS_OK &&
         item < value->as.mapping.length;
         ++item) {
        PikaMapEntry* entry = &((PikaMapEntry*)data)[item];
        status = binding_scalar_to_runtime(
            context, &value->as.mapping.entries[item].key,
            &entry->key);
        if (status == PIKA_STATUS_OK) {
            status = binding_scalar_to_runtime(
                context, &value->as.mapping.entries[item].value,
                &entry->value);
        }
    }
    if (status == PIKA_STATUS_OK) {
        memset(result, 0, sizeof(*result));
        result->kind = PIKA_RUNTIME_VALUE_DICT;
        result->as.object_index = object_index;
    }
#if PIKA_GC_ENABLE
    --context->gc_retry_block_depth;
#endif
    return status;
}

static PikaStatus binding_value_to_runtime(
    PikaRuntimeContext* context,
    const PikaBindingValue* value,
    PikaRuntimeValue* result) {
    if (value->kind == PIKA_BINDING_VALUE_LIST ||
        value->kind == PIKA_BINDING_VALUE_TUPLE) {
        return binding_sequence_to_runtime(
            context, value,
            value->kind == PIKA_BINDING_VALUE_LIST
                ? PIKA_RUNTIME_VALUE_LIST
                : PIKA_RUNTIME_VALUE_TUPLE,
            result);
    }
    if (value->kind == PIKA_BINDING_VALUE_DICT) {
        return binding_mapping_to_runtime(
            context, value, result);
    }
    return binding_scalar_to_runtime(context, value, result);
}

static PikaStatus write_result(
    PikaRuntimeContext* context,
    const PikaRuntimeFrame* frame,
    uint8_t slot,
    const PikaBindingValue* value) {
    uint32_t index = frame->value_base + slot;
    if (slot >= frame->slot_count) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (context->typed_values != NULL &&
        value->kind == PIKA_BINDING_VALUE_OPAQUE) {
        PikaRuntimeValue runtime_value;
        PikaStatus status = binding_object_to_runtime(
            context, value, &runtime_value);
        if (status != PIKA_STATUS_OK) return status;
        return pika_runtime_write_value(
            context, index, runtime_value);
    }
    if (context->typed_values != NULL &&
        value->kind != PIKA_BINDING_VALUE_OPAQUE) {
        PikaRuntimeValue runtime_value;
        uint16_t object_count =
            context->objects != NULL ? context->objects->count : 0u;
        uint32_t arena_used =
            context->objects != NULL ? context->objects->arena_used : 0u;
        PikaStatus status = binding_value_to_runtime(
            context, value, &runtime_value);
        if (status == PIKA_STATUS_OK) {
            status = pika_runtime_write_value(
                context, index, runtime_value);
        }
        if (status != PIKA_STATUS_OK && context->objects != NULL) {
            PikaObjectStorage* objects = context->objects;
            uint16_t object_index;
            for (object_index = object_count;
                 object_index < objects->count;
                 ++object_index) {
                PikaObjectSlot* object =
                    &objects->slots[object_index];
                if (object->active != 0u && object->owned != 0u &&
                    objects->release != NULL) {
                    objects->release(objects->context, object->data);
                }
                memset(object, 0, sizeof(*object));
            }
            objects->count = object_count;
            objects->arena_used = arena_used;
        }
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        return PIKA_STATUS_OK;
    }
    if (value->kind == PIKA_BINDING_VALUE_INTEGER) {
        context->values[index] = value->as.integer;
        return PIKA_STATUS_OK;
    }
    if (value->kind == PIKA_BINDING_VALUE_BOOLEAN) {
        context->values[index] =
            value->as.boolean != 0u ? 1 : 0;
        return PIKA_STATUS_OK;
    }
    if (value->kind == PIKA_BINDING_VALUE_NONE) {
        context->values[index] = 0;
        return PIKA_STATUS_OK;
    }
    if (context->binding_storage == NULL ||
        context->binding_storage->values == NULL) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    context->binding_storage->values[index] = *value;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding_schedule(
    const PikaBindingPythonCallable* callable,
    const PikaBindingValue* arguments,
    uint8_t argument_count) {
    PikaRuntimeContext* context;
    const PikaFunction* function;
    PikaRuntimeFrame* frame;
    uint32_t original_depth;
    uint32_t index;
    PikaStatus status;
    if (callable == NULL || callable->context == NULL ||
        callable->active == 0u ||
        argument_count > PIKA_BINDING_ARGUMENT_LIMIT ||
        (argument_count > 0u && arguments == NULL)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (argument_count != callable->parameter_count) {
        return PIKA_STATUS_ARGUMENT_COUNT;
    }
    context = (PikaRuntimeContext*)callable->context;
    if (context->binding_callback_active == 0u ||
        context->depth != context->binding_callback_depth ||
        callable->function_index >=
            context->program->function_count ||
        context->typed_values == NULL ||
        context->binding_storage == NULL) {
        return PIKA_STATUS_CALLBACK_ERROR;
    }
    function = &context->program
                    ->functions[callable->function_index];
    if (function->parameter_count != argument_count) {
        return PIKA_STATUS_ARGUMENT_COUNT;
    }
    for (index = 0u; index < argument_count; ++index) {
        PikaBindingValueKind kind = arguments[index].kind;
        if ((uint32_t)kind >
                (uint32_t)PIKA_BINDING_VALUE_FLOAT &&
            kind != PIKA_BINDING_VALUE_OPAQUE) {
            return PIKA_STATUS_TYPE_MISMATCH;
        }
        if (kind == PIKA_BINDING_VALUE_OPAQUE &&
            arguments[index].as.opaque == NULL) {
            return PIKA_STATUS_TYPE_MISMATCH;
        }
    }
    original_depth = context->depth;
    status = pika_runtime_push_callback(
        context, callable->function_index);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    frame = &context->frames[context->depth];
    for (index = 0u; index < argument_count; ++index) {
        uint32_t value_index = frame->value_base + index;
        context->binding_storage->values[value_index] =
            arguments[index];
        if (arguments[index].kind ==
            PIKA_BINDING_VALUE_OPAQUE) {
            context->typed_values[value_index].kind =
                PIKA_RUNTIME_VALUE_NONE;
        } else {
            status = binding_scalar_to_runtime(
                context, &arguments[index],
                &context->typed_values[value_index]);
            if (status != PIKA_STATUS_OK) {
                context->depth = original_depth;
                return status;
            }
        }
    }
    return PIKA_STATUS_OK;
}

static void account_binding_object(
    PikaRuntimeContext* context) {
    ++context->binding_object_count;
#if PIKA_GC_ENABLE
    pika_runtime_note_object_pressure(context);
#endif
    if (context->metrics != NULL &&
        context->binding_object_count >
            context->metrics->binding_object_peak) {
        context->metrics->binding_object_peak =
            context->binding_object_count;
        pika_runtime_update_storage_peak(context);
    }
}

static PikaStatus binding_call_inner(
    PikaRuntimeContext* context,
    const PikaInstruction* instruction,
    const PikaBindingCallSite* site
#if PIKA_CAPABILITY_BINDING_CONTAINER_ARGUMENT_ENABLE
    , PikaBindingArgumentScratch* argument_scratch
#endif
    ) {
    PikaBindingValue arguments[PIKA_BINDING_ARGUMENT_LIMIT];
    PikaBindingValue result;
    PikaBindingObject* created_object = NULL;
    PikaRuntimeFrame* frame;
    uint32_t original_depth;
    uint32_t index;
    int allows_callback;
    int creates_object;
#if PIKA_GC_ENABLE
    int collected_objects = 0;
#endif
    PikaStatus status = PIKA_STATUS_OK;
    frame = &context->frames[context->depth];
    original_depth = context->depth;
    allows_callback =
        context->program->allows_binding_callback != 0u;
    creates_object =
        site->kind == PIKA_BINDING_CALL_CONSTRUCTOR;
#if PIKA_CAPABILITY_BINDING_OBJECT_RESULT_ENABLE
    creates_object =
        creates_object ||
        site->result_kind == PIKA_BINDING_VALUE_OBJECT;
#endif
    if (creates_object &&
        (context->binding_storage == NULL ||
         context->binding_storage->objects == NULL)) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    if (creates_object &&
        context->binding_object_count >
            context->binding_storage->object_capacity) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
#if PIKA_GC_ENABLE
    if (creates_object &&
        context->object_collection_pending != 0u) {
        pika_runtime_collect_objects(context);
        collected_objects = 1;
    }
#endif
    if (creates_object &&
        context->binding_object_count >=
            context->binding_storage->object_capacity) {
#if PIKA_GC_ENABLE
        if (collected_objects == 0) {
            pika_runtime_collect_objects(context);
        }
#endif
        if (context->binding_object_count >=
            context->binding_storage->object_capacity) {
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
    }
    for (index = 0u; index < site->argument_count; ++index) {
        status = read_argument(
            context, frame,
            (uint8_t)(instruction->b + index),
            (PikaBindingValueKind)site->argument_kinds[index],
            &arguments[index]
#if PIKA_CAPABILITY_BINDING_CONTAINER_ARGUMENT_ENABLE
            , argument_scratch
#endif
            );
        if (status != PIKA_STATUS_OK) {
            return status;
        }
    }
    if (context->binding_storage != NULL &&
        context->binding_object_count >
            context->binding_storage->object_capacity) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (allows_callback) {
        if (context->binding_callback_active != 0u) {
            return PIKA_STATUS_CALLBACK_ERROR;
        }
        context->binding_callback_active = 1u;
        context->binding_callback_depth = original_depth;
    }
    if (site->kind == PIKA_BINDING_CALL_FUNCTION) {
        status = pika_binding_invoke_call_site(
            site->module, site->callable, NULL, arguments,
            site->argument_count, &result, site->flags);
    } else if (site->kind == PIKA_BINDING_CALL_CONSTRUCTOR) {
        PikaBindingObject* object;
        object = &context->binding_storage
                      ->objects[context->binding_object_count];
        status = pika_binding_construct_call_site(
            site->module, site->class_descriptor, arguments,
            site->argument_count, object, site->flags);
        if (status == PIKA_STATUS_OK) {
            created_object = object;
            account_binding_object(context);
            result.kind = PIKA_BINDING_VALUE_OPAQUE;
            result.as.opaque = object;
        }
    } else if (site->kind == PIKA_BINDING_CALL_METHOD) {
        PikaBindingValue receiver;
        status = read_argument(
            context, frame, site->receiver_slot,
            PIKA_BINDING_VALUE_OPAQUE, &receiver
#if PIKA_CAPABILITY_BINDING_CONTAINER_ARGUMENT_ENABLE
            , argument_scratch
#endif
            );
        if (status == PIKA_STATUS_OK) {
            PikaBindingObject* receiver_object =
                (PikaBindingObject*)receiver.as.opaque;
            status = pika_binding_invoke_method_call_site(
                site->module,
                receiver_object,
                site->callable, arguments,
                site->argument_count, &result, site->flags);
            if (status == PIKA_STATUS_OK &&
                (site->callable->flags &
                 PIKA_BINDING_CALLABLE_DEINITIALIZER) != 0u) {
                pika_binding_object_release(receiver_object);
#if PIKA_GC_ENABLE
                context->object_collection_pending = 1u;
#endif
            }
        }
    } else {
        status = PIKA_STATUS_INVALID_OPERAND;
    }
#if PIKA_CAPABILITY_BINDING_OBJECT_RESULT_ENABLE
    if (status == PIKA_STATUS_OK &&
        site->result_kind == PIKA_BINDING_VALUE_OBJECT) {
        PikaBindingObject* object =
            &context->binding_storage
                 ->objects[context->binding_object_count];
        status = pika_binding_object_adopt_result(
            site->module, site->callable, &result, object);
        if (status == PIKA_STATUS_OK) {
            created_object = object;
            account_binding_object(context);
            result.kind = PIKA_BINDING_VALUE_OPAQUE;
            result.as.opaque = object;
        }
    }
#endif
    if (allows_callback) {
        context->binding_callback_active = 0u;
    }
    if (status != PIKA_STATUS_OK) {
        if (allows_callback) {
            context->depth = original_depth;
        }
        return status;
    }
    if (context->metrics != NULL) {
        ++context->metrics->binding_call_count;
    }
    status = write_result(
        context, frame, instruction->a, &result);
    if (status != PIKA_STATUS_OK &&
        created_object != NULL) {
        pika_binding_object_release(created_object);
        --context->binding_object_count;
    }
    if (allows_callback && status != PIKA_STATUS_OK) {
        context->depth = original_depth;
    }
    return status;
}

#if PIKA_CAPABILITY_BINDING_CONTAINER_ARGUMENT_ENABLE
static PikaStatus binding_call_with_container_scratch(
    PikaRuntimeContext* context,
    const PikaInstruction* instruction,
    const PikaBindingCallSite* site) {
    PikaBindingArgumentScratch scratch;
    scratch.value_count = 0u;
    scratch.entry_count = 0u;
    return binding_call_inner(
        context, instruction, site, &scratch);
}
#endif

PikaStatus pika_runtime_binding_call(
    PikaRuntimeContext* context,
    const PikaInstruction* instruction) {
    const PikaBindingCallSite* site;
    if (context == NULL || instruction == NULL ||
        context->program == NULL || context->frames == NULL ||
        context->program->binding_calls == NULL ||
        instruction->immediate < 0 ||
        (uint32_t)instruction->immediate >=
            context->program->binding_call_count) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    site = &context->program
                ->binding_calls[instruction->immediate];
    if (site->argument_count > PIKA_BINDING_ARGUMENT_LIMIT) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
#if PIKA_CAPABILITY_BINDING_CONTAINER_ARGUMENT_ENABLE
    if ((site->flags &
         PIKA_BINDING_CALL_SITE_CONTAINER_ARGUMENT) != 0u) {
        return binding_call_with_container_scratch(
            context, instruction, site);
    }
    return binding_call_inner(
        context, instruction, site, NULL);
#else
    return binding_call_inner(context, instruction, site);
#endif
}

PikaStatus pika_runtime_execute_binding_opcode(
    PikaRuntimeContext* context,
    const PikaInstruction* instruction) {
    PikaRuntimeFrame* frame;
    PikaStatus status;
    if (context == NULL || instruction == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    frame = &context->frames[context->depth];
    if (instruction->opcode == PIKA_OP_BIND_MOVE) {
        if (context->binding_storage == NULL ||
            context->binding_storage->values == NULL) {
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
        context->binding_storage
            ->values[frame->value_base + instruction->a] =
            context->binding_storage
                ->values[frame->value_base + instruction->b];
        ++frame->instruction_index;
        return PIKA_STATUS_OK;
    }
    if (instruction->opcode != PIKA_OP_BIND_CALL) {
        return pika_runtime_execute_fallback_opcode(
            context, instruction);
    }
    status = pika_runtime_binding_call(context, instruction);
    if (status == PIKA_STATUS_OK) {
        ++frame->instruction_index;
    }
    return status;
}

#endif
