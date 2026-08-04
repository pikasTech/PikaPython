import fuzzypid


kp_rules = []
kp_rules.append([
    fuzzypid.PB, fuzzypid.PB, fuzzypid.PM, fuzzypid.PM,
    fuzzypid.PS, fuzzypid.ZO, fuzzypid.ZO,
])
kp_rules.append([
    fuzzypid.PB, fuzzypid.PB, fuzzypid.PM, fuzzypid.PS,
    fuzzypid.PS, fuzzypid.ZO, fuzzypid.NS,
])
kp_rules.append([
    fuzzypid.PM, fuzzypid.PM, fuzzypid.PM, fuzzypid.PS,
    fuzzypid.ZO, fuzzypid.NS, fuzzypid.NS,
])
kp_rules.append([
    fuzzypid.PM, fuzzypid.PM, fuzzypid.PS, fuzzypid.ZO,
    fuzzypid.NS, fuzzypid.NM, fuzzypid.NM,
])
kp_rules.append([
    fuzzypid.PS, fuzzypid.PS, fuzzypid.ZO, fuzzypid.NS,
    fuzzypid.NS, fuzzypid.NM, fuzzypid.NM,
])
kp_rules.append([
    fuzzypid.PS, fuzzypid.ZO, fuzzypid.NS, fuzzypid.NM,
    fuzzypid.NM, fuzzypid.NM, fuzzypid.NB,
])
kp_rules.append([
    fuzzypid.ZO, fuzzypid.ZO, fuzzypid.NM, fuzzypid.NM,
    fuzzypid.NM, fuzzypid.NB, fuzzypid.NB,
])

ki_rules = []
ki_rules.append([
    fuzzypid.NB, fuzzypid.NB, fuzzypid.NM, fuzzypid.NM,
    fuzzypid.NS, fuzzypid.ZO, fuzzypid.ZO,
])
ki_rules.append([
    fuzzypid.NB, fuzzypid.NB, fuzzypid.NM, fuzzypid.NS,
    fuzzypid.NS, fuzzypid.ZO, fuzzypid.ZO,
])
ki_rules.append([
    fuzzypid.NB, fuzzypid.NM, fuzzypid.NS, fuzzypid.NS,
    fuzzypid.ZO, fuzzypid.PS, fuzzypid.PS,
])
ki_rules.append([
    fuzzypid.NM, fuzzypid.NM, fuzzypid.NS, fuzzypid.ZO,
    fuzzypid.PS, fuzzypid.PM, fuzzypid.PM,
])
ki_rules.append([
    fuzzypid.NM, fuzzypid.NS, fuzzypid.ZO, fuzzypid.PS,
    fuzzypid.PS, fuzzypid.PM, fuzzypid.PB,
])
ki_rules.append([
    fuzzypid.ZO, fuzzypid.ZO, fuzzypid.PS, fuzzypid.PS,
    fuzzypid.PM, fuzzypid.PB, fuzzypid.PB,
])
ki_rules.append([
    fuzzypid.ZO, fuzzypid.ZO, fuzzypid.PS, fuzzypid.PM,
    fuzzypid.PM, fuzzypid.PB, fuzzypid.PB,
])

kd_rules = []
kd_rules.append([
    fuzzypid.PS, fuzzypid.NS, fuzzypid.NB, fuzzypid.NB,
    fuzzypid.NB, fuzzypid.NM, fuzzypid.PS,
])
kd_rules.append([
    fuzzypid.PS, fuzzypid.NS, fuzzypid.NB, fuzzypid.NM,
    fuzzypid.NM, fuzzypid.NS, fuzzypid.ZO,
])
kd_rules.append([
    fuzzypid.ZO, fuzzypid.NS, fuzzypid.NM, fuzzypid.NM,
    fuzzypid.NS, fuzzypid.NS, fuzzypid.ZO,
])
kd_rules.append([
    fuzzypid.ZO, fuzzypid.NS, fuzzypid.NS, fuzzypid.NS,
    fuzzypid.NS, fuzzypid.NS, fuzzypid.ZO,
])
kd_rules.append([
    fuzzypid.ZO, fuzzypid.ZO, fuzzypid.ZO, fuzzypid.ZO,
    fuzzypid.ZO, fuzzypid.ZO, fuzzypid.ZO,
])
kd_rules.append([
    fuzzypid.PB, fuzzypid.PS, fuzzypid.PS, fuzzypid.PS,
    fuzzypid.PS, fuzzypid.PS, fuzzypid.PB,
])
kd_rules.append([
    fuzzypid.PB, fuzzypid.PM, fuzzypid.PM, fuzzypid.PM,
    fuzzypid.PS, fuzzypid.PS, fuzzypid.PB,
])


