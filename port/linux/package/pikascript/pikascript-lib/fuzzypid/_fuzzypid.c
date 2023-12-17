#include "_fuzzypid.h"
#include "_fuzzypid_FuzzyPIDController.h"
#include "_fuzzypid_FuzzyPIDParams.h"
#include "_fuzzypid_MembershipFunction.h"
#include "_fuzzypid_PIDDirect.h"
#include "_fuzzypid_RuleBase.h"
#include "fuzzy_PID.h"

void _fuzzypid___init__(PikaObj* self) {
    obj_setInt(self, "PB", PB);
    obj_setInt(self, "PM", PM);
    obj_setInt(self, "PS", PS);
    obj_setInt(self, "ZO", ZO);
    obj_setInt(self, "NS", NS);
    obj_setInt(self, "NM", NM);
    obj_setInt(self, "NB", NB);
}

static void _fillRuleBase(PikaObj* self,
                          PikaObj* rules,
                          int (*rules_base)[qf_default],
                          size_t* offset) {
    size_t rules_len = pikaList_getSize(rules);
    for (size_t i = 0; i < rules_len; ++i) {
        PikaObj* rule = pikaList_getPtr(rules, i);
        size_t rule_len = pikaList_getSize(rule);
        for (size_t j = 0; j < rule_len; ++j) {
            rules_base[*offset][j] = pikaList_getInt(rule, j);
        }
        ++(*offset);
    }
}

void _fuzzypid_RuleBase___init__(PikaObj* self,
                                 PikaObj* kp_rules,
                                 PikaObj* ki_rules,
                                 PikaObj* kd_rules) {
    size_t kp_rules_len = pikaList_getSize(kp_rules);
    size_t ki_rules_len = pikaList_getSize(ki_rules);
    size_t kd_rules_len = pikaList_getSize(kd_rules);
    if (kp_rules_len != ki_rules_len || ki_rules_len != kd_rules_len) {
        obj_setSysOut(
            self,
            "Error: kp_rules, ki_rules, kd_rules must have the same length");
        obj_setErrorCode(self, -__LINE__);
        return;
    }
    size_t rules_len = kp_rules_len;
    if (rules_len == 0) {
        obj_setSysOut(self,
                      "Error: kp_rules, ki_rules, kd_rules must not be "
                      "empty");
        obj_setErrorCode(self, -__LINE__);
        return;
    }
    size_t rule_len = pikaList_getSize(pikaList_getPtr(kp_rules, 0));
    if (rule_len != qf_default) {
        obj_setSysOut(self,
                      "Error: kp_rules, ki_rules, kd_rules must have "
                      "qf_default columns");
        obj_setErrorCode(self, -__LINE__);
        return;
    }
    for (size_t i = 0; i < rules_len; ++i) {
        PikaObj* rule = pikaList_getPtr(kp_rules, i);
        if (pikaList_getSize(rule) != rule_len) {
            obj_setSysOut(self,
                          "Error: kp_rules, ki_rules, kd_rules must have "
                          "the same length");
            obj_setErrorCode(self, -__LINE__);
            return;
        }
    }
    size_t rules_size = sizeof(int) * rules_len * rule_len * 3;
    int(*rules)[qf_default] = pikaMalloc(rules_size);
    obj_setInt(self, "rules_size", rules_size);
    size_t offset = 0;
    _fillRuleBase(self, kp_rules, rules, &offset);
    _fillRuleBase(self, ki_rules, rules, &offset);
    _fillRuleBase(self, kd_rules, rules, &offset);
    obj_setPtr(self, "rules", rules);
}

void _fuzzypid_RuleBase___del__(PikaObj* self) {
    int* rules = obj_getPtr(self, "rules");
    pikaFree(rules, obj_getInt(self, "rules_size"));
}

void _fuzzypid_MembershipFunction___init__(PikaObj* self, PikaObj* params) {
    size_t params_len = pikaList_getSize(params);
    if (params_len != 4 * qf_default) {
        obj_setSysOut(self, "Error: params must have 4 * qf_default columns");
        obj_setErrorCode(self, -__LINE__);
        return;
    }
    int* mf_params = pikaMalloc(sizeof(int) * params_len);
    for (size_t i = 0; i < params_len; ++i) {
        mf_params[i] = pikaList_getInt(params, i);
    }
    obj_setPtr(self, "mf_params", mf_params);
}

void _fuzzypid_MembershipFunction___del__(PikaObj* self) {
    int* mf_params = obj_getPtr(self, "mf_params");
    pikaFree(mf_params, sizeof(int) * 4 * qf_default);
}

