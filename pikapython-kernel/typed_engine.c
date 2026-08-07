/* SPEC: PJ2026-0501 V2 kernel v0.47; tagged value execution dispatch. */
#include "runtime_internal.h"

#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if PIKA_TYPED_RUNTIME_ENABLE

#if PIKA_CAPABILITY_VALUE_NONE_ENABLE || \
    PIKA_CAPABILITY_OP_SLICE_ENABLE || \
    PIKA_TYPED_MUTABLE_CONTAINER_RUNTIME_ENABLE
static PikaRuntimeValue make_none(void) {
    PikaRuntimeValue value;
    memset(&value, 0, sizeof(value));
    value.kind = PIKA_RUNTIME_VALUE_NONE;
    return value;
}
#endif

static PikaRuntimeValue make_integer(int64_t integer) {
    PikaRuntimeValue value;
    memset(&value, 0, sizeof(value));
    value.kind = PIKA_RUNTIME_VALUE_INTEGER;
    value.as.integer = integer;
    return value;
}

#if PIKA_CAPABILITY_VALUE_BOOL_ENABLE || \
    PIKA_TYPED_INTEGER_RUNTIME_ENABLE || \
    PIKA_CAPABILITY_OP_COMPARE_BASIC_ENABLE || \
    PIKA_CAPABILITY_OP_MEMBERSHIP_ENABLE || \
    PIKA_CAPABILITY_LOGIC_SHORT_CIRCUIT_ENABLE
static PikaRuntimeValue make_boolean(int boolean) {
    PikaRuntimeValue value = make_integer(boolean != 0 ? 1 : 0);
    value.kind = PIKA_RUNTIME_VALUE_BOOLEAN;
    return value;
}
#endif

#if PIKA_CAPABILITY_VALUE_FLOAT_ENABLE || \
    PIKA_TYPED_NUMERIC_RUNTIME_ENABLE
static PikaRuntimeValue make_float(double floating) {
    PikaRuntimeValue value;
    memset(&value, 0, sizeof(value));
    value.kind = PIKA_RUNTIME_VALUE_FLOAT;
    value.as.floating = floating;
    return value;
}
#endif

#if PIKA_TYPED_INTEGER_HELPER_ENABLE
static int is_integer_value(PikaRuntimeValue value) {
    return value.kind == PIKA_RUNTIME_VALUE_INTEGER ||
           value.kind == PIKA_RUNTIME_VALUE_BOOLEAN;
}
#endif

#if PIKA_TYPED_NUMERIC_RUNTIME_ENABLE || \
    PIKA_CAPABILITY_OP_COMPARE_BASIC_ENABLE
static int is_number_value(PikaRuntimeValue value) {
    return is_integer_value(value) || value.kind == PIKA_RUNTIME_VALUE_FLOAT;
}
#endif

#if PIKA_TYPED_TEXT_HELPER_ENABLE
static int is_text_sequence_kind(PikaRuntimeValueKind kind) {
    return kind == PIKA_RUNTIME_VALUE_STRING ||
           kind == PIKA_RUNTIME_VALUE_BYTES ||
           kind == PIKA_RUNTIME_VALUE_BYTEARRAY;
}

static int object_payload_is_valid(const PikaObjectSlot* slot) {
    return slot != NULL &&
           (slot->length == 0u || slot->data != NULL);
}
#endif

#if PIKA_TYPED_NUMERIC_RUNTIME_ENABLE
static PikaStatus concat_text_sequence(PikaRuntimeContext* context,
                                       PikaRuntimeValue left,
                                       PikaRuntimeValue right,
                                       PikaRuntimeValue* result) {
    PikaObjectSlot* left_slot;
    PikaObjectSlot* right_slot;
    unsigned char* output = NULL;
    uint32_t left_length;
    uint32_t right_length;
    uint32_t total;
    PikaStatus status;
    left_slot = pika_runtime_object(context, left);
    right_slot = pika_runtime_object(context, right);
    if (left_slot == NULL || right_slot == NULL) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    if (!object_payload_is_valid(left_slot) ||
        !object_payload_is_valid(right_slot)) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (left_slot->length > UINT32_MAX - right_slot->length) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    left_length = left_slot->length;
    right_length = right_slot->length;
    total = left_length + right_length;
    status = pika_runtime_create_text_buffer(
        context, left.kind, total, result, &output);
    if (status != PIKA_STATUS_OK) return status;
    left_slot = pika_runtime_object(context, left);
    right_slot = pika_runtime_object(context, right);
    if (!object_payload_is_valid(left_slot) ||
        !object_payload_is_valid(right_slot) ||
        left_slot->length != left_length ||
        right_slot->length != right_length) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (left_length > 0u) {
        memmove(output, left_slot->data, left_length);
    }
    if (right_length > 0u) {
        memmove(&output[left_length], right_slot->data,
                right_length);
    }
    return PIKA_STATUS_OK;
}

static PikaStatus repeat_text_sequence(PikaRuntimeContext* context,
                                       PikaRuntimeValue sequence,
                                       int64_t count,
                                       PikaRuntimeValue* result) {
    PikaObjectSlot* slot;
    unsigned char* output = NULL;
    uint32_t length;
    uint64_t total;
    uint32_t index;
    PikaStatus status;
    slot = pika_runtime_object(context, sequence);
    if (!object_payload_is_valid(slot)) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (count < 0) count = 0;
    if (slot->length > 0u &&
        (uint64_t)count > UINT32_MAX / slot->length) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    length = slot->length;
    total = (uint64_t)length * (uint64_t)count;
    status = pika_runtime_create_text_buffer(
        context, sequence.kind, (uint32_t)total, result, &output);
    if (status != PIKA_STATUS_OK) return status;
    if (total == 0u) return PIKA_STATUS_OK;
    slot = pika_runtime_object(context, sequence);
    if (!object_payload_is_valid(slot) ||
        slot->length != length) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    for (index = 0u; index < (uint32_t)count; ++index) {
        memmove(&output[index * length], slot->data, length);
    }
    return PIKA_STATUS_OK;
}
#endif

static PikaStatus read_slot(const PikaRuntimeContext* context,
                            const PikaRuntimeFrame* frame,
                            uint8_t slot,
                            PikaRuntimeValue* value) {
    if (context->typed_values == NULL || slot >= frame->slot_count) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    *value = context->typed_values[frame->value_base + slot];
    return PIKA_STATUS_OK;
}

static PikaStatus write_slot(PikaRuntimeContext* context,
                             const PikaRuntimeFrame* frame,
                             uint8_t slot,
                             PikaRuntimeValue value) {
    if (context->typed_values == NULL || slot >= frame->slot_count) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    context->typed_values[frame->value_base + slot] = value;
    return PIKA_STATUS_OK;
}

#if PIKA_TYPED_INTEGER_HELPER_ENABLE
static PikaStatus integer_operand(PikaRuntimeValue value,
                                  int64_t* integer) {
    if (!is_integer_value(value)) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    *integer = value.as.integer;
    return PIKA_STATUS_OK;
}
#endif

#if PIKA_TYPED_NUMERIC_RUNTIME_ENABLE || \
    PIKA_CAPABILITY_OP_COMPARE_BASIC_ENABLE
static PikaStatus number_operands(PikaRuntimeValue left,
                                  PikaRuntimeValue right,
                                  double* left_value,
                                  double* right_value,
                                  int* floating) {
    if (!is_number_value(left) || !is_number_value(right)) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    *floating = left.kind == PIKA_RUNTIME_VALUE_FLOAT ||
                right.kind == PIKA_RUNTIME_VALUE_FLOAT;
    *left_value = left.kind == PIKA_RUNTIME_VALUE_FLOAT
                      ? left.as.floating : (double)left.as.integer;
    *right_value = right.kind == PIKA_RUNTIME_VALUE_FLOAT
                       ? right.as.floating : (double)right.as.integer;
    return PIKA_STATUS_OK;
}
#endif

