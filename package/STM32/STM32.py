from PikaObj import *
import PikaStdDevice


class GPIO(PikaStdDevice.GPIO):
    # override
    def platformHigh():
        pass

    # override
    def platformLow():
        pass

    # override
    def platformEnable():
        pass

    # override
    def platformDisable():
        pass

    # override
    def platformSetMode(mode: str):
        pass

    # override
    def platformRead() -> int:
        pass


class Time(PikaStdDevice.Time):
    # override
    def sleep_s(s: int):
        pass

    # override
    def sleep_ms(ms: int):
        pass


class ADC(PikaStdDevice.ADC):
    # override
    def platformEnable(pin: str):
        pass

    # override
    def platformRead(pin: str) -> float:
        pass


class UART(PikaStdDevice.UART):
    # override
    def platformEnable(id: int, baudRate: int):
        pass

    # override
    def platformWrite(id: int, data: str):
        pass

    # override
    def platformRead(id: int, length: int) -> str:
        pass


class PWM(PikaStdDevice.PWM):
    # override
    def platformEnable(pin: str, freq: int, duty: float):
        pass

    # override
    def platformSetFrequency(pin: str, freq: int):
        pass

    # override
    def platformSetDuty(pin: str, duty: float):
        pass


class IIC(PikaStdDevice.IIC):
    SCL = GPIO()
    SDA = GPIO()
    # override

    def platformEnable():
        pass

    # override
    def platformWrite(addr: int, data: str):
        pass

    # override
    def platformRead(addr: int, length: int) -> str:
        pass


class lowLevel(TinyObj):
    def readPin(pin: str) -> int:
        pass
