from PikaObj import *


def __init__(): ...


class struct_time:
    def __iter__(self) -> any: ...

    def __next__(self) -> any: ...

    def __getitem__(self, __key: int) -> int: ...

    def __str__(self) -> str: ...

    def __len__(self) -> int: ...


@PIKA_C_MACRO_IF("PIKA_STD_DEVICE_UNIX_TIME_ENABLE")
def time() -> float:
    """Get the current time."""


@PIKA_C_MACRO_IF("PIKA_STD_DEVICE_UNIX_TIME_ENABLE")
def time_ns() -> int:
    """Get the current time in nanoseconds."""


@PIKA_C_MACRO_IF("PIKA_STD_DEVICE_UNIX_TIME_ENABLE")
def gmtime(unix_time: float):
    """Convert unix time to struct_time."""


@PIKA_C_MACRO_IF("PIKA_STD_DEVICE_UNIX_TIME_ENABLE")
def localtime(unix_time: float) -> struct_time:
    """Convert unix time to struct_time."""


@PIKA_C_MACRO_IF("PIKA_STD_DEVICE_UNIX_TIME_ENABLE")
def mktime(tm: tuple) -> int:
    """Convert struct_time to unix time."""


@PIKA_C_MACRO_IF("PIKA_STD_DEVICE_UNIX_TIME_ENABLE")
def asctime() -> str:
    """Convert struct_time to string."""


@PIKA_C_MACRO_IF("PIKA_STD_DEVICE_UNIX_TIME_ENABLE")
def ctime(unix_time: float) -> str:
    """Convert unix time to string."""


@abstractmethod
def sleep_s(s: int): ...


@abstractmethod
def sleep_ms(ms: int): ...


@abstractmethod
@PIKA_C_MACRO_IF("PIKA_STD_DEVICE_UNIX_TIME_ENABLE")
def platformGetTick(): ...
