/* SPEC: PJ2026-0501 V2 kernel v0.47; source frontend API. */
#ifndef PIKA_FRONTEND_H
#define PIKA_FRONTEND_H

#include <stddef.h>
#include <stdint.h>
#include "pika_binding.h"
#include "pika_capability.h"
#include "pika_frontend_config.h"
#include "pika_program.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PIKA_TOKEN_EOF = 0,
    PIKA_TOKEN_NEWLINE = 1,
    PIKA_TOKEN_INDENT = 2,
    PIKA_TOKEN_DEDENT = 3,
    PIKA_TOKEN_INTEGER = 4,
    PIKA_TOKEN_NAME = 5,
    PIKA_TOKEN_IF = 6,
    PIKA_TOKEN_ELSE = 7,
    PIKA_TOKEN_PRINT = 8,
    PIKA_TOKEN_PLUS = 9,
    PIKA_TOKEN_MINUS = 10,
    PIKA_TOKEN_STAR = 11,
    PIKA_TOKEN_PERCENT = 12,
    PIKA_TOKEN_LESS = 13,
    PIKA_TOKEN_EQUAL_EQUAL = 14,
    PIKA_TOKEN_ASSIGN = 15,
    PIKA_TOKEN_COLON = 16,
    PIKA_TOKEN_LEFT_PAREN = 17,
    PIKA_TOKEN_RIGHT_PAREN = 18,
    PIKA_TOKEN_DEF = 19,
    PIKA_TOKEN_RETURN = 20,
    PIKA_TOKEN_WHILE = 21,
    PIKA_TOKEN_FOR = 22,
    PIKA_TOKEN_IN = 23,
    PIKA_TOKEN_RANGE = 24,
    PIKA_TOKEN_BREAK = 25,
    PIKA_TOKEN_CONTINUE = 26,
    PIKA_TOKEN_COMMA = 27,
    PIKA_TOKEN_IMPORT = 28,
    PIKA_TOKEN_DOT = 29,
    PIKA_TOKEN_FLOAT = 30,
    PIKA_TOKEN_STRING = 31,
    PIKA_TOKEN_TRUE = 32,
    PIKA_TOKEN_FALSE = 33,
    PIKA_TOKEN_NONE = 34,
    PIKA_TOKEN_SLASH = 35,
    PIKA_TOKEN_DOUBLE_SLASH = 36,
    PIKA_TOKEN_DOUBLE_STAR = 37,
    PIKA_TOKEN_AT = 38,
    PIKA_TOKEN_LESS_EQUAL = 39,
    PIKA_TOKEN_GREATER = 40,
    PIKA_TOKEN_GREATER_EQUAL = 41,
    PIKA_TOKEN_NOT_EQUAL = 42,
    PIKA_TOKEN_AMPERSAND = 43,
    PIKA_TOKEN_CARET = 44,
    PIKA_TOKEN_PIPE = 45,
    PIKA_TOKEN_TILDE = 46,
    PIKA_TOKEN_LEFT_SHIFT = 47,
    PIKA_TOKEN_RIGHT_SHIFT = 48,
    PIKA_TOKEN_LEFT_BRACKET = 49,
    PIKA_TOKEN_RIGHT_BRACKET = 50,
    PIKA_TOKEN_PLUS_ASSIGN = 51,
    PIKA_TOKEN_MINUS_ASSIGN = 52,
    PIKA_TOKEN_STAR_ASSIGN = 53,
    PIKA_TOKEN_SLASH_ASSIGN = 54,
    PIKA_TOKEN_DOUBLE_SLASH_ASSIGN = 55,
    PIKA_TOKEN_PERCENT_ASSIGN = 56,
    PIKA_TOKEN_DOUBLE_STAR_ASSIGN = 57,
    PIKA_TOKEN_AMPERSAND_ASSIGN = 58,
    PIKA_TOKEN_CARET_ASSIGN = 59,
    PIKA_TOKEN_PIPE_ASSIGN = 60,
    PIKA_TOKEN_LEFT_SHIFT_ASSIGN = 61,
    PIKA_TOKEN_RIGHT_SHIFT_ASSIGN = 62,
    PIKA_TOKEN_AND = 63,
    PIKA_TOKEN_OR = 64,
    PIKA_TOKEN_NOT = 65,
    PIKA_TOKEN_IS = 66,
    PIKA_TOKEN_AT_ASSIGN = 67,
    PIKA_TOKEN_LEFT_BRACE = 68,
    PIKA_TOKEN_RIGHT_BRACE = 69,
    PIKA_TOKEN_BYTES = 70,
    PIKA_TOKEN_ELIF = 71,
    PIKA_TOKEN_PASS = 72,
    PIKA_TOKEN_CLASS = 73,
    PIKA_TOKEN_FROM = 74,
    PIKA_TOKEN_TRY = 75,
    PIKA_TOKEN_RAISE = 76,
    PIKA_TOKEN_EXCEPT = 77,
    PIKA_TOKEN_FINALLY = 78,
    PIKA_TOKEN_GLOBAL = 79,
    PIKA_TOKEN_DEL = 80,
    PIKA_TOKEN_ASSERT = 81,
    PIKA_TOKEN_LAMBDA = 82,
    PIKA_TOKEN_KIND_COUNT = 83,
} PikaTokenKind;

