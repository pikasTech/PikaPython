#api
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
    def platformSetMode():
        pass

    # override
    def platformRead():
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
    def platformEnable():
        pass

    # override
    def platformRead():
        pass


class UART(PikaStdDevice.UART):
    # override
    def platformEnable():
        pass

    # override
    def platformWrite():
        pass

    # override
    def platformRead():
        pass


class PWM(PikaStdDevice.PWM):
    # override
    def platformEnable():
        pass

    # override
    def platformSetFrequency():
        pass

    # override
    def platformSetDuty():
        pass


class IIC(PikaStdDevice.IIC):
    SCL = GPIO()
    SDA = GPIO()
    # override

    def platformEnable():
        pass

    # override
    def platformWrite():
        pass

    # override
    def platformRead():
        pass


# class lowLevel(TinyObj):
#     def readPin(pin: str) -> int:
#         pass
