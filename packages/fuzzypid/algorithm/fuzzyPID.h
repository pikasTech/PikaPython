/* SPDX-License-Identifier: GPL-3.0-only */
/* Derived from FlameAlpha/fuzzy-pid at commit 38fb510656067f151701fc7cb8612cf2e807fec5. */
#ifndef PIKA_FUZZYPID_ALGORITHM_H
#define PIKA_FUZZYPID_ALGORITHM_H

#include <stdint.h>

#define PIKA_FUZZYPID_QUANTITY_FIELD_COUNT 7u
#define PIKA_FUZZYPID_PARAMETER_COUNT 7u
#define PIKA_FUZZYPID_MAX_OUTPUT_COUNT 3u
#define PIKA_FUZZYPID_MAX_ERROR 100.0f
#define PIKA_FUZZYPID_MAX_DELTA_ERROR 100.0f
#define PIKA_FUZZYPID_MIN_OUTPUT 0
#define PIKA_FUZZYPID_MIDDLE_OUTPUT 500
#define PIKA_FUZZYPID_MAX_OUTPUT 1000

typedef struct {
    uint32_t input_count;
    uint32_t output_count;
    uint32_t fuzzy_operator_type;
    uint32_t* membership_types;
    const int* membership_parameters;
    uint32_t defuzzifier_type;
    const int* rule_base;
    float* output;
} PikaFuzzyPidInference;

typedef struct {
    float kp;
    float ki;
    float kd;
    float delta_kp_max;
    float delta_ki_max;
    float delta_kd_max;
    float delta_kp;
    float delta_ki;
    float delta_kd;
    float error_max;
    float delta_error_max;
    float last_error;
    float current_error;
    float integral;
    float integral_limit;
    float dead_zone;
    float feed_forward;
    float output;
    int output_min_value;
    int output_middle_value;
    int output_max_value;
    float linear_adaptive_kp;
    PikaFuzzyPidInference* inference;
} PikaFuzzyPidState;

float pika_fuzzypid_control(
    float real,
    float target,
    PikaFuzzyPidState* state);

#endif