typedef char PikaTokenKindStorageCheck[
    PIKA_TOKEN_KIND_COUNT <= 256u ? 1 : -1];

typedef struct {
    uint32_t offset;
    uint16_t length;
    uint8_t kind;
    uint8_t reserved;
} PikaToken;

int64_t pika_token_integer_value(const char* source,
                                 const PikaToken* token);
PikaStatus pika_token_float_value(const char* source,
                                  const PikaToken* token,
                                  double* value);

typedef struct {
    /* NULL with zero capacity performs a count-only tokenization pass. */
    PikaToken* tokens;
    uint32_t capacity;
    uint32_t count;
    PikaTokenKind last_kind;
} PikaTokenBuffer;

typedef enum {
    PIKA_DIAGNOSTIC_REASON_NONE = 0,
    PIKA_DIAGNOSTIC_REASON_LIMIT_EXCEEDED = 1,
    PIKA_DIAGNOSTIC_REASON_NUL_BYTE = 2,
    PIKA_DIAGNOSTIC_REASON_INVALID_UTF8 = 3,
    PIKA_DIAGNOSTIC_REASON_UNSUPPORTED_CHARACTER = 4,
    PIKA_DIAGNOSTIC_REASON_UNSUPPORTED_IDENTIFIER = 5,
    PIKA_DIAGNOSTIC_REASON_INTEGER_LITERAL_RANGE = 6,
    PIKA_DIAGNOSTIC_REASON_INCOMPLETE_INPUT = 7,
} PikaDiagnosticReason;

typedef enum {
    PIKA_FRONTEND_BOUND_NONE = 0,
    PIKA_FRONTEND_BOUND_SOURCE_BYTES = 1,
    PIKA_FRONTEND_BOUND_TOKENS = 2,
    PIKA_FRONTEND_BOUND_IDENTIFIER_BYTES = 3,
    PIKA_FRONTEND_BOUND_NUMERIC_LITERAL_BYTES = 4,
    PIKA_FRONTEND_BOUND_LITERAL_BYTES = 5,
    PIKA_FRONTEND_BOUND_PARAMETERS = 6,
    PIKA_FRONTEND_BOUND_ARGUMENTS = 7,
    PIKA_FRONTEND_BOUND_COLLECTION_ELEMENTS = 8,
    PIKA_FRONTEND_BOUND_SYMBOLS = 9,
    PIKA_FRONTEND_BOUND_EXPRESSION_DEPTH = 10,
    PIKA_FRONTEND_BOUND_BLOCK_DEPTH = 11,
    PIKA_FRONTEND_BOUND_LOOPS = 12,
    PIKA_FRONTEND_BOUND_CALL_DEPTH = 13,
    PIKA_FRONTEND_BOUND_FUNCTION_INSTRUCTIONS = 14,
    PIKA_FRONTEND_BOUND_FUNCTIONS = 15,
    PIKA_FRONTEND_BOUND_CLASSES = 16,
    PIKA_FRONTEND_BOUND_CLASS_MEMBERS = 17,
    PIKA_FRONTEND_BOUND_FIELDS = 18,
    PIKA_FRONTEND_BOUND_MODULES = 19,
    PIKA_FRONTEND_BOUND_EXCEPTION_HANDLERS = 20,
    PIKA_FRONTEND_BOUND_TOKEN_BYTES = 21,
} PikaFrontendBound;

