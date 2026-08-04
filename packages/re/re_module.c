/* SPEC: PJ2026-050110 PikaPython CLI v0.7; C-backed re package. */
#include "_re_binding.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "pcre.h"

#ifndef PIKA_RE_PATTERN_BYTE_LIMIT
#define PIKA_RE_PATTERN_BYTE_LIMIT 256u
#endif

#ifndef PIKA_RE_SUBJECT_BYTE_LIMIT
#define PIKA_RE_SUBJECT_BYTE_LIMIT 4096u
#endif

#define PIKA_RE_ASCII_FLAG 0x02000000
#define PIKA_RE_PUBLIC_FLAGS \
    (PIKA_RE_ASCII_FLAG | PCRE_CASELESS | PCRE_MULTILINE | PCRE_DOTALL)
#define PIKA_RE_OVECTOR_SIZE 30

typedef enum {
    PIKA_RE_MODE_MATCH = 0,
    PIKA_RE_MODE_SEARCH = 1,
    PIKA_RE_MODE_FULLMATCH = 2
} PikaReMode;

static PikaStatus validate_call(const PikaBindingCall* call,
                                PikaBindingValue* result,
                                uint8_t argument_count) {
    if (call == NULL || result == NULL ||
        call->argument_count != argument_count ||
        (argument_count > 0u && call->arguments == NULL)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_OK;
}

static int buffer_contains_nul(PikaBindingBuffer value) {
    uint32_t index;
    if (value.length > 0u && value.data == NULL) {
        return 1;
    }
    for (index = 0u; index < value.length; ++index) {
        if (value.data[index] == 0u) {
            return 1;
        }
    }
    return 0;
}

static PikaStatus compile_options(int64_t flags, int* options) {
    int64_t unsupported;
    if (options == NULL || flags < 0 || flags > INT_MAX) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    unsupported = flags & ~(int64_t)PIKA_RE_PUBLIC_FLAGS;
    if (unsupported != 0) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    *options = (int)(flags & ~(int64_t)PIKA_RE_ASCII_FLAG);
    if ((flags & PIKA_RE_ASCII_FLAG) == 0) {
        *options |= PCRE_UTF8;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus execute_match(const PikaBindingCall* call,
                                PikaBindingValue* result,
                                PikaReMode mode) {
    static PikaBindingValue values[2];
    char pattern[PIKA_RE_PATTERN_BYTE_LIMIT + 1u];
    PikaBindingBuffer pattern_value;
    PikaBindingBuffer subject;
    const char* error_message = NULL;
    int error_offset = 0;
    int options = 0;
    int offsets[PIKA_RE_OVECTOR_SIZE];
    int execution;
    pcre* compiled;
    PikaStatus status = validate_call(call, result, 3u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    pattern_value = call->arguments[0].as.buffer;
    subject = call->arguments[1].as.buffer;
    if (pattern_value.length > PIKA_RE_PATTERN_BYTE_LIMIT ||
        subject.length > PIKA_RE_SUBJECT_BYTE_LIMIT ||
        subject.length > (uint32_t)INT_MAX ||
        buffer_contains_nul(pattern_value) ||
        (subject.length > 0u && subject.data == NULL)) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    status = compile_options(call->arguments[2].as.integer, &options);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (pattern_value.length > 0u) {
        memcpy(pattern, pattern_value.data, pattern_value.length);
    }
    pattern[pattern_value.length] = '\0';
    compiled = pcre_compile(
        pattern, options, &error_message, &error_offset, NULL);
    if (compiled == NULL) {
        (void)error_message;
        (void)error_offset;
        return PIKA_STATUS_VALUE_ERROR;
    }
    execution = pcre_exec(
        compiled,
        NULL,
        (const char*)subject.data,
        (int)subject.length,
        0,
        0,
        offsets,
        PIKA_RE_OVECTOR_SIZE);
    pcre_free(compiled);
    if (execution == PCRE_ERROR_NOMATCH ||
        (execution >= 0 && mode == PIKA_RE_MODE_MATCH && offsets[0] != 0) ||
        (execution >= 0 && mode == PIKA_RE_MODE_FULLMATCH &&
         (offsets[0] != 0 || offsets[1] != (int)subject.length))) {
        result->kind = PIKA_BINDING_VALUE_NONE;
        return PIKA_STATUS_OK;
    }
    if (execution < 0 || offsets[0] < 0 || offsets[1] < offsets[0] ||
        offsets[1] > (int)subject.length) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    memset(values, 0, sizeof(values));
    values[0].kind = PIKA_BINDING_VALUE_INTEGER;
    values[0].as.integer = offsets[0];
    values[1].kind = PIKA_BINDING_VALUE_INTEGER;
    values[1].as.integer = offsets[1];
    result->kind = PIKA_BINDING_VALUE_TUPLE;
    result->as.sequence.values = values;
    result->as.sequence.length = 2u;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__re_match(const PikaBindingCall* call,
                                  PikaBindingValue* result) {
    return execute_match(call, result, PIKA_RE_MODE_MATCH);
}

PikaStatus pika_binding__re_search(const PikaBindingCall* call,
                                   PikaBindingValue* result) {
    return execute_match(call, result, PIKA_RE_MODE_SEARCH);
}

PikaStatus pika_binding__re_fullmatch(const PikaBindingCall* call,
                                      PikaBindingValue* result) {
    return execute_match(call, result, PIKA_RE_MODE_FULLMATCH);
}

static int escape_required(uint8_t value) {
    static const char special[] = "()[]{}?*+-|^$\\.&~# \t\n\r\v\f";
    size_t index;
    for (index = 0u; index + 1u < sizeof(special); ++index) {
        if (value == (uint8_t)special[index]) {
            return 1;
        }
    }
    return 0;
}

PikaStatus pika_binding__re_escape(const PikaBindingCall* call,
                                   PikaBindingValue* result) {
    static uint8_t escaped[PIKA_BINDING_RESULT_BYTE_LIMIT];
    PikaBindingBuffer input;
    uint32_t source;
    uint32_t destination = 0u;
    PikaStatus status = validate_call(call, result, 1u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    input = call->arguments[0].as.buffer;
    if ((input.length > 0u && input.data == NULL) ||
        input.length > PIKA_RE_PATTERN_BYTE_LIMIT) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    for (source = 0u; source < input.length; ++source) {
        uint32_t required = escape_required(input.data[source]) ? 2u : 1u;
        if (destination + required > (uint32_t)sizeof(escaped)) {
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
        if (required == 2u) {
            escaped[destination++] = (uint8_t)'\\';
        }
        escaped[destination++] = input.data[source];
    }
    result->kind = PIKA_BINDING_VALUE_STRING;
    result->as.buffer.data = escaped;
    result->as.buffer.length = destination;
    return PIKA_STATUS_OK;
}
