/* SPEC: PJ2026-0501 V2内核 v0.43；状态名与异常映射。 */
#include "pika_status.h"

const char* pika_status_name(PikaStatus status) {
    switch (status) {
        case PIKA_STATUS_OK:
            return "ok";
        case PIKA_STATUS_INVALID_ARGUMENT:
            return "invalid_argument";
        case PIKA_STATUS_UNSUPPORTED_FORMAT:
            return "unsupported_format";
        case PIKA_STATUS_UNKNOWN_OPCODE:
            return "unknown_opcode";
        case PIKA_STATUS_INVALID_OPERAND:
            return "invalid_operand";
        case PIKA_STATUS_INVALID_CONTROL_FLOW:
            return "invalid_control_flow";
        case PIKA_STATUS_INVALID_FUNCTION:
            return "invalid_function";
        case PIKA_STATUS_STORAGE_TOO_SMALL:
            return "storage_too_small";
        case PIKA_STATUS_CALL_DEPTH_EXCEEDED:
            return "call_depth_exceeded";
        case PIKA_STATUS_DIVISION_BY_ZERO:
            return "division_by_zero";
        case PIKA_STATUS_INTERNAL_ERROR:
            return "internal_error";
        case PIKA_STATUS_LEX_ERROR:
            return "lex_error";
        case PIKA_STATUS_SYNTAX_ERROR:
            return "syntax_error";
        case PIKA_STATUS_UNSUPPORTED_SYNTAX:
            return "unsupported_syntax";
        case PIKA_STATUS_UNDEFINED_NAME:
            return "undefined_name";
        case PIKA_STATUS_FRONTEND_LIMIT:
            return "frontend_limit";
        case PIKA_STATUS_OUTPUT_ERROR:
            return "output_error";
        case PIKA_STATUS_IO_ERROR:
            return "io_error";
        case PIKA_STATUS_ARGUMENT_COUNT:
            return "argument_count";
        case PIKA_STATUS_UNSUPPORTED_CAPABILITY:
            return "unsupported_capability";
        case PIKA_STATUS_CAPABILITY_DEPENDENCY:
            return "capability_dependency";
        case PIKA_STATUS_MODULE_NOT_FOUND:
            return "module_not_found";
        case PIKA_STATUS_MEMBER_NOT_FOUND:
            return "member_not_found";
        case PIKA_STATUS_DUPLICATE_BINDING:
            return "duplicate_binding";
        case PIKA_STATUS_BINDING_SCHEMA:
            return "binding_schema";
        case PIKA_STATUS_TYPE_MISMATCH:
            return "type_mismatch";
        case PIKA_STATUS_CALLBACK_ERROR:
            return "callback_error";
        case PIKA_STATUS_MODULE_CYCLE:
            return "module_cycle";
        case PIKA_STATUS_MODULE_CONFLICT:
            return "module_conflict";
        case PIKA_STATUS_UNKNOWN_KEYWORD:
            return "unknown_keyword";
        case PIKA_STATUS_DUPLICATE_ARGUMENT:
            return "duplicate_argument";
        case PIKA_STATUS_POSITIONAL_AFTER_KEYWORD:
            return "positional_after_keyword";
        case PIKA_STATUS_EXCEPTION:
            return "exception";
        case PIKA_STATUS_RUNTIME_ERROR:
            return "runtime_error";
        case PIKA_STATUS_VALUE_ERROR:
            return "value_error";
        case PIKA_STATUS_TYPE_ERROR:
            return "type_error";
        case PIKA_STATUS_OS_ERROR:
            return "os_error";
        case PIKA_STATUS_SOCKET_CLOSED:
            return "socket_closed";
        case PIKA_STATUS_SOCKET_WOULD_BLOCK:
            return "socket_would_block";
        case PIKA_STATUS_SOCKET_TIMEOUT:
            return "socket_timeout";
        case PIKA_STATUS_SOCKET_DNS_ERROR:
            return "socket_dns_error";
        case PIKA_STATUS_SOCKET_CONNECTION_REFUSED:
            return "socket_connection_refused";
        case PIKA_STATUS_SOCKET_CONNECTION_RESET:
            return "socket_connection_reset";
        case PIKA_STATUS_SOCKET_RECEIVE_BYTE_LIMIT:
            return "PIKA_SOCKET_RECEIVE_BYTE_LIMIT";
        case PIKA_STATUS_SOCKET_HOST_BYTE_LIMIT:
            return "PIKA_SOCKET_HOST_BYTE_LIMIT";
        case PIKA_STATUS_SOCKET_OBJECT_LIMIT:
            return "PIKA_SOCKET_OBJECT_LIMIT";
        case PIKA_STATUS_OVERFLOW_ERROR:
            return "overflow_error";
        default:
            return "unknown_status";
    }
}

int pika_status_is_known(PikaStatus status) {
    return (unsigned int)status <=
           (unsigned int)PIKA_STATUS_OVERFLOW_ERROR;
}

