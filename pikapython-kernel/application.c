/* SPEC: PJ2026-0501 V2 kernel v0.47; bounded application storage. */
#include "pika_application.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "pika_frontend.h"
#include "pika_kernel.h"
#include "pika_platform.h"
#include "pika_program_image.h"
#if PIKA_REPL_ENABLE
#include "pika_repl.h"
#endif

#if PIKA_FRONTEND_MEMORY_PROBE_ENABLE
void pika_application_memory_probe_checkpoint(
    const char* phase,
    uint32_t index);
#define PIKA_APPLICATION_MEMORY_CHECKPOINT(phase, index) \
    pika_application_memory_probe_checkpoint((phase), (index))
#else
#define PIKA_APPLICATION_MEMORY_CHECKPOINT(phase, index) ((void)0)
#endif

#if PIKA_REPL_ENABLE
#ifndef PIKA_APPLICATION_REPL_BLOCK_BYTES
#define PIKA_APPLICATION_REPL_BLOCK_BYTES 1024u
#endif
#ifndef PIKA_APPLICATION_REPL_HISTORY_BYTES
#define PIKA_APPLICATION_REPL_HISTORY_BYTES 512u
#endif
#ifndef PIKA_APPLICATION_REPL_HISTORY_ENTRIES
#define PIKA_APPLICATION_REPL_HISTORY_ENTRIES 8u
#endif
#define PIKA_APPLICATION_REPL_READ_BYTES 64u
#endif

typedef struct {
    const PikaApplicationDescriptor* descriptor;
    PikaCompiledModule* compiled;
    PikaBindingRegistry bindings;
    PikaFrontendConfig frontend;
    PikaModuleProvider module_provider;
    const PikaBindingModule** binding_modules;
    void* frames;
    int64_t* values;
    PikaRuntimeValue* typed_values;
    PikaObjectSlot* object_slots;
    uint8_t* object_arena;
    PikaObjectStorage object_storage;
    PikaBindingValue* binding_values;
    PikaBindingObject* binding_objects;
    PikaStorage storage;
    PikaBindingStorage binding_storage;
    uint16_t binding_object_capacity;
#if PIKA_REPL_ENABLE
    PikaExecutionState execution;
    PikaReplSession repl;
    uint8_t* repl_line;
    uint8_t* repl_block;
    uint8_t* repl_escape;
    uint8_t* repl_history;
    uint32_t* repl_history_offsets;
    uint32_t* repl_history_lengths;
    uint8_t repl_running;
#endif
    uint8_t active;
} PikaApplicationState;

static PikaApplicationState pika_application_state;

static int module_name_equal(const char* left, const char* right) {
    return left != NULL && right != NULL &&
           left[0] == right[0] && strcmp(left, right) == 0;
}

static PikaStatus application_module_load(
    void* context,
    const char* module_name,
    const char** source,
    size_t* length) {
    const PikaApplicationDescriptor* descriptor =
        (const PikaApplicationDescriptor*)context;
    size_t index;
    if (descriptor == NULL || module_name == NULL ||
        source == NULL || length == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0u; index < descriptor->module_count; ++index) {
        const PikaFrozenModule* module = &descriptor->modules[index];
        if (module_name_equal(module->name, module_name)) {
            if (module->format != PIKA_FROZEN_MODULE_SOURCE) {
                return PIKA_STATUS_UNSUPPORTED_FORMAT;
            }
            *source = (const char*)module->data;
            *length = module->size;
            return PIKA_STATUS_OK;
        }
    }
    return PIKA_STATUS_MODULE_NOT_FOUND;
}

static void application_module_release(
    void* context,
    const char* source) {
    (void)context;
    (void)source;
}

static PikaStatus application_output_write(
    void* context,
    const char* bytes,
    uint32_t length) {
    (void)context;
    return pika_platform_write(bytes, (size_t)length);
}

static void application_report_instruction(uint32_t instruction_index) {
    static const char prefix[] = "PikaPython startup instruction: ";
    static const char newline[] = "\n";
    char digits[10];
    size_t count = 0u;
    size_t index;
    do {
        digits[count] =
            (char)('0' + (instruction_index % 10u));
        instruction_index /= 10u;
        ++count;
    } while (instruction_index != 0u);
    (void)pika_platform_write(prefix, sizeof(prefix) - 1u);
    for (index = count; index > 0u; --index) {
        (void)pika_platform_write(&digits[index - 1u], 1u);
    }
    (void)pika_platform_write(newline, sizeof(newline) - 1u);
}

static void application_report_failure(
    PikaStatus status,
    const PikaResult* result) {
    static const char prefix[] = "PikaPython startup error: ";
    static const char newline[] = "\n";
    const char* name = pika_status_name(status);
    (void)pika_platform_write(prefix, sizeof(prefix) - 1u);
    (void)pika_platform_write(name, strlen(name));
    (void)pika_platform_write(newline, sizeof(newline) - 1u);
    if (result != NULL) {
        application_report_instruction(result->instruction_index);
    }
}

