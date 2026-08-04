/* SPEC: PJ2026-0501 V2 kernel v0.47; typed value and object storage. */
#include "runtime_internal.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#if PIKA_TYPED_RUNTIME_ENABLE

#define PIKA_RUNTIME_FIELD_ASSIGNED 1u

static PikaRuntimeValue runtime_integer_value(int64_t integer) {
    PikaRuntimeValue value;
    memset(&value, 0, sizeof(value));
    value.kind = PIKA_RUNTIME_VALUE_INTEGER;
    value.as.integer = integer;
    return value;
}

static void release_pointer(PikaObjectStorage* storage, void* pointer) {
    if (pointer == NULL) {
        return;
    }
    if (storage->release != NULL) {
        storage->release(storage->context, pointer);
    }
}

static uint16_t object_slot_limit(
    const PikaObjectStorage* storage) {
    return storage->slot_limit != 0u
               ? storage->slot_limit
               : storage->capacity;
}

static uint32_t object_arena_limit(
    const PikaObjectStorage* storage) {
    return storage->arena_limit != 0u
               ? storage->arena_limit
               : storage->arena_capacity;
}

static int object_arena_offset(
    const PikaObjectStorage* storage,
    const void* pointer,
    size_t size,
    uint32_t* offset) {
    uintptr_t arena_address;
    uintptr_t pointer_address;
    uintptr_t distance;
    if (storage == NULL || storage->arena == NULL ||
        pointer == NULL || offset == NULL) {
        return 0;
    }
    arena_address = (uintptr_t)storage->arena;
    pointer_address = (uintptr_t)pointer;
    if (pointer_address < arena_address) return 0;
    distance = pointer_address - arena_address;
    if (distance > storage->arena_used ||
        size > (size_t)(storage->arena_used - (uint32_t)distance)) {
        return 0;
    }
    *offset = (uint32_t)distance;
    return 1;
}

static PikaStatus reserve_object_storage(
    PikaObjectStorage* storage,
    uint16_t slot_capacity,
    uint32_t arena_capacity) {
    PikaStatus status;
    if (storage == NULL ||
        slot_capacity > object_slot_limit(storage) ||
        arena_capacity > object_arena_limit(storage)) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    if ((slot_capacity == 0u ||
         (storage->slots != NULL &&
          storage->capacity >= slot_capacity)) &&
        (arena_capacity == 0u ||
         (storage->arena != NULL &&
          storage->arena_capacity >= arena_capacity))) {
        return PIKA_STATUS_OK;
    }
    if (storage->reserve == NULL) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    status = storage->reserve(
        storage->context, slot_capacity, arena_capacity);
    if (status != PIKA_STATUS_OK) return status;
    if ((slot_capacity > 0u &&
         (storage->slots == NULL ||
          storage->capacity < slot_capacity)) ||
        (arena_capacity > 0u &&
         (storage->arena == NULL ||
          storage->arena_capacity < arena_capacity))) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    return PIKA_STATUS_OK;
}

void pika_object_storage_reset(PikaObjectStorage* storage) {
    uint16_t index;
    if (storage == NULL) return;
    if (storage->slots != NULL) {
        for (index = 0u; index < storage->count; ++index) {
            PikaObjectSlot* slot = &storage->slots[index];
            if (slot->active != 0u && slot->owned != 0u) {
                release_pointer(storage, slot->data);
            }
        }
        memset(storage->slots, 0,
               (size_t)storage->capacity * sizeof(PikaObjectSlot));
    }
    storage->count = 0u;
    storage->arena_used = 0u;
}

void pika_object_storage_release(PikaObjectStorage* storage) {
    pika_object_storage_reset(storage);
}

static void* allocate_object_data(PikaObjectStorage* storage,
                                   uint32_t size,
                                   uint8_t* owned) {
    uint32_t alignment = (uint32_t)sizeof(void*);
    uint32_t aligned;
    uint32_t required;
    void* pointer = NULL;
    if (storage->arena_used > UINT32_MAX - alignment + 1u) {
        return NULL;
    }
    aligned = (storage->arena_used + alignment - 1u) /
              alignment * alignment;
    if (size <= UINT32_MAX - aligned) {
        required = aligned + size;
        if ((storage->arena == NULL ||
             required > storage->arena_capacity) &&
            reserve_object_storage(
                storage, storage->capacity, required) ==
                PIKA_STATUS_OK) {
            aligned = (storage->arena_used + alignment - 1u) /
                      alignment * alignment;
        }
    }
    if (storage->arena != NULL) {
        if (aligned <= storage->arena_capacity &&
            size <= storage->arena_capacity - aligned) {
            pointer = &storage->arena[aligned];
            storage->arena_used = aligned + size;
            *owned = 0u;
            return pointer;
        }
    }
    if (storage->allocate != NULL && storage->release != NULL) {
        pointer = storage->allocate(storage->context, size);
    }
    if (pointer != NULL) {
        *owned = 1u;
    }
    return pointer;
}

