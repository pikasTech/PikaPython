"""
PikaStdDevice is a standard and abstract device module for PikaScript.

PikaStdDevice supplies the standard device API for users.

Users need to inherit from PikaStdDevice and override the abstract methods, which is with the `@abstractmethod` decorator.

For example, the STM32F1 device module: https://gitee.com/Lyon1998/pikascript/blob/master/package/STM32F1/STM32F1.pyi

And for convenience, make a machine.pyi to inherit from STM32F1 device module for alias purpose.

For example:

- The machine.pyi for STM32F1:https://gitee.com/Lyon1998/pikascript/blob/master/bsp/stm32f103c8/pikascript/machine.pyi

- The machine.pyi for STM32G0: https://gitee.com/Lyon1998/pikascript/blob/master/bsp/stm32g070cb/pikascript/machine.pyi
"""
from PikaObj import *


class GPIO(BaseDev):
    def __init__(self): ...

    def setPin(self, pinName: str):
        """
        Use the name of the pin to select the GPIO pin.

        example: `"PA0"`, `"PA1"` ...
        """

    def setId(self, id: int):
        """
        Use the id of the pin to select the GPIO pin.

        example: 0, 1 ...
        """

    def getId(self) -> int:
        """
        Get the id of the pin.
        """

    def getPin(self) -> str:
        """
        Get the name of the pin.
        """

    def setMode(self, mode: str):
        """
        Set the mode of the pin.
        example: "in", "out" ...
        """

    def getMode(self) -> str:
        """
        Get the mode of the pin.
        """

    def setPull(self, pull: str):
        """
        Set the pull of the pin.
        example: `"up"`, `"down"`, `"none"` ...
        """

    def enable(self):
        """Enable the pin."""

    def disable(self):
        """Disable the pin."""

    def high(self):
        """Set the pin to high."""

    def low(self):
        """Set the pin to low."""

    def read(self) -> int:
        """Read the pin value."""

    @abstractmethod
    def platformHigh(self): ...

    @abstractmethod
    def platformLow(self): ...

    @abstractmethod
    def platformEnable(self): ...

    @abstractmethod
    def platformDisable(self): ...

    @abstractmethod
    def platformSetMode(self): ...

    @abstractmethod
    def platformRead(self): ...


class Time(BaseDev):
    def __init__(self): ...

    def sleep(self, s: float):
        """Sleep for s seconds."""

    @PIKA_C_MACRO_IF("PIKA_STD_DEVICE_UNIX_TIME_ENABLE")
    def time(self) -> float:
        """Get the current time."""

    @PIKA_C_MACRO_IF("PIKA_STD_DEVICE_UNIX_TIME_ENABLE")
    def time_ns(self) -> int:
        """Get the current time in nanoseconds."""

    @PIKA_C_MACRO_IF("PIKA_STD_DEVICE_UNIX_TIME_ENABLE")
    def gmtime(self, unix_time: float):
        """Convert unix time to struct_time."""

    @PIKA_C_MACRO_IF("PIKA_STD_DEVICE_UNIX_TIME_ENABLE")
    def localtime(self, unix_time: float):
        """Convert unix time to struct_time."""

    @PIKA_C_MACRO_IF("PIKA_STD_DEVICE_UNIX_TIME_ENABLE")
    def mktime(self) -> int:
        """Convert struct_time to unix time."""

    @PIKA_C_MACRO_IF("PIKA_STD_DEVICE_UNIX_TIME_ENABLE")
    def asctime(self):
        """Convert struct_time to string."""

    @PIKA_C_MACRO_IF("PIKA_STD_DEVICE_UNIX_TIME_ENABLE")
    def ctime(self, unix_time: float):
        """Convert unix time to string."""

    @abstractmethod
    def sleep_s(self, s: int): ...

    @abstractmethod
    def sleep_ms(self, ms: int): ...

    @abstractmethod
    @PIKA_C_MACRO_IF("PIKA_STD_DEVICE_UNIX_TIME_ENABLE")
    def platformGetTick(): ...


class ADC(BaseDev):
    def __init__(self): ...

    def setPin(self, pin: str):
        """
        Use the name of the pin to select the ADC pin.
        example: `"PA0"`, `"PA1"` ...
        """

    def enable(self):
        """Enable the ADC."""

    def disable(self):
        """Disable the ADC."""

    def read(self) -> float:
        """Read the ADC value."""

    @abstractmethod
    def platformEnable(self): ...

    @abstractmethod
    def platformRead(self): ...

    @abstractmethod
    def platformDisable(self): ...


class UART(BaseDev):
    def __init__(self): ...

    def setBaudRate(self, baudRate: int):
        """Set the baud rate."""

    def setId(self, id: int):
        """Set the id of the UART."""

    def enable(self):
        """Enable the UART."""

    def disable(self):
        """Disable the UART."""

    def write(self, data: str):
        """Write string to the UART."""

    def writeBytes(self, data: bytes, length: int):
        """Write bytes to the UART."""

    def read(self, length: int) -> str:
        """Read string from the UART."""

    def readBytes(self, length: int) -> bytes:
        """Read bytes from the UART."""

    @abstractmethod
    def platformEnable(self): ...

    @abstractmethod
    def platformWrite(self): ...

    @abstractmethod
    def platformWriteBytes(self): ...

    @abstractmethod
    def platformRead(self): ...

    @abstractmethod
    def platformReadBytes(self): ...

    @abstractmethod
    def platformDisable(self): ...


