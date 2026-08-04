from typing import Any


PB: int = 3
PM: int = 2
PS: int = 1
ZO: int = 0
NS: int = -1
NM: int = -2
NB: int = -3


class RuleBase:
    def __init__(self, kp_rows: int, ki_rows: int, kd_rows: int) -> None: ...
    def set_row(self, group: int, index: int, values: list[Any]) -> None: ...
    def deinit(self) -> None: ...


class MembershipFunction:
    def __init__(self, count: int) -> None: ...
    def set_values(self, offset: int, values: list[Any]) -> None: ...
    def deinit(self) -> None: ...


class FuzzyPIDParams:
    def __init__(self, count: int) -> None: ...
    def set_row(self, index: int, values: list[Any]) -> None: ...
    def deinit(self) -> None: ...


class PIDDirect:
    def __init__(self, direct: list[Any]) -> None: ...
    def set_values(self, offset: int, values: list[Any]) -> None: ...
    def deinit(self) -> None: ...


class FuzzyPIDController:
    def __init__(
        self,
        rule_base: Any,
        mf_params: Any,
        pid_params: Any,
        delta_k: Any,
        mf_type: int,
        fo_type: int,
        df_type: int,
        direct: Any,
    ) -> None: ...
    def compute_output(self, control_id: int, real: Any, input: Any) -> float: ...
    def deinit(self) -> None: ...
