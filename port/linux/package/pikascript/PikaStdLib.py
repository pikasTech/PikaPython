from PikaObj import *


class Math(TinyObj):
    def plusInt(num1: int, num2: int) -> int:
        pass

    def plusFloat(num1: float, num2: float) -> float:
        pass

    def minusInt(num1: int, num2: int) -> int:
        pass

    def minusFloat(num1: float, num2: float) -> float:
        pass

    def equalInt(num1: int, num2: int) -> int:
        pass

    def equalFloat(num1: float, num2: float) -> int:
        pass

    def graterThanInt(num1: int, num2: int) -> int:
        pass

    def lessThanInt(num1: int, num2: int) -> int:
        pass

    def graterThanFloat(num1: float, num2: float) -> int:
        pass

    def lessThanFloat(num1: float, num2: float) -> int:
        pass


class MemChecker(BaseObj):
    def max():
        pass

    def now():
        pass

    def resetMax():
        pass


class SysObj(BaseObj):
    def type(argPath: str):
        pass

    def ls(objPath: str):
        pass

    def remove(argPath: str):
        pass

    def new(objPath: str, classPath: str):
        pass