#define PIKA_DIAGNOSTIC_SOURCE_NAME_LIMIT 96u

typedef struct {
    PikaStatus status;
    uint32_t line;
    uint32_t column;
    uint32_t offset;
    PikaDiagnosticReason reason;
    PikaFrontendBound bound;
    uint32_t configured;
    uint32_t observed;
    char source_name[PIKA_DIAGNOSTIC_SOURCE_NAME_LIMIT];
} PikaDiagnostic;

const char* pika_frontend_diagnostic_reason_name(
    PikaDiagnosticReason reason);
const char* pika_frontend_bound_macro(PikaFrontendBound bound);

typedef uint64_t PikaCapability;

#define PIKA_CAPABILITY_EXEC_MODULE (UINT64_C(1) << 0)
#define PIKA_CAPABILITY_VALUE_INTEGER (UINT64_C(1) << 1)
#define PIKA_CAPABILITY_VALUE_BOOLEAN (UINT64_C(1) << 2)
#define PIKA_CAPABILITY_NAME_LOCAL (UINT64_C(1) << 3)
#define PIKA_CAPABILITY_INTEGER_OPERATOR (UINT64_C(1) << 4)
#define PIKA_CAPABILITY_TRUTH_PROTOCOL (UINT64_C(1) << 5)
#define PIKA_CAPABILITY_BRANCH_FLOW (UINT64_C(1) << 6)
#define PIKA_CAPABILITY_LOOP_FLOW (UINT64_C(1) << 7)
#define PIKA_CAPABILITY_POSITIONAL_CALL (UINT64_C(1) << 8)
#define PIKA_CAPABILITY_RANGE_ITERATION (UINT64_C(1) << 9)
#define PIKA_CAPABILITY_PRINT_BUILTIN (UINT64_C(1) << 10)
#define PIKA_CAPABILITY_VALUE_NONE (UINT64_C(1) << 11)
#define PIKA_CAPABILITY_VALUE_FLOAT (UINT64_C(1) << 12)
#define PIKA_CAPABILITY_VALUE_STRING (UINT64_C(1) << 13)
#define PIKA_CAPABILITY_VALUE_BYTES (UINT64_C(1) << 14)
#define PIKA_CAPABILITY_NAME_GLOBAL (UINT64_C(1) << 15)
#define PIKA_CAPABILITY_MODULE_IMPORT (UINT64_C(1) << 16)
#define PIKA_CAPABILITY_C_BINDING (UINT64_C(1) << 17)
#define PIKA_CAPABILITY_OBJECT_ATTRIBUTE (UINT64_C(1) << 18)
#define PIKA_CAPABILITY_C_OBJECT_BINDING (UINT64_C(1) << 19)
#define PIKA_CAPABILITY_COMPLETE_OPERATOR (UINT64_C(1) << 20)
#define PIKA_CAPABILITY_EMBEDDED_BUILTIN (UINT64_C(1) << 21)
#define PIKA_CAPABILITY_VALUE_CONTAINER (UINT64_C(1) << 22)
#define PIKA_CAPABILITY_BUILTIN_ITERATION (UINT64_C(1) << 23)
#define PIKA_CAPABILITY_PYTHON_MODULE (UINT64_C(1) << 24)
#define PIKA_CAPABILITY_OBJECT_CLASS (UINT64_C(1) << 25)
#define PIKA_CAPABILITY_EMBEDDED_INTEGER (UINT64_C(1) << 26)
#define PIKA_CAPABILITY_DEFAULT_CALL (UINT64_C(1) << 27)
#define PIKA_CAPABILITY_KEYWORD_CALL (UINT64_C(1) << 28)
#define PIKA_CAPABILITY_FROM_IMPORT (UINT64_C(1) << 29)
#define PIKA_CAPABILITY_BINDING_CONSTANT (UINT64_C(1) << 30)
#define PIKA_CAPABILITY_EXCEPTION_BASIC (UINT64_C(1) << 31)
#define PIKA_CAPABILITY_EXCEPTION_FINALLY (UINT64_C(1) << 32)
#define PIKA_CAPABILITY_PYTHON_CALLBACK (UINT64_C(1) << 33)
#define PIKA_CAPABILITY_CALL_REFERENCE (UINT64_C(1) << 34)
#define PIKA_CAPABILITY_CONTAINER_TUPLE (UINT64_C(1) << 35)
#define PIKA_CAPABILITY_CONTAINER_LIST (UINT64_C(1) << 36)
#define PIKA_CAPABILITY_CONTAINER_DICT (UINT64_C(1) << 37)
#define PIKA_CAPABILITY_CONTAINER_BYTEARRAY (UINT64_C(1) << 38)
#define PIKA_CAPABILITY_NUMERIC_OPERATOR (UINT64_C(1) << 39)
#define PIKA_CAPABILITY_COMPARISON_OPERATOR (UINT64_C(1) << 40)
#define PIKA_CAPABILITY_BITWISE_OPERATOR (UINT64_C(1) << 41)
#define PIKA_CAPABILITY_MEMBERSHIP_OPERATOR (UINT64_C(1) << 42)
#define PIKA_CAPABILITY_SUBSCRIPT_OPERATOR (UINT64_C(1) << 43)
#define PIKA_CAPABILITY_SLICE_OPERATOR (UINT64_C(1) << 44)
#define PIKA_CAPABILITY_AUGMENTED_OPERATOR (UINT64_C(1) << 45)
#define PIKA_CAPABILITY_MATRIX_OPERATOR (UINT64_C(1) << 46)
#define PIKA_CAPABILITY_SHORT_CIRCUIT_LOGIC (UINT64_C(1) << 47)
#define PIKA_CAPABILITY_PASS_FLOW (UINT64_C(1) << 48)
#define PIKA_CAPABILITY_GLOBAL_STATEMENT (UINT64_C(1) << 49)
#define PIKA_CAPABILITY_LEN_BUILTIN (UINT64_C(1) << 50)
#define PIKA_CAPABILITY_SCALAR_CONVERT_BUILTIN (UINT64_C(1) << 51)
#define PIKA_CAPABILITY_CONTAINER_CONSTRUCT_BUILTIN (UINT64_C(1) << 52)
#define PIKA_CAPABILITY_TEXT_NETWORK (UINT64_C(1) << 53)
#define PIKA_CAPABILITY_BINDING_OBJECT_RESULT (UINT64_C(1) << 54)
#define PIKA_CAPABILITY_ASSIGNMENT_UNPACK (UINT64_C(1) << 55)
#define PIKA_CAPABILITY_TYPE_BUILTIN (UINT64_C(1) << 56)
#define PIKA_CAPABILITY_VARIADIC_CALL (UINT64_C(1) << 57)

