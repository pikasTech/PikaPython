/* SPEC: PJ2026-0501 V2 kernel v0.20; source tokenization. */
#include "frontend_internal.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int is_name_start(char value) {
    return (value >= 'A' && value <= 'Z') ||
           (value >= 'a' && value <= 'z') || value == '_';
}

static int is_name_continue(char value) {
    return is_name_start(value) || (value >= '0' && value <= '9');
}

static int digit_value(char value) {
    if (value >= '0' && value <= '9') {
        return (int)(value - '0');
    }
    if (value >= 'a' && value <= 'f') {
        return (int)(value - 'a') + 10;
    }
    if (value >= 'A' && value <= 'F') {
        return (int)(value - 'A') + 10;
    }
    return -1;
}

int64_t pika_token_integer_value(const char* source,
                                 const PikaToken* token) {
    uint32_t index;
    uint32_t end;
    uint32_t base = 10u;
    int64_t value = 0;
    if (source == NULL || token == NULL ||
        token->kind != PIKA_TOKEN_INTEGER) {
        return 0;
    }
    index = token->offset;
    end = index + token->length;
    if (token->length > 2u && source[index] == '0') {
        char prefix = source[index + 1u];
        if (prefix == 'x' || prefix == 'X') {
            base = 16u;
            index += 2u;
        } else if (prefix == 'b' || prefix == 'B') {
            base = 2u;
            index += 2u;
        } else if (prefix == 'o' || prefix == 'O') {
            base = 8u;
            index += 2u;
        }
    }
    for (; index < end; ++index) {
        int digit;
        if (source[index] == '_') continue;
        digit = digit_value(source[index]);
        if (digit < 0 || (uint32_t)digit >= base) return 0;
        value = value * (int64_t)base + digit;
    }
    return value;
}

