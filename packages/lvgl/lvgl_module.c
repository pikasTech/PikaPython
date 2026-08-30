#include "_lvgl_binding.h"
#include "pika_capability_config.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#if PIKA_CAPABILITY_LIBRARY_LVGL_CORE_ENABLE

#define PIKA_LVGL_OBJECT_LIMIT 16u
#define PIKA_LVGL_TEXT_LIMIT 96u

typedef struct {
    uint8_t active;
    uint8_t kind;
    int64_t x;
    int64_t y;
    int64_t event_code;
    int64_t user_data;
    int64_t period_ms;
    void* parent;
    PikaBindingPythonCallable callback;
    char text[PIKA_LVGL_TEXT_LIMIT];
} PikaLvglObject;

static PikaLvglObject objects[PIKA_LVGL_OBJECT_LIMIT];
static PikaLvglObject active_screen;
static uint8_t initialized;
static uint32_t framebuffer_length;
static int64_t input_state;

static PikaStatus validate(const PikaBindingCall* call,
                           PikaBindingValue* result,
                           uint8_t count) {
    if (call == NULL || result == NULL || call->argument_count != count ||
        (count != 0u && call->arguments == NULL)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_OK;
}

static PikaLvglObject* allocate(uint8_t kind, void* parent) {
    uint16_t index;
    for (index = 0u; index < PIKA_LVGL_OBJECT_LIMIT; ++index) {
        if (objects[index].active == 0u) {
            memset(&objects[index], 0, sizeof(objects[index]));
            objects[index].active = 1u;
            objects[index].kind = kind;
            objects[index].parent = parent;
            return &objects[index];
        }
    }
    return NULL;
}

static PikaLvglObject* find(void* instance) {
    uint16_t index;
    if (instance == &active_screen) {
        return &active_screen;
    }
    for (index = 0u; index < PIKA_LVGL_OBJECT_LIMIT; ++index) {
        if (objects[index].active != 0u && instance == &objects[index]) {
            return &objects[index];
        }
    }
    return NULL;
}

static void clear_object(PikaLvglObject* object) {
    if (object == NULL) {
        return;
    }
    pika_binding_callable_release(&object->callback);
    memset(object, 0, sizeof(*object));
}

static void result_none(PikaBindingValue* result) {
    result->kind = PIKA_BINDING_VALUE_NONE;
    result->as.integer = 0;
}

static PikaStatus parent_value(const PikaBindingValue* value, void** parent) {
    if (value == NULL || parent == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (value->kind == PIKA_BINDING_VALUE_NONE) {
        *parent = NULL;
        return PIKA_STATUS_OK;
    }
    if (value->kind != PIKA_BINDING_VALUE_OPAQUE &&
        value->kind != PIKA_BINDING_VALUE_OBJECT) {
        return PIKA_STATUS_TYPE_ERROR;
    }
    *parent = value->as.opaque;
    return PIKA_STATUS_OK;
}

static PikaStatus construct(const PikaBindingCall* call,
                            PikaBindingValue* result,
                            uint8_t kind) {
    PikaLvglObject* object;
    void* parent;
    PikaStatus status = validate(call, result, 1u);
    if (status != PIKA_STATUS_OK) return status;
    status = parent_value(&call->arguments[0], &parent);
    if (status != PIKA_STATUS_OK) return status;
    object = allocate(kind, parent);
    if (object == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
    result->kind = PIKA_BINDING_VALUE_OPAQUE;
    result->as.opaque = object;
    return PIKA_STATUS_OK;
}

static PikaStatus method_object(const PikaBindingCall* call,
                                PikaBindingValue* result,
                                uint8_t count,
                                PikaLvglObject** object) {
    PikaStatus status = validate(call, result, count);
    if (status != PIKA_STATUS_OK) return status;
    *object = find(call->instance);
    if (*object == NULL || (*object)->active == 0u) {
        return PIKA_STATUS_RUNTIME_ERROR;
    }
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__lvgl_init(const PikaBindingCall* call,
                                  PikaBindingValue* result) {
    uint16_t index;
    PikaStatus status = validate(call, result, 0u);
    if (status == PIKA_STATUS_OK) {
        for (index = 0u; index < PIKA_LVGL_OBJECT_LIMIT; ++index) {
            clear_object(&objects[index]);
        }
        clear_object(&active_screen);
        active_screen.active = 1u;
        active_screen.kind = 1u;
        initialized = 1u;
        framebuffer_length = 0u;
        input_state = 0;
        result_none(result);
    }
    return status;
}

PikaStatus pika_binding__lvgl_scr_act(const PikaBindingCall* call,
                                     PikaBindingValue* result) {
    PikaStatus status = validate(call, result, 0u);
    if (status != PIKA_STATUS_OK) return status;
    if (initialized == 0u) return PIKA_STATUS_RUNTIME_ERROR;
    result->kind = PIKA_BINDING_VALUE_OBJECT;
    result->as.opaque = &active_screen;
    return PIKA_STATUS_OK;
}

#define CONSTRUCTOR(name, kind) \
PikaStatus pika_binding__lvgl_##name##_init(const PikaBindingCall* call, \
                                           PikaBindingValue* result) { \
    return construct(call, result, kind); \
}
CONSTRUCTOR(obj, 1u)
CONSTRUCTOR(btn, 2u)
CONSTRUCTOR(label, 3u)

#define COMMON_METHODS(name) \
PikaStatus pika_binding__lvgl_##name##_delete(const PikaBindingCall* call, PikaBindingValue* result) { \
    PikaLvglObject* object; \
    PikaStatus status = method_object(call, result, 0u, &object); \
    if (status != PIKA_STATUS_OK) { return status; } \
    clear_object(object); \
    result_none(result); \
    return PIKA_STATUS_OK; \
} \
PikaStatus pika_binding__lvgl_##name##_set_x(const PikaBindingCall* call, PikaBindingValue* result) { \
    PikaLvglObject* object; \
    PikaStatus status = method_object(call, result, 1u, &object); \
    if (status != PIKA_STATUS_OK) { return status; } \
    if (call->arguments[0].kind != PIKA_BINDING_VALUE_INTEGER) { return PIKA_STATUS_TYPE_ERROR; } \
    object->x = call->arguments[0].as.integer; \
    result_none(result); \
    return PIKA_STATUS_OK; \
} \
PikaStatus pika_binding__lvgl_##name##_set_y(const PikaBindingCall* call, PikaBindingValue* result) { \
    PikaLvglObject* object; \
    PikaStatus status = method_object(call, result, 1u, &object); \
    if (status != PIKA_STATUS_OK) { return status; } \
    if (call->arguments[0].kind != PIKA_BINDING_VALUE_INTEGER) { return PIKA_STATUS_TYPE_ERROR; } \
    object->y = call->arguments[0].as.integer; \
    result_none(result); \
    return PIKA_STATUS_OK; \
} \
PikaStatus pika_binding__lvgl_##name##_get_x(const PikaBindingCall* call, PikaBindingValue* result) { \
    PikaLvglObject* object; \
    PikaStatus status = method_object(call, result, 0u, &object); \
    if (status != PIKA_STATUS_OK) { return status; } \
    result->kind = PIKA_BINDING_VALUE_INTEGER; \
    result->as.integer = object->x; \
    return PIKA_STATUS_OK; \
} \
PikaStatus pika_binding__lvgl_##name##_get_y(const PikaBindingCall* call, PikaBindingValue* result) { \
    PikaLvglObject* object; \
    PikaStatus status = method_object(call, result, 0u, &object); \
    if (status != PIKA_STATUS_OK) { return status; } \
    result->kind = PIKA_BINDING_VALUE_INTEGER; \
    result->as.integer = object->y; \
    return PIKA_STATUS_OK; \
} \
PikaStatus pika_binding__lvgl_##name##_align(const PikaBindingCall* call, PikaBindingValue* result) { \
    PikaLvglObject* object; \
    PikaStatus status = method_object(call, result, 4u, &object); \
    if (status != PIKA_STATUS_OK) { return status; } \
    if (call->arguments[1].kind != PIKA_BINDING_VALUE_INTEGER || call->arguments[2].kind != PIKA_BINDING_VALUE_INTEGER || call->arguments[3].kind != PIKA_BINDING_VALUE_INTEGER) { return PIKA_STATUS_TYPE_ERROR; } \
    object->x = call->arguments[2].as.integer; \
    object->y = call->arguments[3].as.integer; \
    result_none(result); \
    return PIKA_STATUS_OK; \
}
COMMON_METHODS(obj)
COMMON_METHODS(btn)
COMMON_METHODS(label)