#if PIKA_TYPED_NUMERIC_RUNTIME_ENABLE
static PikaStatus concat_sequence(PikaRuntimeContext* context,
                                  PikaRuntimeValue left,
                                  PikaRuntimeValue right,
                                  PikaRuntimeValue* result) {
    PikaObjectSlot* left_slot = pika_runtime_object(context, left);
    PikaObjectSlot* right_slot = pika_runtime_object(context, right);
    PikaRuntimeValue* values;
    uint32_t total;
    PikaStatus status;
    if (left_slot == NULL || right_slot == NULL) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    if (!object_payload_is_valid(left_slot) ||
        !object_payload_is_valid(right_slot)) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    if (left_slot->length > UINT32_MAX - right_slot->length) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    total = left_slot->length + right_slot->length;
    values = (PikaRuntimeValue*)malloc(
        (size_t)total * sizeof(PikaRuntimeValue));
    if (values == NULL && total != 0u) return PIKA_STATUS_STORAGE_TOO_SMALL;
    if (left_slot->length != 0u) {
        memcpy(values, left_slot->data,
               left_slot->length * sizeof(PikaRuntimeValue));
    }
    if (right_slot->length != 0u) {
        memcpy(&values[left_slot->length], right_slot->data,
               right_slot->length * sizeof(PikaRuntimeValue));
    }
    status = pika_runtime_create_sequence(
        context, left.kind, values, total, result);
    free(values);
    return status;
}

static PikaStatus repeat_sequence(PikaRuntimeContext* context,
                                  PikaRuntimeValue sequence,
                                  int64_t count,
                                  PikaRuntimeValue* result) {
    PikaObjectSlot* slot = pika_runtime_object(context, sequence);
    uint64_t total;
    PikaRuntimeValue* values;
    PikaStatus status;
    if (!object_payload_is_valid(slot)) {
        return slot == NULL ? PIKA_STATUS_TYPE_MISMATCH
                            : PIKA_STATUS_INVALID_OPERAND;
    }
    if (count < 0) count = 0;
    total = (uint64_t)slot->length * (uint64_t)count;
    if (total > UINT32_MAX) return PIKA_STATUS_STORAGE_TOO_SMALL;
    if (total == 0u) {
        return pika_runtime_create_sequence(
            context, sequence.kind, NULL, 0u, result);
    }
    values = (PikaRuntimeValue*)malloc(
        (size_t)total * sizeof(PikaRuntimeValue));
    if (values == NULL && total != 0u) return PIKA_STATUS_STORAGE_TOO_SMALL;
    for (uint64_t index = 0u; index < total; ++index) {
        values[index] = ((PikaRuntimeValue*)slot->data)[index % slot->length];
    }
    status = pika_runtime_create_sequence(
        context, sequence.kind, values, (uint32_t)total, result);
    free(values);
    return status;
}

static PikaStatus add_values(PikaRuntimeContext* context,
                             PikaRuntimeValue left,
                             PikaRuntimeValue right,
                             PikaRuntimeValue* result) {
    double left_number;
    double right_number;
    int floating;
    PikaStatus status;
    if (is_text_sequence_kind(left.kind) && left.kind == right.kind) {
        return concat_text_sequence(context, left, right, result);
    }
    if ((left.kind == PIKA_RUNTIME_VALUE_LIST ||
         left.kind == PIKA_RUNTIME_VALUE_TUPLE) &&
        left.kind == right.kind) {
        return concat_sequence(context, left, right, result);
    }
    status = number_operands(
        left, right, &left_number, &right_number, &floating);
    if (status != PIKA_STATUS_OK) return status;
    if (floating) {
        *result = make_float(left_number + right_number);
    } else {
        if ((right.as.integer > 0 && left.as.integer > INT64_MAX - right.as.integer) ||
            (right.as.integer < 0 && left.as.integer < INT64_MIN - right.as.integer)) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        *result = make_integer(left.as.integer + right.as.integer);
    }
    return PIKA_STATUS_OK;
}

static PikaStatus subtract_values(PikaRuntimeValue left,
                                  PikaRuntimeValue right,
                                  PikaRuntimeValue* result) {
    double left_number;
    double right_number;
    int floating;
    PikaStatus status = number_operands(
        left, right, &left_number, &right_number, &floating);
    if (status != PIKA_STATUS_OK) return status;
    if (floating) {
        *result = make_float(left_number - right_number);
    } else {
        if ((right.as.integer > 0 && left.as.integer < INT64_MIN + right.as.integer) ||
            (right.as.integer < 0 && left.as.integer > INT64_MAX + right.as.integer)) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        *result = make_integer(left.as.integer - right.as.integer);
    }
    return PIKA_STATUS_OK;
}

static PikaStatus multiply_values(PikaRuntimeContext* context,
                                  PikaRuntimeValue left,
                                  PikaRuntimeValue right,
                                  PikaRuntimeValue* result) {
    int64_t count;
    if (is_text_sequence_kind(left.kind) && is_integer_value(right)) {
        return repeat_text_sequence(
            context, left, right.as.integer, result);
    }
    if (is_text_sequence_kind(right.kind) && is_integer_value(left)) {
        return repeat_text_sequence(
            context, right, left.as.integer, result);
    }
    if ((left.kind == PIKA_RUNTIME_VALUE_LIST ||
         left.kind == PIKA_RUNTIME_VALUE_TUPLE) && is_integer_value(right)) {
        return repeat_sequence(context, left, right.as.integer, result);
    }
    if ((right.kind == PIKA_RUNTIME_VALUE_LIST ||
         right.kind == PIKA_RUNTIME_VALUE_TUPLE) && is_integer_value(left)) {
        return repeat_sequence(context, right, left.as.integer, result);
    }
    double left_number;
    double right_number;
    int floating;
    PikaStatus status = number_operands(
        left, right, &left_number, &right_number, &floating);
    if (status != PIKA_STATUS_OK) return status;
    if (floating) {
        *result = make_float(left_number * right_number);
        return PIKA_STATUS_OK;
    }
    status = pika_runtime_integer_multiply_checked(
        left.as.integer, right.as.integer, &count);
    if (status != PIKA_STATUS_OK) return status;
    *result = make_integer(count);
    return PIKA_STATUS_OK;
}

static PikaStatus modulo_values(PikaRuntimeValue left,
                                PikaRuntimeValue right,
                                PikaRuntimeValue* result) {
    double left_number;
    double right_number;
    int floating;
    int64_t left_integer;
    int64_t right_integer;
    PikaStatus status = number_operands(
        left, right, &left_number, &right_number, &floating);
    if (status != PIKA_STATUS_OK) return status;
    if (right_number == 0.0) return PIKA_STATUS_DIVISION_BY_ZERO;
    if (floating) {
        double value = fmod(left_number, right_number);
        if (value != 0.0 &&
            ((value < 0.0) != (right_number < 0.0))) {
            value += right_number;
        }
        if (value == 0.0) {
            value = copysign(0.0, right_number);
        }
        *result = make_float(value);
        return PIKA_STATUS_OK;
    }
    left_integer = left.as.integer;
    right_integer = right.as.integer;
    if (left_integer == INT64_MIN && right_integer == -1) {
        *result = make_integer(0);
        return PIKA_STATUS_OK;
    }
    int64_t value = left_integer % right_integer;
    if (value != 0 && ((value < 0) != (right_integer < 0))) value += right_integer;
    *result = make_integer(value);
    return PIKA_STATUS_OK;
}

static PikaStatus divide_values(PikaRuntimeValue left,
                                PikaRuntimeValue right,
                                int floor_mode,
                                PikaRuntimeValue* result) {
    double left_number;
    double right_number;
    int floating;
    PikaStatus status = number_operands(
        left, right, &left_number, &right_number, &floating);
    if (status != PIKA_STATUS_OK) return status;
    if (right_number == 0.0) return PIKA_STATUS_DIVISION_BY_ZERO;
    if (floor_mode && !floating) {
        int64_t a = left.as.integer;
        int64_t b = right.as.integer;
        if (a == INT64_MIN && b == -1) return PIKA_STATUS_INVALID_OPERAND;
        int64_t value = a / b;
        if ((a < 0) != (b < 0) && a % b != 0) --value;
        *result = make_integer(value);
    } else if (floor_mode) {
        *result = make_float(floor(left_number / right_number));
    } else {
        *result = make_float(left_number / right_number);
    }
    return PIKA_STATUS_OK;
}

static PikaStatus power_values(PikaRuntimeValue left,
                               PikaRuntimeValue right,
                               PikaRuntimeValue* result) {
    int64_t base;
    int64_t exponent;
    if (integer_operand(left, &base) == PIKA_STATUS_OK &&
        integer_operand(right, &exponent) == PIKA_STATUS_OK && exponent >= 0) {
        int64_t value = 1;
        PikaStatus status = pika_runtime_integer_power(
            base, exponent, &value);
        if (status == PIKA_STATUS_OK) *result = make_integer(value);
        return status;
    }
    double left_number;
    double right_number;
    int floating;
    PikaStatus status = number_operands(
        left, right, &left_number, &right_number, &floating);
    if (status != PIKA_STATUS_OK) return status;
    *result = make_float(pow(left_number, right_number));
    return PIKA_STATUS_OK;
}
#endif

