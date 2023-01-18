"""
PikaStdDevice is a standard and abstract device module for PikaScript.

PikaStdDevice supplies the standard device API for users.

Document: https://pikadoc.readthedocs.io/en/latest/PikaStdDevice%20%E6%A0%87%E5%87%86%E8%AE%BE%E5%A4%87.html

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
    

    SIGNAL_RISING: int
    SIGNAL_FALLING: int
    SIGNAL_ANY: int

    def setCallBack(self, eventCallBack: any, filter: int):
        """
        Add a callback function to the pin.
        Example: 
        ``` python
        def cb1(signal):
            print("cb1", signal)
        io.setCallBack(cb1, io.SIGNAL_RISING)
        ```
        """

    def close(self): ...

    def platformHigh(self): ...

    def platformLow(self): ...

    def platformEnable(self): ...

    def platformDisable(self): ...

    def platformSetMode(self): ...

    def platformRead(self): ...


def Time() -> time:
    """ # use time module instead """


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

    def close(self): ...

    @abstractmethod
    def platformEnable(self): ...

    @abstractmethod
    def platformRead(self): ...

    @abstractmethod
    def platformDisable(self): ...


class DAC(BaseDev):
    def __init__(self): ...

    def setPin(self, pin: str):
        """
        Use the name of the pin to select the DAC pin.
        example: `"PA0"`, `"PA1"` ...
        """

    def enable(self):
        """Enable the DAC."""

    def disable(self):
        """Disable the DAC."""

    def write(self, val: float):
        """write the DAC value."""

    def close(self): ...


class UART:
    def __init__(self): ...

    def setBaudRate(self, baudRate: int):
        """Set the baud rate."""

    def setId(self, id: int):
        """Set the id of the UART."""

    FLOW_CONTROL_NONE: int
    FLOW_CONTROL_RTS: int
    FLOW_CONTROL_CTS: int
    FLOW_CONTROL_RTS_CTS: int

    def setFlowControl(self, flowControl: int):
        """Set the flow control of the UART."""

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

    def setPinTX(self, pin: str):
        """
        Remap the TX pin.
        """
    
    def setPinRX(self, pin: str):
        """
        Remap the RX pin.
        """
    
    def setPinCTS(self, pin: str):
        """
        Remap the CTS pin.
        """
    
    def setPinRTS(self, pin: str):
        """
        Remap the RTS pin.
        """

    def close(self): ...

    SIGNAL_RX: int
    SIGNAL_TX: int

    def setCallBack(self, eventCallBack: any, filter: int):
        """
        Add a callback function to the pin.
        Example: 
        ``` python
        def cb1(signal):
            print(uart.read(-1))
        io.setCallBack(cb1, uart.SIGNAL_RX)
        ```
        """
    
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

    def close(self): ...

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
