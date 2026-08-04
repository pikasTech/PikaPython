/* SPEC: PJ2026-050111 portable packages v0.8; fuzzypid binding. */
#include "_fuzzypid_binding.h"
#include "pika_capability_config.h"
#include "algorithm/fuzzyPID.h"

#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#if PIKA_CAPABILITY_LIBRARY_FUZZYPID_ENABLE

#define PIKA_FUZZYPID_RULE_GROUP_COUNT 3u
#define PIKA_FUZZYPID_RULE_ROW_COUNT \
    PIKA_FUZZYPID_QUANTITY_FIELD_COUNT
#define PIKA_FUZZYPID_RULE_VALUE_COUNT \
    (PIKA_FUZZYPID_RULE_GROUP_COUNT * PIKA_FUZZYPID_RULE_ROW_COUNT * \
     PIKA_FUZZYPID_RULE_ROW_COUNT)
#define PIKA_FUZZYPID_MF_PARAMETER_COUNT (4u * PIKA_FUZZYPID_RULE_ROW_COUNT)
#define PIKA_FUZZYPID_MAX_FUZZY_OUTPUTS 3u
#define PIKA_FUZZYPID_MAX_MF_TYPES (2u + PIKA_FUZZYPID_MAX_FUZZY_OUTPUTS)

#ifndef PIKA_FUZZYPID_CONFIG_OBJECT_LIMIT
#define PIKA_FUZZYPID_CONFIG_OBJECT_LIMIT 2u
#endif

#ifndef PIKA_FUZZYPID_CONTROLLER_OBJECT_LIMIT
#define PIKA_FUZZYPID_CONTROLLER_OBJECT_LIMIT 2u
#endif

#ifndef PIKA_FUZZYPID_DOF_LIMIT
#define PIKA_FUZZYPID_DOF_LIMIT 8u
#endif

#if PIKA_FUZZYPID_CONFIG_OBJECT_LIMIT == 0u
#error "PIKA_FUZZYPID_CONFIG_OBJECT_LIMIT must be positive"
#endif

#if PIKA_FUZZYPID_CONTROLLER_OBJECT_LIMIT == 0u
#error "PIKA_FUZZYPID_CONTROLLER_OBJECT_LIMIT must be positive"
#endif

#if PIKA_FUZZYPID_DOF_LIMIT == 0u || PIKA_FUZZYPID_DOF_LIMIT > 31u
#error "PIKA_FUZZYPID_DOF_LIMIT must be between 1 and 31"
#endif

typedef struct {
    int values[PIKA_FUZZYPID_RULE_VALUE_COUNT];
    uint32_t initialized;
    uint8_t closed;
    uint8_t active;
} PikaFuzzyPidRuleBase;

typedef struct {
    int values[PIKA_FUZZYPID_MF_PARAMETER_COUNT];
    uint32_t initialized;
    uint8_t closed;
    uint8_t active;
} PikaFuzzyPidMembership;

typedef struct {
    float values[PIKA_FUZZYPID_DOF_LIMIT]
                [PIKA_FUZZYPID_PARAMETER_COUNT];
    uint32_t initialized;
    uint8_t count;
    uint8_t closed;
    uint8_t active;
} PikaFuzzyPidParameters;

typedef struct {
    uint8_t values[PIKA_FUZZYPID_DOF_LIMIT];
    uint32_t initialized;
    uint8_t count;
    uint8_t closed;
    uint8_t active;
} PikaFuzzyPidDirection;

typedef struct {
    int rules[PIKA_FUZZYPID_RULE_VALUE_COUNT];
    int mf_parameters[PIKA_FUZZYPID_MF_PARAMETER_COUNT];
    PikaFuzzyPidState pids[PIKA_FUZZYPID_DOF_LIMIT];
    PikaFuzzyPidInference fuzzy[PIKA_FUZZYPID_DOF_LIMIT];
    unsigned int mf_types[PIKA_FUZZYPID_DOF_LIMIT]
                         [PIKA_FUZZYPID_MAX_MF_TYPES];
    float fuzzy_outputs[PIKA_FUZZYPID_DOF_LIMIT]
                       [PIKA_FUZZYPID_MAX_FUZZY_OUTPUTS];
    uint8_t directions[PIKA_FUZZYPID_DOF_LIMIT];
    uint8_t count;
    uint8_t closed;
    uint8_t active;
} PikaFuzzyPidController;

static PikaFuzzyPidRuleBase
    rule_objects[PIKA_FUZZYPID_CONFIG_OBJECT_LIMIT];
