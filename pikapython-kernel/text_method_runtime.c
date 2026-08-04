/* SPEC: PJ2026-050109 network-ready v0.1; bounded text methods. */
#include "runtime_internal.h"
#include "pika_text_method.h"

#include <stddef.h>
#include <string.h>

#if PIKA_CAPABILITY_PROTOCOL_TEXT_NETWORK_ENABLE

static const char text_method_names[] =
    "encode\0decode\0find\0split\0startswith\0lower";

static const PikaTextMethodDescriptor text_methods[] = {
    {0u, 1u, 2u},
    {7u, 1u, 2u},
    {14u, 2u, 4u},
    {19u, 1u, 3u},
    {25u, 2u, 2u},
    {36u, 1u, 1u},
};

static uint16_t method_offset(uint16_t method_id) {
    return (uint16_t)(method_id - PIKA_TEXT_METHOD_ENCODE);
}

const PikaTextMethodDescriptor* pika_text_method_descriptor(
    uint16_t method_id) {
    uint16_t offset;
    if (method_id < PIKA_TEXT_METHOD_ENCODE ||
        method_id > PIKA_TEXT_METHOD_LOWER) {
        return NULL;
    }
    offset = method_offset(method_id);
    return &text_methods[offset];
}

PikaStatus pika_text_method_find(
    const char* name,
    uint16_t* method_id) {
    uint16_t index;
    if (name == NULL || method_id == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0u;
         index < (uint16_t)(sizeof(text_methods) / sizeof(text_methods[0]));
         ++index) {
        if (strcmp(
                name,
                &text_method_names[
                    text_methods[index].name_offset]) == 0) {
            *method_id = (uint16_t)(PIKA_TEXT_METHOD_ENCODE + index);
            return PIKA_STATUS_OK;
        }
    }
    return PIKA_STATUS_MEMBER_NOT_FOUND;
}

static PikaStatus text_view(
    PikaRuntimeContext* context,
    PikaRuntimeValue value,
    const unsigned char** bytes,
    uint32_t* length) {
    PikaObjectSlot* slot;
    if (value.kind != PIKA_RUNTIME_VALUE_STRING &&
        value.kind != PIKA_RUNTIME_VALUE_BYTES) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    slot = pika_runtime_object(context, value);
    if (slot == NULL || (slot->length != 0u && slot->data == NULL)) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    *bytes = (const unsigned char*)slot->data;
    *length = slot->length;
    return PIKA_STATUS_OK;
}

static int text_equal(
    const unsigned char* left,
    uint32_t left_length,
    const char* right) {
    size_t right_length = strlen(right);
    return right_length == left_length &&
           (left_length == 0u ||
            memcmp(left, right, left_length) == 0);
}

static PikaStatus encoding_kind(
    PikaRuntimeContext* context,
    const PikaRuntimeValue* arguments,
    uint8_t count,
    int* ascii_only) {
    const unsigned char* bytes;
    uint32_t length;
    PikaStatus status;
    *ascii_only = 0;
    if (count == 1u) return PIKA_STATUS_OK;
    status = text_view(context, arguments[1], &bytes, &length);
    if (status != PIKA_STATUS_OK ||
        arguments[1].kind != PIKA_RUNTIME_VALUE_STRING) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    if (text_equal(bytes, length, "utf-8") ||
        text_equal(bytes, length, "utf8")) {
        return PIKA_STATUS_OK;
    }
    if (text_equal(bytes, length, "ascii")) {
        *ascii_only = 1;
        return PIKA_STATUS_OK;
    }
    return PIKA_STATUS_UNSUPPORTED_SYNTAX;
}

static int valid_utf8(
    const unsigned char* bytes,
    uint32_t length) {
    uint32_t index = 0u;
    while (index < length) {
        unsigned char first = bytes[index++];
        uint32_t remaining;
        uint32_t codepoint;
        uint32_t minimum;
        if (first < 0x80u) continue;
        if (first >= 0xc2u && first <= 0xdfu) {
            remaining = 1u;
            codepoint = first & 0x1fu;
            minimum = 0x80u;
        } else if (first >= 0xe0u && first <= 0xefu) {
            remaining = 2u;
            codepoint = first & 0x0fu;
            minimum = 0x800u;
        } else if (first >= 0xf0u && first <= 0xf4u) {
            remaining = 3u;
            codepoint = first & 0x07u;
            minimum = 0x10000u;
        } else {
            return 0;
        }
        if (remaining > length - index) return 0;
        while (remaining-- > 0u) {
            unsigned char next = bytes[index++];
            if ((next & 0xc0u) != 0x80u) return 0;
            codepoint = (codepoint << 6u) | (next & 0x3fu);
        }
        if (codepoint < minimum || codepoint > 0x10ffffu ||
            (codepoint >= 0xd800u && codepoint <= 0xdfffu)) {
            return 0;
        }
    }
    return 1;
}

