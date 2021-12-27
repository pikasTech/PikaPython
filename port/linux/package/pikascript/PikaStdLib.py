from PikaObj import *


class MemChecker(TinyObj):
    def max():
        pass

    def now():
        pass

    def getMax() -> float:
        pass

    def getNow() -> float:
        pass

    def resetMax():
        pass


class SysObj(BaseObj):
    def type(arg: any):
        pass

    def remove(argPath: str):
        pass

    def int(arg: any) -> int:
        pass

    def float(arg: any) -> float:
        pass

    def iter(arg: any) -> any:
        pass

    def next(arg: any) -> any:
        pass

    def range(a1, a2, a3, a4) -> any:
        pass


class RangeObj(TinyObj):
    def __iter__() -> any:
        pass

    def __next__() -> any:
        pass
