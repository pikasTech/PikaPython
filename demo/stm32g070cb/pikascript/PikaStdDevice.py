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


class ADC(TinyObj):
    def init():
        pass

    def setPin(pin: str):
        pass

    def enable():
        pass

    def read() -> float:
        pass

    # need override
    def platformEnable(pin: str):
        pass

    # need override
    def platformRead(pin: str) -> float:
        pass


class UART(TinyObj):
    def init():
        pass

    def setBaudRate(baudRate: int):
        pass

    def setId(id: int):
        pass

    def enable():
        pass

    def write(data: str):
        pass

    def read(length: int) -> str:
        pass

    # need override
    def platformEnable(id: int, baudRate: int):
        pass

    # need override
    def platformWrite(id: int, data: str):
        pass

    # need override
    def platformRead(id: int, length: int) -> str:
        pass
