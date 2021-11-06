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

    def setPull(pull:str):
        pass

    def enable():
        pass

    def disable():
        pass

    def high():
        pass

    def low():
        pass

    def read()->int:
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
    def platformSetMode():
        pass

    # need override
    def platformRead():
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
    def platformEnable():
        pass

    # need override
    def platformRead():
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
    def platformEnable():
        pass

    # need override
    def platformWrite():
        pass

    # need override
    def platformRead():
        pass


class IIC(TinyObj):
    def init():
        pass

    def setPinSCL(pin: str):
        pass

    def setPinSDA(pin: str):
        pass

    def setDeviceAddr(addr: int):
        pass

    def enable():
        pass

    def write(addr: int, data: str):
        pass

    def read(addr: int, length: int) -> str:
        pass

    # need override
    def platformEnable():
        pass

    # need override
    def platformWrite():
        pass

    # need override
    def platformRead():
        pass


class PWM(TinyObj):
    def init():
        pass

    def setPin(pin: str):
        pass

    def setFrequency(freq: int):
        pass

    def setDuty(duty: float):
        pass

    def enable():
        pass

    def getFrequency() -> int:
        pass

    def getDuty() -> float:
        pass

    # need override
    def platformEnable():
        pass

    # need override
    def platformSetFrequency():
        pass

    # need override
    def platformSetDuty():
        pass
