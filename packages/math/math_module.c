/* SPEC: PJ2026-050111 portable packages v0.2; portable math binding. */
#include "_math_binding.h"
#include "pika_capability_config.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>

#define PIKA_MATH_PI 3.14159265358979323846
#define PIKA_MATH_INT64_UPPER 9223372036854775808.0

#if PIKA_CAPABILITY_LIBRARY_MATH_ENABLE

typedef enum {
    PIKA_MATH_FABS = 0,
    PIKA_MATH_EXP,
    PIKA_MATH_LOG,
    PIKA_MATH_LOG2,
    PIKA_MATH_LOG10,
    PIKA_MATH_SQRT,
    PIKA_MATH_ACOS,
    PIKA_MATH_ASIN,
    PIKA_MATH_ATAN,
    PIKA_MATH_COS,
    PIKA_MATH_SIN,
    PIKA_MATH_TAN,
    PIKA_MATH_DEGREES,
    PIKA_MATH_RADIANS,
    PIKA_MATH_COSH,
    PIKA_MATH_SINH,
    PIKA_MATH_TANH
} PikaMathUnaryOperation;

typedef enum {
    PIKA_MATH_FMOD = 0,
    PIKA_MATH_REMAINDER,
    PIKA_MATH_POW,
    PIKA_MATH_ATAN2
} PikaMathBinaryOperation;