static PikaStatus schedule_object_callback(
    PikaLvglObject* object,
    int64_t first) {
    PikaBindingValue arguments[2];
    if (object == NULL || object->callback.active == 0u) {
        return PIKA_STATUS_RUNTIME_ERROR;
    }
    memset(arguments, 0, sizeof(arguments));
    arguments[0].kind = PIKA_BINDING_VALUE_INTEGER;
    arguments[0].as.integer = first;
    arguments[1].kind = PIKA_BINDING_VALUE_INTEGER;
    arguments[1].as.integer = object->user_data;
    return pika_binding_schedule(&object->callback, arguments, 2u);
}

#define EVENT_METHODS(name) \
PikaStatus pika_binding__lvgl_##name##_add_event_cb(const PikaBindingCall* call, PikaBindingValue* result) { \
    PikaLvglObject* object; \
    PikaStatus status = method_object(call, result, 3u, &object); \
    if (status != PIKA_STATUS_OK) { return status; } \
    if (call->arguments[0].kind != PIKA_BINDING_VALUE_CALLABLE || call->arguments[1].kind != PIKA_BINDING_VALUE_INTEGER || call->arguments[2].kind != PIKA_BINDING_VALUE_INTEGER) { return PIKA_STATUS_TYPE_ERROR; } \
    pika_binding_callable_release(&object->callback); \
    object->callback = call->arguments[0].as.callable; \
    object->event_code = call->arguments[1].as.integer; \
    object->user_data = call->arguments[2].as.integer; \
    result_none(result); \
    return PIKA_STATUS_OK; \
} \
PikaStatus pika_binding__lvgl_##name##_trigger_event(const PikaBindingCall* call, PikaBindingValue* result) { \
    PikaLvglObject* object; \
    PikaStatus status = method_object(call, result, 1u, &object); \
    if (status != PIKA_STATUS_OK) { return status; } \
    if (call->arguments[0].kind != PIKA_BINDING_VALUE_INTEGER) { return PIKA_STATUS_TYPE_ERROR; } \
    if (object->callback.active != 0u && object->event_code == call->arguments[0].as.integer) { \
        status = schedule_object_callback(object, call->arguments[0].as.integer); \
        if (status != PIKA_STATUS_OK) { return status; } \
    } \
    result_none(result); \
    return PIKA_STATUS_OK; \
}
EVENT_METHODS(obj)
EVENT_METHODS(btn)
EVENT_METHODS(label)