static PikaStatus text_convert(
    PikaRuntimeContext* context,
    const PikaRuntimeValue* arguments,
    uint8_t count,
    PikaRuntimeValueKind source_kind,
    PikaRuntimeValueKind result_kind,
    PikaRuntimeValue* result) {
    const unsigned char* bytes;
    uint32_t length;
    uint32_t index;
    int ascii_only;
    PikaStatus status;
    if (arguments[0].kind != source_kind) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    status = text_view(context, arguments[0], &bytes, &length);
    if (status != PIKA_STATUS_OK) return status;
    status = encoding_kind(context, arguments, count, &ascii_only);
    if (status != PIKA_STATUS_OK) return status;
    if (ascii_only != 0) {
        for (index = 0u; index < length; ++index) {
            if (bytes[index] > 0x7fu) {
                return PIKA_STATUS_INVALID_OPERAND;
            }
        }
    } else if (source_kind == PIKA_RUNTIME_VALUE_BYTES &&
               !valid_utf8(bytes, length)) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    return pika_runtime_create_text(
        context, result_kind, (const char*)bytes, length, result);
}

static int64_t normalized_index(int64_t value, uint32_t length) {
    int64_t size = (int64_t)length;
    if (value < 0) value += size;
    if (value < 0) return 0;
    if (value > size) return size;
    return value;
}

static PikaStatus integer_value(
    PikaRuntimeValue value,
    int64_t* result) {
    if (value.kind != PIKA_RUNTIME_VALUE_INTEGER &&
        value.kind != PIKA_RUNTIME_VALUE_BOOLEAN) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    *result = value.as.integer;
    return PIKA_STATUS_OK;
}

static int32_t find_bytes(
    const unsigned char* haystack,
    uint32_t haystack_length,
    const unsigned char* needle,
    uint32_t needle_length,
    uint32_t start) {
    uint32_t index;
    if (needle_length == 0u) return (int32_t)start;
    if (needle_length > haystack_length - start) return -1;
    for (index = start;
         index <= haystack_length - needle_length;
         ++index) {
        if (memcmp(&haystack[index], needle, needle_length) == 0) {
            return (int32_t)index;
        }
    }
    return -1;
}

static PikaStatus text_find(
    PikaRuntimeContext* context,
    const PikaRuntimeValue* arguments,
    uint8_t count,
    PikaRuntimeValue* result) {
    const unsigned char* text;
    const unsigned char* needle;
    uint32_t text_length;
    uint32_t needle_length;
    int64_t start = 0;
    int64_t end;
    int32_t found;
    int start_beyond_end = 0;
    PikaStatus status;
    if (arguments[0].kind != arguments[1].kind) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    status = text_view(context, arguments[0], &text, &text_length);
    if (status == PIKA_STATUS_OK) {
        status = text_view(context, arguments[1], &needle, &needle_length);
    }
    if (status != PIKA_STATUS_OK) return status;
    end = (int64_t)text_length;
    if (count > 2u) {
        status = integer_value(arguments[2], &start);
        if (status == PIKA_STATUS_OK) {
            start_beyond_end = start > (int64_t)text_length;
        }
    }
    if (status == PIKA_STATUS_OK && count > 3u) {
        status = integer_value(arguments[3], &end);
    }
    if (status != PIKA_STATUS_OK) return status;
    start = normalized_index(start, text_length);
    end = normalized_index(end, text_length);
    found = start_beyond_end || end < start ||
            needle_length > (uint32_t)(end - start)
                ? -1
                : find_bytes(
                      text, (uint32_t)end, needle, needle_length,
                      (uint32_t)start);
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_RUNTIME_VALUE_INTEGER;
    result->as.integer = found;
    return PIKA_STATUS_OK;
}

static int ascii_space(unsigned char value) {
    return value == ' ' || value == '\t' || value == '\n' ||
           value == '\r' || value == '\f' || value == '\v';
}

