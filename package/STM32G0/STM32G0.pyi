#api
from PikaObj import *
import PikaStdDevice
import PikaStdTask


class GPIO(PikaStdDevice.GPIO): ...


class Time(PikaStdDevice.Time):
    # override
    def sleep_s(self, s: int):
        pass

    # override
    def sleep_ms(self, ms: int):
        pass


class ADC(PikaStdDevice.ADC):
    # override
    def platformEnable(self):
        pass

    # override
    def platformRead(self):
        pass


class UART(PikaStdDevice.UART):
    # override
    def platformEnable(self):
        pass

    # override
    def platformWrite(self):
        pass

    # override
    def platformRead(self):
        pass


class PWM(PikaStdDevice.PWM):
    # override
    def platformEnable(self):
        pass

    # override
    def platformSetFrequency(self):
        pass

    # override
    def platformSetDuty(self):
        pass


class IIC(PikaStdDevice.IIC):
    SCL = GPIO()
    SDA = GPIO()
    # override

    def platformEnable(self):
        pass

    # override
    def platformWrite(self):
        pass

    # override
    def platformRead(self):
        pass


class lowLevel(TinyObj):
    def readPin(self, pin: str) -> int:
        pass


class Task(PikaStdTask.Task):
    # override
    def platformGetTick(self):
        pass