#define PIKA_CAPABILITY_MASK(enabled, capability) \
    ((enabled) ? (capability) : UINT64_C(0))

#define PIKA_CAPABILITY_PROVIDED                                        \
    (PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_EXEC_MODULE_ENABLE,           \
                          PIKA_CAPABILITY_EXEC_MODULE) |                 \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_VALUE_INT_ENABLE,             \
                          PIKA_CAPABILITY_VALUE_INTEGER) |               \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_VALUE_BOOL_ENABLE,            \
                          PIKA_CAPABILITY_VALUE_BOOLEAN) |               \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_NAME_LOCAL_ENABLE,            \
                          PIKA_CAPABILITY_NAME_LOCAL) |                  \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_OP_INTEGER_ENABLE,            \
                          PIKA_CAPABILITY_INTEGER_OPERATOR) |            \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_PROTOCOL_TRUTH_ENABLE,        \
                          PIKA_CAPABILITY_TRUTH_PROTOCOL) |              \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_FLOW_BRANCH_ENABLE,           \
                          PIKA_CAPABILITY_BRANCH_FLOW) |                 \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_FLOW_LOOP_ENABLE,             \
                          PIKA_CAPABILITY_LOOP_FLOW) |                   \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_CALL_POSITIONAL_ENABLE,       \
                          PIKA_CAPABILITY_POSITIONAL_CALL) |             \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_ITER_RANGE_ENABLE,            \
                          PIKA_CAPABILITY_RANGE_ITERATION) |             \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_BUILTIN_PRINT_ENABLE,         \
                          PIKA_CAPABILITY_PRINT_BUILTIN) |               \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_VALUE_NONE_ENABLE,            \
                          PIKA_CAPABILITY_VALUE_NONE) |                  \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_VALUE_FLOAT_ENABLE,           \
                          PIKA_CAPABILITY_VALUE_FLOAT) |                 \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_VALUE_STRING_ENABLE,          \
                          PIKA_CAPABILITY_VALUE_STRING) |                \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_VALUE_BYTES_ENABLE,           \
                          PIKA_CAPABILITY_VALUE_BYTES) |                 \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_NAME_GLOBAL_ENABLE,           \
                          PIKA_CAPABILITY_NAME_GLOBAL) |                 \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_MODULE_IMPORT_ENABLE,         \
                          PIKA_CAPABILITY_MODULE_IMPORT) |               \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_BINDING_C_ENABLE,             \
                          PIKA_CAPABILITY_C_BINDING) |                   \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE,      \
                          PIKA_CAPABILITY_OBJECT_ATTRIBUTE) |            \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_BINDING_C_OBJECT_ENABLE,      \
                          PIKA_CAPABILITY_C_OBJECT_BINDING) |            \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_OP_COMPLETE_ENABLE,           \
                          PIKA_CAPABILITY_COMPLETE_OPERATOR) |           \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE,      \
                          PIKA_CAPABILITY_EMBEDDED_BUILTIN) |            \
     PIKA_CAPABILITY_MASK(PIKA_CONTAINER_RUNTIME_ENABLE,                \
                          PIKA_CAPABILITY_VALUE_CONTAINER) |             \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_ITER_BUILTIN_ENABLE,          \
                          PIKA_CAPABILITY_BUILTIN_ITERATION) |           \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_MODULE_PYTHON_ENABLE,         \
                          PIKA_CAPABILITY_PYTHON_MODULE) |               \
     PIKA_CAPABILITY_MASK(                                              \
         PIKA_CAPABILITY_OBJECT_USER_BASIC_ENABLE ||                    \
             PIKA_CAPABILITY_OBJECT_CLASS_ENABLE,                       \
         PIKA_CAPABILITY_OBJECT_CLASS) |                                \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_LITERAL_EMBEDDED_INT_ENABLE,  \
                          PIKA_CAPABILITY_EMBEDDED_INTEGER) |            \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_CALL_DEFAULTS_ENABLE,         \
                          PIKA_CAPABILITY_DEFAULT_CALL) |                \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_CALL_KEYWORD_ENABLE,          \
                          PIKA_CAPABILITY_KEYWORD_CALL) |                \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_MODULE_FROM_IMPORT_ENABLE,    \
                          PIKA_CAPABILITY_FROM_IMPORT) |                 \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_BINDING_CONSTANTS_ENABLE,     \
                          PIKA_CAPABILITY_BINDING_CONSTANT) |            \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_EXCEPTION_BASIC_ENABLE,       \
                          PIKA_CAPABILITY_EXCEPTION_BASIC) |             \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_EXCEPTION_FINALLY_ENABLE,     \
                          PIKA_CAPABILITY_EXCEPTION_FINALLY) |           \
     PIKA_CAPABILITY_MASK(                                              \
         PIKA_CAPABILITY_BINDING_PYTHON_CALLABLE_ENABLE,                \
         PIKA_CAPABILITY_PYTHON_CALLBACK) |                             \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_CALL_REFERENCE_ENABLE,       \
                          PIKA_CAPABILITY_CALL_REFERENCE) |             \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_CONTAINER_TUPLE_ENABLE,      \
                          PIKA_CAPABILITY_CONTAINER_TUPLE) |            \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_CONTAINER_LIST_ENABLE,       \
                          PIKA_CAPABILITY_CONTAINER_LIST) |             \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_CONTAINER_DICT_ENABLE,       \
                          PIKA_CAPABILITY_CONTAINER_DICT) |             \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_CONTAINER_BYTEARRAY_ENABLE,  \
                          PIKA_CAPABILITY_CONTAINER_BYTEARRAY) |        \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_OP_NUMERIC_BASIC_ENABLE,     \
                          PIKA_CAPABILITY_NUMERIC_OPERATOR) |           \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_OP_COMPARE_BASIC_ENABLE,     \
                          PIKA_CAPABILITY_COMPARISON_OPERATOR) |        \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_OP_BITWISE_ENABLE,           \
                          PIKA_CAPABILITY_BITWISE_OPERATOR) |           \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_OP_MEMBERSHIP_ENABLE,        \
                          PIKA_CAPABILITY_MEMBERSHIP_OPERATOR) |        \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_OP_SUBSCRIPT_ENABLE,         \
                          PIKA_CAPABILITY_SUBSCRIPT_OPERATOR) |         \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_OP_SLICE_ENABLE,             \
                          PIKA_CAPABILITY_SLICE_OPERATOR) |             \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_OP_AUGMENTED_ENABLE,         \
                          PIKA_CAPABILITY_AUGMENTED_OPERATOR) |         \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_OP_MATRIX_MULTIPLY_ENABLE,   \
                          PIKA_CAPABILITY_MATRIX_OPERATOR) |            \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_LOGIC_SHORT_CIRCUIT_ENABLE,  \
                          PIKA_CAPABILITY_SHORT_CIRCUIT_LOGIC) |        \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_FLOW_PASS_ENABLE,            \
                          PIKA_CAPABILITY_PASS_FLOW) |                  \
     PIKA_CAPABILITY_MASK(                                             \
         PIKA_CAPABILITY_SCOPE_GLOBAL_STATEMENT_ENABLE,                \
         PIKA_CAPABILITY_GLOBAL_STATEMENT) |                           \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_BUILTIN_LEN_ENABLE,          \
                          PIKA_CAPABILITY_LEN_BUILTIN) |                \
     PIKA_CAPABILITY_MASK(                                             \
         PIKA_CAPABILITY_BUILTIN_SCALAR_CONVERT_ENABLE,                \
         PIKA_CAPABILITY_SCALAR_CONVERT_BUILTIN) |                     \
     PIKA_CAPABILITY_MASK(                                             \
         PIKA_CAPABILITY_BUILTIN_CONTAINER_CONSTRUCT_ENABLE,           \
         PIKA_CAPABILITY_CONTAINER_CONSTRUCT_BUILTIN) |                \
     PIKA_CAPABILITY_MASK(                                             \
         PIKA_CAPABILITY_PROTOCOL_TEXT_NETWORK_ENABLE,                 \
         PIKA_CAPABILITY_TEXT_NETWORK) |                               \
     PIKA_CAPABILITY_MASK(                                             \
         PIKA_CAPABILITY_BINDING_OBJECT_RESULT_ENABLE,                 \
         PIKA_CAPABILITY_BINDING_OBJECT_RESULT) |                      \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_ASSIGNMENT_UNPACK_ENABLE,    \
                          PIKA_CAPABILITY_ASSIGNMENT_UNPACK) |         \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_BUILTIN_TYPE_ENABLE,        \
                          PIKA_CAPABILITY_TYPE_BUILTIN) |             \
     PIKA_CAPABILITY_MASK(PIKA_CAPABILITY_CALL_VARIADIC_ENABLE,      \
                          PIKA_CAPABILITY_VARIADIC_CALL))