def verify_constructor_failure_recovery():
    invalid_row = [
        fuzzypid.PB + 1, fuzzypid.PB, fuzzypid.PM, fuzzypid.PM,
        fuzzypid.PS, fuzzypid.ZO, fuzzypid.ZO,
    ]
    invalid_kp_rules = []
    invalid_kp_rules.append(invalid_row)
    for row_index in range(1, 7):
        invalid_kp_rules.append(kp_rules[row_index])

    attempts = 0
    while attempts < 3:
        rejected = False
        try:
            fuzzypid.RuleBase(invalid_kp_rules, ki_rules, kd_rules)
        except ValueError:
            rejected = True
        if not rejected:
            raise ValueError("fuzzypid invalid rule was accepted")
        attempts += 1

    recovered_first = fuzzypid.RuleBase(kp_rules, ki_rules, kd_rules)
    recovered_second = fuzzypid.RuleBase(kp_rules, ki_rules, kd_rules)
    recovered_first.deinit()
    recovered_second.deinit()
    recovered_first = None
    recovered_second = None
    print("FUZZYPID_CONSTRUCTOR_RECOVERY_OK")


def create_controller(kp, ki, kd):
    rules = fuzzypid.RuleBase(kp, ki, kd)
    membership_values = []
    membership_values.append(fuzzypid.NB)
    membership_values.append(fuzzypid.NB)
    membership_values.append(fuzzypid.NM)
    membership_values.append(fuzzypid.ZO)
    membership_values.append(fuzzypid.NB)
    membership_values.append(fuzzypid.NM)
    membership_values.append(fuzzypid.NS)
    membership_values.append(fuzzypid.ZO)
    membership_values.append(fuzzypid.NM)
    membership_values.append(fuzzypid.NS)
    membership_values.append(fuzzypid.ZO)
    membership_values.append(fuzzypid.ZO)
    membership_values.append(fuzzypid.NS)
    membership_values.append(fuzzypid.ZO)
    membership_values.append(fuzzypid.PS)
    membership_values.append(fuzzypid.ZO)
    membership_values.append(fuzzypid.ZO)
    membership_values.append(fuzzypid.PS)
    membership_values.append(fuzzypid.PM)
    membership_values.append(fuzzypid.ZO)
    membership_values.append(fuzzypid.PS)
    membership_values.append(fuzzypid.PM)
    membership_values.append(fuzzypid.PB)
    membership_values.append(fuzzypid.ZO)
    membership_values.append(fuzzypid.PM)
    membership_values.append(fuzzypid.PB)
    membership_values.append(fuzzypid.PB)
    membership_values.append(fuzzypid.ZO)
    membership = fuzzypid.MembershipFunction(membership_values)

    parameter_values = []
    parameter_values.append([0.65, 0, 0, 0, 0, 0, 1])
    parameter_values.append([-0.34, 0, 0, 0, 0, 0, 1])
    parameter_values.append([-1.1, 0, 0, 0, 0, 0, 1])
    parameter_values.append([-2.4, 0, 0, 0, 0, 0, 1])
    parameter_values.append([1.2, 0, 0, 0, 0, 0, 1])
    parameter_values.append([1.2, 0.05, 0.1, 0, 0, 0, 1])
    parameters = fuzzypid.FuzzyPIDParams(parameter_values)
    return fuzzypid.FuzzyPIDController(
        rules, membership, parameters,
        direct=fuzzypid.PIDDirect([
            True, False, False, False, True, True,
        ]),
    )


def verify_vector(instance):
    expected = [682, 685, 683, 680, 678, 675, 672, 670, 668, 665]
    real = 0
    for index in range(10):
        output = instance.compute_output(5, real, 90)
        if int(output) != expected[index]:
            raise ValueError("fuzzypid vector mismatch")
        real += (output - 500) / 500 * 10

    invalid_rejected = False
    try:
        instance.compute_output(6, real, 90)
    except ValueError:
        invalid_rejected = True
    if not invalid_rejected:
        raise ValueError("fuzzypid invalid control id was accepted")
    instance.compute_output(5, real, 90)


verify_constructor_failure_recovery()
verify_vector(create_controller(kp_rules, ki_rules, kd_rules))
print("FUZZYPID_VECTOR_OK")
