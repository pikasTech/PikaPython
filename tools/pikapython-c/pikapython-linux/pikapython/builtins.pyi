from PikaObj import *


def int(arg: any, *base) -> any: ...


def bool(arg: any) -> bool: ...


def float(arg: any) -> float: ...


def str(arg: any) -> str: ...


def iter(arg: any) -> any: ...


def range(*ax) -> any: ...


def print(*val, **ops): ...


def __setitem__(obj: any, key: any, val: any) -> any: ...


def __getitem__(obj: any, key: any) -> any: ...


def type(arg: any) -> any: ...


def isinstance(object: any, classinfo: any) -> bool: ...


def len(arg: any) -> int: ...


def list(*val) -> any: ...


def dict(*val) -> any: ...


def tuple(*val) -> any: ...


def hex(val: int) -> str: ...


def ord(val: str) -> int: ...


def chr(val: int) -> str: ...


def bytes(val: any) -> bytes: ...


def cformat(fmt: str, *var) -> str: ...


def id(obj: any) -> int: ...


def open(path: str, mode: str) -> object: ...


def dir(obj: any) -> list: ...


def exec(code: str): ...


def eval(code: str) -> any: ...


def getattr(obj: object, name: str) -> any: ...


def setattr(obj: object, name: str, val: any): ...


def hasattr(obj: object, name: str) -> int: ...


def exit(): ...


def input(*info) -> str: ...


def abs(val: any) -> any: ...


def max(*val) -> any: ...


def min(*val) -> any: ...


def help(name: str): ...


def reboot(): ...


def clear(): ...


def gcdump(): ...


class RangeObj:
    def __next__(self) -> any: ...


class StringObj:
    def __next__(self) -> any: ...


class object:
    pass


class bytearray:
    def __init__(self, bytes: any):
        """ convert a bytes to ByteArray """

    def __iter__(self) -> any:
        """ support for loop """

    def __next__(self) -> any:
        """ support for loop """

    def __getitem__(self, __key: int) -> int:
        """ support [] index """

    def __setitem__(self, __key: int, __val: int): ...

    def __str__(self) -> str: ...

    def decode(self) -> str: ...

    def __len__(self) -> int: ...

    def __contains__(self, others: any) -> int: ...

    def split(self, *vars) -> list: ...


class BaseException:
    pass


class Exception(BaseException):
    pass


class SystemExit(BaseException):
    pass


class KeyboardInterrupt(BaseException):
    pass


class GeneratorExit(BaseException):
    pass


class Exception(BaseException):
    pass


class StopIteration(Exception):
    pass


class StopAsyncIteration(Exception):
    pass


class ArithmeticError(Exception):
    pass


class FloatingPointError(ArithmeticError):
    pass


class OverflowError(ArithmeticError):
    pass


class ZeroDivisionError(ArithmeticError):
    pass


class AssertionError(Exception):
    pass


class AttributeError(Exception):
    pass


class BufferError(Exception):
    pass


class EOFError(Exception):
    pass


class ImportError(Exception):
    pass


class ModuleNotFoundError(ImportError):
    pass


class LookupError(Exception):
    pass


class IndexError(LookupError):
    pass


class KeyError(LookupError):
    pass


class MemoryError(Exception):
    pass


class NameError(Exception):
    pass


class UnboundLocalError(NameError):
    pass


class OSError(Exception):
    pass


class BlockingIOError(OSError):
    pass


class ChildProcessError(OSError):
    pass


class ConnectionError(OSError):
    pass


class BrokenPipeError(ConnectionError):
    pass


class ConnectionAbortedError(ConnectionError):
    pass


class ConnectionRefusedError(ConnectionError):
    pass


class ConnectionResetError(ConnectionError):
    pass


class FileExistsError(OSError):
    pass


class FileNotFoundError(OSError):
    pass


class InterruptedError(OSError):
    pass


class IsADirectoryError(OSError):
    pass


class NotADirectoryError(OSError):
    pass


class PermissionError(OSError):
    pass


class ProcessLookupError(OSError):
    pass


class TimeoutError(OSError):
    pass


class ReferenceError(Exception):
    pass


class RuntimeError(Exception):
    pass


class NotImplementedError(RuntimeError):
    pass


class RecursionError(RuntimeError):
    pass


class SyntaxError(Exception):
    pass


class IndentationError(SyntaxError):
    pass


class TabError(IndentationError):
    pass


class SystemError(Exception):
    pass


class TypeError(Exception):
    pass


class ValueError(Exception):
    pass


class UnicodeError(ValueError):
    pass


class UnicodeDecodeError(UnicodeError):
    pass


class UnicodeEncodeError(UnicodeError):
    pass


class UnicodeTranslateError(UnicodeError):
    pass


class Warning(Exception):
    pass


class DeprecationWarning(Warning):
    pass


class PendingDeprecationWarning(Warning):
    pass


class RuntimeWarning(Warning):
    pass


class SyntaxWarning(Warning):
    pass


class UserWarning(Warning):
    pass


class FutureWarning(Warning):
    pass


class ImportWarning(Warning):
    pass


class UnicodeWarning(Warning):
    pass


class BytesWarning(Warning):
    pass


class ResourceWarning(Warning):
    pass