#define PIKA_CAPABILITY_DEFAULT                                        \
    (PIKA_CAPABILITY_PROVIDED &                                       \
     (PIKA_CAPABILITY_EXEC_MODULE | PIKA_CAPABILITY_VALUE_INTEGER |   \
      PIKA_CAPABILITY_VALUE_BOOLEAN | PIKA_CAPABILITY_NAME_LOCAL |    \
      PIKA_CAPABILITY_INTEGER_OPERATOR |                              \
      PIKA_CAPABILITY_TRUTH_PROTOCOL | PIKA_CAPABILITY_BRANCH_FLOW |  \
      PIKA_CAPABILITY_LOOP_FLOW | PIKA_CAPABILITY_POSITIONAL_CALL |   \
      PIKA_CAPABILITY_RANGE_ITERATION |                               \
      PIKA_CAPABILITY_PRINT_BUILTIN |                                 \
      PIKA_CAPABILITY_COMPLETE_OPERATOR |                             \
      PIKA_CAPABILITY_EMBEDDED_BUILTIN |                              \
      PIKA_CAPABILITY_VALUE_CONTAINER |                               \
      PIKA_CAPABILITY_BUILTIN_ITERATION |                             \
      PIKA_CAPABILITY_VALUE_NONE | PIKA_CAPABILITY_VALUE_FLOAT |      \
      PIKA_CAPABILITY_VALUE_STRING | PIKA_CAPABILITY_VALUE_BYTES |    \
      PIKA_CAPABILITY_EMBEDDED_INTEGER |                              \
      PIKA_CAPABILITY_DEFAULT_CALL | PIKA_CAPABILITY_KEYWORD_CALL |   \
      PIKA_CAPABILITY_EXCEPTION_BASIC |                               \
      PIKA_CAPABILITY_EXCEPTION_FINALLY |                             \
      PIKA_CAPABILITY_CALL_REFERENCE |                               \
      PIKA_CAPABILITY_VARIADIC_CALL |                                \
      PIKA_CAPABILITY_CONTAINER_TUPLE |                              \
      PIKA_CAPABILITY_CONTAINER_LIST |                               \
      PIKA_CAPABILITY_CONTAINER_DICT |                               \
      PIKA_CAPABILITY_CONTAINER_BYTEARRAY |                          \
      PIKA_CAPABILITY_NUMERIC_OPERATOR |                             \
      PIKA_CAPABILITY_COMPARISON_OPERATOR |                          \
      PIKA_CAPABILITY_BITWISE_OPERATOR |                             \
      PIKA_CAPABILITY_MEMBERSHIP_OPERATOR |                          \
      PIKA_CAPABILITY_SUBSCRIPT_OPERATOR |                           \
      PIKA_CAPABILITY_SLICE_OPERATOR |                               \
      PIKA_CAPABILITY_AUGMENTED_OPERATOR |                           \
      PIKA_CAPABILITY_MATRIX_OPERATOR |                              \
      PIKA_CAPABILITY_SHORT_CIRCUIT_LOGIC |                          \
      PIKA_CAPABILITY_PASS_FLOW | PIKA_CAPABILITY_LEN_BUILTIN |      \
      PIKA_CAPABILITY_SCALAR_CONVERT_BUILTIN |                       \
      PIKA_CAPABILITY_TYPE_BUILTIN |                                 \
      PIKA_CAPABILITY_CONTAINER_CONSTRUCT_BUILTIN |                   \
      PIKA_CAPABILITY_TEXT_NETWORK |                                  \
      PIKA_CAPABILITY_ASSIGNMENT_UNPACK))

