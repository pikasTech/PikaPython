from PikaObj import *


class Operator(TinyObj):
    def plusInt(self, num1: int, num2: int) -> int: ...
    def plusFloat(self, num1: float, num2: float) -> float: ...
    def minusInt(self, num1: int, num2: int) -> int: ...
    def minusFloat(self, num1: float, num2: float) -> float: ...
    def equalInt(self, num1: int, num2: int) -> int: ...
    def equalFloat(self, num1: float, num2: float) -> int: ...
    def graterThanInt(self, num1: int, num2: int) -> int: ...
    def graterThanFloat(self, num1: float, num2: float) -> int: ...
    def lessThanInt(self, num1: int, num2: int) -> int: ...
    def lessThanFloat(self, num1: float, num2: float) -> int: ...
    def AND(self, flag1: int, flag2: int) -> int: ...
    def OR(self, flag1: int, flag2: int) -> int: ...
    def NOT(self, flag: int) -> int: ...
    def __str__(self) -> str: ...
    def __del__(self): ...


class Math(TinyObj):
    pi: float
    e: float

    def __init__(self):
        pass

    def ceil(self, x: float) -> int:
        pass

    def fabs(self, x: float) -> float:
        pass

    def floor(self, x: float) -> int:
        pass

    def fmod(self, x: float, y: float) -> float:
        pass

    def remainder(self, x: float, y: float) -> float:
        pass

    def trunc(self, x: float) -> float:
        pass

    # 幂函数和对数函数

    def exp(self, x: float) -> float:
        pass

    def log(self, x: float) -> float:
        pass

    def log2(self, x: float) -> float:
        pass

    def log10(self, x: float) -> float:
        pass

    def pow(self, x: float, y: float) -> float:
        pass

    def sqrt(self, x: float) -> float:
        pass

    # 三角函数
    def acos(self, x: float) -> float:
        pass

    def asin(self, x: float) -> float:
        pass

    def atan(self, x: float) -> float:
        pass

    def atan2(self, x: float, y: float) -> float:
        pass

    def cos(self, x: float) -> float:
        pass

    def sin(self, x: float) -> float:
        pass

    def tan(self, x: float) -> float:
        pass

    # 角度转换
    def degrees(self, x: float) -> float:
        pass

    def radians(self, x: float) -> float:
        pass

    # 双曲函数
    def cosh(self, x: float) -> float:
        pass

    def sinh(self, x: float) -> float:
        pass

    def tanh(self, x: float) -> float:
        pass


class Quaternion(TinyObj):
    def __init__(self):
        pass

    def set(self, x: float, y: float, z: float, w: float):
        "xi+yj+zk+w"
        pass

    def get(self, key: int) -> float:
        pass

    def add(self, quat: Quaternion):
        pass

    def sub(self, quat: Quaternion):
        pass

    def mul(self, quat: Quaternion):
        pass

    def magnituded(self) -> float:
        pass

    def magnitudedsquare(self) -> float:
        pass

    def reverse(self):
        pass

    def inverse(self):
        pass

    def normalize(self):
        pass

    def isnormalize(self) -> int:
        pass

    def dot(self, quat: Quaternion) -> float:
        pass

    def crossproduct(self, quat: Quaternion):
        pass

    def fromEuler(self, yaw: float, pitch: float, roll: float, mode: int):
        "mode=1 is deg and mode=0 is rad"
        pass

    def toEuler(self) -> list:
        "Z-Y-X"
        pass
