from PikaObj import *


@staticmethod
def int(arg: any, *base) -> int: ...


@staticmethod
def bool(arg: any) -> bool: ...


@staticmethod
def float(arg: any) -> float: ...


@staticmethod
def str(arg: any) -> str: ...


@staticmethod
def iter(arg: any) -> any: ...


@staticmethod
def range(*ax) -> any: ...


@staticmethod
def print(*val, **ops): ...


@staticmethod
def __setitem__(obj: any, key: any, val: any) -> any: ...


@staticmethod
def __getitem__(obj: any, key: any) -> any: ...


@staticmethod
@PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
def type(arg: any) -> any: ...


@staticmethod
@PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
def isinstance(object: any, classinfo: any) -> bool: ...


@staticmethod
def len(arg: any) -> int: ...


@staticmethod
@PIKA_C_MACRO_IF("PIKA_BUILTIN_STRUCT_ENABLE")
def list(*val) -> any: ...


@staticmethod
@PIKA_C_MACRO_IF("PIKA_BUILTIN_STRUCT_ENABLE")
def dict(*val) -> any: ...


@staticmethod
@PIKA_C_MACRO_IF("PIKA_BUILTIN_STRUCT_ENABLE")
def tuple(arg: any) -> any: ...


@staticmethod
@PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
def hex(val: int) -> str: ...


@staticmethod
@PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
def ord(val: str) -> int: ...


@staticmethod
@PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
def chr(val: int) -> str: ...


@staticmethod
@PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
def bytes(val: any) -> bytes: ...


@staticmethod
@PIKA_C_MACRO_IF("PIKA_SYNTAX_FORMAT_ENABLE")
def cformat(fmt: str, *var) -> str: ...


@staticmethod
@PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
def id(obj: any) -> int: ...


@staticmethod
@PIKA_C_MACRO_IF("PIKA_FILEIO_ENABLE")
def open(path: str, mode: str) -> object: ...


@staticmethod
@PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
def dir(obj: any) -> list: ...


@staticmethod
@PIKA_C_MACRO_IF("PIKA_EXEC_ENABLE")
def exec(code: str): ...


@staticmethod
@PIKA_C_MACRO_IF("PIKA_EXEC_ENABLE")
def eval(code: str) -> any: ...


@staticmethod
@PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
def getattr(obj: object, name: str) -> any: ...


@staticmethod
@PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
def setattr(obj: object, name: str, val: any): ...


@staticmethod
@PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
def hasattr(obj: object, name: str) -> int: ...


@staticmethod
@PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
def exit(): ...


@staticmethod
@PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
def input(*info) -> str: ...


@staticmethod
@PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
def abs(val: any) -> any: ...


@staticmethod
@PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
def max(*val) -> any: ...


@staticmethod
@PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
def min(*val) -> any: ...


@staticmethod
@PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
def help(name: str): ...


@staticmethod
@PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
def reboot(): ...


@staticmethod
@PIKA_C_MACRO_IF("!PIKA_NANO_ENABLE")
def clear(): ...


@staticmethod
@PIKA_C_MACRO_IF("PIKA_GC_MARK_SWEEP_ENABLE")
def gcdump(): ...


@PIKA_C_MACRO_IF("0")
class RangeObj:
    def __next__(self) -> any: ...


@PIKA_C_MACRO_IF("0")
class StringObj:
    def __next__(self) -> any: ...


class object:
    pass


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
