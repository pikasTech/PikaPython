from fuzzypid import NB, NM, NS, ZO, PB, PM, PS
from fuzzypid import RuleBase, MembershipFunction, FuzzyPIDParams, PIDDirect, FuzzyPIDController

# Default fuzzy rule base of delta kp/ki/kd
kp_rule_base = [
    [PB, PB, PM, PM, PS, ZO, ZO],
    [PB, PB, PM, PS, PS, ZO, NS],
    [PM, PM, PM, PS, ZO, NS, NS],
    [PM, PM, PS, ZO, NS, NM, NM],
    [PS, PS, ZO, NS, NS, NM, NM],
    [PS, ZO, NS, NM, NM, NM, NB],
    [ZO, ZO, NM, NM, NM, NB, NB],
]

ki_rule_base = [
    [NB, NB, NM, NM, NS, ZO, ZO],
    [NB, NB, NM, NS, NS, ZO, ZO],
    [NB, NM, NS, NS, ZO, PS, PS],
    [NM, NM, NS, ZO, PS, PM, PM],
    [NM, NS, ZO, PS, PS, PM, PB],
    [ZO, ZO, PS, PS, PM, PB, PB],
    [ZO, ZO, PS, PM, PM, PB, PB],
]

kd_rule_base = [
    [PS, NS, NB, NB, NB, NM, PS],
    [PS, NS, NB, NM, NM, NS, ZO],
    [ZO, NS, NM, NM, NS, NS, ZO],
    [ZO, NS, NS, NS, NS, NS, ZO],
    [ZO, ZO, ZO, ZO, ZO, ZO, ZO],
    [PB, PS, PS, PS, PS, PS, PB],
    [PB, PM, PM, PM, PS, PS, PB],
]

rule_base = RuleBase(kp_rule_base, ki_rule_base, kd_rule_base)

mf_params = MembershipFunction(
    [-3, -3, -2, 0, -3, -2, -1, 0, -2, -1, 0, 0, -1, 0, 1, 0, 0, 1, 2, 0, 1, 2, 3, 0, 2, 3, 3, 0])

fuzzy_pid_params = FuzzyPIDParams([
    [0.65, 0, 0, 0, 0, 0, 1],
    [-0.34, 0, 0, 0, 0, 0, 1],
    [-1.1, 0, 0, 0, 0, 0, 1],
])

direct = PIDDirect([True, False, False])

fuzzy_pid_controller = FuzzyPIDController(
    rule_base, mf_params, fuzzy_pid_params, direct=direct)

max_error = 100
middle_pwm_output = 500
control_id = 2
real = 0
idea = max_error * 0.9
print("idea value: ", idea)
for j in range(10):
    out = fuzzy_pid_controller.compute_output(control_id, real, idea)
    real += (out - middle_pwm_output) / middle_pwm_output * max_error * 0.1
    print("%d,%s" % (int(out), real))