static PikaStatus add_piece(
    uint32_t* starts,
    uint32_t* lengths,
    uint32_t* count,
    uint32_t start,
    uint32_t length) {
    if (*count >= PIKA_TEXT_METHOD_PART_LIMIT) {
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    starts[*count] = start;
    lengths[*count] = length;
    ++*count;
    return PIKA_STATUS_OK;
}

static PikaStatus split_positions(
    const unsigned char* text,
    uint32_t text_length,
    const unsigned char* separator,
    uint32_t separator_length,
    int64_t maxsplit,
    int whitespace,
    uint32_t* starts,
    uint32_t* lengths,
    uint32_t* piece_count) {
    uint32_t cursor = 0u;
    int64_t splits = 0;
    PikaStatus status;
    *piece_count = 0u;
    if (whitespace != 0) {
        while (cursor < text_length) {
            uint32_t start;
            while (cursor < text_length && ascii_space(text[cursor])) ++cursor;
            if (cursor == text_length) break;
            start = cursor;
            if (maxsplit >= 0 && splits >= maxsplit) {
                return add_piece(
                    starts, lengths, piece_count, start,
                    text_length - start);
            }
            while (cursor < text_length && !ascii_space(text[cursor])) ++cursor;
            status = add_piece(
                starts, lengths, piece_count, start, cursor - start);
            if (status != PIKA_STATUS_OK) return status;
            ++splits;
        }
        return PIKA_STATUS_OK;
    }
    if (separator_length == 0u) return PIKA_STATUS_INVALID_OPERAND;
    while (cursor <= text_length) {
        int32_t found;
        if (maxsplit >= 0 && splits >= maxsplit) {
            return add_piece(
                starts, lengths, piece_count, cursor,
                text_length - cursor);
        }
        found = find_bytes(
            text, text_length, separator, separator_length, cursor);
        if (found < 0) {
            return add_piece(
                starts, lengths, piece_count, cursor,
                text_length - cursor);
        }
        status = add_piece(
            starts, lengths, piece_count, cursor,
            (uint32_t)found - cursor);
        if (status != PIKA_STATUS_OK) return status;
        cursor = (uint32_t)found + separator_length;
        ++splits;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus text_split(
    PikaRuntimeContext* context,
    const PikaRuntimeValue* arguments,
    uint8_t count,
    PikaRuntimeValue* result) {
    const unsigned char* text;
    const unsigned char* separator = NULL;
    uint32_t text_length;
    uint32_t separator_length = 0u;
    uint32_t starts[PIKA_TEXT_METHOD_PART_LIMIT];
    uint32_t lengths[PIKA_TEXT_METHOD_PART_LIMIT];
    PikaRuntimeValue pieces[PIKA_TEXT_METHOD_PART_LIMIT];
    uint32_t piece_count;
    uint32_t index;
    int64_t maxsplit = -1;
    int whitespace = count == 1u;
    PikaStatus status;
    status = text_view(context, arguments[0], &text, &text_length);
    if (status != PIKA_STATUS_OK) return status;
    if (count > 1u) {
        if (arguments[1].kind == PIKA_RUNTIME_VALUE_NONE) {
            whitespace = 1;
        } else {
            if (arguments[1].kind != arguments[0].kind) {
                return PIKA_STATUS_TYPE_MISMATCH;
            }
            status = text_view(
                context, arguments[1], &separator,
                &separator_length);
        }
    }
    if (status == PIKA_STATUS_OK && count > 2u) {
        status = integer_value(arguments[2], &maxsplit);
    }
    if (status != PIKA_STATUS_OK) return status;
    status = split_positions(
        text, text_length, separator, separator_length, maxsplit,
        whitespace, starts, lengths, &piece_count);
    if (status != PIKA_STATUS_OK) return status;
    for (index = 0u; index < piece_count; ++index) {
        status = text_view(
            context, arguments[0], &text, &text_length);
        if (status != PIKA_STATUS_OK) break;
        const char* piece = text == NULL
                                ? NULL
                                : (const char*)&text[starts[index]];
        status = pika_runtime_create_text(
            context, arguments[0].kind,
            piece, lengths[index], &pieces[index]);
        if (status != PIKA_STATUS_OK) break;
    }
    if (status == PIKA_STATUS_OK) {
        status = pika_runtime_create_sequence(
            context, PIKA_RUNTIME_VALUE_LIST,
            pieces, piece_count, result);
    }
    return status;
}

static PikaStatus text_startswith(
    PikaRuntimeContext* context,
    const PikaRuntimeValue* arguments,
    PikaRuntimeValue* result) {
    const unsigned char* text;
    const unsigned char* prefix;
    uint32_t text_length;
    uint32_t prefix_length;
    PikaStatus status;
    if (arguments[0].kind != arguments[1].kind) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    status = text_view(context, arguments[0], &text, &text_length);
    if (status == PIKA_STATUS_OK) {
        status = text_view(context, arguments[1], &prefix, &prefix_length);
    }
    if (status != PIKA_STATUS_OK) return status;
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_RUNTIME_VALUE_BOOLEAN;
    result->as.integer =
        prefix_length <= text_length &&
        (prefix_length == 0u ||
         memcmp(text, prefix, prefix_length) == 0);
    return PIKA_STATUS_OK;
}

static PikaStatus text_lower(
    PikaRuntimeContext* context,
    PikaRuntimeValue value,
    PikaRuntimeValue* result) {
    const unsigned char* source;
    unsigned char* destination;
    uint32_t length;
    uint32_t index;
    PikaStatus status = text_view(
        context, value, &source, &length);
    if (status != PIKA_STATUS_OK) return status;
    status = pika_runtime_create_text_buffer(
        context, value.kind, length, result, &destination);
    if (status != PIKA_STATUS_OK) return status;
    status = text_view(context, value, &source, &length);
    if (status != PIKA_STATUS_OK) return status;
    for (index = 0u; index < length; ++index) {
        destination[index] =
            source[index] >= 'A' && source[index] <= 'Z'
                ? (unsigned char)(source[index] + ('a' - 'A'))
                : source[index];
    }
    return PIKA_STATUS_OK;
}

static void rollback_objects(
    PikaRuntimeContext* context,
    uint16_t count,
    uint32_t arena_used) {
    PikaObjectStorage* storage = context->objects;
    if (storage == NULL || storage->slots == NULL) return;
    while (storage->count > count) {
        PikaObjectSlot* slot = &storage->slots[storage->count - 1u];
        if (slot->active != 0u && slot->owned != 0u &&
            slot->data != NULL && storage->release != NULL) {
            storage->release(storage->context, slot->data);
        }
        memset(slot, 0, sizeof(*slot));
        --storage->count;
    }
    storage->arena_used = arena_used;
}

PikaStatus pika_runtime_execute_text_method(
    PikaRuntimeContext* context,
    const PikaRuntimeFrame* frame,
    uint16_t method_id,
    uint8_t argument_base,
    uint8_t argument_count,
    PikaRuntimeValue* result) {
    PikaRuntimeValue arguments[4];
    uint16_t object_count =
        context->objects != NULL ? context->objects->count : 0u;
    uint32_t arena_used =
        context->objects != NULL ? context->objects->arena_used : 0u;
    uint32_t index;
    PikaStatus status;
    if (argument_count > 4u) return PIKA_STATUS_ARGUMENT_COUNT;
    for (index = 0u; index < argument_count; ++index) {
        arguments[index] = pika_runtime_read_value(
            context, frame->value_base + argument_base + index);
    }
#if PIKA_GC_ENABLE
    ++context->gc_retry_block_depth;
#endif
    switch (method_id) {
        case PIKA_TEXT_METHOD_ENCODE:
            status = text_convert(
                context, arguments, argument_count,
                PIKA_RUNTIME_VALUE_STRING, PIKA_RUNTIME_VALUE_BYTES,
                result);
            break;
        case PIKA_TEXT_METHOD_DECODE:
            status = text_convert(
                context, arguments, argument_count,
                PIKA_RUNTIME_VALUE_BYTES, PIKA_RUNTIME_VALUE_STRING,
                result);
            break;
        case PIKA_TEXT_METHOD_FIND:
            status = text_find(
                context, arguments, argument_count, result);
            break;
        case PIKA_TEXT_METHOD_SPLIT:
            status = text_split(
                context, arguments, argument_count, result);
            break;
        case PIKA_TEXT_METHOD_STARTSWITH:
            status = text_startswith(context, arguments, result);
            break;
        case PIKA_TEXT_METHOD_LOWER:
            status = text_lower(context, arguments[0], result);
            break;
        default:
            status = PIKA_STATUS_UNSUPPORTED_SYNTAX;
            break;
    }
#if PIKA_GC_ENABLE
    --context->gc_retry_block_depth;
#endif
    if (status != PIKA_STATUS_OK) {
        rollback_objects(context, object_count, arena_used);
    }
    return status;
}

#endif