int pika_status_can_raise(PikaStatus status) {
    return status == PIKA_STATUS_EXCEPTION ||
           status == PIKA_STATUS_RUNTIME_ERROR ||
           status == PIKA_STATUS_VALUE_ERROR ||
           status == PIKA_STATUS_TYPE_ERROR ||
           status == PIKA_STATUS_DIVISION_BY_ZERO ||
           status == PIKA_STATUS_OS_ERROR ||
           status == PIKA_STATUS_SOCKET_CLOSED ||
           status == PIKA_STATUS_SOCKET_WOULD_BLOCK ||
           status == PIKA_STATUS_SOCKET_TIMEOUT ||
           status == PIKA_STATUS_SOCKET_DNS_ERROR ||
           status == PIKA_STATUS_SOCKET_CONNECTION_REFUSED ||
           status == PIKA_STATUS_SOCKET_CONNECTION_RESET ||
           status == PIKA_STATUS_SOCKET_RECEIVE_BYTE_LIMIT ||
           status == PIKA_STATUS_SOCKET_HOST_BYTE_LIMIT ||
           status == PIKA_STATUS_SOCKET_OBJECT_LIMIT ||
           status == PIKA_STATUS_OVERFLOW_ERROR;
}

PikaExceptionKind pika_status_exception_kind(PikaStatus status) {
    switch (status) {
        case PIKA_STATUS_EXCEPTION:
            return PIKA_EXCEPTION_EXCEPTION;
        case PIKA_STATUS_RUNTIME_ERROR:
        case PIKA_STATUS_CALL_DEPTH_EXCEEDED:
        case PIKA_STATUS_CALLBACK_ERROR:
            return PIKA_EXCEPTION_RUNTIME_ERROR;
        case PIKA_STATUS_VALUE_ERROR:
        case PIKA_STATUS_INVALID_OPERAND:
            return PIKA_EXCEPTION_VALUE_ERROR;
        case PIKA_STATUS_TYPE_ERROR:
        case PIKA_STATUS_TYPE_MISMATCH:
        case PIKA_STATUS_ARGUMENT_COUNT:
        case PIKA_STATUS_UNKNOWN_KEYWORD:
        case PIKA_STATUS_DUPLICATE_ARGUMENT:
        case PIKA_STATUS_POSITIONAL_AFTER_KEYWORD:
            return PIKA_EXCEPTION_TYPE_ERROR;
        case PIKA_STATUS_DIVISION_BY_ZERO:
            return PIKA_EXCEPTION_ZERO_DIVISION_ERROR;
        case PIKA_STATUS_OVERFLOW_ERROR:
            return PIKA_EXCEPTION_OVERFLOW_ERROR;
        case PIKA_STATUS_OS_ERROR:
        case PIKA_STATUS_IO_ERROR:
        case PIKA_STATUS_OUTPUT_ERROR:
        case PIKA_STATUS_SOCKET_CLOSED:
        case PIKA_STATUS_SOCKET_WOULD_BLOCK:
        case PIKA_STATUS_SOCKET_TIMEOUT:
        case PIKA_STATUS_SOCKET_DNS_ERROR:
        case PIKA_STATUS_SOCKET_CONNECTION_REFUSED:
        case PIKA_STATUS_SOCKET_CONNECTION_RESET:
        case PIKA_STATUS_SOCKET_RECEIVE_BYTE_LIMIT:
        case PIKA_STATUS_SOCKET_HOST_BYTE_LIMIT:
        case PIKA_STATUS_SOCKET_OBJECT_LIMIT:
            return PIKA_EXCEPTION_OS_ERROR;
        default:
            return PIKA_EXCEPTION_NONE;
    }
}

const char* pika_status_exception_name(PikaStatus status) {
    switch (pika_status_exception_kind(status)) {
        case PIKA_EXCEPTION_RUNTIME_ERROR:
            return "RuntimeError";
        case PIKA_EXCEPTION_VALUE_ERROR:
            return "ValueError";
        case PIKA_EXCEPTION_TYPE_ERROR:
            return "TypeError";
        case PIKA_EXCEPTION_ZERO_DIVISION_ERROR:
            return "ZeroDivisionError";
        case PIKA_EXCEPTION_OS_ERROR:
            return "OSError";
        case PIKA_EXCEPTION_OVERFLOW_ERROR:
            return "OverflowError";
        case PIKA_EXCEPTION_EXCEPTION:
        case PIKA_EXCEPTION_ANY:
        case PIKA_EXCEPTION_NONE:
        default:
            return "Exception";
    }
}

int pika_status_matches_exception(PikaStatus status,
                                  PikaExceptionKind kind) {
    PikaExceptionKind actual;
    if (status == PIKA_STATUS_OK || kind == PIKA_EXCEPTION_NONE) {
        return 0;
    }
    if (kind == PIKA_EXCEPTION_ANY) {
        return 1;
    }
    actual = pika_status_exception_kind(status);
    if (kind == PIKA_EXCEPTION_EXCEPTION) {
        return actual != PIKA_EXCEPTION_NONE;
    }
    return actual == kind;
}
