/* SPEC: PJ2026-050109 network-ready v0.1; compiled capability selection. */
#ifndef PIKA_CAPABILITY_H
#define PIKA_CAPABILITY_H

#include "pika_capability_config.h"

#if PIKA_FROZEN_PROGRAM_ENABLE != 0 && PIKA_FROZEN_PROGRAM_ENABLE != 1
#error "PIKA_FROZEN_PROGRAM_ENABLE must be 0 or 1"
#endif

#if PIKA_FROZEN_SOURCE_ENABLE != 0 && PIKA_FROZEN_SOURCE_ENABLE != 1
#error "PIKA_FROZEN_SOURCE_ENABLE must be 0 or 1"
#endif

#define PIKA_TYPED_RUNTIME_ENABLE                                      \
    (PIKA_CAPABILITY_VALUE_NONE_ENABLE ||                              \
     PIKA_CAPABILITY_VALUE_FLOAT_ENABLE ||                             \
     PIKA_CAPABILITY_VALUE_STRING_ENABLE ||                            \
     PIKA_CAPABILITY_VALUE_BYTES_ENABLE ||                             \
     PIKA_CAPABILITY_CONTAINER_TUPLE_ENABLE ||                         \
     PIKA_CAPABILITY_CONTAINER_LIST_ENABLE ||                          \
     PIKA_CAPABILITY_CONTAINER_DICT_ENABLE ||                          \
     PIKA_CAPABILITY_CONTAINER_BYTEARRAY_ENABLE ||                     \
     PIKA_CAPABILITY_OP_NUMERIC_BASIC_ENABLE ||                        \
     PIKA_CAPABILITY_OP_COMPARE_BASIC_ENABLE ||                        \
     PIKA_CAPABILITY_OP_BITWISE_ENABLE ||                              \
     PIKA_CAPABILITY_OP_MEMBERSHIP_ENABLE ||                           \
     PIKA_CAPABILITY_OP_SUBSCRIPT_ENABLE ||                            \
     PIKA_CAPABILITY_OP_SLICE_ENABLE ||                                \
     PIKA_CAPABILITY_OP_MATRIX_MULTIPLY_ENABLE ||                      \
     PIKA_CAPABILITY_LOGIC_SHORT_CIRCUIT_ENABLE ||                     \
     PIKA_CAPABILITY_OP_COMPLETE_ENABLE ||                             \
     PIKA_CAPABILITY_NAME_GLOBAL_ENABLE ||                             \
     PIKA_CAPABILITY_ITER_RANGE_ENABLE ||                              \
     PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE ||                        \
     PIKA_CAPABILITY_BUILTIN_TYPE_ENABLE ||                            \
     PIKA_CAPABILITY_BINDING_C_ENABLE ||                               \
     PIKA_CAPABILITY_PROTOCOL_TEXT_NETWORK_ENABLE ||                   \
     PIKA_CAPABILITY_OBJECT_USER_BASIC_ENABLE ||                       \
     PIKA_CAPABILITY_EXCEPTION_BASIC_ENABLE)

#define PIKA_BINDING_RUNTIME_ENABLE                                    \
    (PIKA_CAPABILITY_BINDING_C_ENABLE ||                               \
     PIKA_CAPABILITY_BINDING_C_OBJECT_ENABLE ||                        \
     PIKA_CAPABILITY_BINDING_CONSTANTS_ENABLE ||                       \
     PIKA_CAPABILITY_BINDING_OBJECT_ARGUMENT_ENABLE ||                 \
     PIKA_CAPABILITY_BINDING_OBJECT_RESULT_ENABLE ||                   \
     PIKA_CAPABILITY_BINDING_MUTABLE_BUFFER_ENABLE ||                  \
     PIKA_CAPABILITY_BINDING_CONTAINER_ARGUMENT_ENABLE ||              \
     PIKA_CAPABILITY_BINDING_CONTAINER_RESULT_ENABLE ||                \
     PIKA_CAPABILITY_BINDING_PYTHON_CALLABLE_ENABLE)

#define PIKA_TYPED_INTEGER_RUNTIME_ENABLE (                            \
    PIKA_CAPABILITY_OP_INTEGER_ENABLE && PIKA_TYPED_RUNTIME_ENABLE)

#define PIKA_BUILTIN_RUNTIME_ENABLE                                    \
    ((PIKA_CAPABILITY_BUILTIN_PRINT_ENABLE &&                          \
      PIKA_TYPED_RUNTIME_ENABLE) ||                                    \
     PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE ||                        \
     PIKA_CAPABILITY_BUILTIN_LEN_ENABLE ||                             \
     PIKA_CAPABILITY_BUILTIN_SCALAR_CONVERT_ENABLE ||                  \
     PIKA_CAPABILITY_BUILTIN_TYPE_ENABLE ||                            \
     PIKA_CAPABILITY_BUILTIN_HELP_ENABLE ||                            \
     PIKA_CAPABILITY_BUILTIN_CONTAINER_CONSTRUCT_ENABLE ||             \
     PIKA_CAPABILITY_ITER_RANGE_ENABLE ||                              \
     PIKA_CAPABILITY_PROTOCOL_TEXT_NETWORK_ENABLE)

