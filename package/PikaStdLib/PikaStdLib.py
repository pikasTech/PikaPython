from PikaObj import *


class MemChecker(TinyObj):
    def max(self):
        pass

    def now(self):
        pass

    def getMax(self) -> float:
        pass

    def getNow(self) -> float:
        pass

    def resetMax(self):
        pass


class SysObj(BaseObj):
    def type(self, arg: any):
        pass

    def remove(self, argPath: str):
        pass

    def int(self, arg: any) -> int:
        pass

    def float(self, arg: any) -> float:
        pass

    def str(self, arg: any) -> str:
        pass

    def iter(self, arg: any) -> any:
        pass

    def range(self, a1: int, a2: int) -> any:
        pass

    def __set__(self, obj: any, key: any, val: any, obj_str: str):
        pass

    def __get__(self, obj: any, key: any) -> any:
        pass


class RangeObj(TinyObj):
    def __next__(self) -> any:
        pass


class StringObj(TinyObj):
    def __next__(self) -> any:
        pass

class PikaObj(TinyObj):
    pass