PikaStatus pika_token_float_value(const char* source,
                                  const PikaToken* token,
                                  double* value) {
    char* text;
    char* end;
    uint32_t source_index;
    uint32_t target_index = 0u;
    if (source == NULL || token == NULL || value == NULL ||
        token->kind != PIKA_TOKEN_FLOAT || token->length == 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    text = (char*)malloc((size_t)token->length + 1u);
    if (text == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
    for (source_index = token->offset;
         source_index < token->offset + token->length;
         ++source_index) {
        if (source[source_index] != '_') {
            text[target_index++] = source[source_index];
        }
    }
    text[target_index] = '\0';
    *value = strtod(text, &end);
    if (end != &text[target_index]) {
        free(text);
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    free(text);
    return PIKA_STATUS_OK;
}

static int scan_digit_group(const char* source,
                            uint32_t length,
                            uint32_t* offset,
                            uint32_t base,
                            int allow_leading_separator) {
    int has_digit = 0;
    int previous_was_digit = 0;
    if (allow_leading_separator && *offset < length &&
        source[*offset] == '_' && *offset + 1u < length &&
        digit_value(source[*offset + 1u]) >= 0 &&
        (uint32_t)digit_value(source[*offset + 1u]) < base) {
        ++*offset;
    }
    while (*offset < length) {
        int digit = digit_value(source[*offset]);
        if (digit >= 0 && (uint32_t)digit < base) {
            has_digit = 1;
            previous_was_digit = 1;
            ++*offset;
            continue;
        }
        if (source[*offset] != '_') {
            break;
        }
        if (!previous_was_digit || *offset + 1u >= length) {
            return -1;
        }
        digit = digit_value(source[*offset + 1u]);
        if (digit < 0 || (uint32_t)digit >= base) {
            return -1;
        }
        previous_was_digit = 0;
        ++*offset;
    }
    return has_digit;
}

static uint32_t bounded_observed_size(size_t value) {
    return value > UINT32_MAX ? UINT32_MAX : (uint32_t)value;
}

static PikaStatus validate_source_encoding(
    const char* source,
    size_t length,
    PikaDiagnostic* diagnostic) {
    size_t offset = 0u;
    uint32_t line = 1u;
    size_t line_start = 0u;
    while (offset < length) {
        const unsigned char value =
            (unsigned char)source[offset];
        size_t width = 1u;
        int valid = 1;
        if (value == 0u) {
            pika_frontend_set_reason_diagnostic(
                diagnostic, PIKA_STATUS_LEX_ERROR,
                PIKA_DIAGNOSTIC_REASON_NUL_BYTE, line,
                bounded_observed_size(offset - line_start + 1u),
                bounded_observed_size(offset));
            return PIKA_STATUS_LEX_ERROR;
        }
        if (value < 0x80u) {
            if (value == '\r' || value == '\n') {
                if (value == '\r' && offset + 1u < length &&
                    source[offset + 1u] == '\n') {
                    ++offset;
                }
                ++line;
                line_start = offset + 1u;
            }
            ++offset;
            continue;
        }
        if (value >= 0xc2u && value <= 0xdfu) {
            width = 2u;
        } else if (value >= 0xe0u && value <= 0xefu) {
            width = 3u;
        } else if (value >= 0xf0u && value <= 0xf4u) {
            width = 4u;
        } else {
            valid = 0;
        }
        if (valid && width > length - offset) {
            valid = 0;
        }
        for (size_t index = 1u; valid && index < width; ++index) {
            const unsigned char continuation =
                (unsigned char)source[offset + index];
            if (continuation < 0x80u || continuation > 0xbfu) {
                valid = 0;
            }
        }
        if (valid && width == 3u) {
            const unsigned char second =
                (unsigned char)source[offset + 1u];
            if ((value == 0xe0u && second < 0xa0u) ||
                (value == 0xedu && second > 0x9fu)) {
                valid = 0;
            }
        }
        if (valid && width == 4u) {
            const unsigned char second =
                (unsigned char)source[offset + 1u];
            if ((value == 0xf0u && second < 0x90u) ||
                (value == 0xf4u && second > 0x8fu)) {
                valid = 0;
            }
        }
        if (!valid) {
            pika_frontend_set_reason_diagnostic(
                diagnostic, PIKA_STATUS_LEX_ERROR,
                PIKA_DIAGNOSTIC_REASON_INVALID_UTF8, line,
                bounded_observed_size(offset - line_start + 1u),
                bounded_observed_size(offset));
            return PIKA_STATUS_LEX_ERROR;
        }
        offset += width;
    }
    return PIKA_STATUS_OK;
}

static uint32_t source_codepoint(const char* source) {
    const unsigned char first = (unsigned char)source[0];
    if (first < 0xe0u) {
        return ((uint32_t)(first & 0x1fu) << 6u) |
               (uint32_t)((unsigned char)source[1] & 0x3fu);
    }
    if (first < 0xf0u) {
        return ((uint32_t)(first & 0x0fu) << 12u) |
               ((uint32_t)((unsigned char)source[1] & 0x3fu) << 6u) |
               (uint32_t)((unsigned char)source[2] & 0x3fu);
    }
    return ((uint32_t)(first & 0x07u) << 18u) |
           ((uint32_t)((unsigned char)source[1] & 0x3fu) << 12u) |
           ((uint32_t)((unsigned char)source[2] & 0x3fu) << 6u) |
           (uint32_t)((unsigned char)source[3] & 0x3fu);
}

static int codepoint_can_start_identifier(uint32_t value) {
    return (value >= 0x00c0u && value <= 0x02afu) ||
           (value >= 0x0370u && value <= 0x052fu) ||
           (value >= 0x3040u && value <= 0x30ffu) ||
           (value >= 0x3400u && value <= 0x9fffu) ||
           (value >= 0xac00u && value <= 0xd7afu);
}

static PikaTokenKind name_kind(const char* text, uint32_t length) {
    if (length == 3u && memcmp(text, "try", 3u) == 0) {
        return PIKA_TOKEN_TRY;
    }
    if (length == 2u && memcmp(text, "in", 2u) == 0) {
        return PIKA_TOKEN_IN;
    }
    if (length == 2u && memcmp(text, "if", 2u) == 0) {
        return PIKA_TOKEN_IF;
    }
    if (length == 4u && memcmp(text, "elif", 4u) == 0) {
        return PIKA_TOKEN_ELIF;
    }
    if (length == 3u && memcmp(text, "and", 3u) == 0) {
        return PIKA_TOKEN_AND;
    }
    if (length == 2u && memcmp(text, "or", 2u) == 0) {
        return PIKA_TOKEN_OR;
    }
    if (length == 3u && memcmp(text, "not", 3u) == 0) {
        return PIKA_TOKEN_NOT;
    }
    if (length == 2u && memcmp(text, "is", 2u) == 0) {
        return PIKA_TOKEN_IS;
    }
    if (length == 4u && memcmp(text, "True", 4u) == 0) {
        return PIKA_TOKEN_TRUE;
    }
    if (length == 5u && memcmp(text, "False", 5u) == 0) {
        return PIKA_TOKEN_FALSE;
    }
    if (length == 4u && memcmp(text, "None", 4u) == 0) {
        return PIKA_TOKEN_NONE;
    }
    if (length == 3u && memcmp(text, "def", 3u) == 0) {
        return PIKA_TOKEN_DEF;
    }
    if (length == 6u && memcmp(text, "lambda", 6u) == 0) {
        return PIKA_TOKEN_LAMBDA;
    }
    if (length == 3u && memcmp(text, "del", 3u) == 0) {
        return PIKA_TOKEN_DEL;
    }
    if (length == 3u && memcmp(text, "for", 3u) == 0) {
        return PIKA_TOKEN_FOR;
    }
    if (length == 4u && memcmp(text, "else", 4u) == 0) {
        return PIKA_TOKEN_ELSE;
    }
    if (length == 5u && memcmp(text, "break", 5u) == 0) {
        return PIKA_TOKEN_BREAK;
    }
    if (length == 5u && memcmp(text, "raise", 5u) == 0) {
        return PIKA_TOKEN_RAISE;
    }
    if (length == 6u && memcmp(text, "assert", 6u) == 0) {
        return PIKA_TOKEN_ASSERT;
    }
    if (length == 5u && memcmp(text, "range", 5u) == 0) {
        return PIKA_TOKEN_RANGE;
    }
    if (length == 5u && memcmp(text, "while", 5u) == 0) {
        return PIKA_TOKEN_WHILE;
    }
    if (length == 5u && memcmp(text, "print", 5u) == 0) {
        return PIKA_TOKEN_PRINT;
    }
    if (length == 6u && memcmp(text, "return", 6u) == 0) {
        return PIKA_TOKEN_RETURN;
    }
    if (length == 4u && memcmp(text, "pass", 4u) == 0) {
        return PIKA_TOKEN_PASS;
    }
    if (length == 5u && memcmp(text, "class", 5u) == 0) {
        return PIKA_TOKEN_CLASS;
    }
    if (length == 8u && memcmp(text, "continue", 8u) == 0) {
        return PIKA_TOKEN_CONTINUE;
    }
    if (length == 6u && memcmp(text, "import", 6u) == 0) {
        return PIKA_TOKEN_IMPORT;
    }
    if (length == 6u && memcmp(text, "except", 6u) == 0) {
        return PIKA_TOKEN_EXCEPT;
    }
    if (length == 7u && memcmp(text, "finally", 7u) == 0) {
        return PIKA_TOKEN_FINALLY;
    }
    if (length == 6u && memcmp(text, "global", 6u) == 0) {
        return PIKA_TOKEN_GLOBAL;
    }
    if (length == 4u && memcmp(text, "from", 4u) == 0) {
        return PIKA_TOKEN_FROM;
    }
    return PIKA_TOKEN_NAME;
}

static PikaStatus emit_token(PikaTokenBuffer* buffer,
                             PikaTokenKind kind,
                             uint32_t offset,
                             uint32_t length,
                             uint32_t line,
                             uint32_t column,
                             PikaDiagnostic* diagnostic) {
    if (length > PIKA_FRONTEND_TOKEN_BYTE_LIMIT) {
        pika_frontend_set_limit_diagnostic(
            diagnostic, PIKA_FRONTEND_BOUND_TOKEN_BYTES,
            PIKA_FRONTEND_TOKEN_BYTE_LIMIT, length, line, column, offset);
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    if (buffer->count >= PIKA_FRONTEND_TOKEN_LIMIT) {
        pika_frontend_set_limit_diagnostic(
            diagnostic, PIKA_FRONTEND_BOUND_TOKENS,
            PIKA_FRONTEND_TOKEN_LIMIT, buffer->count + 1u,
            line, column, offset);
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    buffer->last_kind = kind;
    if (buffer->tokens == NULL) {
        ++buffer->count;
        return PIKA_STATUS_OK;
    }
    if (buffer->count >= buffer->capacity) {
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_FRONTEND_LIMIT, line, column);
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    PikaToken* token = &buffer->tokens[buffer->count++];
    token->offset = offset;
    token->length = (uint16_t)length;
    token->kind = (uint8_t)kind;
    token->reserved = 0u;
    return PIKA_STATUS_OK;
}

PikaStatus pika_source_tokenize(const char* source,
                                size_t length,
                                PikaTokenBuffer* buffer,
                                PikaDiagnostic* diagnostic) {
    uint32_t indent_stack[PIKA_FRONTEND_BLOCK_DEPTH_LIMIT + 1u] = {0u};
    uint32_t indent_depth = 0u;
    uint32_t delimiter_depth = 0u;
    uint32_t offset = 0u;
    uint32_t line = 1u;
    uint32_t line_start = 0u;
    int at_line_start = 1;
    if (source == NULL || buffer == NULL ||
        (buffer->tokens == NULL && buffer->capacity != 0u) ||
        (buffer->tokens != NULL && buffer->capacity == 0u)) {
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_INVALID_ARGUMENT, 1u, 1u);
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    buffer->count = 0u;
    pika_frontend_set_diagnostic(
        diagnostic, PIKA_STATUS_OK, 1u, 1u);
    if (length > PIKA_FRONTEND_SOURCE_BYTE_LIMIT) {
        pika_frontend_set_limit_diagnostic(
            diagnostic, PIKA_FRONTEND_BOUND_SOURCE_BYTES,
            PIKA_FRONTEND_SOURCE_BYTE_LIMIT,
            bounded_observed_size(length), 1u, 1u, 0u);
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    if (length > UINT32_MAX) {
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_INVALID_ARGUMENT, 1u, 1u);
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (validate_source_encoding(source, length, diagnostic) !=
        PIKA_STATUS_OK) {
        return PIKA_STATUS_LEX_ERROR;
    }

    while (offset < (uint32_t)length) {
        uint32_t column;
        char current;
        PikaStatus status;
        if (at_line_start) {
            uint32_t indent = 0u;
            uint32_t logical_offset = offset;
            while (logical_offset < (uint32_t)length &&
                   (source[logical_offset] == ' ' ||
                    source[logical_offset] == '\t' ||
                    source[logical_offset] == '\f')) {
                ++logical_offset;
            }
            if (logical_offset >= (uint32_t)length ||
                source[logical_offset] == '#' ||
                source[logical_offset] == '\n' ||
                source[logical_offset] == '\r') {
                offset = logical_offset;
                if (offset < (uint32_t)length &&
                    source[offset] == '#') {
                    while (offset < (uint32_t)length &&
                           source[offset] != '\n' &&
                           source[offset] != '\r') {
                        ++offset;
                    }
                }
                if (offset >= (uint32_t)length) {
                    break;
                }
                if (source[offset] == '\r' &&
                    offset + 1u < (uint32_t)length &&
                    source[offset + 1u] == '\n') {
                    offset += 2u;
                } else {
                    ++offset;
                }
                ++line;
                line_start = offset;
                continue;
            }
            if (delimiter_depth > 0u) {
                while (offset < (uint32_t)length &&
                       (source[offset] == ' ' ||
                        source[offset] == '\t' ||
                        source[offset] == '\f')) {
                    ++offset;
                }
            } else {
                while (offset < (uint32_t)length &&
                       source[offset] == ' ') {
                    ++offset;
                    ++indent;
                }
                if (offset < (uint32_t)length &&
                    source[offset] == '\t') {
                    pika_frontend_set_diagnostic(
                        diagnostic, PIKA_STATUS_LEX_ERROR, line,
                        indent + 1u);
                    return PIKA_STATUS_LEX_ERROR;
                }
                if (offset < (uint32_t)length &&
                    source[offset] != '\n' &&
                    source[offset] != '\r' &&
                    source[offset] != '#') {
                    if ((indent % 4u) != 0u) {
                        pika_frontend_set_diagnostic(
                            diagnostic, PIKA_STATUS_LEX_ERROR,
                            line, 1u);
                        return PIKA_STATUS_LEX_ERROR;
                    }
                    if (indent > indent_stack[indent_depth]) {
                        if (indent_depth >=
                            PIKA_FRONTEND_BLOCK_DEPTH_LIMIT) {
                            pika_frontend_set_limit_diagnostic(
                                diagnostic,
                                PIKA_FRONTEND_BOUND_BLOCK_DEPTH,
                                PIKA_FRONTEND_BLOCK_DEPTH_LIMIT,
                                indent_depth + 1u, line, 1u,
                                line_start);
                            return PIKA_STATUS_FRONTEND_LIMIT;
                        }
                        indent_stack[++indent_depth] = indent;
                        status = emit_token(
                            buffer, PIKA_TOKEN_INDENT, line_start,
                            indent, line, 1u, diagnostic);
                        if (status != PIKA_STATUS_OK) {
                            return status;
                        }
                    } else {
                        while (indent <
                               indent_stack[indent_depth]) {
                            --indent_depth;
                            status = emit_token(
                                buffer, PIKA_TOKEN_DEDENT, offset,
                                0u, line, 1u, diagnostic);
                            if (status != PIKA_STATUS_OK) {
                                return status;
                            }
                        }
                        if (indent !=
                            indent_stack[indent_depth]) {
                            pika_frontend_set_diagnostic(
                                diagnostic,
                                PIKA_STATUS_LEX_ERROR,
                                line, 1u);
                            return PIKA_STATUS_LEX_ERROR;
                        }
                    }
                }
            }
            at_line_start = 0;
            if (offset >= (uint32_t)length) {
                break;
            }
        }

        current = source[offset];
        column = offset - line_start + 1u;
        if (current == ' ' || current == '\f') {
            ++offset;
            continue;
        }
        if (current == '\t') {
            pika_frontend_set_diagnostic(
                diagnostic, PIKA_STATUS_LEX_ERROR, line, column);
            return PIKA_STATUS_LEX_ERROR;
        }
        if (current == '#') {
            while (offset < (uint32_t)length &&
                   source[offset] != '\n' && source[offset] != '\r') {
                ++offset;
            }
            continue;
        }
        if (current == '\n' || current == '\r') {
            uint32_t newline_offset = offset;
            if (current == '\r' &&
                offset + 1u < (uint32_t)length &&
                source[offset + 1u] == '\n') {
                offset += 2u;
            } else {
                ++offset;
            }
            if (delimiter_depth == 0u) {
                status = emit_token(
                    buffer, PIKA_TOKEN_NEWLINE, newline_offset,
                    offset - newline_offset, line, column,
                    diagnostic);
                if (status != PIKA_STATUS_OK) {
                    return status;
                }
            }
            ++line;
            line_start = offset;
            at_line_start = 1;
            continue;
        }
        if ((current >= '0' && current <= '9') ||
            (current == '.' && offset + 1u < (uint32_t)length &&
             source[offset + 1u] >= '0' &&
             source[offset + 1u] <= '9')) {
            uint32_t start = offset;
            int64_t value = 0;
            int is_float = 0;
            uint32_t base = 10u;
            uint32_t number_length;
            int scan_status;
            if (current == '0' &&
                offset + 1u < (uint32_t)length &&
                (source[offset + 1u] == 'x' ||
                 source[offset + 1u] == 'X' ||
                 source[offset + 1u] == 'b' ||
                 source[offset + 1u] == 'B' ||
                 source[offset + 1u] == 'o' ||
                 source[offset + 1u] == 'O')) {
                char prefix = source[offset + 1u];
                base = prefix == 'x' || prefix == 'X'
                           ? 16u
                           : prefix == 'b' || prefix == 'B'
                                 ? 2u
                                 : 8u;
                offset += 2u;
                scan_status = scan_digit_group(
                    source, (uint32_t)length, &offset, base, 1);
                if (scan_status <= 0 ||
                    (offset < (uint32_t)length &&
                     (is_name_continue(source[offset]) ||
                      source[offset] == '.' ||
                      (unsigned char)source[offset] >= 0x80u))) {
                    pika_frontend_set_diagnostic(
                        diagnostic, PIKA_STATUS_LEX_ERROR, line, column);
                    return PIKA_STATUS_LEX_ERROR;
                }
            } else {
                scan_status = scan_digit_group(
                    source, (uint32_t)length, &offset, 10u, 0);
                if (scan_status < 0) {
                    pika_frontend_set_diagnostic(
                        diagnostic, PIKA_STATUS_LEX_ERROR, line, column);
                    return PIKA_STATUS_LEX_ERROR;
                }
            }
            if (base == 10u && offset < (uint32_t)length &&
                source[offset] == '.') {
                is_float = 1;
                ++offset;
                scan_status = scan_digit_group(
                    source, (uint32_t)length, &offset, 10u, 0);
                if (scan_status < 0) {
                    pika_frontend_set_diagnostic(
                        diagnostic, PIKA_STATUS_LEX_ERROR, line, column);
                    return PIKA_STATUS_LEX_ERROR;
                }
            }
            if (base == 10u && offset < (uint32_t)length &&
                (source[offset] == 'e' || source[offset] == 'E')) {
                is_float = 1;
                ++offset;
                if (offset < (uint32_t)length &&
                    (source[offset] == '+' || source[offset] == '-')) {
                    ++offset;
                }
                scan_status = scan_digit_group(
                    source, (uint32_t)length, &offset, 10u, 0);
                if (scan_status <= 0) {
                    pika_frontend_set_diagnostic(
                        diagnostic, PIKA_STATUS_LEX_ERROR, line, column);
                    return PIKA_STATUS_LEX_ERROR;
                }
            }
            number_length = offset - start;
            if (number_length >
                PIKA_FRONTEND_NUMERIC_LITERAL_BYTE_LIMIT) {
                pika_frontend_set_limit_diagnostic(
                    diagnostic,
                    PIKA_FRONTEND_BOUND_NUMERIC_LITERAL_BYTES,
                    PIKA_FRONTEND_NUMERIC_LITERAL_BYTE_LIMIT,
                    number_length, line, column, start);
                return PIKA_STATUS_FRONTEND_LIMIT;
            }
            if (offset < (uint32_t)length &&
                (is_name_start(source[offset]) ||
                 (unsigned char)source[offset] >= 0x80u)) {
                pika_frontend_set_reason_diagnostic(
                    diagnostic, PIKA_STATUS_LEX_ERROR,
                    PIKA_DIAGNOSTIC_REASON_UNSUPPORTED_IDENTIFIER,
                    line, column, start);
                return PIKA_STATUS_LEX_ERROR;
            }
            if (!is_float) {
                uint32_t index =
                    base == 10u ? start : start + 2u;
                for (; index < offset; ++index) {
                    int digit;
                    if (source[index] == '_') {
                        continue;
                    }
                    digit = digit_value(source[index]);
                    if (digit < 0 || (uint32_t)digit >= base ||
                        value > (INT64_MAX - digit) / (int64_t)base) {
                        pika_frontend_set_reason_diagnostic(
                            diagnostic, PIKA_STATUS_LEX_ERROR,
                            PIKA_DIAGNOSTIC_REASON_INTEGER_LITERAL_RANGE,
                            line, column, start);
                        return PIKA_STATUS_LEX_ERROR;
                    }
                    value = value * (int64_t)base + digit;
                }
            }
            status = emit_token(
                buffer, is_float ? PIKA_TOKEN_FLOAT : PIKA_TOKEN_INTEGER,
                start, number_length,
                line, column, diagnostic);
            if (status != PIKA_STATUS_OK) {
                return status;
            }
            continue;
        }
        if (current == '\'' || current == '"') {
            char quote = current;
            uint32_t start = offset;
            int triple = offset + 2u < (uint32_t)length &&
                         source[offset + 1u] == quote &&
                         source[offset + 2u] == quote;
            offset += triple ? 3u : 1u;
            while (offset < (uint32_t)length) {
                if (triple && offset + 2u < (uint32_t)length &&
                    source[offset] == quote &&
                    source[offset + 1u] == quote &&
                    source[offset + 2u] == quote) {
                    offset += 3u;
                    break;
                }
                if (!triple && source[offset] == quote) {
                    ++offset;
                    break;
                }
                if (!triple && (source[offset] == '\n' ||
                                source[offset] == '\r')) {
                    break;
                }
                if (source[offset] == '\\' &&
                    offset + 1u < (uint32_t)length) {
                    offset += 2u;
                } else if (triple && (source[offset] == '\n' ||
                                      source[offset] == '\r')) {
                    if (source[offset] == '\r' &&
                        offset + 1u < (uint32_t)length &&
                        source[offset + 1u] == '\n') {
                        offset += 2u;
                    } else {
                        ++offset;
                    }
                    ++line;
                    line_start = offset;
                } else {
                    ++offset;
                }
            }
            if (offset > (uint32_t)length ||
                (triple ? (offset < 3u ||
                           source[offset - 1u] != quote ||
                           source[offset - 2u] != quote ||
                           source[offset - 3u] != quote)
                        : (offset == 0u ||
                           source[offset - 1u] != quote))) {
                pika_frontend_set_diagnostic(
                    diagnostic, PIKA_STATUS_LEX_ERROR, line, column);
                return PIKA_STATUS_LEX_ERROR;
            }
            if (offset - start - (triple ? 6u : 2u) >
                PIKA_FRONTEND_LITERAL_BYTE_LIMIT) {
                pika_frontend_set_limit_diagnostic(
                    diagnostic, PIKA_FRONTEND_BOUND_LITERAL_BYTES,
                    PIKA_FRONTEND_LITERAL_BYTE_LIMIT,
                    offset - start - (triple ? 6u : 2u),
                    line, column, start);
                return PIKA_STATUS_FRONTEND_LIMIT;
            }
            status = emit_token(
                buffer, PIKA_TOKEN_STRING, start, offset - start,
                line, column, diagnostic);
            if (status != PIKA_STATUS_OK) {
                return status;
            }
            continue;
        }
        if ((current == 'b' || current == 'B') &&
            offset + 1u < (uint32_t)length &&
            (source[offset + 1u] == '\'' ||
             source[offset + 1u] == '"')) {
            char quote = source[offset + 1u];
            uint32_t start = offset;
            int triple = offset + 3u < (uint32_t)length &&
                         source[offset + 2u] == quote &&
                         source[offset + 3u] == quote;
            offset += triple ? 4u : 2u;
            while (offset < (uint32_t)length) {
                if (triple && offset + 2u < (uint32_t)length &&
                    source[offset] == quote &&
                    source[offset + 1u] == quote &&
                    source[offset + 2u] == quote) {
                    offset += 3u;
                    break;
                }
                if (!triple && source[offset] == quote) {
                    ++offset;
                    break;
                }
                if (!triple && (source[offset] == '\n' ||
                                source[offset] == '\r')) {
                    break;
                }
                if (source[offset] == '\\' &&
                    offset + 1u < (uint32_t)length) {
                    offset += 2u;
                } else if (triple && (source[offset] == '\n' ||
                                      source[offset] == '\r')) {
                    if (source[offset] == '\r' &&
                        offset + 1u < (uint32_t)length &&
                        source[offset + 1u] == '\n') {
                        offset += 2u;
                    } else {
                        ++offset;
                    }
                    ++line;
                    line_start = offset;
                } else {
                    ++offset;
                }
            }
            if (offset > (uint32_t)length ||
                (triple ? (offset < 3u ||
                           source[offset - 1u] != quote ||
                           source[offset - 2u] != quote ||
                           source[offset - 3u] != quote)
                        : (offset == 0u ||
                           source[offset - 1u] != quote))) {
                pika_frontend_set_diagnostic(
                    diagnostic, PIKA_STATUS_LEX_ERROR, line, column);
                return PIKA_STATUS_LEX_ERROR;
            }
            if (offset - start - (triple ? 7u : 3u) >
                PIKA_FRONTEND_LITERAL_BYTE_LIMIT) {
                pika_frontend_set_limit_diagnostic(
                    diagnostic, PIKA_FRONTEND_BOUND_LITERAL_BYTES,
                    PIKA_FRONTEND_LITERAL_BYTE_LIMIT,
                    offset - start - (triple ? 7u : 3u),
                    line, column, start);
                return PIKA_STATUS_FRONTEND_LIMIT;
            }
            status = emit_token(
                buffer, PIKA_TOKEN_BYTES, start, offset - start,
                line, column, diagnostic);
            if (status != PIKA_STATUS_OK) return status;
            continue;
        }
        if (is_name_start(current)) {
            uint32_t start = offset++;
            while (offset < (uint32_t)length &&
                   is_name_continue(source[offset])) {
                ++offset;
            }
            if (offset - start >
                PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT) {
                pika_frontend_set_limit_diagnostic(
                    diagnostic,
                    PIKA_FRONTEND_BOUND_IDENTIFIER_BYTES,
                    PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT,
                    offset - start, line, column, start);
                return PIKA_STATUS_FRONTEND_LIMIT;
            }
            status = emit_token(
                buffer, name_kind(&source[start], offset - start),
                start, offset - start, line, column, diagnostic);
            if (status != PIKA_STATUS_OK) {
                return status;
            }
            continue;
        }
        {
            PikaTokenKind kind;
            uint32_t width = 1u;
            if (current == '=' &&
                offset + 1u < (uint32_t)length &&
                source[offset + 1u] == '=') {
                kind = PIKA_TOKEN_EQUAL_EQUAL;
                width = 2u;
            } else if (current == '!' &&
                       offset + 1u < (uint32_t)length &&
                       source[offset + 1u] == '=') {
                kind = PIKA_TOKEN_NOT_EQUAL;
                width = 2u;
            } else if (current == '<' &&
                       offset + 1u < (uint32_t)length &&
                       source[offset + 1u] == '=') {
                kind = PIKA_TOKEN_LESS_EQUAL;
                width = 2u;
            } else if (current == '>' &&
                       offset + 1u < (uint32_t)length &&
                       source[offset + 1u] == '=') {
                kind = PIKA_TOKEN_GREATER_EQUAL;
                width = 2u;
            } else if (current == '<' &&
                       offset + 1u < (uint32_t)length &&
                       source[offset + 1u] == '<') {
                kind = (offset + 2u < (uint32_t)length &&
                        source[offset + 2u] == '=')
                           ? PIKA_TOKEN_LEFT_SHIFT_ASSIGN
                           : PIKA_TOKEN_LEFT_SHIFT;
                width = kind == PIKA_TOKEN_LEFT_SHIFT_ASSIGN ? 3u : 2u;
            } else if (current == '>' &&
                       offset + 1u < (uint32_t)length &&
                       source[offset + 1u] == '>') {
                kind = (offset + 2u < (uint32_t)length &&
                        source[offset + 2u] == '=')
                           ? PIKA_TOKEN_RIGHT_SHIFT_ASSIGN
                           : PIKA_TOKEN_RIGHT_SHIFT;
                width = kind == PIKA_TOKEN_RIGHT_SHIFT_ASSIGN ? 3u : 2u;
            } else if (current == '*' &&
                       offset + 1u < (uint32_t)length &&
                       source[offset + 1u] == '*') {
                kind = (offset + 2u < (uint32_t)length &&
                        source[offset + 2u] == '=')
                           ? PIKA_TOKEN_DOUBLE_STAR_ASSIGN
                           : PIKA_TOKEN_DOUBLE_STAR;
                width = kind == PIKA_TOKEN_DOUBLE_STAR_ASSIGN ? 3u : 2u;
            } else if (current == '/' &&
                       offset + 1u < (uint32_t)length &&
                       source[offset + 1u] == '/') {
                kind = (offset + 2u < (uint32_t)length &&
                        source[offset + 2u] == '=')
                           ? PIKA_TOKEN_DOUBLE_SLASH_ASSIGN
                           : PIKA_TOKEN_DOUBLE_SLASH;
                width = kind == PIKA_TOKEN_DOUBLE_SLASH_ASSIGN ? 3u : 2u;
            } else if (current == '@' &&
                       offset + 1u < (uint32_t)length &&
                       source[offset + 1u] == '=') {
                kind = PIKA_TOKEN_AT_ASSIGN;
                width = 2u;
            } else {
                switch (current) {
                    case '+':
                        kind = (offset + 1u < (uint32_t)length &&
                                source[offset + 1u] == '=')
                                   ? PIKA_TOKEN_PLUS_ASSIGN
                                   : PIKA_TOKEN_PLUS;
                        width = kind == PIKA_TOKEN_PLUS_ASSIGN ? 2u : 1u;
                        break;
                    case '-':
                        kind = (offset + 1u < (uint32_t)length &&
                                source[offset + 1u] == '=')
                                   ? PIKA_TOKEN_MINUS_ASSIGN
                                   : PIKA_TOKEN_MINUS;
                        width = kind == PIKA_TOKEN_MINUS_ASSIGN ? 2u : 1u;
                        break;
                    case '*':
                        kind = (offset + 1u < (uint32_t)length &&
                                source[offset + 1u] == '=')
                                   ? PIKA_TOKEN_STAR_ASSIGN
                                   : PIKA_TOKEN_STAR;
                        width = kind == PIKA_TOKEN_STAR_ASSIGN ? 2u : 1u;
                        break;
                    case '/':
                        kind = (offset + 1u < (uint32_t)length &&
                                source[offset + 1u] == '=')
                                   ? PIKA_TOKEN_SLASH_ASSIGN
                                   : PIKA_TOKEN_SLASH;
                        width = kind == PIKA_TOKEN_SLASH_ASSIGN ? 2u : 1u;
                        break;
                    case '%':
                        kind = (offset + 1u < (uint32_t)length &&
                                source[offset + 1u] == '=')
                                   ? PIKA_TOKEN_PERCENT_ASSIGN
                                   : PIKA_TOKEN_PERCENT;
                        width = kind == PIKA_TOKEN_PERCENT_ASSIGN ? 2u : 1u;
                        break;
                    case '&':
                        kind = (offset + 1u < (uint32_t)length &&
                                source[offset + 1u] == '=')
                                   ? PIKA_TOKEN_AMPERSAND_ASSIGN
                                   : PIKA_TOKEN_AMPERSAND;
                        width = kind == PIKA_TOKEN_AMPERSAND_ASSIGN ? 2u : 1u;
                        break;
                    case '^':
                        kind = (offset + 1u < (uint32_t)length &&
                                source[offset + 1u] == '=')
                                   ? PIKA_TOKEN_CARET_ASSIGN
                                   : PIKA_TOKEN_CARET;
                        width = kind == PIKA_TOKEN_CARET_ASSIGN ? 2u : 1u;
                        break;
                    case '|':
                        kind = (offset + 1u < (uint32_t)length &&
                                source[offset + 1u] == '=')
                                   ? PIKA_TOKEN_PIPE_ASSIGN
                                   : PIKA_TOKEN_PIPE;
                        width = kind == PIKA_TOKEN_PIPE_ASSIGN ? 2u : 1u;
                        break;
                    case '<':
                        kind = PIKA_TOKEN_LESS;
                        break;
                    case '>':
                        kind = PIKA_TOKEN_GREATER;
                        break;
                    case '=':
                        kind = PIKA_TOKEN_ASSIGN;
                        break;
                    case '~':
                        kind = PIKA_TOKEN_TILDE;
                        break;
                    case '@':
                        kind = PIKA_TOKEN_AT;
                        break;
                    case ':':
                        kind = PIKA_TOKEN_COLON;
                        break;
                    case '(':
                        kind = PIKA_TOKEN_LEFT_PAREN;
                        break;
                    case ')':
                        kind = PIKA_TOKEN_RIGHT_PAREN;
                        break;
                    case '[':
                        kind = PIKA_TOKEN_LEFT_BRACKET;
                        break;
                    case ']':
                        kind = PIKA_TOKEN_RIGHT_BRACKET;
                        break;
                    case '{':
                        kind = PIKA_TOKEN_LEFT_BRACE;
                        break;
                    case '}':
                        kind = PIKA_TOKEN_RIGHT_BRACE;
                        break;
                    case ',':
                        kind = PIKA_TOKEN_COMMA;
                        break;
                    case '.':
                        kind = PIKA_TOKEN_DOT;
                        break;
                    default:
                        if ((unsigned char)current >= 0x80u) {
                            const uint32_t codepoint =
                                source_codepoint(&source[offset]);
                            pika_frontend_set_reason_diagnostic(
                                diagnostic, PIKA_STATUS_LEX_ERROR,
                                codepoint_can_start_identifier(codepoint)
                                    ? PIKA_DIAGNOSTIC_REASON_UNSUPPORTED_IDENTIFIER
                                    : PIKA_DIAGNOSTIC_REASON_UNSUPPORTED_CHARACTER,
                                line, column, offset);
                        } else {
                            pika_frontend_set_reason_diagnostic(
                                diagnostic, PIKA_STATUS_LEX_ERROR,
                                PIKA_DIAGNOSTIC_REASON_UNSUPPORTED_CHARACTER,
                                line, column, offset);
                        }
                        return PIKA_STATUS_LEX_ERROR;
                }
            }
            status = emit_token(
                buffer, kind, offset, width, line, column, diagnostic);
            if (status == PIKA_STATUS_OK) {
                if (kind == PIKA_TOKEN_LEFT_PAREN ||
                    kind == PIKA_TOKEN_LEFT_BRACKET ||
                    kind == PIKA_TOKEN_LEFT_BRACE) {
                    ++delimiter_depth;
                } else if (
                    delimiter_depth > 0u &&
                    (kind == PIKA_TOKEN_RIGHT_PAREN ||
                     kind == PIKA_TOKEN_RIGHT_BRACKET ||
                     kind == PIKA_TOKEN_RIGHT_BRACE)) {
                    --delimiter_depth;
                }
            }
            offset += width;
        }
        if (status != PIKA_STATUS_OK) {
            return status;
        }
    }

    if (buffer->count > 0u &&
        buffer->last_kind != PIKA_TOKEN_NEWLINE) {
        PikaStatus status = emit_token(
            buffer, PIKA_TOKEN_NEWLINE, offset, 0u, line,
            offset - line_start + 1u, diagnostic);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
    }
    while (indent_depth > 0u) {
        PikaStatus status;
        --indent_depth;
        status = emit_token(
            buffer, PIKA_TOKEN_DEDENT, offset, 0u, line, 1u,
            diagnostic);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
    }
    return emit_token(
        buffer, PIKA_TOKEN_EOF, offset, 0u, line,
        offset - line_start + 1u, diagnostic);
}

PikaStatus pika_frontend_tokenize_allocated(
    const char* source,
    size_t length,
    PikaTokenBuffer* buffer,
    PikaDiagnostic* diagnostic) {
    PikaTokenBuffer counter;
    PikaStatus status;
    if (buffer == NULL) {
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_INVALID_ARGUMENT, 1u, 1u);
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    memset(buffer, 0, sizeof(*buffer));
    memset(&counter, 0, sizeof(counter));
    status = pika_source_tokenize(
        source, length, &counter, diagnostic);
    if (status != PIKA_STATUS_OK) return status;
    if (counter.count == 0u) {
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_INTERNAL_ERROR, 1u, 1u);
        return PIKA_STATUS_INTERNAL_ERROR;
    }
    buffer->tokens = (PikaToken*)calloc(
        counter.count, sizeof(PikaToken));
    if (buffer->tokens == NULL) {
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_FRONTEND_LIMIT, 1u, 1u);
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    buffer->capacity = counter.count;
    status = pika_source_tokenize(
        source, length, buffer, diagnostic);
    if (status != PIKA_STATUS_OK) {
        free(buffer->tokens);
        memset(buffer, 0, sizeof(*buffer));
    }
    return status;
}