#define PIKA_CONTAINER_RUNTIME_ENABLE                                  \
    (PIKA_CAPABILITY_CONTAINER_TUPLE_ENABLE ||                         \
     PIKA_CAPABILITY_CONTAINER_LIST_ENABLE ||                          \
     PIKA_CAPABILITY_CONTAINER_DICT_ENABLE ||                          \
     PIKA_CAPABILITY_CONTAINER_BYTEARRAY_ENABLE)

#define PIKA_OBJECT_RUNTIME_ENABLE                                     \
    (PIKA_CONTAINER_RUNTIME_ENABLE ||                                  \
     PIKA_CAPABILITY_BUILTIN_TYPE_ENABLE ||                            \
     PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE ||                        \
     PIKA_CAPABILITY_OBJECT_USER_BASIC_ENABLE ||                       \
     PIKA_CAPABILITY_OBJECT_CLASS_ENABLE)

#define PIKA_OBJECT_BYTECODE_RUNTIME_ENABLE (                          \
    PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE ||                         \
    PIKA_CAPABILITY_OBJECT_USER_BASIC_ENABLE ||                        \
    PIKA_CAPABILITY_OBJECT_CLASS_ENABLE)

#define PIKA_TYPED_NUMERIC_RUNTIME_ENABLE (                            \
    PIKA_CAPABILITY_OP_NUMERIC_BASIC_ENABLE ||                         \
    PIKA_CAPABILITY_OP_MATRIX_MULTIPLY_ENABLE)

#define PIKA_RUNTIME_POWER_HELPER_ENABLE (                             \
    PIKA_TYPED_NUMERIC_RUNTIME_ENABLE ||                               \
    PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE)

#define PIKA_RUNTIME_MULTIPLY_HELPER_ENABLE (                          \
    PIKA_CAPABILITY_OP_INTEGER_ENABLE ||                               \
    PIKA_RUNTIME_POWER_HELPER_ENABLE)

#define PIKA_TYPED_BINARY_RUNTIME_ENABLE (                             \
    PIKA_TYPED_INTEGER_RUNTIME_ENABLE ||                               \
    PIKA_TYPED_NUMERIC_RUNTIME_ENABLE ||                               \
    PIKA_CAPABILITY_OP_COMPARE_BASIC_ENABLE ||                         \
    PIKA_CAPABILITY_OP_BITWISE_ENABLE ||                               \
    PIKA_CAPABILITY_OP_MEMBERSHIP_ENABLE)

#define PIKA_TYPED_UNARY_RUNTIME_ENABLE (                              \
    PIKA_TYPED_INTEGER_RUNTIME_ENABLE ||                               \
    PIKA_CAPABILITY_OP_NUMERIC_BASIC_ENABLE ||                         \
    PIKA_CAPABILITY_OP_BITWISE_ENABLE ||                               \
    PIKA_CAPABILITY_LOGIC_SHORT_CIRCUIT_ENABLE)

#define PIKA_TYPED_INDEX_RUNTIME_ENABLE (                              \
    PIKA_CAPABILITY_OP_SUBSCRIPT_ENABLE ||                             \
    PIKA_CAPABILITY_OP_SLICE_ENABLE)

#define PIKA_TYPED_MUTABLE_CONTAINER_RUNTIME_ENABLE (                  \
    PIKA_CAPABILITY_CONTAINER_LIST_ENABLE ||                           \
    PIKA_CAPABILITY_CONTAINER_DICT_ENABLE ||                           \
    PIKA_CAPABILITY_CONTAINER_BYTEARRAY_ENABLE)

#define PIKA_TYPED_ITERATION_RUNTIME_ENABLE (                          \
    PIKA_CAPABILITY_ITER_BUILTIN_ENABLE ||                             \
    PIKA_CAPABILITY_ITER_RANGE_ENABLE)

#define PIKA_TYPED_INTEGER_HELPER_ENABLE (                             \
    PIKA_TYPED_BINARY_RUNTIME_ENABLE ||                                \
    PIKA_TYPED_UNARY_RUNTIME_ENABLE ||                                 \
    PIKA_CAPABILITY_OP_SUBSCRIPT_ENABLE ||                             \
    PIKA_TYPED_ITERATION_RUNTIME_ENABLE)

#define PIKA_TYPED_TEXT_HELPER_ENABLE (                                \
    PIKA_TYPED_NUMERIC_RUNTIME_ENABLE ||                               \
    PIKA_CAPABILITY_OP_COMPARE_BASIC_ENABLE ||                         \
    PIKA_CAPABILITY_OP_MEMBERSHIP_ENABLE)

#endif