#if PIKA_CAPABILITY_OP_COMPARE_BASIC_ENABLE
static PikaStatus compare_values(PikaRuntimeContext* context,
                                 PikaRuntimeValue left,
                                 PikaRuntimeValue right,
                                 int* comparison) {
    double left_number;
    double right_number;
    if (is_number_value(left) && is_number_value(right)) {
        left_number = left.kind == PIKA_RUNTIME_VALUE_FLOAT
                          ? left.as.floating
                          : (double)left.as.integer;
        right_number = right.kind == PIKA_RUNTIME_VALUE_FLOAT
                           ? right.as.floating
                           : (double)right.as.integer;
        *comparison = left_number < right_number ? -1
                    : left_number > right_number ? 1 : 0;
        return PIKA_STATUS_OK;
    }
    if (is_text_sequence_kind(left.kind) && left.kind == right.kind) {
        PikaObjectSlot* l = pika_runtime_object(context, left);
        PikaObjectSlot* r = pika_runtime_object(context, right);
        uint32_t common;
        int compared;
        if (!object_payload_is_valid(l) ||
            !object_payload_is_valid(r)) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        common = l->length < r->length ? l->length : r->length;
        compared = memcmp(l->data, r->data, common);
        *comparison = compared < 0 ? -1 : compared > 0 ? 1
                           : l->length < r->length ? -1
                           : l->length > r->length ? 1 : 0;
        return PIKA_STATUS_OK;
    }
    return PIKA_STATUS_TYPE_MISMATCH;
}
#endif

#if PIKA_CAPABILITY_OP_MATRIX_MULTIPLY_ENABLE
static int is_matrix_sequence(PikaRuntimeValueKind kind) {
    return kind == PIKA_RUNTIME_VALUE_LIST ||
           kind == PIKA_RUNTIME_VALUE_TUPLE;
}

static PikaStatus matrix_multiply_values(PikaRuntimeContext* context,
                                         PikaRuntimeValue left,
                                         PikaRuntimeValue right,
                                         PikaRuntimeValue* result) {
    PikaObjectSlot* left_slot;
    PikaObjectSlot* right_slot;
    PikaRuntimeValue accumulated;
    if (!is_matrix_sequence(left.kind) ||
        !is_matrix_sequence(right.kind) || left.kind != right.kind) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    left_slot = pika_runtime_object(context, left);
    right_slot = pika_runtime_object(context, right);
    if (left_slot == NULL || right_slot == NULL ||
        left_slot->length != right_slot->length) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    if (!object_payload_is_valid(left_slot) ||
        !object_payload_is_valid(right_slot)) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    accumulated = make_integer(0);
    for (uint32_t index = 0u; index < left_slot->length; ++index) {
        PikaRuntimeValue product;
        PikaStatus status = multiply_values(
            context,
            ((PikaRuntimeValue*)left_slot->data)[index],
            ((PikaRuntimeValue*)right_slot->data)[index],
            &product);
        if (status == PIKA_STATUS_OK) {
            status = add_values(
                context, accumulated, product, &accumulated);
        }
        if (status != PIKA_STATUS_OK) return status;
    }
    *result = accumulated;
    return PIKA_STATUS_OK;
}
#endif

#if PIKA_CAPABILITY_OP_MEMBERSHIP_ENABLE
static PikaStatus contains_value(PikaRuntimeContext* context,
                                 PikaRuntimeValue needle,
                                 PikaRuntimeValue container,
                                 int* found) {
    PikaObjectSlot* slot;
    *found = 0;
    if (is_text_sequence_kind(container.kind)) {
        PikaObjectSlot* n;
        slot = pika_runtime_object(context, container);
        if (!object_payload_is_valid(slot)) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        if (container.kind != PIKA_RUNTIME_VALUE_STRING &&
            is_integer_value(needle)) {
            if (needle.as.integer < 0 || needle.as.integer > 255) {
                return PIKA_STATUS_OK;
            }
            for (uint32_t index = 0u; index < slot->length; ++index) {
                if (((const unsigned char*)slot->data)[index] ==
                    (unsigned char)needle.as.integer) {
                    *found = 1;
                    break;
                }
            }
            return PIKA_STATUS_OK;
        }
        if ((container.kind == PIKA_RUNTIME_VALUE_STRING &&
             needle.kind != PIKA_RUNTIME_VALUE_STRING) ||
            (container.kind != PIKA_RUNTIME_VALUE_STRING &&
             needle.kind != PIKA_RUNTIME_VALUE_BYTES &&
             needle.kind != PIKA_RUNTIME_VALUE_BYTEARRAY)) {
            return PIKA_STATUS_TYPE_MISMATCH;
        }
        n = pika_runtime_object(context, needle);
        if (!object_payload_is_valid(n)) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        if (n->length == 0u) {
            *found = 1;
        } else if (n->length <= slot->length) {
            for (uint32_t index = 0u; index + n->length <= slot->length; ++index) {
                if (memcmp(&((const char*)slot->data)[index], n->data,
                           n->length) == 0) {
                    *found = 1;
                    break;
                }
            }
        }
        return PIKA_STATUS_OK;
    }
    if (container.kind == PIKA_RUNTIME_VALUE_RANGE) {
        int64_t value;
        int64_t start;
        int64_t stop;
        int64_t step;
        uint64_t distance;
        uint64_t stride;
        if (integer_operand(needle, &value) != PIKA_STATUS_OK) {
            return PIKA_STATUS_TYPE_MISMATCH;
        }
        slot = pika_runtime_object(context, container);
        if (slot == NULL || slot->data == NULL) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        PikaRangeData* range = (PikaRangeData*)slot->data;
        start = range->start; stop = range->stop; step = range->step;
        if (step == 0) {
            return PIKA_STATUS_DIVISION_BY_ZERO;
        }
        if (step > 0) {
            if (value < start || value >= stop) {
                return PIKA_STATUS_OK;
            }
            distance = (uint64_t)value - (uint64_t)start;
            stride = (uint64_t)step;
        } else {
            if (value > start || value <= stop) {
                return PIKA_STATUS_OK;
            }
            distance = (uint64_t)start - (uint64_t)value;
            stride = (uint64_t)(-(step + 1)) + 1u;
        }
        *found = distance % stride == 0u;
        return PIKA_STATUS_OK;
    }
    if (container.kind == PIKA_RUNTIME_VALUE_LIST ||
        container.kind == PIKA_RUNTIME_VALUE_TUPLE ||
        container.kind == PIKA_RUNTIME_VALUE_SET) {
        slot = pika_runtime_object(context, container);
        if (!object_payload_is_valid(slot)) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        for (uint32_t index = 0u; index < slot->length; ++index) {
            PikaRuntimeValue item = ((PikaRuntimeValue*)slot->data)[index];
            PikaStatus status = pika_runtime_value_equal(
                context, needle, item, found);
            if (status != PIKA_STATUS_OK || *found) return status;
        }
        return PIKA_STATUS_OK;
    }
#if PIKA_CAPABILITY_CONTAINER_DICT_ENABLE
    if (container.kind == PIKA_RUNTIME_VALUE_DICT) {
        PikaRuntimeValue ignored;
        return pika_runtime_dict_lookup(
            context, container, needle, &ignored, found);
    }
#endif
    return PIKA_STATUS_TYPE_MISMATCH;
}
#endif

