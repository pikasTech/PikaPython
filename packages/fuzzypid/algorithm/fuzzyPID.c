/* SPDX-License-Identifier: GPL-3.0-only */
/* Derived from FlameAlpha/fuzzy-pid at commit 38fb510656067f151701fc7cb8612cf2e807fec5. */
#include "fuzzyPID.h"

#include <math.h>

static float pika_fuzzypid_gaussian(float x, float sigma, float center) {
    return expf(-powf((x - center) / sigma, 2.0f));
}

static float pika_fuzzypid_bell(
    float x,
    float width,
    float slope,
    float center) {
    return 1.0f /
           (1.0f + powf(fabsf((x - center) / width), 2.0f * slope));
}

static float pika_fuzzypid_sigmoid(float x, float slope, float center) {
    return 1.0f / (1.0f + expf(slope * (center - x)));
}

static float pika_fuzzypid_trapezoid(
    float x,
    float a,
    float b,
    float c,
    float d) {
    if (x < a || x > d) return 0.0f;
    if (x >= b && x <= c) return 1.0f;
    if (x < b) {
        return b == a ? 1.0f : (x - a) / (b - a);
    }
    return d == c ? 1.0f : (d - x) / (d - c);
}

static float pika_fuzzypid_triangle(
    float x,
    float a,
    float b,
    float c) {
    return pika_fuzzypid_trapezoid(x, a, b, b, c);
}

static float pika_fuzzypid_z_shape(float x, float a, float b) {
    if (x <= a) return 1.0f;
    if (x <= (a + b) / 2.0f) {
        return 1.0f - 2.0f * powf((x - a) / (b - a), 2.0f);
    }
    if (x < b) return 2.0f * powf((x - b) / (b - a), 2.0f);
    return 0.0f;
}

static float pika_fuzzypid_membership(
    float x,
    uint32_t type,
    const int* parameters) {
    switch (type) {
        case 0u:
            return pika_fuzzypid_gaussian(
                x, (float)parameters[0], (float)parameters[1]);
        case 1u:
            return pika_fuzzypid_bell(
                x, (float)parameters[0], (float)parameters[1],
                (float)parameters[2]);
        case 2u:
            return pika_fuzzypid_sigmoid(
                x, (float)parameters[0], (float)parameters[2]);
        case 3u:
            return pika_fuzzypid_trapezoid(
                x, (float)parameters[0], (float)parameters[1],
                (float)parameters[2], (float)parameters[3]);
        case 5u:
            return pika_fuzzypid_z_shape(
                x, (float)parameters[0], (float)parameters[1]);
        default:
            return pika_fuzzypid_triangle(
                x, (float)parameters[0], (float)parameters[1],
                (float)parameters[2]);
    }
}

static float pika_fuzzypid_union(float left, float right, uint32_t type) {
    if (type == 1u) return left + right - left * right;
    if (type == 2u) return fminf(1.0f, left + right);
    return fmaxf(left, right);
}

static float pika_fuzzypid_intersection(
    float left,
    float right,
    uint32_t type) {
    if (type == 1u) return left * right;
    if (type == 2u) return fmaxf(0.0f, left + right - 1.0f);
    return fminf(left, right);
}

static float pika_fuzzypid_equilibrium(
    float left,
    float right,
    float parameter) {
    return powf(left * right, 1.0f - parameter) *
           powf(1.0f - (1.0f - left) * (1.0f - right), parameter);
}

static float pika_fuzzypid_operator(
    float left,
    float right,
    uint32_t type) {
    if (type < 3u) return pika_fuzzypid_intersection(left, right, type);
    if (type < 6u) return pika_fuzzypid_union(left, right, type - 3u);
    return pika_fuzzypid_equilibrium(left, right, 0.5f);
}

