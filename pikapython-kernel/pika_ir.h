/* SPEC: PJ2026-0501 V2 kernel v0.20; bytecode contract. */
#ifndef PIKA_IR_H
#define PIKA_IR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PIKA_PROGRAM_FORMAT_VERSION 13u
#define PIKA_TYPE_NAME_ATTRIBUTE 65535u

typedef enum {
    PIKA_OP_CONSTANT = 0,
    PIKA_OP_MOVE = 1,
    PIKA_OP_ADD = 2,
    PIKA_OP_SUBTRACT = 3,
    PIKA_OP_MULTIPLY = 4,
    PIKA_OP_MODULO = 5,
    PIKA_OP_LESS_THAN = 6,
    PIKA_OP_EQUAL = 7,
    PIKA_OP_JUMP = 8,
    PIKA_OP_BRANCH_FALSE = 9,
    PIKA_OP_CALL = 10,
    PIKA_OP_RETURN = 11,
    PIKA_OP_PRINT = 12,
    PIKA_OP_BIND_MOVE = 13,
    PIKA_OP_BIND_CALL = 14,
    PIKA_OP_DIVIDE = 15,
    PIKA_OP_FLOOR_DIVIDE = 16,
    PIKA_OP_POWER = 17,
    PIKA_OP_LESS_EQUAL = 18,
    PIKA_OP_GREATER_THAN = 19,
    PIKA_OP_GREATER_EQUAL = 20,
    PIKA_OP_NOT_EQUAL = 21,
    PIKA_OP_BITWISE_AND = 22,
    PIKA_OP_BITWISE_XOR = 23,
    PIKA_OP_BITWISE_OR = 24,
    PIKA_OP_SHIFT_LEFT = 25,
    PIKA_OP_SHIFT_RIGHT = 26,
    PIKA_OP_BRANCH_TRUE = 27,
    PIKA_OP_LOGICAL_NOT = 28,
    PIKA_OP_UNARY_PLUS = 29,
    PIKA_OP_UNARY_MINUS = 30,
    PIKA_OP_BITWISE_NOT = 31,
    PIKA_OP_BUILD_LIST = 32,
    PIKA_OP_BUILD_TUPLE = 33,
    PIKA_OP_BUILTIN_CALL = 34,
    PIKA_OP_SUBSCRIPT = 35,
    PIKA_OP_SLICE = 36,
    PIKA_OP_CONTAINS = 37,
    PIKA_OP_IDENTITY = 38,
    PIKA_OP_LOAD_CONSTANT = 39,
    PIKA_OP_MATRIX_MULTIPLY = 40,
    PIKA_OP_SELECT = 41,
    PIKA_OP_SET_SUBSCRIPT = 42,
    PIKA_OP_APPEND = 43,
    PIKA_OP_POP = 44,
    PIKA_OP_ITERATE = 45,
    PIKA_OP_NEW_INSTANCE = 46,
    PIKA_OP_LOAD_ATTRIBUTE = 47,
    PIKA_OP_STORE_ATTRIBUTE = 48,
    PIKA_OP_RAISE = 49,
    PIKA_OP_LOAD_CALLABLE = 50,
    PIKA_OP_LOAD_GLOBAL = 51,
    PIKA_OP_STORE_GLOBAL = 52,
    PIKA_OP_UNPACK_SEQUENCE = 53,
    PIKA_OP_BUILD_DICT = 54,
    PIKA_OP_CALL_DYNAMIC_METHOD = 55,
    PIKA_OP_CALL_CALLABLE = 56,
} PikaOpcode;

typedef enum {
    PIKA_PRINT_INTEGER = 0,
    PIKA_PRINT_BOOLEAN = 1,
    PIKA_PRINT_VALUE = 2,
} PikaPrintKind;

typedef struct {
    uint8_t opcode;
    uint8_t a;
    uint8_t b;
    uint8_t c;
    int32_t immediate;
} PikaInstruction;

typedef struct {
    uint32_t first_instruction;
    uint32_t instruction_count;
    uint16_t slot_count;
    uint8_t parameter_count;
    uint8_t reserved;
} PikaFunction;

typedef struct {
    uint16_t field_count;
    uint16_t reserved;
} PikaClassLayout;

#ifdef __cplusplus
}
#endif

#endif