#define PIKA_CAPABILITY_APPLICATION_DEFAULT                            \
    (PIKA_CAPABILITY_PROVIDED &                                       \
     (PIKA_CAPABILITY_DEFAULT | PIKA_CAPABILITY_NAME_GLOBAL |         \
      PIKA_CAPABILITY_MODULE_IMPORT |                                 \
      PIKA_CAPABILITY_OBJECT_ATTRIBUTE |                              \
      PIKA_CAPABILITY_OBJECT_CLASS |                                  \
      PIKA_CAPABILITY_PYTHON_MODULE | PIKA_CAPABILITY_FROM_IMPORT |   \
      PIKA_CAPABILITY_GLOBAL_STATEMENT))

#define PIKA_CAPABILITY_BINDING_DEFAULT                                \
    (PIKA_CAPABILITY_PROVIDED &                                       \
     (PIKA_CAPABILITY_APPLICATION_DEFAULT | PIKA_CAPABILITY_C_BINDING | \
      PIKA_CAPABILITY_C_OBJECT_BINDING |                              \
      PIKA_CAPABILITY_BINDING_CONSTANT |                              \
      PIKA_CAPABILITY_BINDING_OBJECT_RESULT |                         \
      PIKA_CAPABILITY_PYTHON_CALLBACK))