static PikaStatus load_constant(PikaRuntimeContext* context,
                                int32_t index,
                                PikaRuntimeValue* value) {
    const PikaConstant* constant;
    if (index < 0 || (uint32_t)index >= context->program->constant_count) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    constant = &context->program->constants[index];
    switch ((PikaConstantKind)constant->kind) {
#if PIKA_CAPABILITY_VALUE_INT_ENABLE
        case PIKA_CONSTANT_INTEGER: *value = make_integer(constant->as.integer); return PIKA_STATUS_OK;
#endif
#if PIKA_CAPABILITY_VALUE_FLOAT_ENABLE
        case PIKA_CONSTANT_FLOAT: *value = make_float(constant->as.floating); return PIKA_STATUS_OK;
#endif
#if PIKA_CAPABILITY_VALUE_BOOL_ENABLE
        case PIKA_CONSTANT_BOOLEAN: *value = make_boolean(constant->as.integer != 0); return PIKA_STATUS_OK;
#endif
#if PIKA_CAPABILITY_VALUE_NONE_ENABLE
        case PIKA_CONSTANT_NONE: *value = make_none(); return PIKA_STATUS_OK;
#endif
#if PIKA_CAPABILITY_VALUE_STRING_ENABLE
        case PIKA_CONSTANT_STRING:
            return pika_runtime_create_text(
                context, PIKA_RUNTIME_VALUE_STRING, constant->as.bytes,
                constant->length, value);
#endif
#if PIKA_CAPABILITY_VALUE_BYTES_ENABLE
        case PIKA_CONSTANT_BYTES:
            return pika_runtime_create_text(
                context, PIKA_RUNTIME_VALUE_BYTES, constant->as.bytes,
                constant->length, value);
#endif
#if PIKA_CAPABILITY_BUILTIN_TYPE_ENABLE
        case PIKA_CONSTANT_TYPE:
            return pika_runtime_create_text(
                context, PIKA_RUNTIME_VALUE_TYPE, constant->as.bytes,
                constant->length, value);
#endif
#if PIKA_CAPABILITY_MODULE_IMPORT_ENABLE
        case PIKA_CONSTANT_MODULE:
            return pika_runtime_create_text(
                context, PIKA_RUNTIME_VALUE_MODULE, constant->as.bytes,
                constant->length, value);
#endif
        default: return PIKA_STATUS_INVALID_OPERAND;
    }
}

#if PIKA_TYPED_INTEGER_RUNTIME_ENABLE && \
    (!PIKA_CAPABILITY_OP_NUMERIC_BASIC_ENABLE || \
     !PIKA_CAPABILITY_OP_COMPARE_BASIC_ENABLE)
static PikaStatus execute_integer_binary(PikaOpcode opcode,
                                         PikaRuntimeValue left,
                                         PikaRuntimeValue right,
                                         PikaRuntimeValue* result) {
    int64_t left_integer;
    int64_t right_integer;
    int64_t value;
    PikaStatus status = integer_operand(left, &left_integer);
    if (status == PIKA_STATUS_OK) {
        status = integer_operand(right, &right_integer);
    }
    if (status != PIKA_STATUS_OK) return status;
    switch (opcode) {
        case PIKA_OP_ADD:
            if ((right_integer > 0 &&
                 left_integer > INT64_MAX - right_integer) ||
                (right_integer < 0 &&
                 left_integer < INT64_MIN - right_integer)) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            *result = make_integer(left_integer + right_integer);
            return PIKA_STATUS_OK;
        case PIKA_OP_SUBTRACT:
            if ((right_integer > 0 &&
                 left_integer < INT64_MIN + right_integer) ||
                (right_integer < 0 &&
                 left_integer > INT64_MAX + right_integer)) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            *result = make_integer(left_integer - right_integer);
            return PIKA_STATUS_OK;
        case PIKA_OP_MULTIPLY:
            status = pika_runtime_integer_multiply_checked(
                left_integer, right_integer, &value);
            if (status == PIKA_STATUS_OK) {
                *result = make_integer(value);
            }
            return status;
        case PIKA_OP_MODULO:
            if (right_integer == 0) {
                return PIKA_STATUS_DIVISION_BY_ZERO;
            }
            if (left_integer == INT64_MIN && right_integer == -1) {
                *result = make_integer(0);
                return PIKA_STATUS_OK;
            }
            value = left_integer % right_integer;
            if (value != 0 &&
                ((value < 0) != (right_integer < 0))) {
                value += right_integer;
            }
            *result = make_integer(value);
            return PIKA_STATUS_OK;
        case PIKA_OP_LESS_THAN:
            *result = make_boolean(left_integer < right_integer);
            return PIKA_STATUS_OK;
        case PIKA_OP_EQUAL:
            *result = make_boolean(left_integer == right_integer);
            return PIKA_STATUS_OK;
        default:
            return PIKA_STATUS_UNKNOWN_OPCODE;
    }
}
#endif

#if PIKA_TYPED_NUMERIC_RUNTIME_ENABLE
static int binary_result_may_allocate(PikaOpcode opcode,
                                      PikaRuntimeValue left,
                                      PikaRuntimeValue right) {
    if (opcode == PIKA_OP_ADD) {
        return is_text_sequence_kind(left.kind) &&
               left.kind == right.kind;
    }
    if (opcode == PIKA_OP_MULTIPLY) {
        return (is_text_sequence_kind(left.kind) &&
                is_integer_value(right)) ||
               (is_integer_value(left) &&
                is_text_sequence_kind(right.kind));
    }
    return 0;
}
#endif

#if PIKA_TYPED_BINARY_RUNTIME_ENABLE
static PikaStatus execute_binary(PikaRuntimeContext* context,
                                 PikaOpcode opcode,
                                 PikaRuntimeValue left,
                                 PikaRuntimeValue right,
                                 PikaRuntimeValue* result) {
    (void)context;
#if PIKA_CAPABILITY_OP_COMPARE_BASIC_ENABLE
    int comparison;
#endif
#if PIKA_CAPABILITY_OP_COMPARE_BASIC_ENABLE || \
    PIKA_CAPABILITY_OP_MEMBERSHIP_ENABLE
    int found;
#endif
#if PIKA_CAPABILITY_OP_BITWISE_ENABLE
    int64_t left_integer;
    int64_t right_integer;
#endif
#if PIKA_CAPABILITY_OP_BITWISE_ENABLE || \
    PIKA_CAPABILITY_OP_COMPARE_BASIC_ENABLE || \
    PIKA_CAPABILITY_OP_MEMBERSHIP_ENABLE
    PikaStatus status;
#endif
    switch (opcode) {
#if PIKA_CAPABILITY_OP_NUMERIC_BASIC_ENABLE
        case PIKA_OP_ADD: return add_values(context, left, right, result);
        case PIKA_OP_SUBTRACT: return subtract_values(left, right, result);
        case PIKA_OP_MULTIPLY: return multiply_values(context, left, right, result);
        case PIKA_OP_MODULO: return modulo_values(left, right, result);
        case PIKA_OP_DIVIDE: return divide_values(left, right, 0, result);
        case PIKA_OP_FLOOR_DIVIDE: return divide_values(left, right, 1, result);
        case PIKA_OP_POWER: return power_values(left, right, result);
#elif PIKA_TYPED_INTEGER_RUNTIME_ENABLE
        case PIKA_OP_ADD:
        case PIKA_OP_SUBTRACT:
        case PIKA_OP_MULTIPLY:
        case PIKA_OP_MODULO:
            return execute_integer_binary(opcode, left, right, result);
#endif
#if PIKA_CAPABILITY_OP_BITWISE_ENABLE
        case PIKA_OP_BITWISE_AND:
        case PIKA_OP_BITWISE_XOR:
        case PIKA_OP_BITWISE_OR:
        case PIKA_OP_SHIFT_LEFT:
        case PIKA_OP_SHIFT_RIGHT:
            status = integer_operand(left, &left_integer);
            if (status == PIKA_STATUS_OK) status = integer_operand(right, &right_integer);
            if (status != PIKA_STATUS_OK) return status;
            if (opcode == PIKA_OP_BITWISE_AND) result->as.integer = left_integer & right_integer;
            else if (opcode == PIKA_OP_BITWISE_XOR) result->as.integer = left_integer ^ right_integer;
            else if (opcode == PIKA_OP_BITWISE_OR) result->as.integer = left_integer | right_integer;
            else if (right_integer < 0) return PIKA_STATUS_INVALID_OPERAND;
            else if (opcode == PIKA_OP_SHIFT_LEFT) {
                int64_t factor;
                if (right_integer >= 63) return PIKA_STATUS_INVALID_OPERAND;
                factor = (int64_t)1 << right_integer;
                if (left_integer > INT64_MAX / factor ||
                    left_integer < INT64_MIN / factor) {
                    return PIKA_STATUS_INVALID_OPERAND;
                }
                result->as.integer = left_integer * factor;
            } else if (right_integer >= 63) {
                result->as.integer = left_integer < 0 ? -1 : 0;
            } else {
                int64_t divisor = (int64_t)1 << right_integer;
                result->as.integer = left_integer / divisor;
                if (left_integer < 0 && left_integer % divisor != 0) {
                    --result->as.integer;
                }
            }
            result->kind = PIKA_RUNTIME_VALUE_INTEGER;
            result->reserved = 0u; result->flags = 0u;
            return PIKA_STATUS_OK;
#endif
#if PIKA_CAPABILITY_OP_COMPARE_BASIC_ENABLE
        case PIKA_OP_LESS_THAN:
        case PIKA_OP_LESS_EQUAL:
        case PIKA_OP_GREATER_THAN:
        case PIKA_OP_GREATER_EQUAL:
        case PIKA_OP_EQUAL:
        case PIKA_OP_NOT_EQUAL:
            if (opcode == PIKA_OP_EQUAL || opcode == PIKA_OP_NOT_EQUAL) {
                status = pika_runtime_value_equal(context, left, right, &found);
                if (status != PIKA_STATUS_OK) return status;
                *result = make_boolean(opcode == PIKA_OP_EQUAL ? found : !found);
                return PIKA_STATUS_OK;
            }
            status = compare_values(context, left, right, &comparison);
            if (status != PIKA_STATUS_OK) return status;
            if (opcode == PIKA_OP_LESS_THAN) found = comparison < 0;
            else if (opcode == PIKA_OP_LESS_EQUAL) found = comparison <= 0;
            else if (opcode == PIKA_OP_GREATER_THAN) found = comparison > 0;
            else found = comparison >= 0;
            *result = make_boolean(found);
            return PIKA_STATUS_OK;
#elif PIKA_TYPED_INTEGER_RUNTIME_ENABLE
        case PIKA_OP_LESS_THAN:
        case PIKA_OP_EQUAL:
            return execute_integer_binary(opcode, left, right, result);
#endif
#if PIKA_CAPABILITY_OP_MEMBERSHIP_ENABLE
        case PIKA_OP_CONTAINS:
            status = contains_value(context, left, right, &found);
            if (status != PIKA_STATUS_OK) return status;
            *result = make_boolean(found);
            return PIKA_STATUS_OK;
#endif
#if PIKA_CAPABILITY_OP_COMPARE_BASIC_ENABLE
        case PIKA_OP_IDENTITY:
            if (left.kind == PIKA_RUNTIME_VALUE_NONE ||
                left.kind == PIKA_RUNTIME_VALUE_BOOLEAN ||
                left.kind == PIKA_RUNTIME_VALUE_INTEGER) {
                found = left.kind == right.kind && left.as.integer == right.as.integer;
            } else {
                found = left.kind == right.kind &&
                        left.as.object_index == right.as.object_index;
            }
            *result = make_boolean(found);
            return PIKA_STATUS_OK;
#endif
#if PIKA_CAPABILITY_OP_MATRIX_MULTIPLY_ENABLE
        case PIKA_OP_MATRIX_MULTIPLY:
            return matrix_multiply_values(context, left, right, result);
#endif
        default:
            return PIKA_STATUS_UNKNOWN_OPCODE;
    }
}
#endif