PikaStatus pika_binding__lvgl_timer_init(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaLvglObject* object;
    PikaStatus status = validate(call, result, 3u);
    if (status != PIKA_STATUS_OK) return status;
    if (call->arguments[0].kind != PIKA_BINDING_VALUE_CALLABLE ||
        call->arguments[1].kind != PIKA_BINDING_VALUE_INTEGER ||
        call->arguments[2].kind != PIKA_BINDING_VALUE_INTEGER) {
        return PIKA_STATUS_TYPE_ERROR;
    }
    object = allocate(4u, NULL);
    if (object == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
    object->callback = call->arguments[0].as.callable;
    object->period_ms = call->arguments[1].as.integer;
    object->user_data = call->arguments[2].as.integer;
    result->kind = PIKA_BINDING_VALUE_OPAQUE;
    result->as.opaque = object;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__lvgl_timer_trigger(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaLvglObject* object;
    PikaStatus status = method_object(call, result, 0u, &object);
    if (status != PIKA_STATUS_OK) return status;
    status = schedule_object_callback(object, object->period_ms);
    if (status != PIKA_STATUS_OK) return status;
    result_none(result);
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__lvgl_timer_delete(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaLvglObject* object;
    PikaStatus status = method_object(call, result, 0u, &object);
    if (status != PIKA_STATUS_OK) return status;
    clear_object(object);
    result_none(result);
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__lvgl_timer_handler(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    uint16_t index;
    int64_t count = 0;
    PikaStatus status = validate(call, result, 0u);
    if (status != PIKA_STATUS_OK) return status;
    for (index = 0u; index < PIKA_LVGL_OBJECT_LIMIT; ++index) {
        PikaLvglObject* object = &objects[index];
        if (object->active == 0u || object->kind != 4u ||
            object->callback.active == 0u) {
            continue;
        }
        status = schedule_object_callback(object, object->period_ms);
        if (status != PIKA_STATUS_OK) return status;
        count += 1;
    }
    result->kind = PIKA_BINDING_VALUE_INTEGER;
    result->as.integer = count;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__lvgl_display_register(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaStatus status = validate(call, result, 1u);
    if (status != PIKA_STATUS_OK) return status;
    if (call->arguments[0].kind != PIKA_BINDING_VALUE_BYTEARRAY ||
        call->arguments[0].as.mutable_buffer.data == NULL ||
        call->arguments[0].as.mutable_buffer.length == 0u) {
        return PIKA_STATUS_TYPE_ERROR;
    }
    framebuffer_length = call->arguments[0].as.mutable_buffer.length;
    call->arguments[0].as.mutable_buffer.data[0] += 1u;
    result_none(result);
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__lvgl_input_register(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaStatus status = validate(call, result, 1u);
    if (status != PIKA_STATUS_OK) return status;
    if (call->arguments[0].kind != PIKA_BINDING_VALUE_INTEGER) {
        return PIKA_STATUS_TYPE_ERROR;
    }
    input_state = call->arguments[0].as.integer;
    result_none(result);
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__lvgl_driver_step(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaStatus status = validate(call, result, 0u);
    if (status != PIKA_STATUS_OK) return status;
    result->kind = PIKA_BINDING_VALUE_INTEGER;
    result->as.integer = (int64_t)framebuffer_length + input_state;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__lvgl_label_set_text(const PikaBindingCall* call, PikaBindingValue* result) {
    PikaLvglObject* object;
    PikaStatus status = method_object(call, result, 1u, &object);
    if (status != PIKA_STATUS_OK) return status;
    if (call->arguments[0].kind != PIKA_BINDING_VALUE_STRING ||
        call->arguments[0].as.buffer.length >= PIKA_LVGL_TEXT_LIMIT) {
        return PIKA_STATUS_TYPE_ERROR;
    }
    memcpy(object->text, call->arguments[0].as.buffer.data,
           call->arguments[0].as.buffer.length);
    object->text[call->arguments[0].as.buffer.length] = '\0';
    result_none(result);
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__lvgl_label_get_text(const PikaBindingCall* call,
                                            PikaBindingValue* result) {
    PikaLvglObject* object;
    PikaStatus status = method_object(call, result, 0u, &object);
    if (status != PIKA_STATUS_OK) return status;
    result->kind = PIKA_BINDING_VALUE_STRING;
    result->as.buffer.data = (const uint8_t*)object->text;
    result->as.buffer.length = (uint32_t)strlen(object->text);
    return PIKA_STATUS_OK;
}

static void destroy_instance(void* instance) {
    clear_object(find(instance));
}

void pika_binding__lvgl_obj_destroy(void* module_context, void* instance) { (void)module_context; destroy_instance(instance); }
void pika_binding__lvgl_btn_destroy(void* module_context, void* instance) { (void)module_context; destroy_instance(instance); }
void pika_binding__lvgl_label_destroy(void* module_context, void* instance) { (void)module_context; destroy_instance(instance); }
void pika_binding__lvgl_timer_destroy(void* module_context, void* instance) { (void)module_context; destroy_instance(instance); }

#endif
