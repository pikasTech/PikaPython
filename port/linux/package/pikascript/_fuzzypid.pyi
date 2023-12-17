PB: int
PM: int
PS: int
ZO: int
NS: int
NM: int
NB: int


def __init__(): ...


class RuleBase:
    def __init__(self,
                 kp_rules: list[list[int]],
                 ki_rules: list[list[int]],
                 kd_rules: list[list[int]]): ...

    def __del__(self): ...


class MembershipFunction:
    def __init__(self, params: list[int]): ...
    def __del__(self): ...


class FuzzyPIDParams:
    def __init__(self, pid_params: list[list[float]]): ...
    def __del__(self): ...


class PIDDirect:
    def __init__(self, direct: list[int]): ...
    def __del__(self): ...


class FuzzyPIDController:
    def __init__(self,
                 rule_base: RuleBase,
                 mf_params: MembershipFunction,
                 pid_params: FuzzyPIDParams,
                 delta_k: float,
                 mf_type: int,
                 fo_type: int,
                 df_type: int,
                 direct: list): ...

    def compute_output(self,
                       control_id: int,
                       real: float,
                       input: float,
                       ) -> float: ...

    def __del__(self): ...
