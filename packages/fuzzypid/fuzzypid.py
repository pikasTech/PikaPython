import _fuzzypid


PB = _fuzzypid.PB
PM = _fuzzypid.PM
PS = _fuzzypid.PS
ZO = _fuzzypid.ZO
NS = _fuzzypid.NS
NM = _fuzzypid.NM
NB = _fuzzypid.NB


class RuleBase:
    def __init__(self, kp_rules, ki_rules, kd_rules):
        self._native = _fuzzypid.RuleBase(
            len(kp_rules), len(ki_rules), len(kd_rules))
        try:
            groups = [kp_rules, ki_rules, kd_rules]
            for group in range(3):
                index = 0
                while index < len(groups[group]):
                    values = groups[group][index]
                    if index + 1 < len(groups[group]):
                        values = values + groups[group][index + 1]
                    self._native.set_row(group, index, values)
                    index += 2
        except:
            self._native.deinit()
            self._native = None
            raise

    def deinit(self):
        self._native.deinit()
        self._native = None

    def _rule_native(self):
        return self._native


class MembershipFunction:
    def __init__(self, params):
        self._native = _fuzzypid.MembershipFunction(len(params))
        try:
            self._native.set_values(0, params[0:7])
            self._native.set_values(7, params[7:14])
            self._native.set_values(14, params[14:21])
            self._native.set_values(21, params[21:28])
        except:
            self._native.deinit()
            self._native = None
            raise

    def deinit(self):
        self._native.deinit()
        self._native = None

    def _membership_native(self):
        return self._native


class FuzzyPIDParams:
    def __init__(self, pid_params):
        self._native = _fuzzypid.FuzzyPIDParams(len(pid_params))
        try:
            index = 0
            while index < len(pid_params):
                values = pid_params[index]
                if index + 1 < len(pid_params):
                    values = values + pid_params[index + 1]
                self._native.set_row(index, values)
                index += 2
        except:
            self._native.deinit()
            self._native = None
            raise

    def deinit(self):
        self._native.deinit()
        self._native = None

    def _parameters_native(self):
        return self._native


class PIDDirect:
    def __init__(self, direct):
        self._native = _fuzzypid.PIDDirect(direct)

    def deinit(self):
        self._native.deinit()
        self._native = None

    def _direction_native(self):
        return self._native


class FuzzyPIDController:
    def __init__(self, rule_base, mf_params, pid_params, delta_k=2, mf_type=4, fo_type=1, df_type=0, direct=None):
        native_direct = None
        if direct is not None:
            native_direct = direct._direction_native()
        self._native = _fuzzypid.FuzzyPIDController(
            rule_base._rule_native(),
            mf_params._membership_native(),
            pid_params._parameters_native(),
            delta_k,
            mf_type,
            fo_type,
            df_type,
            native_direct,
        )

    def compute_output(self, control_id, real, input):
        return self._native.compute_output(control_id, real, input)

    def deinit(self):
        self._native.deinit()
        self._native = None
