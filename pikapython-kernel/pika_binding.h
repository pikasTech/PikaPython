/* SPEC: PJ2026-050109 network-ready v0.1; public C binding API. */
#ifndef PIKA_BINDING_H
#define PIKA_BINDING_H

#include <stdint.h>

#include "pika_status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PIKA_BINDING_SCHEMA_VERSION 4u
#define PIKA_BINDING_ARGUMENT_LIMIT 8u

#ifndef PIKA_BINDING_RUNTIME_OBJECT_LIMIT
#define PIKA_BINDING_RUNTIME_OBJECT_LIMIT 8u
#endif

#if PIKA_BINDING_RUNTIME_OBJECT_LIMIT < 1u || \
    PIKA_BINDING_RUNTIME_OBJECT_LIMIT > 65535u
#error "PIKA_BINDING_RUNTIME_OBJECT_LIMIT must be between 1 and 65535"
#endif
#define PIKA_BINDING_CALLABLE_DEINITIALIZER 0x0001u
#ifndef PIKA_BINDING_CONTAINER_ELEMENT_LIMIT
#define PIKA_BINDING_CONTAINER_ELEMENT_LIMIT 16u
#endif
#ifndef PIKA_BINDING_RESULT_BYTE_LIMIT
#define PIKA_BINDING_RESULT_BYTE_LIMIT 256u
#endif

typedef enum {
    PIKA_BINDING_VALUE_NONE = 0,
    PIKA_BINDING_VALUE_BOOLEAN = 1,
    PIKA_BINDING_VALUE_INTEGER = 2,
    PIKA_BINDING_VALUE_FLOAT = 3,
    PIKA_BINDING_VALUE_STRING = 4,
    PIKA_BINDING_VALUE_BYTES = 5,
    PIKA_BINDING_VALUE_ANY = 6,
    PIKA_BINDING_VALUE_OPAQUE = 7,
    PIKA_BINDING_VALUE_BYTEARRAY = 8,
    PIKA_BINDING_VALUE_LIST = 9,
    PIKA_BINDING_VALUE_TUPLE = 10,
    PIKA_BINDING_VALUE_DICT = 11,
    PIKA_BINDING_VALUE_CALLABLE = 12,
    PIKA_BINDING_VALUE_OBJECT = 13,
} PikaBindingValueKind;

typedef struct PikaBindingValue PikaBindingValue;
typedef struct PikaBindingMapEntry PikaBindingMapEntry;
typedef struct PikaBindingObject PikaBindingObject;

typedef struct {
    void* context;
    uint16_t function_index;
    uint8_t parameter_count;
    uint8_t active;
} PikaBindingPythonCallable;

typedef struct {
    const uint8_t* data;
    uint32_t length;
} PikaBindingBuffer;

typedef struct {
    uint8_t* data;
    uint32_t length;
} PikaBindingMutableBuffer;

typedef struct {
    const PikaBindingValue* values;
    uint32_t length;
} PikaBindingSequence;

typedef struct {
    const PikaBindingMapEntry* entries;
    uint32_t length;
} PikaBindingMapping;

struct PikaBindingValue {
    PikaBindingValueKind kind;
    union {
        uint8_t boolean;
        int64_t integer;
        double floating;
        PikaBindingBuffer buffer;
        PikaBindingMutableBuffer mutable_buffer;
        PikaBindingSequence sequence;
        PikaBindingMapping mapping;
        PikaBindingPythonCallable callable;
        void* opaque;
    } as;
};

struct PikaBindingMapEntry {
    PikaBindingValue key;
    PikaBindingValue value;
};

typedef struct {
    uint32_t record_size;
    const char* name;
    PikaBindingValue value;
} PikaBindingConstant;

typedef struct {
    const PikaBindingValue* arguments;
    uint8_t argument_count;
    void* instance;
    void* module_context;
    const PikaBindingObject* receiver;
} PikaBindingCall;

typedef PikaStatus (*PikaBindingCallback)(
    const PikaBindingCall* call,
    PikaBindingValue* result);
typedef void (*PikaBindingDestroy)(
    void* module_context,
    void* instance);

