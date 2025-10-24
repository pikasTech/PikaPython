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


class REPL:
    def setEcho(self, echo: bool): ...


class SysObj:
    pass