static PikaStatus math_validate(const PikaBindingCall* call,
                                PikaBindingValue* result,
                                uint8_t count) {
    if (call == NULL || result == NULL || call->argument_count != count ||
        (count > 0u && call->arguments == NULL)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus math_number(const PikaBindingValue* value,
                              double* result) {
    if (value == NULL || result == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (value->kind == PIKA_BINDING_VALUE_INTEGER) {
        *result = (double)value->as.integer;
        return PIKA_STATUS_OK;
    }
    if (value->kind == PIKA_BINDING_VALUE_BOOLEAN) {
        *result = value->as.boolean != 0u ? 1.0 : 0.0;
        return PIKA_STATUS_OK;
    }
    if (value->kind == PIKA_BINDING_VALUE_FLOAT) {
        *result = value->as.floating;
        return PIKA_STATUS_OK;
    }
    return PIKA_STATUS_TYPE_MISMATCH;
}

static PikaStatus math_float_result(PikaBindingValue* result,
                                    double value) {
    if (!isfinite(value)) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    result->kind = PIKA_BINDING_VALUE_FLOAT;
    result->as.floating = value;
    return PIKA_STATUS_OK;
}

static PikaStatus math_integer_result(PikaBindingValue* result,
                                      double value) {
    if (!isfinite(value) || value < -PIKA_MATH_INT64_UPPER ||
        value >= PIKA_MATH_INT64_UPPER) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    result->kind = PIKA_BINDING_VALUE_INTEGER;
    result->as.integer = (int64_t)value;
    return PIKA_STATUS_OK;
}

static PikaStatus math_integer_unary(const PikaBindingCall* call,
                                     PikaBindingValue* result,
                                     uint8_t operation) {
    double input;
    double output;
    PikaStatus status = math_validate(call, result, 1u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = math_number(&call->arguments[0], &input);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (!isfinite(input)) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (operation == 0u) {
        output = ceil(input);
    } else if (operation == 1u) {
        output = floor(input);
    } else {
        output = trunc(input);
    }
    return math_integer_result(result, output);
}

static PikaStatus math_unary(const PikaBindingCall* call,
                             PikaBindingValue* result,
                             PikaMathUnaryOperation operation) {
    double input;
    double output = 0.0;
    PikaStatus status = math_validate(call, result, 1u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = math_number(&call->arguments[0], &input);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (!isfinite(input)) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if ((operation == PIKA_MATH_LOG || operation == PIKA_MATH_LOG2 ||
         operation == PIKA_MATH_LOG10) && input <= 0.0) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (operation == PIKA_MATH_SQRT && input < 0.0) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if ((operation == PIKA_MATH_ACOS || operation == PIKA_MATH_ASIN) &&
        (input < -1.0 || input > 1.0)) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    switch (operation) {
        case PIKA_MATH_FABS:
            output = fabs(input);
            break;
        case PIKA_MATH_EXP:
            output = exp(input);
            break;
        case PIKA_MATH_LOG:
            output = log(input);
            break;
        case PIKA_MATH_LOG2:
            output = log2(input);
            break;
        case PIKA_MATH_LOG10:
            output = log10(input);
            break;
        case PIKA_MATH_SQRT:
            output = sqrt(input);
            break;
        case PIKA_MATH_ACOS:
            output = acos(input);
            break;
        case PIKA_MATH_ASIN:
            output = asin(input);
            break;
        case PIKA_MATH_ATAN:
            output = atan(input);
            break;
        case PIKA_MATH_COS:
            output = cos(input);
            break;
        case PIKA_MATH_SIN:
            output = sin(input);
            break;
        case PIKA_MATH_TAN:
            output = tan(input);
            break;
        case PIKA_MATH_DEGREES:
            output = input * (180.0 / PIKA_MATH_PI);
            break;
        case PIKA_MATH_RADIANS:
            output = input * (PIKA_MATH_PI / 180.0);
            break;
        case PIKA_MATH_COSH:
            output = cosh(input);
            break;
        case PIKA_MATH_SINH:
            output = sinh(input);
            break;
        case PIKA_MATH_TANH:
            output = tanh(input);
            break;
        default:
            return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return math_float_result(result, output);
}

static PikaStatus math_binary(const PikaBindingCall* call,
                              PikaBindingValue* result,
                              PikaMathBinaryOperation operation) {
    double left;
    double right;
    double output = 0.0;
    PikaStatus status = math_validate(call, result, 2u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = math_number(&call->arguments[0], &left);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = math_number(&call->arguments[1], &right);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (!isfinite(left) || !isfinite(right)) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if ((operation == PIKA_MATH_FMOD ||
         operation == PIKA_MATH_REMAINDER) && right == 0.0) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    switch (operation) {
        case PIKA_MATH_FMOD:
            output = fmod(left, right);
            break;
        case PIKA_MATH_REMAINDER:
            output = remainder(left, right);
            break;
        case PIKA_MATH_POW:
            output = pow(left, right);
            break;
        case PIKA_MATH_ATAN2:
            output = atan2(left, right);
            break;
        default:
            return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return math_float_result(result, output);
}

#define PIKA_MATH_UNARY_CALLBACK(name, operation)                         \
    PikaStatus pika_binding__math_##name(const PikaBindingCall* call,     \
                                         PikaBindingValue* result) {      \
        return math_unary(call, result, operation);                       \
    }

#define PIKA_MATH_BINARY_CALLBACK(name, operation)                        \
    PikaStatus pika_binding__math_##name(const PikaBindingCall* call,     \
                                         PikaBindingValue* result) {      \
        return math_binary(call, result, operation);                      \
    }

PikaStatus pika_binding__math_ceil(const PikaBindingCall* call,
                                   PikaBindingValue* result) {
    return math_integer_unary(call, result, 0u);
}

PikaStatus pika_binding__math_floor(const PikaBindingCall* call,
                                    PikaBindingValue* result) {
    return math_integer_unary(call, result, 1u);
}

PikaStatus pika_binding__math_trunc(const PikaBindingCall* call,
                                    PikaBindingValue* result) {
    return math_integer_unary(call, result, 2u);
}

PIKA_MATH_UNARY_CALLBACK(fabs, PIKA_MATH_FABS)
PIKA_MATH_UNARY_CALLBACK(exp, PIKA_MATH_EXP)
PIKA_MATH_UNARY_CALLBACK(log, PIKA_MATH_LOG)
PIKA_MATH_UNARY_CALLBACK(log2, PIKA_MATH_LOG2)
PIKA_MATH_UNARY_CALLBACK(log10, PIKA_MATH_LOG10)
PIKA_MATH_UNARY_CALLBACK(sqrt, PIKA_MATH_SQRT)
PIKA_MATH_UNARY_CALLBACK(acos, PIKA_MATH_ACOS)
PIKA_MATH_UNARY_CALLBACK(asin, PIKA_MATH_ASIN)
PIKA_MATH_UNARY_CALLBACK(atan, PIKA_MATH_ATAN)
PIKA_MATH_UNARY_CALLBACK(cos, PIKA_MATH_COS)
PIKA_MATH_UNARY_CALLBACK(sin, PIKA_MATH_SIN)
PIKA_MATH_UNARY_CALLBACK(tan, PIKA_MATH_TAN)
PIKA_MATH_UNARY_CALLBACK(degrees, PIKA_MATH_DEGREES)
PIKA_MATH_UNARY_CALLBACK(radians, PIKA_MATH_RADIANS)
PIKA_MATH_UNARY_CALLBACK(cosh, PIKA_MATH_COSH)
PIKA_MATH_UNARY_CALLBACK(sinh, PIKA_MATH_SINH)
PIKA_MATH_UNARY_CALLBACK(tanh, PIKA_MATH_TANH)

PIKA_MATH_BINARY_CALLBACK(fmod, PIKA_MATH_FMOD)
PIKA_MATH_BINARY_CALLBACK(remainder, PIKA_MATH_REMAINDER)
PIKA_MATH_BINARY_CALLBACK(pow, PIKA_MATH_POW)
PIKA_MATH_BINARY_CALLBACK(atan2, PIKA_MATH_ATAN2)

#endif