#if PIKA_TYPED_UNARY_RUNTIME_ENABLE
static PikaStatus execute_unary(PikaRuntimeContext* context,
                                PikaOpcode opcode,
                                PikaRuntimeValue operand,
                                PikaRuntimeValue* result) {
#if PIKA_CAPABILITY_OP_NUMERIC_BASIC_ENABLE || \
    PIKA_TYPED_INTEGER_RUNTIME_ENABLE || \
    PIKA_CAPABILITY_OP_BITWISE_ENABLE
    int64_t integer;
    PikaStatus status;
#endif
#if !PIKA_CAPABILITY_LOGIC_SHORT_CIRCUIT_ENABLE
    (void)context;
#endif
    switch (opcode) {
#if PIKA_CAPABILITY_LOGIC_SHORT_CIRCUIT_ENABLE
        case PIKA_OP_LOGICAL_NOT:
            *result = make_boolean(!pika_runtime_value_truthy(context, operand));
            return PIKA_STATUS_OK;
#endif
#if PIKA_CAPABILITY_OP_NUMERIC_BASIC_ENABLE
        case PIKA_OP_UNARY_PLUS:
            if (!is_number_value(operand)) return PIKA_STATUS_TYPE_MISMATCH;
            *result = operand;
            return PIKA_STATUS_OK;
        case PIKA_OP_UNARY_MINUS:
            if (operand.kind == PIKA_RUNTIME_VALUE_FLOAT) {
                *result = make_float(-operand.as.floating);
                return PIKA_STATUS_OK;
            }
            status = integer_operand(operand, &integer);
            if (status != PIKA_STATUS_OK || integer == INT64_MIN) {
                return status != PIKA_STATUS_OK ? status : PIKA_STATUS_INVALID_OPERAND;
            }
            *result = make_integer(-integer);
            return PIKA_STATUS_OK;
#elif PIKA_TYPED_INTEGER_RUNTIME_ENABLE
        case PIKA_OP_UNARY_PLUS:
            status = integer_operand(operand, &integer);
            if (status != PIKA_STATUS_OK) return status;
            *result = make_integer(integer);
            return PIKA_STATUS_OK;
        case PIKA_OP_UNARY_MINUS:
            status = integer_operand(operand, &integer);
            if (status != PIKA_STATUS_OK || integer == INT64_MIN) {
                return status != PIKA_STATUS_OK
                           ? status
                           : PIKA_STATUS_INVALID_OPERAND;
            }
            *result = make_integer(-integer);
            return PIKA_STATUS_OK;
#endif
#if PIKA_CAPABILITY_OP_BITWISE_ENABLE
        case PIKA_OP_BITWISE_NOT:
            status = integer_operand(operand, &integer);
            if (status != PIKA_STATUS_OK) return status;
            *result = make_integer(~integer);
            return PIKA_STATUS_OK;
#endif
        default: return PIKA_STATUS_UNKNOWN_OPCODE;
    }
}
#endif

#if PIKA_CAPABILITY_OP_SUBSCRIPT_ENABLE
static PikaStatus execute_subscript(PikaRuntimeContext* context,
                                    const PikaRuntimeFrame* frame,
                                    const PikaIndexSite* site,
                                    PikaRuntimeValue* result) {
    PikaRuntimeValue container;
    PikaRuntimeValue index;
    int64_t integer;
    PikaStatus status = read_slot(context, frame, site->container_slot, &container);
    if (status != PIKA_STATUS_OK) return status;
    status = read_slot(context, frame, site->index_slot, &index);
    if (status != PIKA_STATUS_OK) return status;
#if PIKA_CAPABILITY_CONTAINER_DICT_ENABLE
    if (container.kind == PIKA_RUNTIME_VALUE_DICT) {
        int found = 0;
        status = pika_runtime_dict_lookup(
            context, container, index, result, &found);
        if (status != PIKA_STATUS_OK) return status;
        return found ? PIKA_STATUS_OK : PIKA_STATUS_INVALID_OPERAND;
    }
#endif
    status = integer_operand(index, &integer);
    if (status != PIKA_STATUS_OK) return status;
    return pika_runtime_sequence_item(context, container, integer, result);
}
#endif

#if PIKA_CAPABILITY_OP_SLICE_ENABLE
static PikaStatus execute_slice(PikaRuntimeContext* context,
                                const PikaRuntimeFrame* frame,
                                const PikaIndexSite* site,
                                PikaRuntimeValue* result) {
    PikaRuntimeValue container;
    PikaRuntimeValue start = make_none();
    PikaRuntimeValue stop = make_none();
    PikaRuntimeValue step = make_none();
#if PIKA_GC_ENABLE
    if (pika_runtime_object_collection_needed(context)) {
        pika_runtime_collect_objects(context);
    }
#endif
    PikaStatus status = read_slot(
        context, frame, site->container_slot, &container);
    if (status != PIKA_STATUS_OK) return status;
    if ((site->flags & 1u) != 0u) {
        status = read_slot(context, frame, site->start_slot, &start);
        if (status != PIKA_STATUS_OK) return status;
    }
    if ((site->flags & 2u) != 0u) {
        status = read_slot(context, frame, site->stop_slot, &stop);
        if (status != PIKA_STATUS_OK) return status;
    }
    if ((site->flags & 4u) != 0u) {
        status = read_slot(context, frame, site->step_slot, &step);
        if (status != PIKA_STATUS_OK) return status;
    }
    return pika_runtime_sequence_slice(
        context, container, start, stop, step, result);
}
#endif