static void pika_fuzzypid_defuzzify(
    const float* joint_membership,
    const uint32_t* indexes,
    const uint32_t* counts,
    PikaFuzzyPidInference* inference) {
    float denominator = 0.0f;
    float numerators[PIKA_FUZZYPID_MAX_OUTPUT_COUNT] = {0.0f, 0.0f, 0.0f};
    uint32_t output;
    uint32_t first;
    uint32_t second;
    for (first = 0u; first < counts[0]; ++first) {
        for (second = 0u; second < counts[1]; ++second) {
            denominator += joint_membership[first * counts[1] + second];
        }
    }
    for (output = 0u; output < inference->output_count; ++output) {
        for (first = 0u; first < counts[0]; ++first) {
            for (second = 0u; second < counts[1]; ++second) {
                uint32_t rule =
                    output * PIKA_FUZZYPID_QUANTITY_FIELD_COUNT *
                        PIKA_FUZZYPID_QUANTITY_FIELD_COUNT +
                    indexes[first] * PIKA_FUZZYPID_QUANTITY_FIELD_COUNT +
                    indexes[counts[0] + second];
                numerators[output] +=
                    joint_membership[first * counts[1] + second] *
                    (float)inference->rule_base[rule];
            }
        }
        inference->output[output] = numerators[output] / denominator;
    }
}

static void pika_fuzzypid_infer(
    float error,
    float delta_error,
    PikaFuzzyPidInference* inference) {
    float memberships[PIKA_FUZZYPID_QUANTITY_FIELD_COUNT * 2u];
    uint32_t indexes[PIKA_FUZZYPID_QUANTITY_FIELD_COUNT * 2u];
    uint32_t counts[2] = {0u, 0u};
    float joint[PIKA_FUZZYPID_QUANTITY_FIELD_COUNT *
                PIKA_FUZZYPID_QUANTITY_FIELD_COUNT];
    uint32_t stored = 0u;
    uint32_t index;
    uint32_t first;
    uint32_t second;
    for (index = 0u; index < PIKA_FUZZYPID_QUANTITY_FIELD_COUNT; ++index) {
        float value = pika_fuzzypid_membership(
            error, inference->membership_types[0],
            inference->membership_parameters + 4u * index);
        if (value > 1e-4f) {
            memberships[stored] = value;
            indexes[stored++] = index;
        }
    }
    counts[0] = stored;
    for (index = 0u; index < PIKA_FUZZYPID_QUANTITY_FIELD_COUNT; ++index) {
        float value = pika_fuzzypid_membership(
            delta_error, inference->membership_types[1],
            inference->membership_parameters + 4u * index);
        if (value > 1e-4f) {
            memberships[stored] = value;
            indexes[stored++] = index;
        }
    }
    counts[1] = stored - counts[0];
    if (counts[0] == 0u || counts[1] == 0u) {
        for (index = 0u; index < inference->output_count; ++index) {
            inference->output[index] = 0.0f;
        }
        return;
    }
    for (first = 0u; first < counts[0]; ++first) {
        for (second = 0u; second < counts[1]; ++second) {
            joint[first * counts[1] + second] = pika_fuzzypid_operator(
                memberships[first], memberships[counts[0] + second],
                inference->fuzzy_operator_type);
        }
    }
    pika_fuzzypid_defuzzify(joint, indexes, counts, inference);
}

float pika_fuzzypid_control(
    float real,
    float target,
    PikaFuzzyPidState* state) {
    float delta_error;
    state->last_error = state->current_error;
    state->current_error = target - real;
    delta_error = state->current_error - state->last_error;
    pika_fuzzypid_infer(
        state->current_error / state->error_max * 3.0f,
        delta_error / state->delta_error_max * 3.0f,
        state->inference);
    state->delta_kp = state->inference->output[0] / 3.0f *
                          state->delta_kp_max +
                      state->kp;
    if (state->inference->output_count >= 2u) {
        state->delta_ki = state->inference->output[1] / 3.0f *
                          state->delta_ki_max;
    } else {
        state->delta_ki = 0.0f;
    }
    if (state->inference->output_count >= 3u) {
        state->delta_kd = state->inference->output[2] / 3.0f *
                          state->delta_kd_max;
    } else {
        state->delta_kd = 0.0f;
    }
    state->integral +=
        (state->ki + state->delta_ki) * state->current_error;
    state->output =
        (state->kp + state->delta_kp) * state->current_error +
        state->integral +
        (state->kd + state->delta_kd) *
            (state->current_error - state->last_error);
    state->output += state->feed_forward * target;
    return state->output;
}
