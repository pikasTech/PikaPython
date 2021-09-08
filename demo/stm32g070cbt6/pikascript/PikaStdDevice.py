from PikaObj import *


class GPIO(TinyObj):
    def init():
        pass

    def setPin(pinName: str):
        pass

    def getPin() -> str:
        pass

    def setMode(mode: str):
        pass

    def getMode() -> str:
        pass

    def enable():
        pass

    def disable():
        pass

    def high():
        pass

    def low():
        pass

    # need be overrid
    def platformHigh():
        pass

    # need override
    def platformLow():
        pass

    # need override
    def platformEnable():
        pass

    # need override
    def platformDisable():
        pass

    # need override
    def platformSetMode(mode: str):
        pass


class Time(TinyObj):
    # need override
    def sleep_s(s: int):
        pass

    # need override
    def sleep_ms(ms: int):
        pass