static PikaFuzzyPidMembership
    membership_objects[PIKA_FUZZYPID_CONFIG_OBJECT_LIMIT];
static PikaFuzzyPidParameters
    parameter_objects[PIKA_FUZZYPID_CONFIG_OBJECT_LIMIT];
static PikaFuzzyPidDirection
    direction_objects[PIKA_FUZZYPID_CONFIG_OBJECT_LIMIT];
static PikaFuzzyPidController
    controller_objects[PIKA_FUZZYPID_CONTROLLER_OBJECT_LIMIT];

static void* fuzzypid_allocate(
    void* storage,
    size_t stride,
    uint32_t count,
    size_t active_offset) {
    uint8_t* bytes = (uint8_t*)storage;
    uint32_t index;
    for (index = 0u; index < count; ++index) {
        uint8_t* slot = bytes + index * stride;
        if (slot[active_offset] == 0u) {
            memset(slot, 0, stride);
            slot[active_offset] = 1u;
            return slot;
        }
    }
    return NULL;
}

static void* fuzzypid_lookup(
    void* storage,
    size_t stride,
    uint32_t count,
    size_t active_offset,
    void* instance) {
    uint8_t* bytes = (uint8_t*)storage;
    uint32_t index;
    for (index = 0u; index < count; ++index) {
        uint8_t* slot = bytes + index * stride;
        if (instance == slot && slot[active_offset] != 0u) return slot;
    }
    return NULL;
}

static PikaFuzzyPidRuleBase* rule_allocate(void) {
    return (PikaFuzzyPidRuleBase*)fuzzypid_allocate(
        rule_objects, sizeof(rule_objects[0]),
        PIKA_FUZZYPID_CONFIG_OBJECT_LIMIT,
        offsetof(PikaFuzzyPidRuleBase, active));
}

static PikaFuzzyPidRuleBase* rule_from_instance(void* instance) {
    return (PikaFuzzyPidRuleBase*)fuzzypid_lookup(
        rule_objects, sizeof(rule_objects[0]),
        PIKA_FUZZYPID_CONFIG_OBJECT_LIMIT,
        offsetof(PikaFuzzyPidRuleBase, active), instance);
}

static PikaFuzzyPidMembership* membership_allocate(void) {
    return (PikaFuzzyPidMembership*)fuzzypid_allocate(
        membership_objects, sizeof(membership_objects[0]),
        PIKA_FUZZYPID_CONFIG_OBJECT_LIMIT,
        offsetof(PikaFuzzyPidMembership, active));
}

static PikaFuzzyPidMembership* membership_from_instance(void* instance) {
    return (PikaFuzzyPidMembership*)fuzzypid_lookup(
        membership_objects, sizeof(membership_objects[0]),
        PIKA_FUZZYPID_CONFIG_OBJECT_LIMIT,
        offsetof(PikaFuzzyPidMembership, active), instance);
}

static PikaFuzzyPidParameters* parameters_allocate(void) {
    return (PikaFuzzyPidParameters*)fuzzypid_allocate(
        parameter_objects, sizeof(parameter_objects[0]),
        PIKA_FUZZYPID_CONFIG_OBJECT_LIMIT,
        offsetof(PikaFuzzyPidParameters, active));
}

static PikaFuzzyPidParameters* parameters_from_instance(void* instance) {
    return (PikaFuzzyPidParameters*)fuzzypid_lookup(
        parameter_objects, sizeof(parameter_objects[0]),
        PIKA_FUZZYPID_CONFIG_OBJECT_LIMIT,
        offsetof(PikaFuzzyPidParameters, active), instance);
}

static PikaFuzzyPidDirection* direction_allocate(void) {
    return (PikaFuzzyPidDirection*)fuzzypid_allocate(
        direction_objects, sizeof(direction_objects[0]),
        PIKA_FUZZYPID_CONFIG_OBJECT_LIMIT,
        offsetof(PikaFuzzyPidDirection, active));
}

static PikaFuzzyPidDirection* direction_from_instance(void* instance) {
    return (PikaFuzzyPidDirection*)fuzzypid_lookup(
        direction_objects, sizeof(direction_objects[0]),
        PIKA_FUZZYPID_CONFIG_OBJECT_LIMIT,
        offsetof(PikaFuzzyPidDirection, active), instance);
}

static PikaFuzzyPidController* controller_allocate(void) {
    return (PikaFuzzyPidController*)fuzzypid_allocate(
        controller_objects, sizeof(controller_objects[0]),
        PIKA_FUZZYPID_CONTROLLER_OBJECT_LIMIT,
        offsetof(PikaFuzzyPidController, active));
}

