from PikaObj import *
import PikaStdDevice

class GPIO(PikaStdDevice.GPIO):
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
    
class Time(PikaStdDevice.Time):
    # need override
    def sleep_s(s: int):
        pass

    # need override
    def sleep_ms(ms: int):
        pass
class UART(PikaStdDevice.UART):
     #need override
    def platformEnable():
        pass

    # need override
    def platformWrite():
        pass

    # need override
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
        
class ADC(TinyObj):
    def __init__():
        pass

    def init():
        pass

    def setChannel(channel: int):
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


