import _fuzzypid

PB: int = _fuzzypid.PB
PM: int = _fuzzypid.PM
PS: int = _fuzzypid.PS
ZO: int = _fuzzypid.ZO
NS: int = _fuzzypid.NS
NM: int = _fuzzypid.NM
NB: int = _fuzzypid.NB


class RuleBase(_fuzzypid.RuleBase):
    ...


class MembershipFunction(_fuzzypid.MembershipFunction):
    ...


class FuzzyPIDParams(_fuzzypid.FuzzyPIDParams):
    ...


class PIDDirect(_fuzzypid.PIDDirect):
    ...


class FuzzyPIDController(_fuzzypid.FuzzyPIDController):
    def __init__(self,
                 rule_base: RuleBase,
                 mf_params: MembershipFunction,
                 pid_params: FuzzyPIDParams,
                 delta_k: float = 2,
                 mf_type: int = 4,
                 fo_type: int = 1,
                 df_type: int = 0,
                 direct: list = None):
        super().__init__(rule_base, mf_params, pid_params,
                         delta_k, mf_type, fo_type, df_type, direct)
