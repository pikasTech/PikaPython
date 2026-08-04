/* SPEC: PJ2026-0501 V2 kernel v0.20; frontend diagnostics. */
#include "frontend_internal.h"

#include <stddef.h>
#include <stdint.h>

void pika_frontend_source_location(const char* source,
                                   uint32_t offset,
                                   uint32_t* line,
                                   uint32_t* column) {
    uint32_t cursor = 0u;
    uint32_t current_line = 1u;
    uint32_t line_start = 0u;
    if (source != NULL) {
        while (cursor < offset) {
            if (source[cursor] == '\r' ||
                source[cursor] == '\n') {
                if (source[cursor] == '\r' &&
                    cursor + 1u < offset &&
                    source[cursor + 1u] == '\n') {
                    cursor += 2u;
                } else {
                    ++cursor;
                }
                ++current_line;
                line_start = cursor;
            } else {
                ++cursor;
            }
        }
    }
    if (line != NULL) {
        *line = current_line;
    }
    if (column != NULL) {
        *column = offset - line_start + 1u;
    }
}

const char* pika_frontend_diagnostic_reason_name(
    PikaDiagnosticReason reason) {
    switch (reason) {
        case PIKA_DIAGNOSTIC_REASON_NONE:
            return "none";
        case PIKA_DIAGNOSTIC_REASON_LIMIT_EXCEEDED:
            return "configured_limit_exceeded";
        case PIKA_DIAGNOSTIC_REASON_NUL_BYTE:
            return "nul_byte";
        case PIKA_DIAGNOSTIC_REASON_INVALID_UTF8:
            return "invalid_utf8";
        case PIKA_DIAGNOSTIC_REASON_UNSUPPORTED_CHARACTER:
            return "unsupported_character";
        case PIKA_DIAGNOSTIC_REASON_UNSUPPORTED_IDENTIFIER:
            return "unsupported_identifier";
        case PIKA_DIAGNOSTIC_REASON_INTEGER_LITERAL_RANGE:
            return "integer_literal_out_of_range";
        case PIKA_DIAGNOSTIC_REASON_INCOMPLETE_INPUT:
            return "incomplete_input";
        default:
            return "unknown";
    }
}

const char* pika_frontend_bound_macro(PikaFrontendBound bound) {
    switch (bound) {
        case PIKA_FRONTEND_BOUND_NONE:
            return "none";
        case PIKA_FRONTEND_BOUND_SOURCE_BYTES:
            return "PIKA_FRONTEND_SOURCE_BYTE_LIMIT";
        case PIKA_FRONTEND_BOUND_TOKENS:
            return "PIKA_FRONTEND_TOKEN_LIMIT";
        case PIKA_FRONTEND_BOUND_IDENTIFIER_BYTES:
            return "PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT";
        case PIKA_FRONTEND_BOUND_NUMERIC_LITERAL_BYTES:
            return "PIKA_FRONTEND_NUMERIC_LITERAL_BYTE_LIMIT";
        case PIKA_FRONTEND_BOUND_LITERAL_BYTES:
            return "PIKA_FRONTEND_LITERAL_BYTE_LIMIT";
        case PIKA_FRONTEND_BOUND_PARAMETERS:
            return "PIKA_FRONTEND_PARAMETER_LIMIT";
        case PIKA_FRONTEND_BOUND_ARGUMENTS:
            return "PIKA_FRONTEND_ARGUMENT_LIMIT";
        case PIKA_FRONTEND_BOUND_COLLECTION_ELEMENTS:
            return "PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT";
        case PIKA_FRONTEND_BOUND_SYMBOLS:
            return "PIKA_FRONTEND_SYMBOL_LIMIT";
        case PIKA_FRONTEND_BOUND_EXPRESSION_DEPTH:
            return "PIKA_FRONTEND_EXPRESSION_DEPTH_LIMIT";
        case PIKA_FRONTEND_BOUND_BLOCK_DEPTH:
            return "PIKA_FRONTEND_BLOCK_DEPTH_LIMIT";
        case PIKA_FRONTEND_BOUND_LOOPS:
            return "PIKA_FRONTEND_LOOP_LIMIT";
        case PIKA_FRONTEND_BOUND_CALL_DEPTH:
            return "PIKA_FRONTEND_CALL_DEPTH_LIMIT";
        case PIKA_FRONTEND_BOUND_FUNCTION_INSTRUCTIONS:
            return "PIKA_FRONTEND_FUNCTION_INSTRUCTION_LIMIT";
        case PIKA_FRONTEND_BOUND_FUNCTIONS:
            return "PIKA_FRONTEND_FUNCTION_LIMIT";
        case PIKA_FRONTEND_BOUND_CLASSES:
            return "PIKA_FRONTEND_CLASS_LIMIT";
        case PIKA_FRONTEND_BOUND_CLASS_MEMBERS:
            return "PIKA_FRONTEND_CLASS_MEMBER_LIMIT";
        case PIKA_FRONTEND_BOUND_FIELDS:
            return "PIKA_FRONTEND_FIELD_LIMIT";
        case PIKA_FRONTEND_BOUND_MODULES:
            return "PIKA_FRONTEND_MODULE_LIMIT";
        case PIKA_FRONTEND_BOUND_EXCEPTION_HANDLERS:
            return "PIKA_FRONTEND_EXCEPTION_HANDLER_LIMIT";
        case PIKA_FRONTEND_BOUND_TOKEN_BYTES:
            return "PIKA_FRONTEND_TOKEN_BYTE_LIMIT";
        default:
            return "unknown";
    }
}

void pika_frontend_set_reason_diagnostic(
    PikaDiagnostic* diagnostic,
    PikaStatus status,
    PikaDiagnosticReason reason,
    uint32_t line,
    uint32_t column,
    uint32_t offset) {
    if (diagnostic != NULL) {
        diagnostic->status = status;
        diagnostic->line = line;
        diagnostic->column = column;
        diagnostic->offset = offset;
        diagnostic->reason = reason;
        diagnostic->bound = PIKA_FRONTEND_BOUND_NONE;
        diagnostic->configured = 0u;
        diagnostic->observed = 0u;
        diagnostic->source_name[0] = '\0';
    }
}

void pika_frontend_set_diagnostic(PikaDiagnostic* diagnostic,
                                  PikaStatus status,
                                  uint32_t line,
                                  uint32_t column) {
    pika_frontend_set_reason_diagnostic(
        diagnostic, status, PIKA_DIAGNOSTIC_REASON_NONE,
        line, column, 0u);
}

void pika_frontend_set_limit_diagnostic(
    PikaDiagnostic* diagnostic,
    PikaFrontendBound bound,
    uint32_t configured,
    uint32_t observed,
    uint32_t line,
    uint32_t column,
    uint32_t offset) {
    pika_frontend_set_reason_diagnostic(
        diagnostic, PIKA_STATUS_FRONTEND_LIMIT,
        PIKA_DIAGNOSTIC_REASON_LIMIT_EXCEEDED,
        line, column, offset);
    if (diagnostic != NULL) {
        diagnostic->bound = bound;
        diagnostic->configured = configured;
        diagnostic->observed = observed;
    }
}
