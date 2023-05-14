from PikaObj import *


def __init__(): ...


def eval(cmd: str) -> any: ...


def evalLine(line: str) -> any: ...

class LuaTable:
    pass

def require(module: str)-> LuaTable: ...

def __del__():...
