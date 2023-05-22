from PikaObj import *


class MemChecker:
    def max(self): ...
    def now(self): ...

    @PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
    def getMax(self) -> float: ...

    @PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
    def getNow(self) -> float: ...

    @PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
    def resetMax(self): ...


class SysObj:
    pass


@PIKA_C_MACRO_IF("0")
class RangeObj:
    def __next__(self) -> any: ...


@PIKA_C_MACRO_IF("0")
class StringObj:
    def __next__(self) -> any: ...