class IIC(BaseDev):
    def __init__(self): ...

    def setPinSCL(self, pin: str):
        """Set the SCL pin."""

    def setPinSDA(self, pin: str):
        """Set the SDA pin."""

    def setDeviceAddr(self, addr: int):
        """Set the device address."""

    def enable(self):
        """Enable the IIC."""

    def disable(self):
        """Disable the IIC."""

    def write(self, addr: int, data: str):
        """Write string to the IIC."""

    def writeBytes(self, addr: int, data: bytes, length: int):
        """Write bytes to the IIC."""

    def read(self, addr: int, length: int) -> str:
        """Read string from the IIC."""

    def readBytes(self, addr: int, length: int) -> bytes:
        """Read bytes from the IIC."""

    @abstractmethod
    def platformEnable(self): ...

    @abstractmethod
    def platformWrite(self): ...

    @abstractmethod
    def platformWriteBytes(self): ...

    @abstractmethod
    def platformRead(self): ...

    @abstractmethod
    def platformReadBytes(self): ...

    @abstractmethod
    def platformDisable(self): ...


class PWM(BaseDev):
    def __init__(self): ...

    def setName(self, name: str):
        """Use the device name to select the PWM pin.
        exmpale: `"PWM0"`, `"PWM1"` ...
        """

    def getName(self) -> str:
        """Get the device name."""

    def setChannel(self, ch: int):
        """Set the channel."""

    def getChannel(self) -> int:
        """Get the channel."""

    def setPin(self, pin: str):
        """Use the name of the pin to select the PWM pin.
        example: `"PA0"`, `"PA1"` ...
        """

    def setFrequency(self, freq: int):
        """Set the frequency."""

    def setFreq(self, freq: int):
        """Set the frequency."""

    def setDuty(self, duty: float):
        """Set the duty."""

    def enable(self):
        """Enable the PWM."""

    def disable(self):
        """Disable the PWM."""

    def getFrequency(self) -> int:
        """Get the frequency."""

    def getDuty(self) -> float:
        """Get the duty."""

    @abstractmethod
    def platformEnable(self): ...

    @abstractmethod
    def platformSetFrequency(self): ...

    @abstractmethod
    def platformSetDuty(self): ...

    @abstractmethod
    def platformDisable(self): ...


class SPI(BaseDev):
    def __init__(self): ...

    def setPinSCK(self, pin: str):
        """Set the SCK pin."""

    def setPinMOSI(self, pin: str):
        """Set the MOSI pin."""

    def setPinMISO(self, pin: str):
        """Set the MISO pin."""

    def setName(self, name: str):
        """Use the device name to select the SPI pin.
        exmpale: `"SPI0"`, `"SPI1"` ...
        """

    def setId(self, id: int):
        """Set the id of the SPI.
        example: `0`, `1` ...
        """

    def setPolarity(self, polarity: int):
        """Set the polarity."""

    def setPhase(self, phase: int):
        """Set the phase."""

    def setBaudRate(self, baudRate: int):
        """Set the baud rate."""

    def enable(self):
        """Enable the SPI."""

    def disable(self):
        """Disable the SPI."""

    def write(self, data: str):
        """Write string to the SPI."""

    def writeBytes(self, data: bytes, length: int):
        """Write bytes to the SPI."""

    def read(self, length: int) -> str:
        """Read string from the SPI."""

    def readBytes(self, length: int) -> bytes:
        """Read bytes from the SPI."""

    @abstractmethod
    def platformEnable(self): ...

    @abstractmethod
    def platformWrite(self): ...

    @abstractmethod
    def platformWriteBytes(self): ...

    @abstractmethod
    def platformRead(self): ...

    @abstractmethod
    def platformReadBytes(self): ...

    @abstractmethod
    def platformDisable(self): ...


class CAN(BaseDev):
    def __init__(self): ...

    def setName(self, name: str):
        """Use the device name to select the CAN pin.
        exmpale: `"CAN0"`, `"CAN1"` ...
        """

    def setId(self, id: int):
        """Use the id to select the CAN pin.
        example: `0`, `1` ...
        """

    def setBaudRate(self, baudRate: int):
        """Set the baud rate."""

    def setMode(self, mode: str):
        """Set the mode.
        example: `"normal"`, `"loopback"`, `"silent"`, `"silent_loopback"`
        """

    def enable(self):
        """Enable the CAN."""

    def disable(self):
        """Disable the CAN."""

    def write(self, data: str):
        """Write string to the CAN."""

    def writeBytes(self, data: bytes, length: int):
        """Write bytes to the CAN."""

    def read(self, length: int) -> str:
        """Read string from the CAN."""

    def readBytes(self, length: int) -> bytes:
        """Read bytes from the CAN."""

    def addFilter(self, id: int, ide: int, rtr: int, mode: int, mask: int, hdr: int): 
        """Add a filter."""

    @abstractmethod
    def platformEnable(self): ...

    @abstractmethod
    def platformWrite(self): ...

    @abstractmethod
    def platformWriteBytes(self): ...

    @abstractmethod
    def platformRead(self): ...

    @abstractmethod
    def platformReadBytes(self): ...

    @abstractmethod
    def platformDisable(self): ...


class BaseDev:
    @PIKA_C_MACRO_IF("PIKA_EVENT_ENABLE")
    def addEventCallBack(self, eventCallback: any): 
        """ Add an event callback. """

    @abstractmethod
    @PIKA_C_MACRO_IF("PIKA_EVENT_ENABLE")
    def platformGetEventId(self): ...