#define PIKA_CAPABILITY_ALL PIKA_CAPABILITY_PROVIDED

typedef struct {
    PikaCapability enabled;
    const PikaBindingRegistry* bindings;
} PikaFrontendConfig;

typedef struct PikaCompiledModule PikaCompiledModule;
typedef PikaStatus (*PikaModuleCommitCheck)(
    void* context,
    const PikaCompiledModule* module);

typedef enum {
    PIKA_INTERACTIVE_COMPILE_COMPLETE = 0,
    PIKA_INTERACTIVE_COMPILE_INCOMPLETE = 1,
    PIKA_INTERACTIVE_COMPILE_INVALID = 2,
} PikaInteractiveCompileState;

typedef PikaStatus (*PikaModuleSourceLoad)(
    void* context,
    const char* module_name,
    const char** source,
    size_t* length);
typedef void (*PikaModuleSourceRelease)(void* context,
                                        const char* source);

typedef struct {
    PikaModuleSourceLoad load;
    PikaModuleSourceRelease release;
    void* context;
} PikaModuleProvider;

PikaStatus pika_source_tokenize(const char* source,
                                size_t length,
                                PikaTokenBuffer* buffer,
                                PikaDiagnostic* diagnostic);
PikaStatus pika_module_compile(const char* source,
                               size_t length,
                               PikaCompiledModule** module,
                               PikaDiagnostic* diagnostic);
