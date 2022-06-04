from PikaObj import *


class GPIO(TinyObj):
    def __init__(self):
        pass

    def init(self):
        pass

    def setPin(self, pinName: str):
        pass

    def setId(self, id: int):
        pass

    def getId(self) -> int:
        pass

    def getPin(self) -> str:
        pass

    def setMode(self, mode: str):
        pass

    def getMode(self) -> str:
        pass

    def setPull(self, pull: str):
        pass

    def enable(self):
        pass

    def disable(self):
        pass

    def high(self):
        pass

    def low(self):
        pass

    def read(self) -> int:
        pass

    # need be overrid
    def platformHigh(self):
        pass

    # need override
    def platformLow(self):
        pass

    # need override
    def platformEnable(self):
        pass

    # need override
    def platformDisable(self):
        pass

    # need override
    def platformSetMode(self):
        pass

    # need override
    def platformRead(self):
        pass


class Time(TinyObj):
    def __init__(self):
        pass
        
    # need override
    def sleep_s(self, s: int):
        pass

    # need override
    def sleep_ms(self, ms: int):
        pass

    # need override
    def time(self) -> float:
        pass

    # need override
    def time_ns(self) -> int:
        pass

    # need override
    def gmtime(self, unix_time: float):
        pass

    # need override
    def localtime(self, unix_time: float):
        pass

    # need override
    def mktime(self) -> int:
        pass
    
    # need override
    def asctime(self):
        pass
    
    # need override
    def ctime(self, unix_time: float):
        pass


class ADC(TinyObj):
    def __init__(self):
        pass

    def init(self):
        pass

    def setPin(self, pin: str):
        pass

    def enable(self):
        pass

    def disable(self):
        pass

    def read(self) -> float:
        pass

    # need override
    def platformEnable(self):
        pass

    # need override
    def platformRead(self):
        pass

    # need override
    def platformDisable(self):
        pass


class UART(TinyObj):
    def __init__(self):
        pass

    def init(self):
        pass

    def setBaudRate(self, baudRate: int):
        pass

    def setId(self, id: int):
        pass

    def enable(self):
        pass

    def disable(self):
        pass

    def write(self, data: str):
        pass

    def writeBytes(self, data: bytes, length: int):
        pass

    def read(self, length: int) -> str:
        pass

    def readBytes(self, length: int) -> bytes:
        pass

    # need override
    def platformEnable(self):
        pass

    # need override
    def platformWrite(self):
        pass

    def platformWriteBytes(self):
        pass

    # need override
    def platformRead(self):
        pass

    # need override
    def platformReadBytes(self):
        pass

    # need override
    def platformDisable(self):
        pass


class IIC(TinyObj):
    def __init__(self):
        pass

    def init(self):
        pass

    def setPinSCL(self, pin: str):
        pass

    def setPinSDA(self, pin: str):
        pass

    def setDeviceAddr(self, addr: int):
        pass

    def enable(self):
        pass

    def disable(self):
        pass

    def write(self, addr: int, data: str):
        pass

    def writeBytes(self, addr: int, data: bytes, length: int):
        pass

    def read(self, addr: int, length: int) -> str:
        pass

    def readBytes(self, addr: int, length: int) -> bytes:
        pass

    # need override
    def platformEnable(self):
        pass

    # need override
    def platformWrite(self):
        pass

    # need override
    def platformWriteBytes(self):
        pass

    # need override
    def platformRead(self):
        pass

    # need override
    def platformReadBytes(self):
        pass

    # need override
    def platformDisable(self):
        pass


class PWM(TinyObj):
    def __init__(self):
        pass

    def init(self):
        pass

    def setName(self, name: str):
        pass

    def getName(self) -> str:
        pass

    def setChannel(self, ch: int):
        pass

    def getChannel(self) -> int:
        pass

    def setPin(self, pin: str):
        pass

    def setFrequency(self, freq: int):
        pass

    def setFreq(self, freq: int):
        pass

    def setDuty(self, duty: float):
        pass

    def enable(self):
        pass

    def disable(self):
        pass

    def getFrequency(self) -> int:
        pass

    def getDuty(self) -> float:
        pass

    # need override
    def platformEnable(self):
        pass

    # need override
    def platformSetFrequency(self):
        pass

    # need override
    def platformSetDuty(self):
        pass

    # need override
    def platformDisable(self):
        pass


class SPI(TinyObj):
    def __init__(self):
        pass

    def setPinSCK(self, pin: str):
        pass

    def setPinMOSI(self, pin: str):
        pass

    def setPinMISO(self, pin: str):
        pass

    def setName(self, name: str):
        pass

    def setId(self, id: int):
        pass

    def setPolarity(self, polarity: int):
        pass

    def setPhase(self, phase: int):
        pass

    def setBaudRate(self, baudRate: int):
        pass

    def enable(self):
        pass

    def disable(self):
        pass

    def write(self, data: str):
        pass

    def writeBytes(self, data: bytes, length: int):
        pass

    def read(self, length: int) -> str:
        pass

    def readBytes(self, length: int) -> bytes:
        pass

    # need override
    def platformEnable(self):
        pass

    # need override
    def platformWrite(self):
        pass

    # need override
    def platformWriteBytes(self):
        pass

    # need override
    def platformRead(self):
        pass

    # need override
    def platformReadBytes(self):
        pass

    # need override
    def platformDisable(self):
        pass


class CAN(TinyObj):
    def __init__(self):
        pass

    def setName(self, name: str):
        pass

    def setId(self, id: int):
        pass

    def setBaudRate(self, baudRate: int):
        pass

    def setMode(self, mode: str):
        pass

    def enable(self):
        pass

    def disable(self):
        pass

    def write(self, data: str):
        pass

    def writeBytes(self, data: bytes, length: int):
        pass

    def read(self, length: int) -> str:
        pass

    def readBytes(self, length: int) -> bytes:
        pass

    def addFilter(self, id: int, ide: int, rtr: int, mode: int, mask: int, hdr: int):
        pass

    # need override
    def platformEnable(self):
        pass

    # need override
    def platformWrite(self):
        pass

    # need override
    def platformWriteBytes(self):
        pass

    # need override
    def platformRead(self):
        pass

    # need override
    def platformReadBytes(self):
        pass

    # need override
    def platformDisable(self):
        pass