void _fuzzypid_FuzzyPIDParams___init__(PikaObj* self, PikaObj* pid_params) {
    size_t DOF = pikaList_getSize(pid_params);
    size_t pid_params_len = pikaList_getSize(pikaList_getPtr(pid_params, 0));
    if (pid_params_len != pid_params_count) {
        obj_setSysOut(self,
                      "Error: pid_params must have pid_params_count columns");
        obj_setErrorCode(self, -__LINE__);
        return;
    }
    float(*fuzzy_pid_params)[pid_params_count] =
        pikaMalloc(sizeof(float) * DOF * pid_params_count);
    for (size_t i = 0; i < DOF; ++i) {
        PikaObj* pid_param = pikaList_getPtr(pid_params, i);
        if (pikaList_getSize(pid_param) != pid_params_len) {
            obj_setSysOut(self, "Error: pid_params must have the same length");
            obj_setErrorCode(self, -__LINE__);
            return;
        }
        for (size_t j = 0; j < pid_params_len; ++j) {
            fuzzy_pid_params[i][j] = pikaList_getFloat(pid_param, j);
        }
    }
    obj_setPtr(self, "fuzzy_pid_params", fuzzy_pid_params);
    obj_setInt(self, "DOF", DOF);
}

void _fuzzypid_FuzzyPIDParams___del__(PikaObj* self) {
    float* fuzzy_pid_params = obj_getPtr(self, "fuzzy_pid_params");
    size_t DOF = obj_getInt(self, "DOF");
    pikaFree(fuzzy_pid_params, sizeof(float) * DOF * pid_params_count);
}

void _fuzzypid_PIDDirect___init__(PikaObj* self, PikaObj* direct) {
    size_t DOF = pikaList_getSize(direct);
    pika_bool* direct_vector = pikaMalloc(sizeof(pika_bool) * DOF);
    for (size_t i = 0; i < DOF; ++i) {
        direct_vector[i] = pikaList_getBool(direct, i);
    }
    obj_setInt(self, "DOF", DOF);
    obj_setPtr(self, "direct_vector", direct_vector);
}

void _fuzzypid_PIDDirect___del__(PikaObj* self) {
    pika_bool* direct_vector = obj_getPtr(self, "direct_vector");
    pikaFree(direct_vector, sizeof(pika_bool) * obj_getInt(self, "DOF"));
}

void _fuzzypid_FuzzyPIDController___init__(PikaObj* self,
                                           PikaObj* rule_base,
                                           PikaObj* mf_params,
                                           PikaObj* pid_params,
                                           pika_float delta_k,
                                           int mf_type,
                                           int fo_type,
                                           int df_type,
                                           PikaObj* direct) {
    obj_setPtr(self, "rule_base", rule_base);
    obj_setPtr(self, "mf_params", mf_params);
    obj_setPtr(self, "pid_params", pid_params);
    obj_setPtr(self, "direct", direct);
    int* rules = obj_getPtr(rule_base, "rules");
    int* mf_params_mat = obj_getPtr(mf_params, "mf_params");
    float* fuzzy_pid_params = obj_getPtr(pid_params, "fuzzy_pid_params");
    int DOF_pid_parms = obj_getInt(pid_params, "DOF");
    int DOF_direct = obj_getInt(direct, "DOF");
    if (DOF_pid_parms != DOF_direct) {
        obj_setSysOut(self,
                      "Error: pid_params and direct must have the same length");
        obj_setErrorCode(self, -__LINE__);
        return;
    }
    struct PID** pid_vector = fuzzy_pid_vector_init(
        (float(*)[pid_params_count])fuzzy_pid_params, delta_k, mf_type, fo_type,
        df_type, mf_params_mat, (int(*)[pid_params_count])rules, DOF_pid_parms);
    obj_setPtr(self, "pid_vector", pid_vector);
    obj_setInt(self, "DOF", DOF_pid_parms);
    obj_setPtr(self, "direct_vector", obj_getPtr(direct, "direct_vector"));
}

void _fuzzypid_FuzzyPIDController___del__(PikaObj* self) {
    struct PID** pid_vector = obj_getPtr(self, "pid_vector");
    if (NULL != pid_vector) {
        int DOF = obj_getInt(self, "DOF");
        delete_pid_vector(pid_vector, DOF);
    }
}

pika_float _fuzzypid_FuzzyPIDController_compute_output(PikaObj* self,
                                                       int control_id,
                                                       pika_float real,
                                                       pika_float input) {
    struct PID** pid_vector = obj_getPtr(self, "pid_vector");
    pika_bool* direct_vector = obj_getPtr(self, "direct_vector");
    int DOF = obj_getInt(self, "DOF");
    if (control_id < 0 || control_id >= DOF) {
        obj_setSysOut(self, "Error: control_id out of range");
        obj_setErrorCode(self, -__LINE__);
        return 0;
    }
    return fuzzy_pid_motor_pwd_output(real, input, direct_vector[control_id],
                                      pid_vector[control_id]);
}