static void application_report_storage_stage(
    PikaStatus status,
    const char* stage) {
    static const char prefix[] = "PikaPython startup storage stage: ";
    static const char newline[] = "\n";
    if (status != PIKA_STATUS_STORAGE_TOO_SMALL || stage == NULL) {
        return;
    }
    (void)pika_platform_write(prefix, sizeof(prefix) - 1u);
    (void)pika_platform_write(stage, strlen(stage));
    (void)pika_platform_write(newline, sizeof(newline) - 1u);
}

#if PIKA_FROZEN_SOURCE_ENABLE
static void application_report_frontend_failure(
    const PikaDiagnostic* diagnostic) {
    static const char prefix[] = "PikaPython frontend limit: ";
    static const char newline[] = "\n";
    const char* macro;
    if (diagnostic == NULL ||
        diagnostic->reason != PIKA_DIAGNOSTIC_REASON_LIMIT_EXCEEDED ||
        diagnostic->bound == PIKA_FRONTEND_BOUND_NONE) {
        return;
    }
    macro = pika_frontend_bound_macro(diagnostic->bound);
    (void)pika_platform_write(prefix, sizeof(prefix) - 1u);
    (void)pika_platform_write(macro, strlen(macro));
    (void)pika_platform_write(newline, sizeof(newline) - 1u);
}
#endif

static size_t application_main_count(
    const PikaApplicationDescriptor* descriptor) {
    size_t index;
    size_t count = 0u;
    for (index = 0u; index < descriptor->module_count; ++index) {
        const PikaFrozenModule* module = &descriptor->modules[index];
        if (module_name_equal(module->name, "__main__")) {
            ++count;
        }
    }
    return count;
}

static const PikaFrozenModule* application_main_module(
    const PikaApplicationDescriptor* descriptor) {
    size_t index;
    for (index = 0u; index < descriptor->module_count; ++index) {
        const PikaFrozenModule* module =
            &descriptor->modules[index];
        if (module_name_equal(module->name, "__main__")) {
            return module;
        }
    }
    return NULL;
}

static const PikaProgramImageModule* application_image_module(
    const PikaProgramImage* image,
    const char* name) {
    size_t length = strlen(name);
    uint16_t index;
    if (image == NULL ||
        image->magic != PIKA_PROGRAM_IMAGE_MAGIC ||
        image->schema_version != PIKA_PROGRAM_IMAGE_SCHEMA_VERSION ||
        image->modules == NULL) {
        return NULL;
    }
    for (index = 0u; index < image->module_count; ++index) {
        const PikaProgramImageModule* module = &image->modules[index];
        if (module->name != NULL &&
            module->name_length == length &&
            memcmp(module->name, name, length) == 0) {
            return module;
        }
    }
    return NULL;
}

static int application_digest_valid(
    const uint8_t* digest, uint16_t length) {
    return digest != NULL &&
           length == PIKA_PROGRAM_IMAGE_DIGEST_LENGTH;
}