PikaStatus pika_runtime_execute_typed(PikaRuntimeContext* context) {
    uint64_t instruction_count = context->metrics->instruction_count;
    for (;;) {
        PikaRuntimeFrame* frame = &context->frames[context->depth];
        const PikaFunction* function =
            &context->program->functions[frame->function_index];
        const PikaInstruction* instruction_storage;
        uint32_t instruction_storage_base;
        uint32_t end = function->first_instruction + function->instruction_count;
        uint32_t current_index;
        const PikaInstruction* instruction;
        PikaRuntimeValue left;
#if PIKA_TYPED_BINARY_RUNTIME_ENABLE ||                               \
    PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE ||                        \
    PIKA_CAPABILITY_OP_SUBSCRIPT_ENABLE ||                            \
    PIKA_TYPED_MUTABLE_CONTAINER_RUNTIME_ENABLE ||                    \
    PIKA_TYPED_ITERATION_RUNTIME_ENABLE
        PikaRuntimeValue right;
#endif
        PikaRuntimeValue result;
        PikaStatus status = PIKA_STATUS_OK;
        instruction_storage =
            pika_program_function_instructions(
                context->program, function,
                &instruction_storage_base);
        if (instruction_storage == NULL) {
            context->metrics->instruction_count =
                instruction_count;
            pika_runtime_set_error(
                context, PIKA_STATUS_INVALID_CONTROL_FLOW,
                frame->instruction_index);
            return PIKA_STATUS_INVALID_CONTROL_FLOW;
        }
        if (frame->instruction_index < function->first_instruction ||
            frame->instruction_index >= end) {
            context->metrics->instruction_count = instruction_count;
            pika_runtime_set_error(context, PIKA_STATUS_INVALID_CONTROL_FLOW,
                                   frame->instruction_index);
            return PIKA_STATUS_INVALID_CONTROL_FLOW;
        }
        current_index = frame->instruction_index;
        instruction = &instruction_storage[
            current_index - instruction_storage_base];
        ++instruction_count;
        switch ((PikaOpcode)instruction->opcode) {
            case PIKA_OP_CONSTANT:
                result = make_integer(instruction->immediate);
                status = write_slot(context, frame, instruction->a, result);
                if (status == PIKA_STATUS_OK) frame->instruction_index++;
                break;
            case PIKA_OP_LOAD_CONSTANT:
                status = load_constant(context, instruction->immediate, &result);
                if (status == PIKA_STATUS_OK) status = write_slot(context, frame, instruction->a, result);
                if (status == PIKA_STATUS_OK) frame->instruction_index++;
                break;
#if PIKA_CAPABILITY_CALL_REFERENCE_ENABLE
            case PIKA_OP_LOAD_CALLABLE:
                result = make_integer(
                    instruction->immediate);
                result.kind = PIKA_RUNTIME_VALUE_CALLABLE;
                status = write_slot(
                    context, frame, instruction->a, result);
                if (status == PIKA_STATUS_OK &&
                    context->binding_storage != NULL &&
                    context->binding_storage->values != NULL) {
                    memset(
                        &context->binding_storage->values[
                            frame->value_base +
                            instruction->a],
                        0, sizeof(PikaBindingValue));
                }
                if (status == PIKA_STATUS_OK) {
                    frame->instruction_index++;
                }
                break;
#endif
#if PIKA_CAPABILITY_NAME_GLOBAL_ENABLE
            case PIKA_OP_LOAD_GLOBAL: {
                const PikaRuntimeFrame* root =
                    &context->frames[0];
                uint32_t source =
                    root->value_base +
                    (uint32_t)instruction->immediate;
                status = write_slot(
                    context, frame, instruction->a,
                    context->typed_values[source]);
                if (status == PIKA_STATUS_OK &&
                    context->binding_storage != NULL &&
                    context->binding_storage->values != NULL) {
                    context->binding_storage->values[
                        frame->value_base +
                        instruction->a] =
                        context->binding_storage
                            ->values[source];
                }
                if (status == PIKA_STATUS_OK) {
                    frame->instruction_index++;
                }
                break;
            }
            case PIKA_OP_STORE_GLOBAL: {
                const PikaRuntimeFrame* root =
                    &context->frames[0];
                uint32_t destination =
                    root->value_base +
                    (uint32_t)instruction->immediate;
                status = read_slot(
                    context, frame, instruction->a,
                    &result);
                if (status == PIKA_STATUS_OK) {
                    context->typed_values[destination] =
                        result;
                }
                if (status == PIKA_STATUS_OK &&
                    context->binding_storage != NULL &&
                    context->binding_storage->values != NULL) {
                    context->binding_storage
                        ->values[destination] =
                        context->binding_storage->values[
                            frame->value_base +
                            instruction->a];
                }
                if (status == PIKA_STATUS_OK) {
                    frame->instruction_index++;
                }
                break;
            }
#endif
#if PIKA_CAPABILITY_ASSIGNMENT_UNPACK_ENABLE
            case PIKA_OP_UNPACK_SEQUENCE: {
                uint32_t length = 0u;
                int available = 0;
                if (instruction->a >= frame->slot_count ||
                    instruction->b >= frame->slot_count ||
                    instruction->c == 0u ||
                    instruction->immediate < 0 ||
                    (uint32_t)instruction->immediate >=
                        instruction->c) {
                    status = PIKA_STATUS_INVALID_OPERAND;
                    break;
                }
                status = read_slot(
                    context, frame, instruction->b, &left);
                if (status == PIKA_STATUS_OK) {
                    status = pika_runtime_iterable_length(
                        context, left, &length);
                }
                if (status == PIKA_STATUS_OK &&
                    length != instruction->c) {
                    status = PIKA_STATUS_VALUE_ERROR;
                }
                if (status == PIKA_STATUS_OK) {
                    status = pika_runtime_iteration_item(
                        context, left,
                        (uint32_t)instruction->immediate,
                        &result, &available);
                }
                if (status == PIKA_STATUS_OK && !available) {
                    status = PIKA_STATUS_VALUE_ERROR;
                }
                if (status == PIKA_STATUS_OK) {
                    status = write_slot(
                        context, frame, instruction->a, result);
                }
                if (status == PIKA_STATUS_OK) {
                    frame->instruction_index++;
                }
                break;
            }
#endif
            case PIKA_OP_MOVE:
                status = read_slot(context, frame, instruction->b, &result);
                if (status == PIKA_STATUS_OK) status = write_slot(context, frame, instruction->a, result);
                if (status == PIKA_STATUS_OK) frame->instruction_index++;
                break;
#if PIKA_CAPABILITY_BINDING_C_ENABLE
            case PIKA_OP_BIND_MOVE:
                status = read_slot(context, frame, instruction->b, &result);
                if (status == PIKA_STATUS_OK) {
                    status = write_slot(
                        context, frame, instruction->a, result);
                }
                if (status == PIKA_STATUS_OK &&
                    context->binding_storage != NULL &&
                    context->binding_storage->values != NULL) {
                    context->binding_storage
                        ->values[frame->value_base + instruction->a] =
                        context->binding_storage
                            ->values[frame->value_base + instruction->b];
                }
                if (status == PIKA_STATUS_OK) {
                    frame->instruction_index++;
                }
                break;
#endif
#if PIKA_CAPABILITY_OP_NUMERIC_BASIC_ENABLE || \
    PIKA_TYPED_INTEGER_RUNTIME_ENABLE
            case PIKA_OP_ADD:
            case PIKA_OP_SUBTRACT:
            case PIKA_OP_MULTIPLY:
            case PIKA_OP_MODULO:
#endif
#if PIKA_CAPABILITY_OP_NUMERIC_BASIC_ENABLE
            case PIKA_OP_DIVIDE:
            case PIKA_OP_FLOOR_DIVIDE:
            case PIKA_OP_POWER:
#endif
#if PIKA_CAPABILITY_OP_COMPARE_BASIC_ENABLE || \
    PIKA_TYPED_INTEGER_RUNTIME_ENABLE
            case PIKA_OP_LESS_THAN:
            case PIKA_OP_EQUAL:
#endif
#if PIKA_CAPABILITY_OP_COMPARE_BASIC_ENABLE
            case PIKA_OP_LESS_EQUAL:
            case PIKA_OP_GREATER_THAN:
            case PIKA_OP_GREATER_EQUAL:
            case PIKA_OP_NOT_EQUAL:
#endif
#if PIKA_CAPABILITY_OP_BITWISE_ENABLE
            case PIKA_OP_BITWISE_AND:
            case PIKA_OP_BITWISE_XOR:
            case PIKA_OP_BITWISE_OR:
            case PIKA_OP_SHIFT_LEFT:
            case PIKA_OP_SHIFT_RIGHT:
#endif
#if PIKA_CAPABILITY_OP_MATRIX_MULTIPLY_ENABLE
            case PIKA_OP_MATRIX_MULTIPLY:
#endif
#if PIKA_CAPABILITY_OP_MEMBERSHIP_ENABLE
            case PIKA_OP_CONTAINS:
#endif
#if PIKA_CAPABILITY_OP_COMPARE_BASIC_ENABLE
            case PIKA_OP_IDENTITY:
#endif
#if PIKA_TYPED_BINARY_RUNTIME_ENABLE
                status = read_slot(context, frame, instruction->b, &left);
                if (status == PIKA_STATUS_OK) status = read_slot(context, frame, instruction->c, &right);
#if PIKA_GC_ENABLE && PIKA_TYPED_NUMERIC_RUNTIME_ENABLE
                if (status == PIKA_STATUS_OK &&
                    binary_result_may_allocate(
                        (PikaOpcode)instruction->opcode, left, right) &&
                    pika_runtime_object_collection_needed(context)) {
                    pika_runtime_collect_objects(context);
                    status = read_slot(
                        context, frame, instruction->b, &left);
                    if (status == PIKA_STATUS_OK) {
                        status = read_slot(
                            context, frame, instruction->c, &right);
                    }
                }
#endif
                if (status == PIKA_STATUS_OK) status = execute_binary(
                    context, (PikaOpcode)instruction->opcode, left, right, &result);
                if (status == PIKA_STATUS_OK) status = write_slot(
                    context, frame, instruction->a, result);
                if (status == PIKA_STATUS_OK) frame->instruction_index++;
                break;
#endif
#if PIKA_CAPABILITY_LOGIC_SHORT_CIRCUIT_ENABLE
            case PIKA_OP_LOGICAL_NOT:
#endif
#if PIKA_CAPABILITY_OP_NUMERIC_BASIC_ENABLE || \
    PIKA_TYPED_INTEGER_RUNTIME_ENABLE
            case PIKA_OP_UNARY_PLUS:
            case PIKA_OP_UNARY_MINUS:
#endif
#if PIKA_CAPABILITY_OP_BITWISE_ENABLE
            case PIKA_OP_BITWISE_NOT:
#endif
#if PIKA_TYPED_UNARY_RUNTIME_ENABLE
                status = read_slot(context, frame, instruction->b, &left);
                if (status == PIKA_STATUS_OK) status = execute_unary(
                    context, (PikaOpcode)instruction->opcode, left, &result);
                if (status == PIKA_STATUS_OK) status = write_slot(
                    context, frame, instruction->a, result);
                if (status == PIKA_STATUS_OK) frame->instruction_index++;
                break;
#endif
#if PIKA_CAPABILITY_CONTAINER_LIST_ENABLE
            case PIKA_OP_BUILD_LIST:
#endif
#if PIKA_CAPABILITY_CONTAINER_TUPLE_ENABLE
            case PIKA_OP_BUILD_TUPLE:
#endif
#if PIKA_CAPABILITY_CONTAINER_LIST_ENABLE || \
    PIKA_CAPABILITY_CONTAINER_TUPLE_ENABLE
            {
                PikaRuntimeValue values[PIKA_RUNTIME_VALUE_LIMIT];
                if (instruction->c > PIKA_RUNTIME_VALUE_LIMIT) {
                    status = PIKA_STATUS_FRONTEND_LIMIT;
                    break;
                }
#if PIKA_GC_ENABLE
                if (pika_runtime_object_collection_needed(context)) {
                    pika_runtime_collect_objects(context);
                }
#endif
                for (uint32_t index = 0u; index < instruction->c; ++index) {
                    status = read_slot(context, frame,
                                       (uint8_t)(instruction->b + index),
                                       &values[index]);
                    if (status != PIKA_STATUS_OK) break;
                }
                if (status == PIKA_STATUS_OK) status = pika_runtime_create_sequence(
                    context, instruction->opcode == PIKA_OP_BUILD_LIST
                                 ? PIKA_RUNTIME_VALUE_LIST
                                 : PIKA_RUNTIME_VALUE_TUPLE,
                    values, instruction->c, &result);
                if (status == PIKA_STATUS_OK) status = write_slot(
                    context, frame, instruction->a, result);
                if (status == PIKA_STATUS_OK) frame->instruction_index++;
                break;
            }
#endif
#if PIKA_CAPABILITY_CONTAINER_DICT_ENABLE
            case PIKA_OP_BUILD_DICT: {
                uint32_t object_index = 0u;
                void* data = NULL;
                status = pika_runtime_create_object(
                    context, PIKA_RUNTIME_VALUE_DICT, 0u,
                    (uint32_t)sizeof(PikaMapEntry),
                    &object_index, &data);
                if (status == PIKA_STATUS_OK) {
                    memset(&result, 0, sizeof(result));
                    result.kind = PIKA_RUNTIME_VALUE_DICT;
                    result.as.object_index = object_index;
                }
                for (uint32_t index = 0u;
                     status == PIKA_STATUS_OK &&
                     index < instruction->c;
                     ++index) {
                    PikaRuntimeValue key;
                    PikaRuntimeValue value;
                    status = read_slot(
                        context, frame,
                        (uint8_t)(instruction->b + index * 2u),
                        &key);
                    if (status == PIKA_STATUS_OK) {
                        status = read_slot(
                            context, frame,
                            (uint8_t)(
                                instruction->b + index * 2u + 1u),
                            &value);
                    }
                    if (status == PIKA_STATUS_OK) {
                        status = pika_runtime_set_item(
                            context, result, key, value);
                    }
                }
                if (status == PIKA_STATUS_OK) {
                    status = write_slot(
                        context, frame, instruction->a, result);
                }
                if (status == PIKA_STATUS_OK) {
                    frame->instruction_index++;
                }
                break;
            }
#endif
#if PIKA_BUILTIN_RUNTIME_ENABLE
            case PIKA_OP_BUILTIN_CALL:
                context->metrics->instruction_count = instruction_count;
                status = pika_runtime_execute_builtin(
                    context, frame,
                    context->program->builtin_calls[instruction->immediate].builtin_id,
                    instruction->b, instruction->c, instruction->a);
                if (status == PIKA_STATUS_OK) frame->instruction_index++;
                break;
#endif
#if PIKA_CAPABILITY_OP_SUBSCRIPT_ENABLE
            case PIKA_OP_SUBSCRIPT:
                status = execute_subscript(
                    context, frame,
                    &context->program->index_sites[instruction->immediate],
                    &result);
                if (status == PIKA_STATUS_OK) status = write_slot(
                    context, frame, instruction->a, result);
                if (status == PIKA_STATUS_OK) frame->instruction_index++;
                break;
#endif
#if PIKA_CAPABILITY_OP_SLICE_ENABLE
            case PIKA_OP_SLICE:
                status = execute_slice(
                    context, frame,
                    &context->program->index_sites[instruction->immediate],
                    &result);
                if (status == PIKA_STATUS_OK) status = write_slot(
                    context, frame, instruction->a, result);
                if (status == PIKA_STATUS_OK) frame->instruction_index++;
                break;
#endif
#if PIKA_CAPABILITY_OBJECT_USER_BASIC_ENABLE || \
    PIKA_CAPABILITY_OBJECT_CLASS_ENABLE
            case PIKA_OP_NEW_INSTANCE:
                status = pika_runtime_create_instance(
                    context, (uint16_t)instruction->immediate, &result);
                if (status == PIKA_STATUS_OK) {
                    status = write_slot(
                        context, frame, instruction->a, result);
                }
                if (status == PIKA_STATUS_OK) frame->instruction_index++;
                break;
#endif
#if PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE
            case PIKA_OP_LOAD_ATTRIBUTE:
                status = read_slot(
                    context, frame, instruction->b, &left);
                if (status == PIKA_STATUS_OK) {
                    status = pika_runtime_load_attribute(
                        context, left,
                        (uint16_t)instruction->immediate, &result);
                }
                if (status == PIKA_STATUS_OK) {
                    status = write_slot(
                        context, frame, instruction->a, result);
                }
                if (status == PIKA_STATUS_OK) frame->instruction_index++;
                break;
            case PIKA_OP_STORE_ATTRIBUTE:
                status = read_slot(
                    context, frame, instruction->a, &left);
                if (status == PIKA_STATUS_OK) {
                    status = read_slot(
                        context, frame, instruction->b, &right);
                }
                if (status == PIKA_STATUS_OK) {
                    status = pika_runtime_store_attribute(
                        context, left,
                        (uint16_t)instruction->immediate, right);
                }
                if (status == PIKA_STATUS_OK) frame->instruction_index++;
                break;
#endif
#if PIKA_CAPABILITY_FLOW_BRANCH_ENABLE
            case PIKA_OP_SELECT:
                status = read_slot(context, frame, instruction->b, &left);
                if (status == PIKA_STATUS_OK) {
                    status = read_slot(context, frame,
                                       pika_runtime_value_truthy(context, left)
                                           ? instruction->c
                                           : (uint8_t)instruction->immediate,
                                       &result);
                }
                if (status == PIKA_STATUS_OK) status = write_slot(
                    context, frame, instruction->a, result);
                if (status == PIKA_STATUS_OK) frame->instruction_index++;
                break;
#endif
#if PIKA_CAPABILITY_OP_SUBSCRIPT_ENABLE
            case PIKA_OP_SET_SUBSCRIPT:
                status = read_slot(context, frame, instruction->a, &left);
                if (status == PIKA_STATUS_OK) status = read_slot(
                    context, frame, instruction->b, &right);
                if (status == PIKA_STATUS_OK) status = read_slot(
                    context, frame, instruction->c, &result);
                if (status == PIKA_STATUS_OK) status = pika_runtime_set_item(
                    context, left, right, result);
                if (status == PIKA_STATUS_OK) frame->instruction_index++;
                break;
#endif
#if PIKA_TYPED_MUTABLE_CONTAINER_RUNTIME_ENABLE
            case PIKA_OP_APPEND:
                status = read_slot(context, frame, instruction->b, &left);
                if (status == PIKA_STATUS_OK) status = read_slot(
                    context, frame, instruction->c, &right);
                if (status == PIKA_STATUS_OK) status = pika_runtime_append(
                    context, left, right, &result);
                if (status == PIKA_STATUS_OK) status = write_slot(
                    context, frame, instruction->a, result);
                if (status == PIKA_STATUS_OK) frame->instruction_index++;
                break;
            case PIKA_OP_POP: {
                PikaRuntimeValue index = make_none();
                PikaRuntimeValue default_value = make_none();
                int has_index = instruction->c != UINT8_MAX;
                int has_default = instruction->immediate >= 0;
                status = read_slot(context, frame, instruction->b, &left);
                if (status == PIKA_STATUS_OK && has_index) {
                    status = read_slot(context, frame, instruction->c, &index);
                }
                if (status == PIKA_STATUS_OK && has_default) {
                    status = read_slot(
                        context, frame, (uint8_t)instruction->immediate,
                        &default_value);
                }
                if (status == PIKA_STATUS_OK) status = pika_runtime_pop(
                    context, left, index, has_index, default_value,
                    has_default, &result);
                if (status == PIKA_STATUS_OK) status = write_slot(
                    context, frame, instruction->a, result);
                if (status == PIKA_STATUS_OK) frame->instruction_index++;
                break;
            }
#endif
#if PIKA_TYPED_ITERATION_RUNTIME_ENABLE
            case PIKA_OP_ITERATE: {
                int64_t position;
                int available = 0;
                status = read_slot(
                    context, frame, instruction->b, &left);
                if (status == PIKA_STATUS_OK) {
                    status = read_slot(
                        context, frame, instruction->c, &right);
                }
                if (status == PIKA_STATUS_OK) {
                    status = integer_operand(right, &position);
                }
                if (status == PIKA_STATUS_OK &&
                    (position < 0 ||
                     (uint64_t)position > UINT32_MAX)) {
                    status = PIKA_STATUS_INVALID_OPERAND;
                }
                if (status == PIKA_STATUS_OK) {
                    status = pika_runtime_iteration_item(
                        context, left, (uint32_t)position,
                        &result, &available);
                }
                if (status == PIKA_STATUS_OK && available) {
                    status = write_slot(
                        context, frame, instruction->a, result);
                }
                if (status == PIKA_STATUS_OK && available) {
                    status = write_slot(
                        context, frame, instruction->c,
                        make_integer(position + 1));
                }
                if (status == PIKA_STATUS_OK) {
                    frame->instruction_index =
                        available
                            ? frame->instruction_index + 1u
                            : (uint32_t)instruction->immediate;
                }
                break;
            }
#endif
#if PIKA_CAPABILITY_FLOW_BRANCH_ENABLE
            case PIKA_OP_JUMP:
#if PIKA_GC_ENABLE
                if ((uint32_t)instruction->immediate <=
                        current_index &&
                    pika_runtime_object_collection_needed(
                        context)) {
                    pika_runtime_collect_objects(context);
                }
#endif
                frame->instruction_index = (uint32_t)instruction->immediate;
                break;
            case PIKA_OP_BRANCH_FALSE:
#endif
#if PIKA_CAPABILITY_LOGIC_SHORT_CIRCUIT_ENABLE
            case PIKA_OP_BRANCH_TRUE:
#endif
#if PIKA_CAPABILITY_FLOW_BRANCH_ENABLE || \
    PIKA_CAPABILITY_LOGIC_SHORT_CIRCUIT_ENABLE
                status = read_slot(context, frame, instruction->a, &left);
                if (status == PIKA_STATUS_OK) {
                    int truth = pika_runtime_value_truthy(context, left);
                    int take = instruction->opcode == PIKA_OP_BRANCH_TRUE
                                   ? truth : !truth;
#if PIKA_GC_ENABLE
                    if (take &&
                        (uint32_t)instruction->immediate <=
                            current_index &&
                        pika_runtime_object_collection_needed(
                            context)) {
                        pika_runtime_collect_objects(context);
                    }
#endif
                    frame->instruction_index = take
                                                   ? (uint32_t)instruction->immediate
                                                   : frame->instruction_index + 1u;
                }
                break;
#endif
#if PIKA_CAPABILITY_CALL_POSITIONAL_ENABLE
                case PIKA_OP_CALL:
                frame->instruction_index++;
                status = pika_runtime_push_call(
                    context, instruction->a, instruction->b,
                    instruction->c, (uint16_t)instruction->immediate);
                    break;
                case PIKA_OP_CALL_CALLABLE:
                    frame->instruction_index++;
                    status = pika_runtime_push_callable_call(
                        context, instruction->a, instruction->b,
                        instruction->c,
                        (uint16_t)instruction->immediate);
                    break;
                case PIKA_OP_CALL_DYNAMIC_METHOD:
                    frame->instruction_index++;
                    status = pika_runtime_push_dynamic_method_call(
                        context, instruction->a,
                        instruction->b, instruction->c,
                        (uint16_t)instruction->immediate);
                    break;
#endif
            case PIKA_OP_RETURN: {
                int finished = 0;
                status = pika_runtime_return(context, instruction->a, &finished);
                if (status == PIKA_STATUS_OK && finished) {
                    context->metrics->instruction_count = instruction_count;
                    return PIKA_STATUS_OK;
                }
                break;
            }
#if PIKA_CAPABILITY_BUILTIN_PRINT_ENABLE
            case PIKA_OP_PRINT:
                context->metrics->instruction_count = instruction_count;
                status = read_slot(context, frame, instruction->a, &left);
                if (status == PIKA_STATUS_OK) status = pika_runtime_write_value_text(
                    context, left, instruction->b == PIKA_PRINT_VALUE ? 0 : 0, 0);
                if (status == PIKA_STATUS_OK) {
                    status = context->output != NULL && context->output->write != NULL &&
                             context->output->write(context->output->context, "\n", 1u) == PIKA_STATUS_OK
                                 ? PIKA_STATUS_OK : PIKA_STATUS_OUTPUT_ERROR;
                }
                if (status == PIKA_STATUS_OK) frame->instruction_index++;
                break;
#endif
#if PIKA_CAPABILITY_BINDING_C_ENABLE
            case PIKA_OP_BIND_CALL:
                context->metrics->instruction_count = instruction_count;
                status = pika_runtime_binding_call(context, instruction);
                if (status == PIKA_STATUS_OK) frame->instruction_index++;
                break;
#endif
            default:
#if PIKA_CAPABILITY_EXCEPTION_BASIC_ENABLE
                status = pika_runtime_execute_fallback_opcode(
                    context, instruction);
#else
                status = PIKA_STATUS_UNSUPPORTED_CAPABILITY;
#endif
                break;
        }
        if (status != PIKA_STATUS_OK) {
            context->metrics->instruction_count = instruction_count;
            pika_runtime_set_instruction_error(
                context, status, current_index, instruction);
            return status;
        }
    }
}

#endif