static PikaFuzzyPidController* controller_from_instance(void* instance) {
    return (PikaFuzzyPidController*)fuzzypid_lookup(
        controller_objects, sizeof(controller_objects[0]),
        PIKA_FUZZYPID_CONTROLLER_OBJECT_LIMIT,
        offsetof(PikaFuzzyPidController, active), instance);
}

static uint32_t fuzzypid_mask(uint32_t count) {
    return (1u << count) - 1u;
}

static PikaStatus fuzzypid_none(PikaBindingValue* result) {
    if (result == NULL) return PIKA_STATUS_INVALID_ARGUMENT;
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_BINDING_VALUE_NONE;
    return PIKA_STATUS_OK;
}

static PikaStatus fuzzypid_integer(
    const PikaBindingValue* value,
    int64_t minimum,
    int64_t maximum,
    int64_t* output) {
    if (value == NULL || output == NULL ||
        value->kind != PIKA_BINDING_VALUE_INTEGER) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    if (value->as.integer < minimum || value->as.integer > maximum) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    *output = value->as.integer;
    return PIKA_STATUS_OK;
}

static PikaStatus fuzzypid_float(
    const PikaBindingValue* value,
    float* output) {
    double number;
    if (value == NULL || output == NULL) return PIKA_STATUS_INVALID_ARGUMENT;
    if (value->kind == PIKA_BINDING_VALUE_FLOAT) {
        number = value->as.floating;
    } else if (value->kind == PIKA_BINDING_VALUE_INTEGER) {
        number = (double)value->as.integer;
    } else {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    if (!isfinite(number) || number < -(double)FLT_MAX ||
        number > (double)FLT_MAX) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    *output = (float)number;
    return PIKA_STATUS_OK;
}

static PikaStatus fuzzypid_boolean(
    const PikaBindingValue* value,
    uint8_t* output) {
    if (value == NULL || output == NULL) return PIKA_STATUS_INVALID_ARGUMENT;
    if (value->kind == PIKA_BINDING_VALUE_BOOLEAN) {
        *output = value->as.boolean != 0u ? 1u : 0u;
        return PIKA_STATUS_OK;
    }
    if (value->kind == PIKA_BINDING_VALUE_INTEGER &&
        (value->as.integer == 0 || value->as.integer == 1)) {
        *output = value->as.integer != 0 ? 1u : 0u;
        return PIKA_STATUS_OK;
    }
    return value->kind == PIKA_BINDING_VALUE_INTEGER
               ? PIKA_STATUS_VALUE_ERROR
               : PIKA_STATUS_TYPE_MISMATCH;
}

static PikaStatus fuzzypid_sequence(
    const PikaBindingValue* value,
    const PikaBindingSequence** sequence) {
    if (value == NULL || sequence == NULL ||
        value->kind != PIKA_BINDING_VALUE_LIST) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    if (value->as.sequence.length > 0u &&
        value->as.sequence.values == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *sequence = &value->as.sequence;
    return PIKA_STATUS_OK;
}

static PikaStatus fuzzypid_native_object(
    const PikaBindingValue* value,
    const char* class_name,
    void** instance) {
    const PikaBindingObject* object;
    if (value == NULL || class_name == NULL || instance == NULL ||
        value->kind != PIKA_BINDING_VALUE_OPAQUE ||
        value->as.opaque == NULL) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    object = (const PikaBindingObject*)value->as.opaque;
    if (object->active == 0u || object->type == NULL ||
        object->type->name == NULL || object->instance == NULL) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (strcmp(object->type->name, class_name) != 0) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    *instance = object->instance;
    return PIKA_STATUS_OK;
}

static PikaStatus fuzzypid_publish_object(
    void* object,
    PikaBindingValue* result) {
    if (object == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
    if (result == NULL) return PIKA_STATUS_INVALID_ARGUMENT;
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_BINDING_VALUE_OPAQUE;
    result->as.opaque = object;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__fuzzypid_RuleBase_init(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    int64_t count;
    uint32_t index;
    PikaStatus status;
    if (call == NULL || result == NULL || call->arguments == NULL ||
        call->argument_count != 3u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0u; index < 3u; ++index) {
        status = fuzzypid_integer(
            &call->arguments[index], PIKA_FUZZYPID_RULE_ROW_COUNT,
            PIKA_FUZZYPID_RULE_ROW_COUNT, &count);
        if (status != PIKA_STATUS_OK) return status;
    }
    return fuzzypid_publish_object(rule_allocate(), result);
}

PikaStatus pika_binding__fuzzypid_RuleBase_set_row(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaFuzzyPidRuleBase* object;
    const PikaBindingSequence* sequence;
    int rows[2u * PIKA_FUZZYPID_RULE_ROW_COUNT];
    int64_t group;
    int64_t index;
    uint32_t offset;
    uint32_t item;
    uint32_t row_count;
    PikaStatus status;
    if (call == NULL || result == NULL || call->arguments == NULL ||
        call->argument_count != 3u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    object = rule_from_instance(call->instance);
    if (object == NULL || object->closed != 0u) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    status = fuzzypid_integer(&call->arguments[0], 0, 2, &group);
    if (status == PIKA_STATUS_OK) {
        status = fuzzypid_integer(
            &call->arguments[1], 0,
            PIKA_FUZZYPID_RULE_ROW_COUNT - 1u, &index);
    }
    if (status == PIKA_STATUS_OK) {
        status = fuzzypid_sequence(&call->arguments[2], &sequence);
    }
    if (status != PIKA_STATUS_OK) return status;
    if (sequence->length != PIKA_FUZZYPID_RULE_ROW_COUNT &&
        sequence->length != 2u * PIKA_FUZZYPID_RULE_ROW_COUNT) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    row_count = sequence->length / PIKA_FUZZYPID_RULE_ROW_COUNT;
    if ((uint32_t)index + row_count > PIKA_FUZZYPID_RULE_ROW_COUNT) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    for (item = 0u; item < sequence->length; ++item) {
        int64_t value;
        status = fuzzypid_integer(
            &sequence->values[item], -3, 3, &value);
        if (status != PIKA_STATUS_OK) return status;
        rows[item] = (int)value;
    }
    offset = ((uint32_t)group * PIKA_FUZZYPID_RULE_ROW_COUNT +
              (uint32_t)index) * PIKA_FUZZYPID_RULE_ROW_COUNT;
    memcpy(&object->values[offset], rows,
           sequence->length * sizeof(rows[0]));
    for (item = 0u; item < row_count; ++item) {
        object->initialized |= 1u <<
            ((uint32_t)group * PIKA_FUZZYPID_RULE_ROW_COUNT +
             (uint32_t)index + item);
    }
    return fuzzypid_none(result);
}

PikaStatus pika_binding__fuzzypid_RuleBase_deinit(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaFuzzyPidRuleBase* object;
    if (call == NULL || result == NULL || call->argument_count != 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    object = rule_from_instance(call->instance);
    if (object == NULL || object->closed != 0u) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    object->closed = 1u;
    return fuzzypid_none(result);
}

void pika_binding__fuzzypid_RuleBase_destroy(
    void* module_context,
    void* instance) {
    PikaFuzzyPidRuleBase* object = rule_from_instance(instance);
    (void)module_context;
    if (object != NULL) memset(object, 0, sizeof(*object));
}

PikaStatus pika_binding__fuzzypid_MembershipFunction_init(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    int64_t count;
    PikaStatus status;
    if (call == NULL || result == NULL || call->arguments == NULL ||
        call->argument_count != 1u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = fuzzypid_integer(
        &call->arguments[0], PIKA_FUZZYPID_MF_PARAMETER_COUNT,
        PIKA_FUZZYPID_MF_PARAMETER_COUNT, &count);
    if (status != PIKA_STATUS_OK) return status;
    return fuzzypid_publish_object(membership_allocate(), result);
}

PikaStatus pika_binding__fuzzypid_MembershipFunction_set_values(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaFuzzyPidMembership* object;
    const PikaBindingSequence* sequence;
    int values[PIKA_BINDING_CONTAINER_ELEMENT_LIMIT];
    int64_t offset;
    uint32_t item;
    PikaStatus status;
    if (call == NULL || result == NULL || call->arguments == NULL ||
        call->argument_count != 2u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    object = membership_from_instance(call->instance);
    if (object == NULL || object->closed != 0u) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    status = fuzzypid_integer(
        &call->arguments[0], 0, PIKA_FUZZYPID_MF_PARAMETER_COUNT - 1,
        &offset);
    if (status == PIKA_STATUS_OK) {
        status = fuzzypid_sequence(&call->arguments[1], &sequence);
    }
    if (status != PIKA_STATUS_OK) return status;
    if (sequence->length == 0u ||
        sequence->length > PIKA_BINDING_CONTAINER_ELEMENT_LIMIT ||
        (uint32_t)offset + sequence->length >
            PIKA_FUZZYPID_MF_PARAMETER_COUNT) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    for (item = 0u; item < sequence->length; ++item) {
        int64_t value;
        status = fuzzypid_integer(
            &sequence->values[item], INT32_MIN, INT32_MAX, &value);
        if (status != PIKA_STATUS_OK) return status;
        values[item] = (int)value;
    }
    memcpy(&object->values[offset], values,
           sequence->length * sizeof(values[0]));
    for (item = 0u; item < sequence->length; ++item) {
        object->initialized |= 1u << ((uint32_t)offset + item);
    }
    return fuzzypid_none(result);
}

PikaStatus pika_binding__fuzzypid_MembershipFunction_deinit(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaFuzzyPidMembership* object;
    if (call == NULL || result == NULL || call->argument_count != 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    object = membership_from_instance(call->instance);
    if (object == NULL || object->closed != 0u) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    object->closed = 1u;
    return fuzzypid_none(result);
}

void pika_binding__fuzzypid_MembershipFunction_destroy(
    void* module_context,
    void* instance) {
    PikaFuzzyPidMembership* object = membership_from_instance(instance);
    (void)module_context;
    if (object != NULL) memset(object, 0, sizeof(*object));
}

PikaStatus pika_binding__fuzzypid_FuzzyPIDParams_init(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaFuzzyPidParameters* object;
    int64_t count;
    PikaStatus status;
    if (call == NULL || result == NULL || call->arguments == NULL ||
        call->argument_count != 1u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = fuzzypid_integer(
        &call->arguments[0], 1, PIKA_FUZZYPID_DOF_LIMIT, &count);
    if (status != PIKA_STATUS_OK) return status;
    object = parameters_allocate();
    if (object == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
    object->count = (uint8_t)count;
    return fuzzypid_publish_object(object, result);
}

PikaStatus pika_binding__fuzzypid_FuzzyPIDParams_set_row(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaFuzzyPidParameters* object;
    const PikaBindingSequence* sequence;
    float rows[2u * PIKA_FUZZYPID_PARAMETER_COUNT];
    int64_t index;
    uint32_t item;
    uint32_t row_count;
    PikaStatus status;
    if (call == NULL || result == NULL || call->arguments == NULL ||
        call->argument_count != 2u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    object = parameters_from_instance(call->instance);
    if (object == NULL || object->closed != 0u) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    status = fuzzypid_integer(
        &call->arguments[0], 0, (int64_t)object->count - 1, &index);
    if (status == PIKA_STATUS_OK) {
        status = fuzzypid_sequence(&call->arguments[1], &sequence);
    }
    if (status != PIKA_STATUS_OK) return status;
    if (sequence->length != PIKA_FUZZYPID_PARAMETER_COUNT &&
        sequence->length != 2u * PIKA_FUZZYPID_PARAMETER_COUNT) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    row_count = sequence->length / PIKA_FUZZYPID_PARAMETER_COUNT;
    if ((uint32_t)index + row_count > object->count) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    for (item = 0u; item < sequence->length; ++item) {
        status = fuzzypid_float(&sequence->values[item], &rows[item]);
        if (status != PIKA_STATUS_OK) return status;
    }
    memcpy(object->values[index], rows,
           sequence->length * sizeof(rows[0]));
    for (item = 0u; item < row_count; ++item) {
        object->initialized |= 1u << ((uint32_t)index + item);
    }
    return fuzzypid_none(result);
}

PikaStatus pika_binding__fuzzypid_FuzzyPIDParams_deinit(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaFuzzyPidParameters* object;
    if (call == NULL || result == NULL || call->argument_count != 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    object = parameters_from_instance(call->instance);
    if (object == NULL || object->closed != 0u) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    object->closed = 1u;
    return fuzzypid_none(result);
}

void pika_binding__fuzzypid_FuzzyPIDParams_destroy(
    void* module_context,
    void* instance) {
    PikaFuzzyPidParameters* object = parameters_from_instance(instance);
    (void)module_context;
    if (object != NULL) memset(object, 0, sizeof(*object));
}

PikaStatus pika_binding__fuzzypid_PIDDirect_init(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaFuzzyPidDirection* object;
    const PikaBindingSequence* sequence;
    uint8_t values[PIKA_FUZZYPID_DOF_LIMIT];
    int64_t count;
    uint32_t index;
    PikaStatus status;
    if (call == NULL || result == NULL || call->arguments == NULL ||
        call->argument_count != 1u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (call->arguments[0].kind == PIKA_BINDING_VALUE_INTEGER) {
        status = fuzzypid_integer(
            &call->arguments[0], 1, PIKA_FUZZYPID_DOF_LIMIT, &count);
        if (status != PIKA_STATUS_OK) return status;
        object = direction_allocate();
        if (object == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
        object->count = (uint8_t)count;
        return fuzzypid_publish_object(object, result);
    }
    status = fuzzypid_sequence(&call->arguments[0], &sequence);
    if (status != PIKA_STATUS_OK) return status;
    if (sequence->length == 0u || sequence->length > PIKA_FUZZYPID_DOF_LIMIT) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    memset(values, 0, sizeof(values));
    for (index = 0u; index < sequence->length; ++index) {
        status = fuzzypid_boolean(&sequence->values[index], &values[index]);
        if (status != PIKA_STATUS_OK) return status;
    }
    object = direction_allocate();
    if (object == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
    memcpy(object->values, values, sizeof(values));
    object->count = (uint8_t)sequence->length;
    object->initialized = fuzzypid_mask(sequence->length);
    return fuzzypid_publish_object(object, result);
}

PikaStatus pika_binding__fuzzypid_PIDDirect_set_values(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaFuzzyPidDirection* object;
    const PikaBindingSequence* sequence;
    uint8_t values[PIKA_FUZZYPID_DOF_LIMIT];
    int64_t offset;
    uint32_t item;
    PikaStatus status;
    if (call == NULL || result == NULL || call->arguments == NULL ||
        call->argument_count != 2u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    object = direction_from_instance(call->instance);
    if (object == NULL || object->closed != 0u) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    status = fuzzypid_integer(
        &call->arguments[0], 0, (int64_t)object->count - 1, &offset);
    if (status == PIKA_STATUS_OK) {
        status = fuzzypid_sequence(&call->arguments[1], &sequence);
    }
    if (status != PIKA_STATUS_OK) return status;
    if (sequence->length == 0u ||
        (uint32_t)offset + sequence->length > object->count) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    for (item = 0u; item < sequence->length; ++item) {
        status = fuzzypid_boolean(&sequence->values[item], &values[item]);
        if (status != PIKA_STATUS_OK) return status;
    }
    memcpy(&object->values[offset], values, sequence->length);
    for (item = 0u; item < sequence->length; ++item) {
        object->initialized |= 1u << ((uint32_t)offset + item);
    }
    return fuzzypid_none(result);
}

PikaStatus pika_binding__fuzzypid_PIDDirect_deinit(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaFuzzyPidDirection* object;
    if (call == NULL || result == NULL || call->argument_count != 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    object = direction_from_instance(call->instance);
    if (object == NULL || object->closed != 0u) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    object->closed = 1u;
    return fuzzypid_none(result);
}

void pika_binding__fuzzypid_PIDDirect_destroy(
    void* module_context,
    void* instance) {
    PikaFuzzyPidDirection* object = direction_from_instance(instance);
    (void)module_context;
    if (object != NULL) memset(object, 0, sizeof(*object));
}

static int fuzzypid_membership_valid(
    const PikaFuzzyPidMembership* membership,
    int type) {
    uint32_t index;
    for (index = 0u; index < PIKA_FUZZYPID_RULE_ROW_COUNT; ++index) {
        const int* values = &membership->values[index * 4u];
        if ((type == 0 || type == 1) && values[0] == 0) return 0;
        if (type == 3 &&
            !(values[0] <= values[1] && values[1] <= values[2] &&
              values[2] <= values[3] && values[0] < values[3])) {
            return 0;
        }
        if (type == 4 &&
            !(values[0] <= values[1] && values[1] <= values[2] &&
              values[0] < values[2])) {
            return 0;
        }
        if (type == 5 && values[0] >= values[1]) return 0;
    }
    return 1;
}

static void fuzzypid_initialize_controller(
    PikaFuzzyPidController* controller,
    const PikaFuzzyPidRuleBase* rules,
    const PikaFuzzyPidMembership* membership,
    const PikaFuzzyPidParameters* parameters,
    const PikaFuzzyPidDirection* directions,
    float delta_k,
    unsigned int mf_type,
    unsigned int fo_type,
    unsigned int df_type) {
    uint32_t index;
    memcpy(controller->rules, rules->values, sizeof(controller->rules));
    memcpy(controller->mf_parameters, membership->values,
           sizeof(controller->mf_parameters));
    controller->count = parameters->count;
    for (index = 0u; index < parameters->count; ++index) {
        const float* values = parameters->values[index];
        PikaFuzzyPidState* pid = &controller->pids[index];
        PikaFuzzyPidInference* fuzzy = &controller->fuzzy[index];
        uint32_t output_count = 1u;
        uint32_t type_index;
        controller->directions[index] =
            directions == NULL ? 1u : directions->values[index];
        if (values[1] > 1e-4f) {
            ++output_count;
            if (values[2] > 1e-4f) ++output_count;
        }
        pid->kp = values[0];
        pid->ki = values[1];
        pid->kd = values[2];
        pid->delta_kp_max = values[0] / delta_k;
        pid->delta_ki_max = values[1] / delta_k;
        pid->delta_kd_max = values[2] / delta_k;
        pid->error_max = PIKA_FUZZYPID_MAX_ERROR;
        pid->delta_error_max = PIKA_FUZZYPID_MAX_DELTA_ERROR;
        pid->integral_limit = values[3];
        pid->dead_zone = values[4];
        pid->feed_forward = values[5];
        pid->linear_adaptive_kp = values[6];
        pid->output_min_value = PIKA_FUZZYPID_MIN_OUTPUT;
        pid->output_middle_value = PIKA_FUZZYPID_MIDDLE_OUTPUT;
        pid->output_max_value = PIKA_FUZZYPID_MAX_OUTPUT;
        fuzzy->input_count = 2u;
        fuzzy->output_count = output_count;
        fuzzy->fuzzy_operator_type = fo_type;
        fuzzy->defuzzifier_type = df_type;
        fuzzy->membership_types = controller->mf_types[index];
        fuzzy->membership_parameters = controller->mf_parameters;
        fuzzy->rule_base = controller->rules;
        fuzzy->output = controller->fuzzy_outputs[index];
        for (type_index = 0u;
             type_index < 2u + output_count; ++type_index) {
            fuzzy->membership_types[type_index] = mf_type;
        }
        pid->inference = fuzzy;
    }
}

PikaStatus pika_binding__fuzzypid_FuzzyPIDController_init(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaFuzzyPidRuleBase* rules;
    PikaFuzzyPidMembership* membership;
    PikaFuzzyPidParameters* parameters;
    PikaFuzzyPidDirection* directions = NULL;
    PikaFuzzyPidController* controller;
    void* instance;
    float delta_k;
    int64_t mf_type;
    int64_t fo_type;
    int64_t df_type;
    PikaStatus status;
    if (call == NULL || result == NULL || call->arguments == NULL ||
        call->argument_count != 8u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = fuzzypid_native_object(
        &call->arguments[0], "RuleBase", &instance);
    rules = status == PIKA_STATUS_OK ? rule_from_instance(instance) : NULL;
    if (status == PIKA_STATUS_OK && rules == NULL) {
        status = PIKA_STATUS_INVALID_OPERAND;
    }
    if (status == PIKA_STATUS_OK) {
        status = fuzzypid_native_object(
            &call->arguments[1], "MembershipFunction", &instance);
        membership = status == PIKA_STATUS_OK
                         ? membership_from_instance(instance)
                         : NULL;
        if (status == PIKA_STATUS_OK && membership == NULL) {
            status = PIKA_STATUS_INVALID_OPERAND;
        }
    } else {
        membership = NULL;
    }
    if (status == PIKA_STATUS_OK) {
        status = fuzzypid_native_object(
            &call->arguments[2], "FuzzyPIDParams", &instance);
        parameters = status == PIKA_STATUS_OK
                         ? parameters_from_instance(instance)
                         : NULL;
        if (status == PIKA_STATUS_OK && parameters == NULL) {
            status = PIKA_STATUS_INVALID_OPERAND;
        }
    } else {
        parameters = NULL;
    }
    if (status == PIKA_STATUS_OK) {
        status = fuzzypid_float(&call->arguments[3], &delta_k);
        if (status == PIKA_STATUS_OK && delta_k <= 0.0f) {
            status = PIKA_STATUS_VALUE_ERROR;
        }
    }
    if (status == PIKA_STATUS_OK) {
        status = fuzzypid_integer(&call->arguments[4], 0, 5, &mf_type);
    }
    if (status == PIKA_STATUS_OK) {
        status = fuzzypid_integer(&call->arguments[5], 0, 6, &fo_type);
    }
    if (status == PIKA_STATUS_OK) {
        status = fuzzypid_integer(&call->arguments[6], 0, 0, &df_type);
    }
    if (status == PIKA_STATUS_OK &&
        call->arguments[7].kind != PIKA_BINDING_VALUE_NONE) {
        status = fuzzypid_native_object(
            &call->arguments[7], "PIDDirect", &instance);
        directions = status == PIKA_STATUS_OK
                         ? direction_from_instance(instance)
                         : NULL;
        if (status == PIKA_STATUS_OK && directions == NULL) {
            status = PIKA_STATUS_INVALID_OPERAND;
        }
    }
    if (status != PIKA_STATUS_OK) return status;
    if (rules->closed != 0u || membership->closed != 0u ||
        parameters->closed != 0u ||
        (directions != NULL && directions->closed != 0u) ||
        rules->initialized != fuzzypid_mask(
            PIKA_FUZZYPID_RULE_GROUP_COUNT *
            PIKA_FUZZYPID_RULE_ROW_COUNT) ||
        membership->initialized !=
            fuzzypid_mask(PIKA_FUZZYPID_MF_PARAMETER_COUNT) ||
        parameters->initialized != fuzzypid_mask(parameters->count) ||
        (directions != NULL &&
         (directions->count != parameters->count ||
          directions->initialized != fuzzypid_mask(directions->count))) ||
        !fuzzypid_membership_valid(membership, (int)mf_type)) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    controller = controller_allocate();
    if (controller == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
    fuzzypid_initialize_controller(
        controller, rules, membership, parameters, directions, delta_k,
        (unsigned int)mf_type, (unsigned int)fo_type,
        (unsigned int)df_type);
    return fuzzypid_publish_object(controller, result);
}

PikaStatus pika_binding__fuzzypid_FuzzyPIDController_compute_output(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaFuzzyPidController* controller;
    PikaFuzzyPidState* pid;
    PikaFuzzyPidState saved_pid;
    float saved_outputs[PIKA_FUZZYPID_MAX_FUZZY_OUTPUTS];
    float real;
    float input;
    float raw_output;
    double directed;
    int output;
    int64_t control_id;
    uint32_t index;
    PikaStatus status;
    if (call == NULL || call->arguments == NULL ||
        call->argument_count != 3u || result == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    controller = controller_from_instance(call->instance);
    if (controller == NULL || controller->closed != 0u) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    status = fuzzypid_integer(
        &call->arguments[0], 0, (int64_t)controller->count - 1,
        &control_id);
    if (status == PIKA_STATUS_OK) {
        status = fuzzypid_float(&call->arguments[1], &real);
    }
    if (status == PIKA_STATUS_OK) {
        status = fuzzypid_float(&call->arguments[2], &input);
    }
    if (status != PIKA_STATUS_OK) return status;
    pid = &controller->pids[control_id];
    saved_pid = *pid;
    memcpy(saved_outputs, pid->inference->output, sizeof(saved_outputs));
    raw_output = pika_fuzzypid_control(real, input, pid);
    if (!isfinite(raw_output)) {
        *pid = saved_pid;
        memcpy(pid->inference->output, saved_outputs,
               sizeof(saved_outputs));
        return PIKA_STATUS_VALUE_ERROR;
    }
    for (index = 0u; index < pid->inference->output_count; ++index) {
        if (!isfinite(pid->inference->output[index])) {
            *pid = saved_pid;
            memcpy(pid->inference->output, saved_outputs,
                   sizeof(saved_outputs));
            return PIKA_STATUS_VALUE_ERROR;
        }
    }
    directed = controller->directions[control_id] != 0u
                   ? (double)PIKA_FUZZYPID_MIDDLE_OUTPUT +
                         (double)raw_output
                   : (double)PIKA_FUZZYPID_MIDDLE_OUTPUT -
                         (double)raw_output;
    if (directed >= (double)PIKA_FUZZYPID_MAX_OUTPUT) {
        output = PIKA_FUZZYPID_MAX_OUTPUT;
    } else if (directed <= (double)PIKA_FUZZYPID_MIN_OUTPUT) {
        output = PIKA_FUZZYPID_MIN_OUTPUT;
    } else {
        output = (int)directed;
    }
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_BINDING_VALUE_FLOAT;
    result->as.floating = (double)output;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__fuzzypid_FuzzyPIDController_deinit(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaFuzzyPidController* object;
    if (call == NULL || result == NULL || call->argument_count != 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    object = controller_from_instance(call->instance);
    if (object == NULL || object->closed != 0u) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    object->closed = 1u;
    return fuzzypid_none(result);
}

void pika_binding__fuzzypid_FuzzyPIDController_destroy(
    void* module_context,
    void* instance) {
    PikaFuzzyPidController* object = controller_from_instance(instance);
    (void)module_context;
    if (object != NULL) memset(object, 0, sizeof(*object));
}

#endif
