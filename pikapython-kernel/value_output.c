/* SPEC: PJ2026-0501; typed value text output. */
#include "runtime_internal.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if PIKA_CAPABILITY_BUILTIN_PRINT_ENABLE

static PikaStatus register_output_bytes(
    PikaRuntimeContext* context,
    const char* bytes,
    uint32_t length) {
    if (context->output == NULL ||
        context->output->write == NULL ||
        context->output->write(
            context->output->context, bytes, length) !=
            PIKA_STATUS_OK) {
        return PIKA_STATUS_OUTPUT_ERROR;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus register_output_integer(
    PikaRuntimeContext* context,
    int64_t value) {
    char byte;
    uint64_t magnitude;
    uint64_t divisor = 1u;
    PikaStatus status;
    if (value < 0) {
        status = register_output_bytes(context, "-", 1u);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        magnitude = (uint64_t)(-(value + 1)) + 1u;
    } else {
        magnitude = (uint64_t)value;
    }
    while (magnitude / divisor >= 10u) {
        divisor *= 10u;
    }
    do {
        byte = (char)('0' + ((magnitude / divisor) % 10u));
        status = register_output_bytes(context, &byte, 1u);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        divisor /= 10u;
    } while (divisor != 0u);
    return register_output_bytes(context, "\n", 1u);
}

PikaStatus pika_runtime_write_register_value(
    PikaRuntimeContext* context,
    const PikaInstruction* instruction,
    int64_t value) {
    uint8_t stack_marker = 0u;
    if (context == NULL || instruction == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (context->metrics != NULL) {
        pika_runtime_record_stack(context, &stack_marker);
    }
    if (instruction->b == PIKA_PRINT_BOOLEAN) {
        return value != 0
                   ? register_output_bytes(context, "True\n", 5u)
                   : register_output_bytes(context, "False\n", 6u);
    }
    if (instruction->b == PIKA_PRINT_INTEGER) {
        return register_output_integer(context, value);
    }
    return PIKA_STATUS_INVALID_OPERAND;
}

#endif

#if PIKA_TYPED_RUNTIME_ENABLE

static PikaStatus output_bytes(PikaRuntimeContext* context,
                               const char* bytes,
                               uint32_t length) {
    if (context == NULL || context->output == NULL ||
        context->output->write == NULL ||
        context->output->write(context->output->context, bytes, length) !=
            PIKA_STATUS_OK) {
        return PIKA_STATUS_OUTPUT_ERROR;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus output_cstr(PikaRuntimeContext* context,
                              const char* bytes) {
    return output_bytes(context, bytes, (uint32_t)strlen(bytes));
}

static uint32_t format_integer_bytes(char* buffer, int64_t value) {
    uint64_t magnitude;
    uint64_t divisor = 1u;
    uint32_t length = 0u;
    if (value < 0) {
        buffer[length++] = '-';
        magnitude = (uint64_t)(-(value + 1)) + 1u;
    } else {
        magnitude = (uint64_t)value;
    }
    while (magnitude / divisor >= 10u) {
        divisor *= 10u;
    }
    do {
        buffer[length++] =
            (char)('0' + ((magnitude / divisor) % 10u));
        divisor /= 10u;
    } while (divisor != 0u);
    return length;
}

static PikaStatus output_integer(PikaRuntimeContext* context,
                                 int64_t value) {
    char buffer[21];
    uint32_t length = format_integer_bytes(buffer, value);
    return output_bytes(context, buffer, length);
}

static PikaStatus output_float(PikaRuntimeContext* context,
                               double value) {
    char buffer[64];
    int length = 0;
    int precision;
    for (precision = 15; precision <= 17; ++precision) {
        char* end = NULL;
        double parsed;
        length = snprintf(
            buffer, sizeof(buffer), "%.*g", precision, value);
        if (length < 0 || (size_t)length >= sizeof(buffer)) {
            return PIKA_STATUS_OUTPUT_ERROR;
        }
        if (!isfinite(value)) {
            break;
        }
        parsed = strtod(buffer, &end);
        if (end != NULL && *end == '\0' &&
            memcmp(&parsed, &value, sizeof(value)) == 0) {
            break;
        }
    }
    if (isfinite(value) && strchr(buffer, '.') == NULL &&
        strchr(buffer, 'e') == NULL && strchr(buffer, 'E') == NULL) {
        buffer[length++] = '.';
        buffer[length++] = '0';
        buffer[length] = '\0';
    }
    return output_bytes(context, buffer, (uint32_t)length);
}

static PikaStatus output_quoted(PikaRuntimeContext* context,
                                const char* bytes,
                                uint32_t length,
                                int byte_prefix) {
    PikaStatus status;
    uint32_t index;
    status = output_cstr(context, byte_prefix ? "b'" : "'");
    if (status != PIKA_STATUS_OK) return status;
    for (index = 0u; index < length; ++index) {
        unsigned char value = (unsigned char)bytes[index];
        char escaped[5];
        if (value == '\\' || value == '\'') {
            escaped[0] = '\\';
            escaped[1] = (char)value;
            status = output_bytes(context, escaped, 2u);
        } else if (value == '\n') {
            status = output_bytes(context, "\\n", 2u);
        } else if (value == '\r') {
            status = output_bytes(context, "\\r", 2u);
        } else if (value == '\t') {
            status = output_bytes(context, "\\t", 2u);
        } else if (value < 32u || value >= 127u) {
            (void)snprintf(escaped, sizeof(escaped), "\\x%02x", value);
            status = output_bytes(context, escaped, 4u);
        } else {
            status = output_bytes(context, (const char*)&bytes[index], 1u);
        }
        if (status != PIKA_STATUS_OK) return status;
    }
    return output_cstr(context, "'");
}

static PikaStatus output_value(PikaRuntimeContext* context,
                               PikaRuntimeValue value,
                               int representation,
                               int depth);

static PikaObjectSlot* output_object(PikaRuntimeContext* context,
                                     PikaRuntimeValue value) {
    PikaObjectSlot* slot = pika_runtime_object(context, value);
    if (slot == NULL ||
        (slot->data == NULL &&
         (slot->length != 0u ||
          value.kind == PIKA_RUNTIME_VALUE_RANGE ||
          value.kind == PIKA_RUNTIME_VALUE_SLICE))) {
        return NULL;
    }
    return slot;
}

static PikaStatus output_sequence(PikaRuntimeContext* context,
                                  PikaRuntimeValue value,
                                  int depth) {
    PikaObjectSlot* slot = output_object(context, value);
    const char* open;
    const char* close;
    uint32_t index;
    PikaStatus status;
    if (slot == NULL) return PIKA_STATUS_INVALID_OPERAND;
    open = value.kind == PIKA_RUNTIME_VALUE_LIST ? "[" : "(";
    close = value.kind == PIKA_RUNTIME_VALUE_LIST ? "]" : ")";
    status = output_cstr(context, open);
    if (status != PIKA_STATUS_OK) return status;
    for (index = 0u; index < slot->length; ++index) {
        if (index != 0u) {
            status = output_cstr(context, ", ");
            if (status != PIKA_STATUS_OK) return status;
        }
        status = output_value(
            context, ((PikaRuntimeValue*)slot->data)[index], 1, depth + 1);
        if (status != PIKA_STATUS_OK) return status;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_TUPLE && slot->length == 1u) {
        status = output_cstr(context, ",");
        if (status != PIKA_STATUS_OK) return status;
    }
    return output_cstr(context, close);
}

static PikaStatus output_set(PikaRuntimeContext* context,
                             PikaRuntimeValue value,
                             int depth) {
    PikaObjectSlot* slot = output_object(context, value);
    PikaStatus status;
    if (slot == NULL) return PIKA_STATUS_INVALID_OPERAND;
    if (slot->length == 0u) return output_cstr(context, "set()");
    status = output_cstr(context, "{");
    if (status != PIKA_STATUS_OK) return status;
    for (uint32_t index = 0u; index < slot->length; ++index) {
        if (index != 0u) {
            status = output_cstr(context, ", ");
            if (status != PIKA_STATUS_OK) return status;
        }
        status = output_value(
            context, ((PikaRuntimeValue*)slot->data)[index], 1, depth + 1);
        if (status != PIKA_STATUS_OK) return status;
    }
    return output_cstr(context, "}");
}

static PikaStatus output_dict(PikaRuntimeContext* context,
                              PikaRuntimeValue value,
                              int depth) {
    PikaObjectSlot* slot = output_object(context, value);
    PikaStatus status;
    if (slot == NULL) return PIKA_STATUS_INVALID_OPERAND;
    status = output_cstr(context, "{");
    if (status != PIKA_STATUS_OK) return status;
    for (uint32_t index = 0u; index < slot->length; ++index) {
        PikaMapEntry* entry = &((PikaMapEntry*)slot->data)[index];
        if (index != 0u) {
            status = output_cstr(context, ", ");
            if (status != PIKA_STATUS_OK) return status;
        }
        status = output_value(context, entry->key, 1, depth + 1);
        if (status == PIKA_STATUS_OK) status = output_cstr(context, ": ");
        if (status == PIKA_STATUS_OK) {
            status = output_value(context, entry->value, 1, depth + 1);
        }
        if (status != PIKA_STATUS_OK) return status;
    }
    return output_cstr(context, "}");
}

static PikaStatus output_value(PikaRuntimeContext* context,
                               PikaRuntimeValue value,
                               int representation,
                               int depth) {
    PikaObjectSlot* slot;
    if (depth > 32) return output_cstr(context, "...");
    switch (value.kind) {
        case PIKA_RUNTIME_VALUE_NONE:
            return output_cstr(context, "None");
        case PIKA_RUNTIME_VALUE_BOOLEAN:
            return output_cstr(context, value.as.integer != 0 ? "True" : "False");
        case PIKA_RUNTIME_VALUE_INTEGER:
            return output_integer(context, value.as.integer);
        case PIKA_RUNTIME_VALUE_FLOAT:
            return output_float(context, value.as.floating);
        case PIKA_RUNTIME_VALUE_STRING:
            slot = output_object(context, value);
            if (slot == NULL) return PIKA_STATUS_INVALID_OPERAND;
            return representation
                       ? output_quoted(context, (const char*)slot->data,
                                       slot->length, 0)
                       : output_bytes(context, (const char*)slot->data,
                                      slot->length);
        case PIKA_RUNTIME_VALUE_BYTES:
            slot = output_object(context, value);
            if (slot == NULL) return PIKA_STATUS_INVALID_OPERAND;
            return output_quoted(context, (const char*)slot->data,
                                 slot->length, 1);
        case PIKA_RUNTIME_VALUE_BYTEARRAY:
            slot = output_object(context, value);
            if (slot == NULL) return PIKA_STATUS_INVALID_OPERAND;
            if (output_cstr(context, "bytearray(") != PIKA_STATUS_OK) {
                return PIKA_STATUS_OUTPUT_ERROR;
            }
            if (output_quoted(context, (const char*)slot->data,
                              slot->length, 1) != PIKA_STATUS_OK) {
                return PIKA_STATUS_OUTPUT_ERROR;
            }
            return output_cstr(context, ")");
        case PIKA_RUNTIME_VALUE_LIST:
        case PIKA_RUNTIME_VALUE_TUPLE:
            return output_sequence(context, value, depth);
        case PIKA_RUNTIME_VALUE_SET:
            return output_set(context, value, depth);
        case PIKA_RUNTIME_VALUE_DICT:
            return output_dict(context, value, depth);
        case PIKA_RUNTIME_VALUE_RANGE: {
            const PikaRangeData* range;
            slot = output_object(context, value);
            if (slot == NULL) return PIKA_STATUS_INVALID_OPERAND;
            range = (const PikaRangeData*)slot->data;
            if (output_cstr(context, "range(") != PIKA_STATUS_OK) {
                return PIKA_STATUS_OUTPUT_ERROR;
            }
            if (output_integer(context, range->start) != PIKA_STATUS_OK ||
                output_cstr(context, ", ") != PIKA_STATUS_OK ||
                output_integer(context, range->stop) != PIKA_STATUS_OK) {
                return PIKA_STATUS_OUTPUT_ERROR;
            }
            if (range->step != 1 &&
                (output_cstr(context, ", ") != PIKA_STATUS_OK ||
                 output_integer(context, range->step) != PIKA_STATUS_OK)) {
                return PIKA_STATUS_OUTPUT_ERROR;
            }
            return output_cstr(context, ")");
        }
        case PIKA_RUNTIME_VALUE_SLICE: {
            const PikaSliceData* slice;
            slot = output_object(context, value);
            if (slot == NULL) return PIKA_STATUS_INVALID_OPERAND;
            slice = (const PikaSliceData*)slot->data;
            if (output_cstr(context, "slice(") != PIKA_STATUS_OK) {
                return PIKA_STATUS_OUTPUT_ERROR;
            }
            if (output_value(context, slice->start, 1, depth + 1) != PIKA_STATUS_OK ||
                output_cstr(context, ", ") != PIKA_STATUS_OK ||
                output_value(context, slice->stop, 1, depth + 1) != PIKA_STATUS_OK ||
                output_cstr(context, ", ") != PIKA_STATUS_OK ||
                output_value(context, slice->step, 1, depth + 1) != PIKA_STATUS_OK) {
                return PIKA_STATUS_OUTPUT_ERROR;
            }
            return output_cstr(context, ")");
        }
        case PIKA_RUNTIME_VALUE_TYPE:
            slot = output_object(context, value);
            if (slot == NULL) return PIKA_STATUS_INVALID_OPERAND;
            if (output_cstr(context, "<class '") != PIKA_STATUS_OK ||
                output_bytes(context, (const char*)slot->data, slot->length) !=
                    PIKA_STATUS_OK) {
                return PIKA_STATUS_OUTPUT_ERROR;
            }
            return output_cstr(context, "'>");
        case PIKA_RUNTIME_VALUE_MODULE:
            slot = output_object(context, value);
            if (slot == NULL) return PIKA_STATUS_INVALID_OPERAND;
            if (output_cstr(context, "<module '") != PIKA_STATUS_OK ||
                output_bytes(context, (const char*)slot->data, slot->length) !=
                    PIKA_STATUS_OK) {
                return PIKA_STATUS_OUTPUT_ERROR;
            }
            return output_cstr(context, "'>");
#if PIKA_CAPABILITY_EXCEPTION_BASIC_ENABLE
        case PIKA_RUNTIME_VALUE_EXCEPTION:
            if (value.as.object_index ==
                PIKA_RUNTIME_EXCEPTION_NO_MESSAGE) {
                return PIKA_STATUS_OK;
            }
            slot = output_object(context, value);
            if (slot == NULL ||
                slot->kind != PIKA_RUNTIME_VALUE_STRING) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
            return output_bytes(
                context, (const char*)slot->data,
                slot->length);
#endif
        case PIKA_RUNTIME_VALUE_ITERATOR:
            return output_cstr(context, "<iterator>");
        default:
            return PIKA_STATUS_TYPE_MISMATCH;
    }
}

PikaStatus pika_runtime_write_value_text(PikaRuntimeContext* context,
                                         PikaRuntimeValue value,
                                         int representation,
                                         int depth) {
    return output_value(context, value, representation, depth);
}

#endif
