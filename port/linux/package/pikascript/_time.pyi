from PikaObj import *


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