static PikaStatus application_validate_modules(
    const PikaApplicationDescriptor* descriptor) {
    const PikaProgramImage* program_image = NULL;
    size_t index;
    if (!application_digest_valid(
            descriptor->configuration_digest,
            descriptor->configuration_digest_length) ||
        !application_digest_valid(
            descriptor->capability_digest,
            descriptor->capability_digest_length)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0u; index < descriptor->module_count; ++index) {
        const PikaFrozenModule* module = &descriptor->modules[index];
        size_t previous;
        if (module->name == NULL || module->name[0] == '\0' ||
            module->data == NULL || module->size == 0u ||
            !application_digest_valid(
                module->source_digest,
                module->source_digest_length) ||
            (module->format != PIKA_FROZEN_MODULE_SOURCE &&
             module->format != PIKA_FROZEN_MODULE_PROGRAM)) {
            return PIKA_STATUS_INVALID_ARGUMENT;
        }
        for (previous = 0u; previous < index; ++previous) {
            if (module_name_equal(
                    descriptor->modules[previous].name,
                    module->name)) {
                return PIKA_STATUS_MODULE_CONFLICT;
            }
        }
        if (module->format == PIKA_FROZEN_MODULE_PROGRAM) {
            const PikaProgramImage* image;
            if (module->size != sizeof(PikaProgramImage)) {
                return PIKA_STATUS_UNSUPPORTED_FORMAT;
            }
            image = (const PikaProgramImage*)module->data;
            if (program_image == NULL) {
                program_image = image;
            } else if (program_image != image) {
                return PIKA_STATUS_MODULE_CONFLICT;
            }
        }
    }
    if (program_image != NULL) {
        uint16_t image_index;
        if (!application_digest_valid(
                program_image->configuration_digest,
                program_image->configuration_digest_length) ||
            !application_digest_valid(
                program_image->capability_digest,
                program_image->capability_digest_length) ||
            memcmp(
                descriptor->configuration_digest,
                program_image->configuration_digest,
                PIKA_PROGRAM_IMAGE_DIGEST_LENGTH) != 0 ||
            memcmp(
                descriptor->capability_digest,
                program_image->capability_digest,
                PIKA_PROGRAM_IMAGE_DIGEST_LENGTH) != 0) {
            return PIKA_STATUS_UNSUPPORTED_FORMAT;
        }
        for (index = 0u; index < descriptor->module_count; ++index) {
            const PikaFrozenModule* module =
                &descriptor->modules[index];
            const PikaProgramImageModule* image_module =
                application_image_module(program_image, module->name);
            if (module->format == PIKA_FROZEN_MODULE_PROGRAM) {
                if (image_module == NULL ||
                    !application_digest_valid(
                        image_module->source_digest,
                        image_module->source_digest_length) ||
                    memcmp(
                        module->source_digest,
                        image_module->source_digest,
                        PIKA_PROGRAM_IMAGE_DIGEST_LENGTH) != 0) {
                    return PIKA_STATUS_UNSUPPORTED_FORMAT;
                }
            } else if (image_module != NULL) {
                return PIKA_STATUS_MODULE_CONFLICT;
            }
        }
        for (image_index = 0u;
             image_index < program_image->module_count;
             ++image_index) {
            const PikaProgramImageModule* image_module =
                &program_image->modules[image_index];
            int found = 0;
            if (image_module->name == NULL ||
                image_module->name_length == 0u) {
                return PIKA_STATUS_UNSUPPORTED_FORMAT;
            }
            for (index = 0u;
                 index < descriptor->module_count;
                 ++index) {
                const PikaFrozenModule* module =
                    &descriptor->modules[index];
                if (module->format ==
                        PIKA_FROZEN_MODULE_PROGRAM &&
                    strlen(module->name) ==
                        image_module->name_length &&
                    memcmp(
                        module->name,
                        image_module->name,
                        image_module->name_length) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) return PIKA_STATUS_MODULE_CONFLICT;
        }
    }
    return PIKA_STATUS_OK;
}

static PikaStatus application_register_bindings(
    PikaApplicationState* state,
    const PikaApplicationDescriptor* descriptor) {
#if PIKA_BINDING_RUNTIME_ENABLE
    size_t index;
    PikaStatus status;
    if (descriptor->binding_module_count > UINT16_MAX) {
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    if (descriptor->binding_module_count > 0u) {
        state->binding_modules =
            (const PikaBindingModule**)calloc(
                descriptor->binding_module_count,
                sizeof(*state->binding_modules));
        if (state->binding_modules == NULL) {
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
    }
    status = pika_binding_registry_initialize(
        &state->bindings, state->binding_modules,
        (uint16_t)descriptor->binding_module_count);
    if (status != PIKA_STATUS_OK) return status;
    for (index = 0u;
         index < descriptor->binding_module_count;
         ++index) {
        status = pika_binding_registry_register(
            &state->bindings,
            descriptor->binding_modules[index]);
        if (status != PIKA_STATUS_OK) return status;
    }
    return PIKA_STATUS_OK;
#else
    (void)state;
    return descriptor->binding_module_count == 0u
               ? PIKA_STATUS_OK
               : PIKA_STATUS_UNSUPPORTED_CAPABILITY;
#endif
}

static void* application_calloc(size_t count, size_t size) {
    if (count == 0u || size == 0u) return NULL;
    return calloc(count, size);
}

static PikaStatus application_grow_allocation(
    void* pointer,
    uint32_t current_count,
    uint32_t required_count,
    size_t item_size,
    void** grown_pointer) {
    size_t current_bytes;
    size_t required_bytes;
    void* grown;
    if (grown_pointer == NULL || item_size == 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *grown_pointer = pointer;
    if (required_count <= current_count) return PIKA_STATUS_OK;
    if ((size_t)required_count > SIZE_MAX / item_size) {
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    current_bytes = (size_t)current_count * item_size;
    required_bytes = (size_t)required_count * item_size;
    grown = realloc(pointer, required_bytes);
    if (grown == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
    memset((uint8_t*)grown + current_bytes, 0,
           required_bytes - current_bytes);
    *grown_pointer = grown;
    return PIKA_STATUS_OK;
}

#if PIKA_REPL_ENABLE
static uint32_t application_compact_allocation(
    void* pointer,
    uint32_t current_count,
    uint32_t retained_count,
    size_t item_size,
    void** compacted_pointer) {
    void* compacted;
    if (compacted_pointer == NULL || item_size == 0u) {
        return current_count;
    }
    *compacted_pointer = pointer;
    if (retained_count >= current_count) return current_count;
    if (retained_count == 0u) {
        free(pointer);
        *compacted_pointer = NULL;
        return 0u;
    }
    compacted = realloc(
        pointer, (size_t)retained_count * item_size);
    if (compacted == NULL) return current_count;
    *compacted_pointer = compacted;
    return retained_count;
}
#endif

static PikaStatus application_grow_object_arena(
    PikaApplicationState* state,
    uint32_t required_bytes) {
    uint32_t current_bytes = state->object_storage.arena_capacity;
    uintptr_t current_address = (uintptr_t)state->object_arena;
    void* allocation;
    uint8_t* arena;
    uint16_t index;
    PikaStatus status = application_grow_allocation(
        state->object_arena, current_bytes,
        required_bytes, 1u, &allocation);
    if (status != PIKA_STATUS_OK) return status;
    arena = (uint8_t*)allocation;
    if (required_bytes > current_bytes && current_bytes > 0u) {
        for (index = 0u;
             index < state->object_storage.count;
             ++index) {
            PikaObjectSlot* slot =
                &state->object_storage.slots[index];
            uintptr_t address = (uintptr_t)slot->data;
            if (slot->active != 0u && slot->owned == 0u &&
                slot->data != NULL && address >= current_address &&
                address - current_address < current_bytes) {
                slot->data =
                    &arena[address - current_address];
            }
        }
    }
    state->object_arena = arena;
    state->object_storage.arena = arena;
    state->object_storage.arena_capacity = required_bytes;
    return PIKA_STATUS_OK;
}

static uint32_t application_growth_target(
    uint32_t current,
    uint32_t required,
    uint32_t limit,
    uint32_t minimum) {
    uint32_t target = current;
    if (required > limit) return 0u;
    if (target == 0u) {
        target = minimum < limit ? minimum : limit;
    }
    while (target < required) {
        if (target > limit / 2u) {
            target = limit;
        } else {
            target *= 2u;
        }
    }
    return target;
}

static PikaStatus application_reserve_object_storage(
    void* context,
    uint16_t required_slots,
    uint32_t required_bytes) {
    PikaApplicationState* state =
        (PikaApplicationState*)context;
    uint32_t target_slots;
    uint32_t target_bytes;
    void* allocation;
    PikaStatus status;
    if (state == NULL ||
        required_slots > state->object_storage.slot_limit ||
        required_bytes > state->object_storage.arena_limit) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    target_slots = application_growth_target(
        state->object_storage.capacity,
        required_slots,
        state->object_storage.slot_limit, 4u);
    target_bytes = application_growth_target(
        state->object_storage.arena_capacity,
        required_bytes,
        state->object_storage.arena_limit, 256u);
    if ((required_slots > 0u && target_slots == 0u) ||
        (required_bytes > 0u && target_bytes == 0u)) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    status = application_grow_allocation(
        state->object_slots,
        state->object_storage.capacity,
        target_slots,
        sizeof(*state->object_slots), &allocation);
    if (status != PIKA_STATUS_OK) return status;
    state->object_slots = (PikaObjectSlot*)allocation;
    state->object_storage.slots = state->object_slots;
    state->object_storage.capacity = (uint16_t)target_slots;
    status = application_grow_object_arena(
        state, target_bytes);
    if (status != PIKA_STATUS_OK) return status;
    PIKA_APPLICATION_MEMORY_CHECKPOINT(
        "application-object-slot-capacity",
        state->object_storage.capacity);
    PIKA_APPLICATION_MEMORY_CHECKPOINT(
        "application-object-arena-capacity",
        state->object_storage.arena_capacity);
    return PIKA_STATUS_OK;
}

#if PIKA_REPL_ENABLE
static void application_compact_object_arena(
    PikaApplicationState* state) {
    uint32_t current_bytes = state->object_storage.arena_capacity;
    uint32_t retained_bytes = state->object_storage.arena_used;
    uintptr_t current_address = (uintptr_t)state->object_arena;
    uint8_t* arena;
    uint16_t index;
    if (retained_bytes >= current_bytes) return;
    if (retained_bytes == 0u) {
        free(state->object_arena);
        state->object_arena = NULL;
        state->object_storage.arena = NULL;
        state->object_storage.arena_capacity = 0u;
        return;
    }
    arena = (uint8_t*)realloc(
        state->object_arena, retained_bytes);
    if (arena == NULL) return;
    for (index = 0u;
         index < state->object_storage.count;
         ++index) {
        PikaObjectSlot* slot =
            &state->object_storage.slots[index];
        uintptr_t address = (uintptr_t)slot->data;
        if (slot->active != 0u && slot->owned == 0u &&
            slot->data != NULL && address >= current_address &&
            address - current_address < current_bytes) {
            slot->data =
                &arena[address - current_address];
        }
    }
    state->object_arena = arena;
    state->object_storage.arena = arena;
    state->object_storage.arena_capacity = retained_bytes;
}

static void application_compact_repl_storage(
    void* context,
    const PikaExecutionState* execution) {
    PikaApplicationState* state =
        (PikaApplicationState*)context;
    uint32_t persistent_slots;
    uint32_t retained_values;
    uint32_t retained_typed_values;
    uint32_t retained_binding_values;
    void* allocation;
    if (state == NULL || execution == NULL) return;
    persistent_slots =
        execution->initialized != 0u
            ? execution->persistent_slot_count
            : 0u;
    retained_values =
        execution->uses_typed_values == 0u
            ? persistent_slots
            : 0u;
    retained_typed_values =
        execution->uses_typed_values != 0u
            ? persistent_slots
            : 0u;
    retained_binding_values =
        execution->uses_binding_values != 0u
            ? persistent_slots
            : 0u;

    state->storage.frame_bytes =
        application_compact_allocation(
            state->frames, state->storage.frame_bytes,
            0u, 1u, &allocation);
    state->frames = allocation;
    state->storage.frames = allocation;
    state->storage.value_capacity =
        application_compact_allocation(
            state->values, state->storage.value_capacity,
            retained_values, sizeof(*state->values),
            &allocation);
    state->values = (int64_t*)allocation;
    state->storage.values = state->values;
    state->storage.typed_value_capacity =
        application_compact_allocation(
            state->typed_values,
            state->storage.typed_value_capacity,
            retained_typed_values,
            sizeof(*state->typed_values), &allocation);
    state->typed_values = (PikaRuntimeValue*)allocation;
    state->storage.typed_values = state->typed_values;
    state->object_storage.capacity =
        (uint16_t)application_compact_allocation(
            state->object_slots,
            state->object_storage.capacity,
            state->object_storage.count,
            sizeof(*state->object_slots), &allocation);
    state->object_slots = (PikaObjectSlot*)allocation;
    state->object_storage.slots = state->object_slots;
    application_compact_object_arena(state);
    state->binding_storage.value_capacity =
        application_compact_allocation(
            state->binding_values,
            state->binding_storage.value_capacity,
            retained_binding_values,
            sizeof(*state->binding_values), &allocation);
    state->binding_values = (PikaBindingValue*)allocation;
    state->binding_storage.values = state->binding_values;
    state->binding_storage.object_capacity =
        (uint16_t)application_compact_allocation(
            state->binding_objects,
            state->binding_storage.object_capacity,
            execution->binding_object_count,
            sizeof(*state->binding_objects), &allocation);
    state->binding_objects = (PikaBindingObject*)allocation;
    state->binding_storage.objects = state->binding_objects;
    state->binding_object_capacity =
        state->binding_storage.object_capacity;
    PIKA_APPLICATION_MEMORY_CHECKPOINT(
        "application-idle-value-count",
        state->storage.value_capacity);
    PIKA_APPLICATION_MEMORY_CHECKPOINT(
        "application-idle-typed-value-count",
        state->storage.typed_value_capacity);
    PIKA_APPLICATION_MEMORY_CHECKPOINT(
        "application-idle-object-slot-count",
        state->object_storage.capacity);
    PIKA_APPLICATION_MEMORY_CHECKPOINT(
        "application-idle-object-bytes",
        state->object_storage.arena_capacity);
    PIKA_APPLICATION_MEMORY_CHECKPOINT(
        "application-idle-object-live-count",
        state->object_storage.count);
    PIKA_APPLICATION_MEMORY_CHECKPOINT(
        "application-idle-object-live-bytes",
        state->object_storage.arena_used);
}

static PikaStatus application_ensure_repl_storage(
    void* context,
    const PikaProgramRequirements* requirements) {
    PikaApplicationState* state =
        (PikaApplicationState*)context;
    void* allocation;
    PikaStatus status;
    if (state == NULL || requirements == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = application_grow_allocation(
        state->frames, state->storage.frame_bytes,
        requirements->frame_bytes, 1u, &allocation);
    if (status != PIKA_STATUS_OK) return status;
    state->frames = allocation;
    state->storage.frames = allocation;
    if (requirements->frame_bytes > state->storage.frame_bytes) {
        state->storage.frame_bytes = requirements->frame_bytes;
    }
    status = application_grow_allocation(
        state->values, state->storage.value_capacity,
        requirements->value_count, sizeof(*state->values),
        &allocation);
    if (status != PIKA_STATUS_OK) return status;
    state->values = (int64_t*)allocation;
    state->storage.values = state->values;
    if (requirements->value_count > state->storage.value_capacity) {
        state->storage.value_capacity = requirements->value_count;
    }
    status = application_grow_allocation(
        state->typed_values,
        state->storage.typed_value_capacity,
        requirements->typed_value_count,
        sizeof(*state->typed_values), &allocation);
    if (status != PIKA_STATUS_OK) return status;
    state->typed_values = (PikaRuntimeValue*)allocation;
    state->storage.typed_values = state->typed_values;
    if (requirements->typed_value_count >
        state->storage.typed_value_capacity) {
        state->storage.typed_value_capacity =
            requirements->typed_value_count;
    }
    if (requirements->object_slot_count <
            state->object_storage.count ||
        requirements->object_bytes <
            state->object_storage.arena_used) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    state->object_storage.slot_limit =
        requirements->object_slot_count;
    state->object_storage.arena_limit =
        requirements->object_bytes;
    if (requirements->object_slot_count > 0u) {
        state->storage.objects = &state->object_storage;
    }
    status = application_grow_allocation(
        state->binding_values,
        state->binding_storage.value_capacity,
        requirements->binding_value_count,
        sizeof(*state->binding_values), &allocation);
    if (status != PIKA_STATUS_OK) return status;
    state->binding_values = (PikaBindingValue*)allocation;
    state->binding_storage.values = state->binding_values;
    if (requirements->binding_value_count >
        state->binding_storage.value_capacity) {
        state->binding_storage.value_capacity =
            requirements->binding_value_count;
    }
    status = application_grow_allocation(
        state->binding_objects,
        state->binding_storage.object_capacity,
        requirements->binding_object_count,
        sizeof(*state->binding_objects), &allocation);
    if (status != PIKA_STATUS_OK) return status;
    state->binding_objects = (PikaBindingObject*)allocation;
    state->binding_storage.objects = state->binding_objects;
    if (requirements->binding_object_count >
        state->binding_storage.object_capacity) {
        state->binding_storage.object_capacity =
            requirements->binding_object_count;
        state->binding_object_capacity =
            requirements->binding_object_count;
    }
    return PIKA_STATUS_OK;
}
#endif

static PikaStatus application_allocate_storage(
    PikaApplicationState* state,
    const PikaProgramRequirements* requirements) {
    state->frames = application_calloc(
        requirements->frame_bytes, 1u);
    PIKA_APPLICATION_MEMORY_CHECKPOINT(
        "application-frame-bytes",
        requirements->frame_bytes);
    state->values = (int64_t*)application_calloc(
        requirements->value_count, sizeof(*state->values));
    PIKA_APPLICATION_MEMORY_CHECKPOINT(
        "application-value-count",
        requirements->value_count);
    state->typed_values = (PikaRuntimeValue*)application_calloc(
        requirements->typed_value_count,
        sizeof(*state->typed_values));
    PIKA_APPLICATION_MEMORY_CHECKPOINT(
        "application-typed-value-count",
        requirements->typed_value_count);
    state->object_slots = NULL;
    PIKA_APPLICATION_MEMORY_CHECKPOINT(
        "application-object-slot-limit",
        requirements->object_slot_count);
    state->object_arena = NULL;
    PIKA_APPLICATION_MEMORY_CHECKPOINT(
        "application-object-arena-limit",
        requirements->object_bytes);
    state->binding_values = (PikaBindingValue*)application_calloc(
        requirements->binding_value_count,
        sizeof(*state->binding_values));
    PIKA_APPLICATION_MEMORY_CHECKPOINT(
        "application-binding-value-count",
        requirements->binding_value_count);
    state->binding_objects = (PikaBindingObject*)application_calloc(
        requirements->binding_object_count,
        sizeof(*state->binding_objects));
    PIKA_APPLICATION_MEMORY_CHECKPOINT(
        "application-binding-object-count",
        requirements->binding_object_count);
    if ((requirements->frame_bytes > 0u && state->frames == NULL) ||
        (requirements->value_count > 0u && state->values == NULL) ||
        (requirements->typed_value_count > 0u &&
         state->typed_values == NULL) ||
        (requirements->binding_value_count > 0u &&
         state->binding_values == NULL) ||
        (requirements->binding_object_count > 0u &&
         state->binding_objects == NULL)) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    memset(&state->object_storage, 0, sizeof(state->object_storage));
    state->object_storage.slots = state->object_slots;
    state->object_storage.capacity = 0u;
    state->object_storage.slot_limit =
        requirements->object_slot_count;
    state->object_storage.arena = state->object_arena;
    state->object_storage.arena_capacity = 0u;
    state->object_storage.arena_limit =
        requirements->object_bytes;
    state->object_storage.reserve =
        application_reserve_object_storage;
    state->object_storage.context = state;
    state->storage.frames = state->frames;
    state->storage.frame_bytes = requirements->frame_bytes;
    state->storage.values = state->values;
    state->storage.value_capacity = requirements->value_count;
    state->storage.typed_values = state->typed_values;
    state->storage.typed_value_capacity =
        requirements->typed_value_count;
    state->storage.objects = requirements->object_slot_count > 0u
                                 ? &state->object_storage
                                 : NULL;
    state->binding_storage.values = state->binding_values;
    state->binding_storage.value_capacity =
        requirements->binding_value_count;
    state->binding_storage.objects = state->binding_objects;
    state->binding_storage.object_capacity =
        requirements->binding_object_count;
    state->binding_object_capacity =
        requirements->binding_object_count;
    return PIKA_STATUS_OK;
}

void pika_application_stop(void) {
    PikaApplicationState* state = &pika_application_state;
#if PIKA_BINDING_RUNTIME_ENABLE
    uint16_t index;
#endif
#if PIKA_REPL_ENABLE
    if (state->repl.initialized != 0u) {
        pika_repl_session_deinit(&state->repl);
    }
#endif
#if PIKA_BINDING_RUNTIME_ENABLE
    if (state->binding_objects != NULL) {
        for (index = 0u;
             index < state->binding_object_capacity;
             ++index) {
            pika_binding_object_release(
                &state->binding_objects[index]);
        }
    }
#endif
#if PIKA_TYPED_RUNTIME_ENABLE
    if (state->storage.objects != NULL) {
        pika_object_storage_release(&state->object_storage);
    }
#endif
    free(state->binding_objects);
    free(state->binding_values);
    free(state->object_arena);
    free(state->object_slots);
    free(state->typed_values);
    free(state->values);
    free(state->frames);
    free(state->binding_modules);
    pika_compiled_module_destroy(state->compiled);
#if PIKA_REPL_ENABLE
    free(state->repl_history_lengths);
    free(state->repl_history_offsets);
    free(state->repl_history);
    free(state->repl_escape);
    free(state->repl_block);
    free(state->repl_line);
#endif
    memset(state, 0, sizeof(*state));
}

PikaStatus pika_application_start(
    const PikaApplicationDescriptor* descriptor) {
    PikaApplicationState* state = &pika_application_state;
    PikaProgramRequirements requirements;
    PikaArguments arguments = {NULL, 0u};
    PikaOutput output = {application_output_write, NULL};
    PikaResult result;
    PikaMetrics metrics;
    PikaDiagnostic diagnostic;
    PikaStatus status;
    const PikaResult* failure_result = NULL;
    const char* storage_stage = "binding_registry";
    const PikaFrozenModule* main_module;
    size_t main_count;
    if (state->active != 0u) return PIKA_STATUS_OK;
    if (descriptor == NULL ||
        descriptor->schema_version !=
            PIKA_APPLICATION_SCHEMA_VERSION ||
        descriptor->modules == NULL ||
        descriptor->module_count == 0u ||
        (descriptor->binding_module_count > 0u &&
         descriptor->binding_modules == NULL)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = application_validate_modules(descriptor);
    if (status != PIKA_STATUS_OK) return status;
    main_count = application_main_count(descriptor);
    if (main_count == 0u) return PIKA_STATUS_MODULE_NOT_FOUND;
    if (main_count != 1u) return PIKA_STATUS_MODULE_CONFLICT;
    main_module = application_main_module(descriptor);
    if (main_module == NULL || main_module->data == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    memset(state, 0, sizeof(*state));
    state->descriptor = descriptor;
    status = application_register_bindings(state, descriptor);
    if (status != PIKA_STATUS_OK) goto fail;
    state->module_provider.load = application_module_load;
    state->module_provider.release = application_module_release;
    state->module_provider.context = (void*)descriptor;
    memset(&diagnostic, 0, sizeof(diagnostic));
    state->frontend.enabled =
        (descriptor->binding_module_count > 0u
             ? PIKA_CAPABILITY_BINDING_DEFAULT
             : PIKA_CAPABILITY_APPLICATION_DEFAULT) |
        (PIKA_CAPABILITY_OBJECT_CLASS & PIKA_CAPABILITY_PROVIDED);
#if PIKA_REPL_ENABLE
    state->frontend.enabled |=
        PIKA_CAPABILITY_NAME_GLOBAL & PIKA_CAPABILITY_PROVIDED;
#endif
    state->frontend.bindings = descriptor->binding_module_count > 0u
                            ? &state->bindings
                            : NULL;
    if (main_module->format == PIKA_FROZEN_MODULE_PROGRAM) {
#if PIKA_FROZEN_PROGRAM_ENABLE
        storage_stage = "program_image";
        if (main_module->size != sizeof(PikaProgramImage)) {
            status = PIKA_STATUS_UNSUPPORTED_FORMAT;
        } else {
            status = pika_compiled_module_from_image(
                (const PikaProgramImage*)main_module->data,
                state->frontend.enabled,
                state->frontend.bindings,
                &state->compiled);
        }
#else
        status = PIKA_STATUS_UNSUPPORTED_FORMAT;
#endif
    } else if (main_module->format ==
               PIKA_FROZEN_MODULE_SOURCE) {
#if PIKA_FROZEN_SOURCE_ENABLE
        storage_stage = "module_graph";
        status = pika_module_graph_compile(
            "__main__", &state->module_provider,
            &state->frontend, &state->compiled,
            &diagnostic);
        if (status != PIKA_STATUS_OK) {
            application_report_frontend_failure(&diagnostic);
        }
#else
        status = PIKA_STATUS_UNSUPPORTED_FORMAT;
#endif
    } else {
        status = PIKA_STATUS_UNSUPPORTED_FORMAT;
    }
    if (status != PIKA_STATUS_OK) {
        application_report_storage_stage(status, storage_stage);
        application_report_failure(status, NULL);
        pika_application_stop();
        return status;
    }
    status = pika_program_storage_requirements(
        pika_compiled_module_program(state->compiled),
        &requirements);
    if (status != PIKA_STATUS_OK) goto fail;
    storage_stage = "runtime_storage";
    status = application_allocate_storage(state, &requirements);
    if (status != PIKA_STATUS_OK) goto fail;
    storage_stage = "execution";
    memset(&result, 0, sizeof(result));
    failure_result = &result;
#if PIKA_REPL_ENABLE
    status = pika_program_execute_stateful(
        pika_compiled_module_program(state->compiled),
        &arguments, &state->storage,
        descriptor->binding_module_count > 0u
            ? &state->binding_storage
            : NULL,
        &output,
        pika_compiled_module_global_count(state->compiled),
        &state->execution, &result, &metrics);
#else
    status = pika_program_execute_bound(
        pika_compiled_module_program(state->compiled),
        &arguments, &state->storage,
        descriptor->binding_module_count > 0u
            ? &state->binding_storage
            : NULL,
        &output, &result, &metrics);
#endif
    if (status != PIKA_STATUS_OK) goto fail;
    state->active = 1u;
    return PIKA_STATUS_OK;

fail:
    application_report_storage_stage(status, storage_stage);
    application_report_failure(status, failure_result);
    pika_application_stop();
    return status;
}

#if PIKA_REPL_ENABLE
static PikaStatus application_allocate_repl_buffers(
    PikaApplicationState* state) {
    state->repl_line = (uint8_t*)application_calloc(
        PIKA_REPL_MAX_LINE_BYTES, 1u);
    state->repl_block = (uint8_t*)application_calloc(
        PIKA_APPLICATION_REPL_BLOCK_BYTES, 1u);
    state->repl_escape = (uint8_t*)application_calloc(
        PIKA_REPL_MAX_ESCAPE_BYTES, 1u);
    state->repl_history = (uint8_t*)application_calloc(
        PIKA_APPLICATION_REPL_HISTORY_BYTES, 1u);
    state->repl_history_offsets = (uint32_t*)application_calloc(
        PIKA_APPLICATION_REPL_HISTORY_ENTRIES,
        sizeof(*state->repl_history_offsets));
    state->repl_history_lengths = (uint32_t*)application_calloc(
        PIKA_APPLICATION_REPL_HISTORY_ENTRIES,
        sizeof(*state->repl_history_lengths));
    if (state->repl_line == NULL || state->repl_block == NULL ||
        state->repl_escape == NULL || state->repl_history == NULL ||
        state->repl_history_offsets == NULL ||
        state->repl_history_lengths == NULL) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus application_repl_result(
    const PikaApplicationState* state,
    PikaReplResult result) {
    const PikaReplDiagnostic* diagnostic;
    if (result == PIKA_REPL_RESULT_ENDED) return PIKA_STATUS_OK;
    diagnostic = pika_repl_last_diagnostic(&state->repl);
    if (diagnostic != NULL &&
        diagnostic->status != PIKA_STATUS_OK) {
        return diagnostic->status;
    }
    return PIKA_STATUS_RUNTIME_ERROR;
}

PikaStatus pika_application_repl(void) {
    PikaApplicationState* state = &pika_application_state;
    PikaReplConfig config;
    PikaReplIo io;
    PikaReplResult result;
    uint8_t input[PIKA_APPLICATION_REPL_READ_BYTES];
    PikaStatus status;
    size_t received;
    if (state->active == 0u ||
        (state->repl.initialized == 0u &&
         state->compiled == NULL)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (state->repl_running != 0u) {
        return PIKA_STATUS_RUNTIME_ERROR;
    }
    if (state->repl.initialized == 0u) {
        application_compact_repl_storage(
            state, &state->execution);
        status = application_allocate_repl_buffers(state);
        if (status != PIKA_STATUS_OK) return status;
        memset(&config, 0, sizeof(config));
        config.frontend = state->frontend;
#if PIKA_CAPABILITY_MODULE_PYTHON_ENABLE
        config.modules = &state->module_provider;
#endif
        config.storage = &state->storage;
        config.binding_storage =
            state->descriptor->binding_module_count > 0u
                ? &state->binding_storage
                : NULL;
        config.ensure_storage =
            application_ensure_repl_storage;
        config.ensure_storage_context = state;
        config.release_idle_storage =
            application_compact_repl_storage;
        config.release_idle_storage_context = state;
        config.initial_module = state->compiled;
        config.initial_execution = &state->execution;
        config.buffers.line = state->repl_line;
        config.buffers.line_capacity = PIKA_REPL_MAX_LINE_BYTES;
        config.buffers.block = state->repl_block;
        config.buffers.block_capacity =
            PIKA_APPLICATION_REPL_BLOCK_BYTES;
        config.buffers.escape = state->repl_escape;
        config.buffers.escape_capacity = PIKA_REPL_MAX_ESCAPE_BYTES;
        config.buffers.history = state->repl_history;
        config.buffers.history_capacity =
            PIKA_APPLICATION_REPL_HISTORY_BYTES;
        config.buffers.history_offsets =
            state->repl_history_offsets;
        config.buffers.history_lengths =
            state->repl_history_lengths;
        config.buffers.history_entry_capacity =
            PIKA_APPLICATION_REPL_HISTORY_ENTRIES;
        config.output_byte_limit =
            PIKA_REPL_MAX_OUTPUT_BYTES_PER_COMMAND;
        io.write = application_output_write;
        io.context = NULL;
        result = pika_repl_session_init(&state->repl, &config, &io);
        if (state->repl.initialized != 0u) {
            state->compiled = NULL;
            memset(&state->execution, 0, sizeof(state->execution));
        }
        if (result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
            return application_repl_result(state, result);
        }
    }
    if (pika_repl_session_state(&state->repl) ==
        PIKA_REPL_STATE_ENDED) {
        return PIKA_STATUS_OK;
    }
    state->repl_running = 1u;
    for (;;) {
        received = 0u;
        status = pika_platform_repl_read(
            input, sizeof(input), &received);
        if (status != PIKA_STATUS_OK) break;
        if (received > sizeof(input)) {
            status = PIKA_STATUS_IO_ERROR;
            break;
        }
        result = received == 0u
                     ? pika_repl_end_input(&state->repl)
                     : pika_repl_stream_accept(
                           &state->repl, PIKA_REPL_STREAM_BYTES,
                           input, received);
        if (result == PIKA_REPL_RESULT_ENDED) {
            status = PIKA_STATUS_OK;
            break;
        }
        if (result == PIKA_REPL_RESULT_FATAL_ERROR ||
            pika_repl_session_state(&state->repl) ==
                PIKA_REPL_STATE_IO_FAULT) {
            status = application_repl_result(state, result);
            break;
        }
    }
    state->repl_running = 0u;
    return status;
}
#endif