PikaStatus pika_module_compile_configured(
    const char* source,
    size_t length,
    const PikaFrontendConfig* config,
    PikaCompiledModule** module,
    PikaDiagnostic* diagnostic);
PikaStatus pika_module_compile_incremental(
    const PikaCompiledModule* base,
    const char* source,
    size_t length,
    const PikaFrontendConfig* config,
    PikaCompiledModule** module,
    PikaDiagnostic* diagnostic);
PikaStatus pika_module_compile_incremental_interactive(
    const PikaCompiledModule* base,
    const char* source,
    size_t length,
    const PikaFrontendConfig* config,
    PikaCompiledModule** module,
    PikaInteractiveCompileState* state,
    PikaDiagnostic* diagnostic);
PikaStatus pika_module_compile_incremental_owned_interactive(
    PikaCompiledModule* base,
    const char* source,
    size_t length,
    const PikaFrontendConfig* config,
    PikaModuleCommitCheck commit_check,
    void* commit_context,
    PikaCompiledModule** module,
    PikaInteractiveCompileState* state,
    PikaDiagnostic* diagnostic);
PikaStatus pika_module_graph_compile(
    const char* entry_module,
    const PikaModuleProvider* provider,
    const PikaFrontendConfig* config,
    PikaCompiledModule** module,
    PikaDiagnostic* diagnostic);
PikaStatus pika_module_graph_compile_image(
    const char* entry_module,
    const char* const* module_names,
    size_t module_count,
    const PikaModuleProvider* provider,
    const PikaFrontendConfig* config,
    PikaCompiledModule** module,
    PikaDiagnostic* diagnostic);
PikaStatus pika_module_graph_compile_incremental_interactive(
    const PikaCompiledModule* base,
    const char* source,
    size_t length,
    const PikaModuleProvider* provider,
    const PikaFrontendConfig* config,
    PikaCompiledModule** module,
    PikaInteractiveCompileState* state,
    PikaDiagnostic* diagnostic);
PikaStatus pika_module_graph_compile_incremental_owned_interactive(
    PikaCompiledModule* base,
    const char* source,
    size_t length,
    const PikaModuleProvider* provider,
    const PikaFrontendConfig* config,
    PikaModuleCommitCheck commit_check,
    void* commit_context,
    PikaCompiledModule** module,
    PikaInteractiveCompileState* state,
    PikaDiagnostic* diagnostic);
const PikaProgram* pika_compiled_module_program(
    const PikaCompiledModule* module);
uint16_t pika_compiled_module_global_count(
    const PikaCompiledModule* module);
int pika_compiled_module_has_interactive_result(
    const PikaCompiledModule* module);
PikaStatus pika_compiled_module_release_interactive_entry(
    PikaCompiledModule* module);
void pika_compiled_module_destroy(PikaCompiledModule* module);

#ifdef __cplusplus
}
#endif

#endif
