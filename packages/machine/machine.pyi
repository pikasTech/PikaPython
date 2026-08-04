from typing import Any


class Pin:
    IN: int = 0
    OUT: int = 1
    OPEN_DRAIN: int = 2
    PULL_UP: int = 1
    PULL_DOWN: int = 2

    def __init__(
        self,
        identifier: int,
        mode: int = -1,
        pull: int = -1,
        value: Any = None,
    ) -> None: ...
    def init(
        self,
        mode: int = -1,
        pull: int = -1,
        value: Any = None,
    ) -> None: ...
    def value(self, level: Any = None) -> Any: ...
    def on(self) -> None: ...
    def off(self) -> None: ...


class ADC:
    def __init__(self, source: Any) -> None: ...
    def read_u16(self) -> int: ...
    def read_uv(self) -> int: ...


class PWM:
    def __init__(
        self,
        destination: Any,
        freq: Any = None,
        duty_u16: Any = None,
        duty_ns: Any = None,
    ) -> None: ...
    def init(
        self,
        freq: Any = None,
        duty_u16: Any = None,
        duty_ns: Any = None,
    ) -> None: ...
    def freq(self, value: Any = None) -> Any: ...
    def duty_u16(self, value: Any = None) -> Any: ...
    def duty_ns(self, value: Any = None) -> Any: ...
    def deinit(self) -> None: ...