static PikaStatus reserve_object_data(PikaRuntimeContext* context,
                                      PikaObjectSlot** slot_pointer,
                                      uint32_t required,
                                      uint32_t element_size) {
    PikaObjectStorage* storage = context->objects;
    PikaObjectSlot* slot = *slot_pointer;
    uint32_t capacity;
    uint32_t size;
    uint32_t old_size;
    uint8_t owned = 0u;
    void* pointer;
    if (required <= slot->capacity && slot->data != NULL) {
        return PIKA_STATUS_OK;
    }
    capacity = slot->capacity;
    if (capacity == 0u) capacity = 1u;
    while (capacity < required) {
        if (capacity > UINT32_MAX / 2u) {
            capacity = required;
            break;
        }
        capacity *= 2u;
    }
    if (capacity > UINT32_MAX / element_size) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    size = capacity * element_size;
    pointer = allocate_object_data(storage, size, &owned);
#if PIKA_GC_ENABLE
    if (pointer == NULL && context->typed_values != NULL &&
        context->object_collection_pending != 0u &&
        context->gc_retry_block_depth == 0u &&
        context->gc_root_active == 0u) {
        uint32_t object_index =
            (uint32_t)(slot - storage->slots);
        memset(&context->gc_root, 0, sizeof(context->gc_root));
        context->gc_root.kind =
            (PikaRuntimeValueKind)slot->kind;
        context->gc_root.as.object_index = object_index;
        context->gc_root_active = 1u;
        pika_runtime_collect_objects(context);
        object_index = context->gc_root.as.object_index;
        context->gc_root_active = 0u;
        if (object_index < storage->count) {
            slot = &storage->slots[object_index];
            *slot_pointer = slot;
            pointer = allocate_object_data(storage, size, &owned);
        }
    }
#endif
    if (pointer == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
    memset(pointer, 0, size);
    old_size = slot->length;
    if (slot->data != NULL && old_size != 0u) {
        memcpy(pointer, slot->data, old_size * element_size);
    }
    if (slot->owned != 0u) release_pointer(storage, slot->data);
    slot->data = pointer;
    slot->capacity = capacity;
    slot->owned = owned;
#if PIKA_GC_ENABLE
    pika_runtime_note_object_pressure(context);
#endif
    if (slot->kind == PIKA_RUNTIME_VALUE_BYTEARRAY) {
        ((char*)slot->data)[slot->length] = '\0';
    }
    return PIKA_STATUS_OK;
}

PikaStatus pika_runtime_create_object(
    PikaRuntimeContext* context,
    PikaRuntimeValueKind kind,
    uint32_t length,
    uint32_t element_size,
    uint32_t* object_index,
    void** data) {
    PikaObjectStorage* storage;
    PikaObjectSlot* slot;
    uint8_t owned = 0u;
    uint32_t size;
    void* pointer = NULL;
#if PIKA_GC_ENABLE
    int collected = 0;
#endif
    if (context == NULL || context->objects == NULL ||
        object_index == NULL || data == NULL) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    storage = context->objects;
    if ((storage->slots == NULL && storage->reserve == NULL) ||
        (element_size != 0u &&
         length > UINT32_MAX / element_size)) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
#if PIKA_GC_ENABLE
retry:
#endif
    if (storage->count >= storage->capacity) {
#if PIKA_GC_ENABLE
        if (!collected && context->typed_values != NULL &&
            context->object_collection_pending != 0u &&
            context->gc_retry_block_depth == 0u) {
            collected = 1;
            pika_runtime_collect_objects(context);
            goto retry;
        }
#endif
        if (storage->count == UINT16_MAX ||
            reserve_object_storage(
                storage,
                (uint16_t)(storage->count + 1u),
                storage->arena_used) != PIKA_STATUS_OK) {
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
    }
    size = length * element_size;
    if (size != 0u) {
        pointer = allocate_object_data(storage, size, &owned);
        if (pointer == NULL) {
#if PIKA_GC_ENABLE
            if (!collected && context->typed_values != NULL &&
                context->object_collection_pending != 0u &&
                context->gc_retry_block_depth == 0u) {
                collected = 1;
                pika_runtime_collect_objects(context);
                goto retry;
            }
#endif
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
        memset(pointer, 0, size);
    }
    *object_index = storage->count;
    slot = &storage->slots[storage->count++];
    slot->kind = (uint8_t)kind;
    slot->active = 1u;
    slot->owned = owned;
    slot->length = length;
    slot->capacity = length;
    slot->data = pointer;
    *data = pointer;
    if (context->metrics != NULL &&
        storage->count > context->metrics->object_peak) {
        context->metrics->object_peak = storage->count;
    }
#if PIKA_GC_ENABLE
    pika_runtime_note_object_pressure(context);
#endif
    return PIKA_STATUS_OK;
}

PikaObjectSlot* pika_runtime_object(
    PikaRuntimeContext* context,
    PikaRuntimeValue value) {
    if (context == NULL || context->objects == NULL ||
        context->objects->slots == NULL ||
        value.as.object_index >= context->objects->count) {
        return NULL;
    }
    if (context->objects->slots[value.as.object_index].active == 0u) {
        return NULL;
    }
    return &context->objects->slots[value.as.object_index];
}

PikaRuntimeValue pika_runtime_read_value(
    const PikaRuntimeContext* context,
    uint32_t index) {
    PikaRuntimeValue value;
    memset(&value, 0, sizeof(value));
    if (context == NULL) {
        return value;
    }
    if (context->typed_values != NULL) {
        return context->typed_values[index];
    }
    value.kind = PIKA_RUNTIME_VALUE_INTEGER;
    value.as.integer = context->values[index];
    return value;
}

PikaStatus pika_runtime_write_value(
    PikaRuntimeContext* context,
    uint32_t index,
    PikaRuntimeValue value) {
    if (context == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (context->typed_values != NULL) {
        context->typed_values[index] = value;
        return PIKA_STATUS_OK;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_INTEGER ||
        value.kind == PIKA_RUNTIME_VALUE_BOOLEAN ||
        value.kind == PIKA_RUNTIME_VALUE_NONE) {
        context->values[index] =
            value.kind == PIKA_RUNTIME_VALUE_INTEGER
                ? value.as.integer
                : value.kind == PIKA_RUNTIME_VALUE_BOOLEAN
                      ? (value.as.integer != 0 ? 1 : 0)
                      : 0;
        return PIKA_STATUS_OK;
    }
    return PIKA_STATUS_TYPE_MISMATCH;
}

int pika_runtime_value_truthy(
    const PikaRuntimeContext* context,
    PikaRuntimeValue value) {
    PikaObjectSlot* slot;
    if (value.kind == PIKA_RUNTIME_VALUE_NONE) {
        return 0;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_BOOLEAN ||
        value.kind == PIKA_RUNTIME_VALUE_INTEGER) {
        return value.as.integer != 0;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_FLOAT) {
        return value.as.floating != 0.0;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_RANGE) {
        uint32_t length = 0u;
        return pika_runtime_range_length(
                   (PikaRuntimeContext*)context, value, &length) ==
                   PIKA_STATUS_OK &&
               length != 0u;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_INSTANCE ||
        value.kind == PIKA_RUNTIME_VALUE_NATIVE_OBJECT) {
        return 1;
    }
    slot = pika_runtime_object((PikaRuntimeContext*)context, value);
    return slot != NULL && slot->length != 0u;
}

static int is_value_sequence(PikaRuntimeValueKind kind) {
    return kind == PIKA_RUNTIME_VALUE_LIST ||
           kind == PIKA_RUNTIME_VALUE_TUPLE ||
           kind == PIKA_RUNTIME_VALUE_SET;
}

static int is_byte_sequence(PikaRuntimeValueKind kind) {
    return kind == PIKA_RUNTIME_VALUE_BYTES ||
           kind == PIKA_RUNTIME_VALUE_BYTEARRAY;
}

#if PIKA_GC_ENABLE

typedef void (*PikaRuntimeValueVisitor)(
    PikaRuntimeValue* value,
    void* context);

typedef struct {
    uint32_t source;
    uint32_t destination;
} PikaObjectIndexRemap;

static int is_object_value_kind(PikaRuntimeValueKind kind) {
    return is_value_sequence(kind) ||
           is_byte_sequence(kind) ||
           kind == PIKA_RUNTIME_VALUE_STRING ||
           kind == PIKA_RUNTIME_VALUE_DICT ||
           kind == PIKA_RUNTIME_VALUE_RANGE ||
           kind == PIKA_RUNTIME_VALUE_ITERATOR ||
           kind == PIKA_RUNTIME_VALUE_SLICE ||
           kind == PIKA_RUNTIME_VALUE_TYPE ||
           kind == PIKA_RUNTIME_VALUE_MODULE ||
#if PIKA_CAPABILITY_EXCEPTION_BASIC_ENABLE
           kind == PIKA_RUNTIME_VALUE_INSTANCE ||
           kind == PIKA_RUNTIME_VALUE_EXCEPTION;
#else
           kind == PIKA_RUNTIME_VALUE_INSTANCE;
#endif
}

static int object_slot_payload_size(
    const PikaObjectSlot* slot,
    uint32_t* size) {
    uint32_t element_size;
    switch ((PikaRuntimeValueKind)slot->kind) {
        case PIKA_RUNTIME_VALUE_STRING:
        case PIKA_RUNTIME_VALUE_BYTES:
        case PIKA_RUNTIME_VALUE_BYTEARRAY:
        case PIKA_RUNTIME_VALUE_TYPE:
        case PIKA_RUNTIME_VALUE_MODULE:
            element_size = 1u;
            break;
        case PIKA_RUNTIME_VALUE_LIST:
        case PIKA_RUNTIME_VALUE_TUPLE:
        case PIKA_RUNTIME_VALUE_SET:
        case PIKA_RUNTIME_VALUE_INSTANCE:
            element_size = (uint32_t)sizeof(PikaRuntimeValue);
            break;
        case PIKA_RUNTIME_VALUE_DICT:
            element_size = (uint32_t)sizeof(PikaMapEntry);
            break;
        case PIKA_RUNTIME_VALUE_RANGE:
            element_size = (uint32_t)sizeof(PikaRangeData);
            break;
        case PIKA_RUNTIME_VALUE_ITERATOR:
            element_size = (uint32_t)sizeof(PikaIteratorData);
            break;
        case PIKA_RUNTIME_VALUE_SLICE:
            element_size = (uint32_t)sizeof(PikaSliceData);
            break;
        default:
            return 0;
    }
    if (slot->capacity > UINT32_MAX / element_size) {
        return 0;
    }
    *size = slot->capacity * element_size;
    return 1;
}

static void visit_object_values(
    PikaObjectSlot* slot,
    PikaRuntimeValueVisitor visitor,
    void* visitor_context) {
    uint32_t index;
    if (slot->data == NULL || slot->length > slot->capacity) {
        return;
    }
    if (slot->kind == PIKA_RUNTIME_VALUE_LIST ||
        slot->kind == PIKA_RUNTIME_VALUE_TUPLE ||
        slot->kind == PIKA_RUNTIME_VALUE_SET ||
        slot->kind == PIKA_RUNTIME_VALUE_INSTANCE) {
        PikaRuntimeValue* values =
            (PikaRuntimeValue*)slot->data;
        for (index = 0u; index < slot->length; ++index) {
            visitor(&values[index], visitor_context);
        }
        return;
    }
    if (slot->kind == PIKA_RUNTIME_VALUE_DICT) {
        PikaMapEntry* entries = (PikaMapEntry*)slot->data;
        for (index = 0u; index < slot->length; ++index) {
            visitor(&entries[index].key, visitor_context);
            visitor(&entries[index].value, visitor_context);
        }
        return;
    }
    if (slot->kind == PIKA_RUNTIME_VALUE_SLICE &&
        slot->length > 0u) {
        PikaSliceData* slice = (PikaSliceData*)slot->data;
        visitor(&slice->start, visitor_context);
        visitor(&slice->stop, visitor_context);
        visitor(&slice->step, visitor_context);
    }
}

static void mark_object_value(
    PikaRuntimeValue* value,
    void* context) {
    PikaObjectStorage* storage = (PikaObjectStorage*)context;
    uint32_t index;
    if (!is_object_value_kind(value->kind)) {
        return;
    }
    index = value->as.object_index;
    if (index < storage->count &&
        storage->slots[index].active == 1u) {
        storage->slots[index].active = 2u;
    }
}

static void remap_object_value(
    PikaRuntimeValue* value,
    void* context) {
    const PikaObjectIndexRemap* remap =
        (const PikaObjectIndexRemap*)context;
    if (is_object_value_kind(value->kind) &&
        value->as.object_index == remap->source) {
        value->as.object_index = remap->destination;
    }
}

static void visit_runtime_roots(
    PikaRuntimeContext* context,
    PikaRuntimeValueVisitor visitor,
    void* visitor_context) {
    uint32_t depth;
    if (context->frames != NULL &&
        context->typed_values != NULL) {
        for (depth = 0u; depth <= context->depth; ++depth) {
            const PikaRuntimeFrame* frame =
                &context->frames[depth];
            uint32_t count = frame->slot_count;
            uint32_t index;
            if (count > context->max_slots) {
                count = context->max_slots;
            }
            for (index = 0u; index < count; ++index) {
                visitor(
                    &context->typed_values[
                        frame->value_base + index],
                    visitor_context);
            }
        }
    }
    if (context->result != NULL) {
        visitor(
            &context->result->typed_value,
            visitor_context);
    }
    if (context->gc_root_active != 0u) {
        visitor(&context->gc_root, visitor_context);
    }
#if PIKA_CAPABILITY_EXCEPTION_BASIC_ENABLE
    if (context->has_pending_exception != 0u) {
        visitor(
            &context->pending_exception,
            visitor_context);
    }
#endif
}

static void mark_object_graph(PikaRuntimeContext* context) {
    PikaObjectStorage* storage = context->objects;
    uint32_t index;
    int pending;
    for (index = 0u; index < storage->count; ++index) {
        if (storage->slots[index].active != 0u) {
            storage->slots[index].active = 1u;
        }
    }
    visit_runtime_roots(
        context, mark_object_value, storage);
    do {
        pending = 0;
        for (index = 0u; index < storage->count; ++index) {
            PikaObjectSlot* slot = &storage->slots[index];
            if (slot->active != 2u) {
                continue;
            }
            slot->active = 3u;
            pending = 1;
            visit_object_values(
                slot, mark_object_value, storage);
            if (slot->kind ==
                    PIKA_RUNTIME_VALUE_ITERATOR &&
                slot->data != NULL &&
                slot->length > 0u &&
                slot->length <= slot->capacity) {
                PikaIteratorData* iterator =
                    (PikaIteratorData*)slot->data;
                if (iterator->source_index <
                        storage->count &&
                    storage->slots[
                        iterator->source_index]
                            .active == 1u) {
                    storage->slots[
                        iterator->source_index]
                            .active = 2u;
                }
            }
        }
    } while (pending != 0);
}

static void remap_object_graph(
    PikaRuntimeContext* context,
    uint32_t source,
    uint32_t destination) {
    PikaObjectStorage* storage = context->objects;
    PikaObjectIndexRemap remap = {source, destination};
    uint32_t index;
    visit_runtime_roots(
        context, remap_object_value, &remap);
    for (index = 0u; index < storage->count; ++index) {
        PikaObjectSlot* slot = &storage->slots[index];
        if (slot->active == 0u) {
            continue;
        }
        visit_object_values(
            slot, remap_object_value, &remap);
        if (slot->kind ==
                PIKA_RUNTIME_VALUE_ITERATOR &&
            slot->data != NULL &&
            slot->length > 0u &&
            slot->length <= slot->capacity) {
            PikaIteratorData* iterator =
                (PikaIteratorData*)slot->data;
            if (iterator->source_index == source) {
                iterator->source_index = destination;
            }
        }
    }
}

static void compact_object_slots(
    PikaRuntimeContext* context) {
    PikaObjectStorage* storage = context->objects;
    uint32_t old_count = storage->count;
    uint32_t destination = 0u;
    uint32_t source;
    for (source = 0u; source < old_count; ++source) {
        PikaObjectSlot* slot = &storage->slots[source];
        if (slot->active == 1u) {
            if (slot->owned != 0u) {
                release_pointer(storage, slot->data);
            }
            memset(slot, 0, sizeof(*slot));
        }
    }
    for (source = 0u; source < old_count; ++source) {
        PikaObjectSlot* slot = &storage->slots[source];
        if (slot->active != 3u) {
            continue;
        }
        if (destination != source) {
            storage->slots[destination] = *slot;
            storage->slots[destination].active = 1u;
            memset(slot, 0, sizeof(*slot));
            remap_object_graph(
                context, source, destination);
        } else {
            slot->active = 1u;
        }
        ++destination;
    }
    if (destination < old_count) {
        memset(
            &storage->slots[destination], 0,
            (old_count - destination) *
                sizeof(PikaObjectSlot));
    }
    storage->count = (uint16_t)destination;
}

#if PIKA_BINDING_RUNTIME_ENABLE

static void visit_reachable_values(
    PikaRuntimeContext* context,
    PikaRuntimeValueVisitor visitor,
    void* visitor_context) {
    uint32_t index;
    visit_runtime_roots(
        context, visitor, visitor_context);
    if (context->objects == NULL ||
        context->objects->slots == NULL) {
        return;
    }
    for (index = 0u;
         index < context->objects->count;
         ++index) {
        PikaObjectSlot* slot =
            &context->objects->slots[index];
        if (slot->active != 0u) {
            visit_object_values(
                slot, visitor, visitor_context);
        }
    }
}

static void mark_binding_value(
    PikaRuntimeValue* value,
    void* context) {
    PikaRuntimeContext* runtime =
        (PikaRuntimeContext*)context;
    uint32_t index;
    if (value->kind !=
            PIKA_RUNTIME_VALUE_NATIVE_OBJECT) {
        return;
    }
    index = value->as.object_index;
    if (index < runtime->binding_object_count &&
        runtime->binding_storage
                ->objects[index]
                .active == 1u) {
        runtime->binding_storage
            ->objects[index]
            .active = 2u;
    }
}

static void invalidate_released_binding_value(
    PikaRuntimeValue* value,
    void* context) {
    PikaRuntimeContext* runtime =
        (PikaRuntimeContext*)context;
    uint32_t index;
    if (value->kind !=
        PIKA_RUNTIME_VALUE_NATIVE_OBJECT) {
        return;
    }
    index = value->as.object_index;
    if (index < runtime->binding_object_count &&
        runtime->binding_storage->objects[index].active == 0u) {
        memset(value, 0, sizeof(*value));
        value->kind = PIKA_RUNTIME_VALUE_NONE;
    }
}

static void remap_binding_value(
    PikaRuntimeValue* value,
    void* context) {
    const PikaObjectIndexRemap* remap =
        (const PikaObjectIndexRemap*)context;
    if (value->kind ==
            PIKA_RUNTIME_VALUE_NATIVE_OBJECT &&
        value->as.object_index == remap->source) {
        value->as.object_index = remap->destination;
    }
}

static void invalidate_released_binding_storage_values(
    PikaRuntimeContext* context,
    uint32_t object_count) {
    uint32_t depth;
    PikaBindingObject* objects =
        context->binding_storage->objects;
    if (context->binding_storage->values == NULL ||
        context->frames == NULL) {
        return;
    }
    for (depth = 0u; depth <= context->depth; ++depth) {
        const PikaRuntimeFrame* frame =
            &context->frames[depth];
        uint32_t count = frame->slot_count;
        uint32_t index;
        if (frame->value_base >=
            context->binding_storage->value_capacity) {
            continue;
        }
        if (count > context->max_slots) {
            count = context->max_slots;
        }
        if (count >
            context->binding_storage->value_capacity -
                frame->value_base) {
            count =
                context->binding_storage->value_capacity -
                frame->value_base;
        }
        for (index = 0u; index < count; ++index) {
            PikaBindingValue* value =
                &context->binding_storage->values[
                    frame->value_base + index];
            uint32_t object_index;
            if (value->kind !=
                PIKA_BINDING_VALUE_OPAQUE) {
                continue;
            }
            for (object_index = 0u;
                 object_index < object_count;
                 ++object_index) {
                if (objects[object_index].active == 0u &&
                    value->as.opaque ==
                        &objects[object_index]) {
                    memset(value, 0, sizeof(*value));
                    value->kind = PIKA_BINDING_VALUE_NONE;
                    break;
                }
            }
        }
    }
}

static void remap_binding_storage_values(
    PikaRuntimeContext* context,
    uint32_t source,
    uint32_t destination) {
    uint32_t depth;
    PikaBindingObject* objects =
        context->binding_storage->objects;
    if (context->binding_storage->values == NULL ||
        context->frames == NULL) {
        return;
    }
    for (depth = 0u; depth <= context->depth; ++depth) {
        const PikaRuntimeFrame* frame =
            &context->frames[depth];
        uint32_t count = frame->slot_count;
        uint32_t index;
        if (frame->value_base >=
            context->binding_storage->value_capacity) {
            continue;
        }
        if (count > context->max_slots) {
            count = context->max_slots;
        }
        if (count >
            context->binding_storage->value_capacity -
                frame->value_base) {
            count =
                context->binding_storage->value_capacity -
                frame->value_base;
        }
        for (index = 0u; index < count; ++index) {
            PikaBindingValue* value =
                &context->binding_storage->values[
                    frame->value_base + index];
            if (value->kind ==
                    PIKA_BINDING_VALUE_OPAQUE &&
                value->as.opaque ==
                    &objects[source]) {
                value->as.opaque =
                    &objects[destination];
            }
        }
    }
}

static void compact_binding_objects(
    PikaRuntimeContext* context) {
    PikaBindingObject* objects;
    uint32_t old_count;
    uint32_t destination = 0u;
    uint32_t source;
    if (context->binding_storage == NULL ||
        context->binding_storage->objects == NULL ||
        context->binding_object_count == 0u) {
        return;
    }
    if (context->binding_object_count >
        context->binding_storage->object_capacity) {
        return;
    }
    objects = context->binding_storage->objects;
    old_count = context->binding_object_count;
    for (source = 0u; source < old_count; ++source) {
        if (objects[source].active != 0u) {
            objects[source].active = 1u;
        }
    }
    visit_reachable_values(
        context, invalidate_released_binding_value, context);
    invalidate_released_binding_storage_values(
        context, old_count);
    visit_reachable_values(
        context, mark_binding_value, context);
    for (source = 0u; source < old_count; ++source) {
        if (objects[source].active == 1u) {
            pika_binding_object_release(
                &objects[source]);
        }
    }
    for (source = 0u; source < old_count; ++source) {
        PikaObjectIndexRemap remap;
        if (objects[source].active != 2u) {
            continue;
        }
        if (source != destination) {
            objects[destination] = objects[source];
            objects[destination].active = 1u;
            memset(
                &objects[source], 0,
                sizeof(PikaBindingObject));
            remap.source = source;
            remap.destination = destination;
            visit_reachable_values(
                context, remap_binding_value,
                &remap);
            remap_binding_storage_values(
                context, source, destination);
        } else {
            objects[source].active = 1u;
        }
        ++destination;
    }
    if (destination < old_count) {
        memset(
            &objects[destination], 0,
            (old_count - destination) *
                sizeof(PikaBindingObject));
    }
    context->binding_object_count =
        (uint16_t)destination;
}

#endif

static void compact_object_arena(
    PikaObjectStorage* storage) {
    uintptr_t begin;
    uintptr_t end;
    uintptr_t previous_pointer = 0u;
    uintptr_t previous_end = 0u;
    uint32_t index;
    uint32_t destination = 0u;
    uint32_t previous_index = 0u;
    uint32_t alignment = (uint32_t)sizeof(void*);
    int have_previous = 0;
    if (storage->slots == NULL ||
        storage->arena == NULL ||
        storage->arena_capacity == 0u) {
        return;
    }
    begin = (uintptr_t)storage->arena;
    end = begin + storage->arena_capacity;
    if (end < begin ||
        storage->arena_used >
            storage->arena_capacity) {
        return;
    }
    for (index = 0u; index < storage->count; ++index) {
        PikaObjectSlot* slot = &storage->slots[index];
        uint32_t size;
        uintptr_t pointer;
        if (slot->active == 0u ||
            slot->owned != 0u ||
            slot->data == NULL) {
            continue;
        }
        if (!object_slot_payload_size(slot, &size)) {
            return;
        }
        pointer = (uintptr_t)slot->data;
        if (pointer < begin || pointer > end ||
            size > end - pointer) {
            return;
        }
    }
    for (;;) {
        uint32_t selected = UINT32_MAX;
        uintptr_t selected_pointer = UINTPTR_MAX;
        uint32_t selected_size = 0u;
        uint64_t aligned;
        for (index = 0u; index < storage->count; ++index) {
            PikaObjectSlot* slot = &storage->slots[index];
            uintptr_t pointer;
            uint32_t size;
            if (slot->active != 1u ||
                slot->owned != 0u ||
                slot->data == NULL ||
                !object_slot_payload_size(slot, &size) ||
                size == 0u) {
                continue;
            }
            pointer = (uintptr_t)slot->data;
            if (have_previous != 0 &&
                (pointer < previous_pointer ||
                 (pointer == previous_pointer &&
                  index <= previous_index))) {
                continue;
            }
            if (pointer < selected_pointer ||
                (pointer == selected_pointer &&
                 index < selected)) {
                selected = index;
                selected_pointer = pointer;
                selected_size = size;
            }
        }
        if (selected == UINT32_MAX) {
            break;
        }
        if (have_previous != 0 &&
            selected_pointer < previous_end) {
            return;
        }
        aligned =
            ((uint64_t)destination +
             alignment - 1u) /
            alignment * alignment;
        if (aligned > storage->arena_capacity ||
            selected_size >
                storage->arena_capacity - aligned) {
            return;
        }
        destination =
            (uint32_t)aligned + selected_size;
        previous_pointer = selected_pointer;
        previous_end = selected_pointer + selected_size;
        previous_index = selected;
        have_previous = 1;
    }
    destination = 0u;
    for (;;) {
        uint32_t selected = UINT32_MAX;
        uintptr_t selected_pointer = UINTPTR_MAX;
        uint32_t selected_size = 0u;
        uint64_t aligned;
        for (index = 0u; index < storage->count; ++index) {
            PikaObjectSlot* slot = &storage->slots[index];
            uintptr_t pointer;
            uint32_t size;
            if (slot->active != 1u ||
                slot->owned != 0u ||
                slot->data == NULL) {
                continue;
            }
            if (!object_slot_payload_size(slot, &size) ||
                size == 0u) {
                continue;
            }
            pointer = (uintptr_t)slot->data;
            if (pointer < selected_pointer) {
                selected = index;
                selected_pointer = pointer;
                selected_size = size;
            }
        }
        if (selected == UINT32_MAX) {
            break;
        }
        aligned =
            ((uint64_t)destination +
             alignment - 1u) /
            alignment * alignment;
        memmove(
            &storage->arena[(uint32_t)aligned],
            storage->slots[selected].data,
            selected_size);
        storage->slots[selected].data =
            &storage->arena[(uint32_t)aligned];
        storage->slots[selected].active = 4u;
        destination =
            (uint32_t)aligned + selected_size;
    }
    for (index = 0u; index < storage->count; ++index) {
        if (storage->slots[index].active == 4u) {
            storage->slots[index].active = 1u;
        }
    }
    storage->arena_used = destination;
}

void pika_runtime_note_object_pressure(
    PikaRuntimeContext* context) {
    if (context == NULL) {
        return;
    }
    if (context->objects != NULL) {
        const PikaObjectStorage* storage =
            context->objects;
        uint32_t slot_threshold = (uint32_t)(
            ((uint64_t)storage->capacity *
                 PIKA_GC_SLOT_TRIGGER_PERCENT +
             99u) /
            100u);
        uint32_t arena_threshold = (uint32_t)(
            ((uint64_t)storage->arena_capacity *
                 PIKA_GC_ARENA_TRIGGER_PERCENT +
             99u) /
            100u);
        if ((storage->capacity != 0u &&
             storage->count >= slot_threshold) ||
            (storage->arena_capacity != 0u &&
             storage->arena_used >=
                 arena_threshold)) {
            context->object_collection_pending = 1u;
            return;
        }
    }
#if PIKA_BINDING_RUNTIME_ENABLE
    if (context->binding_storage != NULL &&
        context->binding_storage->objects != NULL &&
        context->binding_storage->object_capacity != 0u) {
        uint32_t capacity =
            context->binding_storage->object_capacity;
        uint32_t threshold = (uint32_t)(
            ((uint64_t)capacity *
                 PIKA_GC_BINDING_TRIGGER_PERCENT +
             99u) /
            100u);
        if (context->binding_object_count >=
            threshold) {
            context->object_collection_pending = 1u;
            return;
        }
    }
#endif
}

int pika_runtime_object_collection_needed(
    const PikaRuntimeContext* context) {
    return context != NULL &&
           context->object_collection_pending != 0u;
}

void pika_runtime_collect_objects(
    PikaRuntimeContext* context) {
    uint32_t old_object_count;
    uint32_t old_arena_used;
    uint32_t old_binding_count;
    if (context == NULL ||
        context->typed_values == NULL) {
        return;
    }
    if (context->objects != NULL &&
        context->objects->count >
            context->objects->capacity) {
        return;
    }
#if PIKA_BINDING_RUNTIME_ENABLE
    if (context->binding_storage != NULL &&
        context->binding_storage->objects != NULL &&
        context->binding_object_count >
            context->binding_storage->object_capacity) {
        return;
    }
#endif
    old_object_count =
        context->objects != NULL
            ? context->objects->count
            : 0u;
    old_arena_used =
        context->objects != NULL
            ? context->objects->arena_used
            : 0u;
    old_binding_count = context->binding_object_count;
    if (context->objects != NULL &&
        context->objects->slots != NULL) {
        mark_object_graph(context);
        compact_object_slots(context);
    }
#if PIKA_BINDING_RUNTIME_ENABLE
    compact_binding_objects(context);
#endif
    if (context->objects != NULL) {
        compact_object_arena(context->objects);
    }
    context->object_collection_pending = 0u;
    if (context->metrics != NULL) {
        uint32_t object_count =
            context->objects != NULL
                ? context->objects->count
                : 0u;
        uint32_t arena_used =
            context->objects != NULL
                ? context->objects->arena_used
                : 0u;
        ++context->metrics->gc_collection_count;
        if (object_count < old_object_count) {
            context->metrics->gc_reclaimed_object_count +=
                old_object_count - object_count;
        }
        if (arena_used < old_arena_used) {
            context->metrics->gc_reclaimed_arena_bytes +=
                old_arena_used - arena_used;
        }
        if (context->binding_object_count <
            old_binding_count) {
            context->metrics
                ->gc_reclaimed_binding_object_count +=
                old_binding_count -
                context->binding_object_count;
        }
    }
}

#endif

PikaStatus pika_runtime_range_length(
    PikaRuntimeContext* context,
    PikaRuntimeValue value,
    uint32_t* length) {
    PikaObjectSlot* slot;
    const PikaRangeData* range;
    uint64_t distance;
    uint64_t stride;
    uint64_t count;
    if (context == NULL || length == NULL ||
        value.kind != PIKA_RUNTIME_VALUE_RANGE) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    slot = pika_runtime_object(context, value);
    if (slot == NULL || slot->data == NULL) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    range = (const PikaRangeData*)slot->data;
    if (range->step == 0) return PIKA_STATUS_DIVISION_BY_ZERO;
    if (range->step > 0) {
        if (range->start >= range->stop) {
            *length = 0u;
            return PIKA_STATUS_OK;
        }
        distance = (uint64_t)range->stop - (uint64_t)range->start;
        stride = (uint64_t)range->step;
    } else {
        if (range->start <= range->stop) {
            *length = 0u;
            return PIKA_STATUS_OK;
        }
        distance = (uint64_t)range->start - (uint64_t)range->stop;
        stride = (uint64_t)(-(range->step + 1)) + 1u;
    }
    count = distance / stride;
    if (distance % stride != 0u) ++count;
    if (count > UINT32_MAX) return PIKA_STATUS_STORAGE_TOO_SMALL;
    *length = (uint32_t)count;
    return PIKA_STATUS_OK;
}

static PikaStatus range_item_value(const PikaRangeData* range,
                                   uint32_t index,
                                   PikaRuntimeValue* result) {
    uint64_t magnitude;
    uint64_t offset;
    uint64_t distance_to_zero;
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_RUNTIME_VALUE_INTEGER;
    if (index == 0u) {
        result->as.integer = range->start;
        return PIKA_STATUS_OK;
    }
    if (range->step > 0) {
        magnitude = (uint64_t)range->step;
        offset = (uint64_t)index * magnitude;
        if (range->start >= 0) {
            result->as.integer = range->start + (int64_t)offset;
            return PIKA_STATUS_OK;
        }
        distance_to_zero =
            (uint64_t)(-(range->start + 1)) + 1u;
        if (offset < distance_to_zero) {
            uint64_t remaining = distance_to_zero - offset;
            result->as.integer = -(int64_t)(remaining - 1u) - 1;
        } else {
            result->as.integer = (int64_t)(offset - distance_to_zero);
        }
        return PIKA_STATUS_OK;
    }
    magnitude = (uint64_t)(-(range->step + 1)) + 1u;
    offset = (uint64_t)index * magnitude;
    if (range->start <= 0) {
        distance_to_zero =
            (uint64_t)(-(range->start + 1)) + 1u;
        distance_to_zero += offset;
        result->as.integer =
            -(int64_t)(distance_to_zero - 1u) - 1;
        return PIKA_STATUS_OK;
    }
    distance_to_zero = (uint64_t)range->start;
    if (offset <= distance_to_zero) {
        result->as.integer = (int64_t)(distance_to_zero - offset);
    } else {
        uint64_t remaining = offset - distance_to_zero;
        result->as.integer = -(int64_t)(remaining - 1u) - 1;
    }
    return PIKA_STATUS_OK;
}

#if PIKA_CAPABILITY_OP_SLICE_ENABLE

static PikaStatus checked_add_integer(int64_t left,
                                      int64_t right,
                                      int64_t* result) {
    if ((right > 0 && left > INT64_MAX - right) ||
        (right < 0 && left < INT64_MIN - right)) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    *result = left + right;
    return PIKA_STATUS_OK;
}

static PikaStatus checked_multiply_integer(int64_t left,
                                           int64_t right,
                                           int64_t* result) {
    if (left == 0 || right == 0) {
        *result = 0;
        return PIKA_STATUS_OK;
    }
    if (left == -1 && right == INT64_MIN) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (right == -1 && left == INT64_MIN) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (left > 0) {
        if (right > 0 && left > INT64_MAX / right) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        if (right < 0 && right < INT64_MIN / left) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
    } else {
        if (right > 0 && left < INT64_MIN / right) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        if (right < 0 && left < INT64_MAX / right) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
    }
    *result = left * right;
    return PIKA_STATUS_OK;
}

#endif

PikaStatus pika_runtime_create_sequence(
    PikaRuntimeContext* context,
    PikaRuntimeValueKind kind,
    const PikaRuntimeValue* values,
    uint32_t length,
    PikaRuntimeValue* result) {
    uint32_t object_index;
    uint32_t source_offset = 0u;
    void* data = NULL;
    uint32_t source_size;
    int source_in_arena;
    PikaStatus status;
    if (result == NULL || !is_value_sequence(kind) ||
        (length > 0u && values == NULL)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (length >
        UINT32_MAX / (uint32_t)sizeof(PikaRuntimeValue)) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    source_size =
        length * (uint32_t)sizeof(PikaRuntimeValue);
    source_in_arena = object_arena_offset(
        context != NULL ? context->objects : NULL,
        values, source_size, &source_offset);
    status = pika_runtime_create_object(
        context, kind, length, (uint32_t)sizeof(PikaRuntimeValue),
        &object_index, &data);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (length > 0u) {
        if (source_in_arena != 0) {
            values = (const PikaRuntimeValue*)
                &context->objects->arena[source_offset];
        }
        memmove(data, values, source_size);
    }
    memset(result, 0, sizeof(*result));
    result->kind = kind;
    result->as.object_index = object_index;
    return PIKA_STATUS_OK;
}

PikaStatus pika_runtime_create_text(
    PikaRuntimeContext* context,
    PikaRuntimeValueKind kind,
    const char* bytes,
    uint32_t length,
    PikaRuntimeValue* result) {
    unsigned char* destination = NULL;
    uint32_t source_offset = 0u;
    int source_in_arena;
    PikaStatus status;
    if (length > 0u && bytes == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    source_in_arena = object_arena_offset(
        context != NULL ? context->objects : NULL,
        bytes, length, &source_offset);
    status = pika_runtime_create_text_buffer(
        context, kind, length, result, &destination);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (length > 0u) {
        if (source_in_arena != 0) {
            bytes = (const char*)
                &context->objects->arena[source_offset];
        }
        memmove(destination, bytes, length);
    }
    return PIKA_STATUS_OK;
}

PikaStatus pika_runtime_create_text_buffer(
    PikaRuntimeContext* context,
    PikaRuntimeValueKind kind,
    uint32_t length,
    PikaRuntimeValue* result,
    unsigned char** bytes) {
    uint32_t object_index;
    void* data = NULL;
    PikaStatus status;
    PikaObjectSlot* slot;
    if (result == NULL || bytes == NULL ||
        (!is_byte_sequence(kind) &&
         kind != PIKA_RUNTIME_VALUE_STRING &&
         kind != PIKA_RUNTIME_VALUE_TYPE &&
         kind != PIKA_RUNTIME_VALUE_MODULE) ||
        length == UINT32_MAX) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = pika_runtime_create_object(
        context, kind, length + 1u, 1u, &object_index, &data);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    ((char*)data)[length] = '\0';
    slot = &context->objects->slots[object_index];
    slot->length = length;
    slot->capacity = length + 1u;
    memset(result, 0, sizeof(*result));
    result->kind = kind;
    result->as.object_index = object_index;
    *bytes = (unsigned char*)data;
    return PIKA_STATUS_OK;
}

#if PIKA_CAPABILITY_OBJECT_USER_BASIC_ENABLE || \
    PIKA_CAPABILITY_OBJECT_CLASS_ENABLE

PikaStatus pika_runtime_create_instance(
    PikaRuntimeContext* context,
    uint16_t class_index,
    PikaRuntimeValue* result) {
    uint32_t object_index;
    void* data = NULL;
    PikaObjectSlot* slot;
    uint16_t field_count;
    PikaStatus status;
    if (context == NULL || context->program == NULL || result == NULL ||
        class_index >= context->program->class_count ||
        class_index >= UINT8_MAX) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    field_count = context->program->classes[class_index].field_count;
    status = pika_runtime_create_object(
        context, PIKA_RUNTIME_VALUE_INSTANCE, field_count,
        (uint32_t)sizeof(PikaRuntimeValue), &object_index, &data);
    if (status != PIKA_STATUS_OK) return status;
    slot = &context->objects->slots[object_index];
    slot->reserved = (uint8_t)(class_index + 1u);
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_RUNTIME_VALUE_INSTANCE;
    result->as.object_index = object_index;
    return PIKA_STATUS_OK;
}

#endif

#if PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE

PikaStatus pika_runtime_load_attribute(
    PikaRuntimeContext* context,
    PikaRuntimeValue instance,
    uint16_t field_index,
    PikaRuntimeValue* result) {
    PikaObjectSlot* slot;
    PikaRuntimeValue* fields;
    if (result == NULL) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    if (instance.kind == PIKA_RUNTIME_VALUE_TYPE) {
        if (field_index != PIKA_TYPE_NAME_ATTRIBUTE) {
            return PIKA_STATUS_MEMBER_NOT_FOUND;
        }
        slot = pika_runtime_object(context, instance);
        if (slot == NULL ||
            slot->kind != PIKA_RUNTIME_VALUE_TYPE) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        *result = instance;
        result->kind = PIKA_RUNTIME_VALUE_STRING;
        return PIKA_STATUS_OK;
    }
    if (instance.kind != PIKA_RUNTIME_VALUE_INSTANCE) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    slot = pika_runtime_object(context, instance);
    if (slot == NULL || slot->kind != PIKA_RUNTIME_VALUE_INSTANCE ||
        field_index >= slot->length || slot->data == NULL) {
        return PIKA_STATUS_MEMBER_NOT_FOUND;
    }
    fields = (PikaRuntimeValue*)slot->data;
    if ((fields[field_index].flags & PIKA_RUNTIME_FIELD_ASSIGNED) == 0u) {
        return PIKA_STATUS_MEMBER_NOT_FOUND;
    }
    *result = fields[field_index];
    result->flags &= (uint16_t)~PIKA_RUNTIME_FIELD_ASSIGNED;
    return PIKA_STATUS_OK;
}

PikaStatus pika_runtime_store_attribute(
    PikaRuntimeContext* context,
    PikaRuntimeValue instance,
    uint16_t field_index,
    PikaRuntimeValue value) {
    PikaObjectSlot* slot;
    PikaRuntimeValue* fields;
    if (instance.kind != PIKA_RUNTIME_VALUE_INSTANCE) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    slot = pika_runtime_object(context, instance);
    if (slot == NULL || slot->kind != PIKA_RUNTIME_VALUE_INSTANCE ||
        field_index >= slot->length || slot->data == NULL) {
        return PIKA_STATUS_MEMBER_NOT_FOUND;
    }
    fields = (PikaRuntimeValue*)slot->data;
    fields[field_index] = value;
    fields[field_index].flags |= PIKA_RUNTIME_FIELD_ASSIGNED;
    return PIKA_STATUS_OK;
}

#endif

PikaStatus pika_runtime_create_range(
    PikaRuntimeContext* context,
    int64_t start,
    int64_t stop,
    int64_t step,
    PikaRuntimeValue* result) {
    uint32_t object_index;
    void* data = NULL;
    PikaStatus status;
    PikaRangeData range;
    if (result == NULL) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (step == 0) return PIKA_STATUS_DIVISION_BY_ZERO;
    status = pika_runtime_create_object(
        context, PIKA_RUNTIME_VALUE_RANGE, 1u,
        (uint32_t)sizeof(PikaRangeData), &object_index, &data);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    range.start = start;
    range.stop = stop;
    range.step = step;
    memcpy(data, &range, sizeof(range));
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_RUNTIME_VALUE_RANGE;
    result->as.object_index = object_index;
    return PIKA_STATUS_OK;
}

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE

PikaStatus pika_runtime_create_iterator(
    PikaRuntimeContext* context,
    PikaRuntimeValue source,
    PikaRuntimeValue* result) {
    uint32_t object_index;
    void* data = NULL;
    PikaStatus status;
    PikaIteratorData iterator;
    if (result == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = pika_runtime_create_object(
        context, PIKA_RUNTIME_VALUE_ITERATOR, 1u,
        (uint32_t)sizeof(PikaIteratorData), &object_index, &data);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    iterator.source_index = source.as.object_index;
    iterator.position = 0u;
    memcpy(data, &iterator, sizeof(iterator));
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_RUNTIME_VALUE_ITERATOR;
    result->as.object_index = object_index;
    return PIKA_STATUS_OK;
}

PikaStatus pika_runtime_create_slice(
    PikaRuntimeContext* context,
    PikaRuntimeValue start,
    PikaRuntimeValue stop,
    PikaRuntimeValue step,
    PikaRuntimeValue* result) {
    uint32_t object_index;
    void* data = NULL;
    PikaStatus status;
    PikaSliceData slice;
    if (result == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = pika_runtime_create_object(
        context, PIKA_RUNTIME_VALUE_SLICE, 1u,
        (uint32_t)sizeof(PikaSliceData), &object_index, &data);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    slice.start = start;
    slice.stop = stop;
    slice.step = step;
    memcpy(data, &slice, sizeof(slice));
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_RUNTIME_VALUE_SLICE;
    result->as.object_index = object_index;
    return PIKA_STATUS_OK;
}

#endif

static PikaStatus integer_from_value(PikaRuntimeValue value,
                                     int64_t* integer) {
    if (value.kind == PIKA_RUNTIME_VALUE_INTEGER ||
        value.kind == PIKA_RUNTIME_VALUE_BOOLEAN) {
        *integer = value.as.integer;
        return PIKA_STATUS_OK;
    }
    return PIKA_STATUS_TYPE_MISMATCH;
}

PikaStatus pika_runtime_sequence_item(
    PikaRuntimeContext* context,
    PikaRuntimeValue value,
    int64_t index,
    PikaRuntimeValue* result) {
    PikaObjectSlot* slot;
    uint32_t length;
    PikaStatus status;
    if (result == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (value.kind != PIKA_RUNTIME_VALUE_DICT &&
        value.kind != PIKA_RUNTIME_VALUE_RANGE &&
        !is_value_sequence(value.kind) &&
        value.kind != PIKA_RUNTIME_VALUE_STRING &&
        !is_byte_sequence(value.kind)) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    slot = pika_runtime_object(context, value);
    if (slot == NULL) return PIKA_STATUS_INVALID_OPERAND;
    if (slot->length != 0u && slot->data == NULL) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_DICT) {
        PikaRuntimeValue found_value;
        int found = 0;
        PikaStatus lookup_status = pika_runtime_dict_lookup(
            context, value, (PikaRuntimeValue){
                PIKA_RUNTIME_VALUE_INTEGER, 0u, 0u, {.integer = index}},
            &found_value, &found);
        if (lookup_status != PIKA_STATUS_OK) return lookup_status;
        if (!found) return PIKA_STATUS_INVALID_OPERAND;
        *result = found_value;
        return PIKA_STATUS_OK;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_RANGE) {
        status = pika_runtime_range_length(context, value, &length);
        if (status != PIKA_STATUS_OK) return status;
    } else {
        length = slot->length;
    }
    if (index < 0) index += (int64_t)length;
    if (index < 0 || (uint64_t)index >= (uint64_t)length) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (is_value_sequence(value.kind)) {
        *result = ((PikaRuntimeValue*)slot->data)[index];
        return PIKA_STATUS_OK;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_STRING) {
        char byte = ((const char*)slot->data)[index];
        return pika_runtime_create_text(
            context, PIKA_RUNTIME_VALUE_STRING, &byte, 1u, result);
    }
    if (is_byte_sequence(value.kind)) {
        memset(result, 0, sizeof(*result));
        result->kind = PIKA_RUNTIME_VALUE_INTEGER;
        result->as.integer = (unsigned char)((const char*)slot->data)[index];
        return PIKA_STATUS_OK;
    }
    return range_item_value(
        (const PikaRangeData*)slot->data,
        (uint32_t)index, result);
}

#if PIKA_CAPABILITY_ASSIGNMENT_UNPACK_ENABLE
PikaStatus pika_runtime_iterable_length(
    PikaRuntimeContext* context,
    PikaRuntimeValue value,
    uint32_t* length) {
    PikaObjectSlot* slot;
    if (context == NULL || length == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (value.kind != PIKA_RUNTIME_VALUE_RANGE &&
        value.kind != PIKA_RUNTIME_VALUE_LIST &&
        value.kind != PIKA_RUNTIME_VALUE_TUPLE &&
        value.kind != PIKA_RUNTIME_VALUE_DICT &&
        value.kind != PIKA_RUNTIME_VALUE_STRING &&
        value.kind != PIKA_RUNTIME_VALUE_BYTES &&
        value.kind != PIKA_RUNTIME_VALUE_BYTEARRAY) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    slot = pika_runtime_object(context, value);
    if (slot == NULL) return PIKA_STATUS_INVALID_OPERAND;
    if (slot->length != 0u && slot->data == NULL) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_RANGE) {
        return pika_runtime_range_length(
            context, value, length);
    }
    *length = slot->length;
    return PIKA_STATUS_OK;
}
#endif

PikaStatus pika_runtime_iteration_item(
    PikaRuntimeContext* context,
    PikaRuntimeValue value,
    uint32_t index,
    PikaRuntimeValue* result,
    int* available) {
    PikaObjectSlot* slot;
    uint32_t length;
    PikaStatus status;
    if (result == NULL || available == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (value.kind != PIKA_RUNTIME_VALUE_RANGE &&
        value.kind != PIKA_RUNTIME_VALUE_LIST &&
        value.kind != PIKA_RUNTIME_VALUE_TUPLE &&
        value.kind != PIKA_RUNTIME_VALUE_DICT &&
        value.kind != PIKA_RUNTIME_VALUE_STRING &&
        value.kind != PIKA_RUNTIME_VALUE_BYTES &&
        value.kind != PIKA_RUNTIME_VALUE_BYTEARRAY) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    slot = pika_runtime_object(context, value);
    if (slot == NULL) return PIKA_STATUS_INVALID_OPERAND;
    if (slot->length != 0u && slot->data == NULL) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_RANGE) {
        status = pika_runtime_range_length(context, value, &length);
        if (status != PIKA_STATUS_OK) return status;
    } else {
        length = slot->length;
    }
    if (index >= length) {
        *available = 0;
        return PIKA_STATUS_OK;
    }
    *available = 1;
    if (value.kind == PIKA_RUNTIME_VALUE_DICT) {
        *result = ((PikaMapEntry*)slot->data)[index].key;
        return PIKA_STATUS_OK;
    }
    return pika_runtime_sequence_item(
        context, value, (int64_t)index, result);
}

PikaStatus pika_runtime_dict_lookup(PikaRuntimeContext* context,
                                    PikaRuntimeValue dictionary,
                                    PikaRuntimeValue key,
                                    PikaRuntimeValue* result,
                                    int* found) {
    PikaObjectSlot* slot;
    if (result == NULL || found == NULL) return PIKA_STATUS_INVALID_ARGUMENT;
    if (dictionary.kind != PIKA_RUNTIME_VALUE_DICT) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    slot = pika_runtime_object(context, dictionary);
    if (slot == NULL) return PIKA_STATUS_INVALID_OPERAND;
    if (slot->length != 0u && slot->data == NULL) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    *found = 0;
    for (uint32_t index = 0u; index < slot->length; ++index) {
        PikaMapEntry* entry = &((PikaMapEntry*)slot->data)[index];
        PikaStatus status = pika_runtime_value_equal(
            context, key, entry->key, found);
        if (status != PIKA_STATUS_OK) return status;
        if (*found) {
            *result = entry->value;
            return PIKA_STATUS_OK;
        }
    }
    return PIKA_STATUS_OK;
}

static PikaStatus normalize_mutable_index(int64_t index,
                                          uint32_t length,
                                          uint32_t* normalized) {
    if (index < 0) index += (int64_t)length;
    if (index < 0 || (uint64_t)index >= (uint64_t)length) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    *normalized = (uint32_t)index;
    return PIKA_STATUS_OK;
}

PikaStatus pika_runtime_set_item(PikaRuntimeContext* context,
                                 PikaRuntimeValue container,
                                 PikaRuntimeValue index,
                                 PikaRuntimeValue value) {
    PikaObjectSlot* slot = pika_runtime_object(context, container);
    int64_t integer;
    uint32_t position;
    PikaStatus status;
    if (slot == NULL) return PIKA_STATUS_INVALID_OPERAND;
    if (slot->length != 0u && slot->data == NULL) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (container.kind == PIKA_RUNTIME_VALUE_DICT) {
        if (slot->length == UINT32_MAX) {
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
        for (uint32_t entry_index = 0u;
             entry_index < slot->length; ++entry_index) {
            PikaMapEntry* entry =
                &((PikaMapEntry*)slot->data)[entry_index];
            int found = 0;
            status = pika_runtime_value_equal(
                context, index, entry->key, &found);
            if (status != PIKA_STATUS_OK) return status;
            if (found) {
                entry->value = value;
                return PIKA_STATUS_OK;
            }
        }
        status = reserve_object_data(
            context, &slot, slot->length + 1u,
            (uint32_t)sizeof(PikaMapEntry));
        if (status != PIKA_STATUS_OK) return status;
        ((PikaMapEntry*)slot->data)[slot->length].key = index;
        ((PikaMapEntry*)slot->data)[slot->length].value = value;
        ++slot->length;
        return PIKA_STATUS_OK;
    }
    status = integer_from_value(index, &integer);
    if (status != PIKA_STATUS_OK) return status;
    status = normalize_mutable_index(integer, slot->length, &position);
    if (status != PIKA_STATUS_OK) return status;
    if (container.kind == PIKA_RUNTIME_VALUE_LIST) {
        ((PikaRuntimeValue*)slot->data)[position] = value;
        return PIKA_STATUS_OK;
    }
    if (container.kind == PIKA_RUNTIME_VALUE_BYTEARRAY) {
        int64_t byte;
        status = integer_from_value(value, &byte);
        if (status != PIKA_STATUS_OK || byte < 0 || byte > 255) {
            return status == PIKA_STATUS_OK
                       ? PIKA_STATUS_INVALID_OPERAND
                       : status;
        }
        ((unsigned char*)slot->data)[position] = (unsigned char)byte;
        return PIKA_STATUS_OK;
    }
    return PIKA_STATUS_TYPE_MISMATCH;
}

PikaStatus pika_runtime_append(PikaRuntimeContext* context,
                               PikaRuntimeValue container,
                               PikaRuntimeValue value,
                               PikaRuntimeValue* result) {
    PikaObjectSlot* slot = pika_runtime_object(context, container);
    PikaStatus status;
    if (slot == NULL || result == NULL) return PIKA_STATUS_INVALID_OPERAND;
    if (slot->length != 0u && slot->data == NULL) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (container.kind == PIKA_RUNTIME_VALUE_LIST) {
        if (slot->length == UINT32_MAX) {
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
        status = reserve_object_data(
            context, &slot, slot->length + 1u,
            (uint32_t)sizeof(PikaRuntimeValue));
        if (status != PIKA_STATUS_OK) return status;
        ((PikaRuntimeValue*)slot->data)[slot->length++] = value;
    } else if (container.kind == PIKA_RUNTIME_VALUE_BYTEARRAY) {
        int64_t byte;
        status = integer_from_value(value, &byte);
        if (status != PIKA_STATUS_OK || byte < 0 || byte > 255) {
            return status == PIKA_STATUS_OK
                       ? PIKA_STATUS_INVALID_OPERAND
                       : status;
        }
        if (slot->length > UINT32_MAX - 2u) {
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
        status = reserve_object_data(
            context, &slot, slot->length + 2u, 1u);
        if (status != PIKA_STATUS_OK) return status;
        ((unsigned char*)slot->data)[slot->length++] = (unsigned char)byte;
        ((char*)slot->data)[slot->length] = '\0';
    } else {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_RUNTIME_VALUE_NONE;
    return PIKA_STATUS_OK;
}

PikaStatus pika_runtime_pop(PikaRuntimeContext* context,
                            PikaRuntimeValue container,
                            PikaRuntimeValue index,
                            int has_index,
                            PikaRuntimeValue default_value,
                            int has_default,
                            PikaRuntimeValue* result) {
    PikaObjectSlot* slot;
    PikaStatus status;
    if (result == NULL) return PIKA_STATUS_INVALID_OPERAND;
    if (container.kind != PIKA_RUNTIME_VALUE_DICT &&
        container.kind != PIKA_RUNTIME_VALUE_LIST &&
        container.kind != PIKA_RUNTIME_VALUE_BYTEARRAY) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    slot = pika_runtime_object(context, container);
    if (slot == NULL) return PIKA_STATUS_INVALID_OPERAND;
    if (slot->length != 0u && slot->data == NULL) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (container.kind == PIKA_RUNTIME_VALUE_DICT) {
        if (!has_index) return PIKA_STATUS_ARGUMENT_COUNT;
        for (uint32_t entry_index = 0u;
             entry_index < slot->length; ++entry_index) {
            PikaMapEntry* entry = &((PikaMapEntry*)slot->data)[entry_index];
            int found = 0;
            status = pika_runtime_value_equal(
                context, index, entry->key, &found);
            if (status != PIKA_STATUS_OK) return status;
            if (found) {
                *result = entry->value;
                if (entry_index + 1u < slot->length) {
                    memmove(entry, entry + 1,
                            (slot->length - entry_index - 1u) *
                                sizeof(PikaMapEntry));
                }
                --slot->length;
                return PIKA_STATUS_OK;
            }
        }
        if (has_default) {
            *result = default_value;
            return PIKA_STATUS_OK;
        }
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (container.kind == PIKA_RUNTIME_VALUE_LIST) {
        uint32_t position;
        int64_t integer = -1;
        if (slot->length == 0u) return PIKA_STATUS_INVALID_OPERAND;
        if (has_index) {
            status = integer_from_value(index, &integer);
            if (status != PIKA_STATUS_OK) return status;
        }
        status = normalize_mutable_index(integer, slot->length, &position);
        if (status != PIKA_STATUS_OK) return status;
        *result = ((PikaRuntimeValue*)slot->data)[position];
        if (position + 1u < slot->length) {
            memmove(&((PikaRuntimeValue*)slot->data)[position],
                    &((PikaRuntimeValue*)slot->data)[position + 1u],
                    (slot->length - position - 1u) *
                        sizeof(PikaRuntimeValue));
        }
        --slot->length;
        return PIKA_STATUS_OK;
    }
    if (container.kind == PIKA_RUNTIME_VALUE_BYTEARRAY) {
        uint32_t position;
        int64_t integer = -1;
        if (slot->length == 0u) return PIKA_STATUS_INVALID_OPERAND;
        if (has_index) {
            status = integer_from_value(index, &integer);
            if (status != PIKA_STATUS_OK) return status;
        }
        status = normalize_mutable_index(integer, slot->length, &position);
        if (status != PIKA_STATUS_OK) return status;
        *result = runtime_integer_value(
            (unsigned char)((char*)slot->data)[position]);
        if (position + 1u < slot->length) {
            memmove(&((char*)slot->data)[position],
                    &((char*)slot->data)[position + 1u],
                    slot->length - position - 1u);
        }
        --slot->length;
        ((char*)slot->data)[slot->length] = '\0';
        return PIKA_STATUS_OK;
    }
    return PIKA_STATUS_TYPE_MISMATCH;
}

#if PIKA_CAPABILITY_OP_SLICE_ENABLE

static PikaStatus sequence_bounds(uint32_t length,
                                  PikaRuntimeValue start_value,
                                  PikaRuntimeValue stop_value,
                                  PikaRuntimeValue step_value,
                                  int64_t* start,
                                  int64_t* stop,
                                  int64_t* step) {
    int64_t supplied;
    PikaStatus status;
    if (step_value.kind == PIKA_RUNTIME_VALUE_NONE) {
        *step = 1;
    } else {
        status = integer_from_value(step_value, step);
        if (status != PIKA_STATUS_OK) return PIKA_STATUS_TYPE_MISMATCH;
        if (*step == 0) return PIKA_STATUS_DIVISION_BY_ZERO;
    }
    if (start_value.kind == PIKA_RUNTIME_VALUE_NONE) {
        *start = *step > 0 ? 0 : (int64_t)length - 1;
    } else if (integer_from_value(start_value, start) != PIKA_STATUS_OK) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    if (stop_value.kind == PIKA_RUNTIME_VALUE_NONE) {
        *stop = *step > 0 ? (int64_t)length : -1;
    } else if (integer_from_value(stop_value, stop) != PIKA_STATUS_OK) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    supplied = (int64_t)length;
    if (*step > 0) {
        if (*start < 0) {
            *start += supplied;
        }
        if (*stop < 0) {
            *stop += supplied;
        }
        if (*start < 0) *start = 0;
        if (*start > supplied) *start = supplied;
        if (*stop < 0) *stop = 0;
        if (*stop > supplied) *stop = supplied;
    } else {
        if (start_value.kind != PIKA_RUNTIME_VALUE_NONE && *start < 0) {
            *start += supplied;
        }
        if (*start >= supplied) *start = supplied - 1;
        if (*start < -1) *start = -1;
        if (stop_value.kind != PIKA_RUNTIME_VALUE_NONE && *stop < 0) {
            *stop += supplied;
        }
        if (*stop >= supplied) *stop = supplied - 1;
        if (*stop < -1) *stop = -1;
    }
    return PIKA_STATUS_OK;
}

PikaStatus pika_runtime_sequence_slice(
    PikaRuntimeContext* context,
    PikaRuntimeValue value,
    PikaRuntimeValue start,
    PikaRuntimeValue stop,
    PikaRuntimeValue step,
    PikaRuntimeValue* result) {
    PikaObjectSlot* slot = pika_runtime_object(context, value);
    int64_t first;
    int64_t last;
    int64_t stride;
    uint32_t count = 0u;
    int64_t cursor;
    PikaStatus status;
    if (slot == NULL || result == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_RANGE) {
        const PikaRangeData* range = (const PikaRangeData*)slot->data;
        uint32_t length;
        uint32_t count = 0u;
        PikaRuntimeValue first_value;
        PikaRuntimeValue last_value;
        int64_t last_index;
        int64_t index_offset;
        int64_t new_step;
        int64_t new_stop;
        status = pika_runtime_range_length(context, value, &length);
        if (status != PIKA_STATUS_OK) return status;
        status = sequence_bounds(
            length, start, stop, step, &first, &last, &stride);
        if (status != PIKA_STATUS_OK) return status;
        for (cursor = first;
             (stride > 0 && cursor < last) ||
             (stride < 0 && cursor > last);
             cursor += stride) {
            ++count;
        }
        if (count == 0u) {
            return pika_runtime_create_range(context, 0, 0, 1, result);
        }
        status = range_item_value(
            range, (uint32_t)first, &first_value);
        if (status != PIKA_STATUS_OK) return status;
        status = checked_multiply_integer(
            stride, (int64_t)(count - 1u), &index_offset);
        if (status != PIKA_STATUS_OK) return status;
        status = checked_add_integer(first, index_offset, &last_index);
        if (status != PIKA_STATUS_OK || last_index < 0) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        status = range_item_value(
            range, (uint32_t)last_index,
            &last_value);
        if (status != PIKA_STATUS_OK) return status;
        status = checked_multiply_integer(
            range->step, stride, &new_step);
        if (status != PIKA_STATUS_OK) return status;
        status = checked_add_integer(
            last_value.as.integer, new_step, &new_stop);
        if (status != PIKA_STATUS_OK) return status;
        return pika_runtime_create_range(
            context, first_value.as.integer, new_stop, new_step, result);
    }
    status = sequence_bounds(
        slot->length, start, stop, step, &first, &last, &stride);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    for (cursor = first;
         (stride > 0 && cursor < last) || (stride < 0 && cursor > last);
         cursor += stride) {
        ++count;
    }
    if (is_value_sequence(value.kind)) {
        PikaRuntimeValue* values = NULL;
        uint32_t object_index;
        void* data = NULL;
        status = pika_runtime_create_object(
            context, value.kind, count,
            (uint32_t)sizeof(PikaRuntimeValue), &object_index, &data);
        if (status != PIKA_STATUS_OK) return status;
        slot = pika_runtime_object(context, value);
        if (slot == NULL ||
            (slot->length != 0u && slot->data == NULL)) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        values = (PikaRuntimeValue*)data;
        cursor = first;
        for (uint32_t index = 0u; index < count; ++index) {
            values[index] = ((PikaRuntimeValue*)slot->data)[cursor];
            cursor += stride;
        }
        memset(result, 0, sizeof(*result));
        result->kind = value.kind;
        result->as.object_index = object_index;
        return PIKA_STATUS_OK;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_STRING ||
        is_byte_sequence(value.kind)) {
        unsigned char* bytes = NULL;
        status = pika_runtime_create_text_buffer(
            context, value.kind, count, result, &bytes);
        if (status != PIKA_STATUS_OK) return status;
        slot = pika_runtime_object(context, value);
        if (slot == NULL ||
            (slot->length != 0u && slot->data == NULL)) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        cursor = first;
        for (uint32_t index = 0u; index < count; ++index) {
            bytes[index] = ((const unsigned char*)slot->data)[cursor];
            cursor += stride;
        }
        return PIKA_STATUS_OK;
    }
    return PIKA_STATUS_TYPE_MISMATCH;
}

#endif

static PikaStatus value_equal_depth(PikaRuntimeContext* context,
                                    PikaRuntimeValue left,
                                    PikaRuntimeValue right,
                                    int depth,
                                    int* equal) {
    PikaObjectSlot* left_slot;
    PikaObjectSlot* right_slot;
    if (depth > 32) return PIKA_STATUS_INVALID_OPERAND;
    if ((left.kind == PIKA_RUNTIME_VALUE_INTEGER ||
         left.kind == PIKA_RUNTIME_VALUE_BOOLEAN) &&
        (right.kind == PIKA_RUNTIME_VALUE_INTEGER ||
         right.kind == PIKA_RUNTIME_VALUE_BOOLEAN)) {
        *equal = left.as.integer == right.as.integer;
        return PIKA_STATUS_OK;
    }
    if ((left.kind == PIKA_RUNTIME_VALUE_INTEGER ||
         left.kind == PIKA_RUNTIME_VALUE_BOOLEAN ||
         left.kind == PIKA_RUNTIME_VALUE_FLOAT) &&
        (right.kind == PIKA_RUNTIME_VALUE_INTEGER ||
         right.kind == PIKA_RUNTIME_VALUE_BOOLEAN ||
         right.kind == PIKA_RUNTIME_VALUE_FLOAT)) {
        double l = left.kind == PIKA_RUNTIME_VALUE_FLOAT
                       ? left.as.floating : (double)left.as.integer;
        double r = right.kind == PIKA_RUNTIME_VALUE_FLOAT
                       ? right.as.floating : (double)right.as.integer;
        *equal = l == r;
        return PIKA_STATUS_OK;
    }
    if (left.kind != right.kind) {
        *equal = 0;
        return PIKA_STATUS_OK;
    }
    if (left.kind == PIKA_RUNTIME_VALUE_NONE) {
        *equal = 1;
        return PIKA_STATUS_OK;
    }
    left_slot = pika_runtime_object(context, left);
    right_slot = pika_runtime_object(context, right);
    if (left_slot == NULL || right_slot == NULL) return PIKA_STATUS_INVALID_OPERAND;
    if ((left_slot->length != 0u && left_slot->data == NULL) ||
        (right_slot->length != 0u && right_slot->data == NULL)) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (left.kind == PIKA_RUNTIME_VALUE_STRING ||
        left.kind == PIKA_RUNTIME_VALUE_TYPE ||
        is_byte_sequence(left.kind)) {
        *equal = left_slot->length == right_slot->length &&
                 memcmp(left_slot->data, right_slot->data,
                        left_slot->length) == 0;
        return PIKA_STATUS_OK;
    }
    if (is_value_sequence(left.kind)) {
        if (left_slot->length != right_slot->length) {
            *equal = 0;
            return PIKA_STATUS_OK;
        }
        *equal = 1;
        for (uint32_t index = 0u; index < left_slot->length; ++index) {
            int item_equal = 0;
            PikaStatus status = value_equal_depth(
                context, ((PikaRuntimeValue*)left_slot->data)[index],
                ((PikaRuntimeValue*)right_slot->data)[index], depth + 1,
                &item_equal);
            if (status != PIKA_STATUS_OK) return status;
            if (!item_equal) {
                *equal = 0;
                break;
            }
        }
        return PIKA_STATUS_OK;
    }
    *equal = left.as.object_index == right.as.object_index;
    return PIKA_STATUS_OK;
}

PikaStatus pika_runtime_value_equal(
    PikaRuntimeContext* context,
    PikaRuntimeValue left,
    PikaRuntimeValue right,
    int* equal) {
    if (context == NULL || equal == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return value_equal_depth(context, left, right, 0, equal);
}

#endif