typedef enum {
    PIKA_BINDING_CALL_FUNCTION = 0,
    PIKA_BINDING_CALL_CONSTRUCTOR = 1,
    PIKA_BINDING_CALL_METHOD = 2,
} PikaBindingCallableKind;

typedef struct {
    uint32_t record_size;
    const char* name;
    PikaBindingCallableKind kind;
    const PikaBindingValueKind* parameters;
    uint8_t parameter_count;
    PikaBindingValueKind result_kind;
    PikaBindingCallback callback;
    const char* const* parameter_names;
    const PikaBindingValue* defaults;
    uint8_t required_parameter_count;
    uint16_t result_class_index;
    uint16_t flags;
} PikaBindingCallable;

typedef struct {
    uint32_t record_size;
    const char* name;
    const PikaBindingCallable* constructor;
    const PikaBindingCallable* methods;
    uint16_t method_count;
    PikaBindingDestroy destroy;
    const PikaBindingConstant* constants;
    uint16_t constant_count;
} PikaBindingClass;

typedef struct {
    uint32_t schema_version;
    uint32_t record_size;
    const char* name;
    const PikaBindingCallable* functions;
    uint16_t function_count;
    const PikaBindingClass* classes;
    uint16_t class_count;
    void* context;
    const PikaBindingConstant* constants;
    uint16_t constant_count;
} PikaBindingModule;

typedef struct {
    const PikaBindingModule** modules;
    uint16_t capacity;
    uint16_t count;
} PikaBindingRegistry;

struct PikaBindingObject {
    const PikaBindingClass* type;
    void* module_context;
    void* instance;
    uint8_t active;
};

PikaStatus pika_binding_registry_initialize(
    PikaBindingRegistry* registry,
    const PikaBindingModule** storage,
    uint16_t capacity);
PikaStatus pika_binding_registry_register(
    PikaBindingRegistry* registry,
    const PikaBindingModule* module);
PikaStatus pika_binding_find_module(
    const PikaBindingRegistry* registry,
    const char* name,
    const PikaBindingModule** module);
PikaStatus pika_binding_find_function(
    const PikaBindingModule* module,
    const char* name,
    const PikaBindingCallable** callable);
PikaStatus pika_binding_find_class(
    const PikaBindingModule* module,
    const char* name,
    const PikaBindingClass** class_descriptor);
PikaStatus pika_binding_find_method(
    const PikaBindingClass* class_descriptor,
    const char* name,
    const PikaBindingCallable** callable);
PikaStatus pika_binding_find_module_constant(
    const PikaBindingModule* module,
    const char* name,
    const PikaBindingConstant** constant);
PikaStatus pika_binding_find_class_constant(
    const PikaBindingClass* class_descriptor,
    const char* name,
    const PikaBindingConstant** constant);
PikaStatus pika_binding_invoke(
    const PikaBindingModule* module,
    const PikaBindingCallable* callable,
    void* instance,
    const PikaBindingValue* arguments,
    uint8_t argument_count,
    PikaBindingValue* result);
PikaStatus pika_binding_construct(
    const PikaBindingModule* module,
    const PikaBindingClass* class_descriptor,
    const PikaBindingValue* arguments,
    uint8_t argument_count,
    PikaBindingObject* object);
PikaStatus pika_binding_invoke_method(
    const PikaBindingModule* module,
    const PikaBindingObject* object,
    const PikaBindingCallable* method,
    const PikaBindingValue* arguments,
    uint8_t argument_count,
    PikaBindingValue* result);
/* Transfers a successful fresh object result into runtime-owned storage. */
PikaStatus pika_binding_object_adopt_result(
    const PikaBindingModule* module,
    const PikaBindingCallable* callable,
    PikaBindingValue* result,
    PikaBindingObject* object);
PikaStatus pika_binding_schedule(
    const PikaBindingPythonCallable* callable,
    const PikaBindingValue* arguments,
    uint8_t argument_count);
void pika_binding_callable_release(
    PikaBindingPythonCallable* callable);
void pika_binding_object_release(PikaBindingObject* object);

#ifdef __cplusplus
}
#endif

#endif
