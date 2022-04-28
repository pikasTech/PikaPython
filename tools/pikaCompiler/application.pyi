#api
import PyInfo
import Arm2D
import PikaStdLib
from PikaObj import *

class Compiler(PikaStdLib.SysObj):
    obj = PyInfo.pyiObj()
    PyInfo.pyiMethod()
    PyInfo.pyiClass()
    line = Arm2D.Line()

    def build(pythonApiPath: str, outputPath: str) -> int:
        pass

    def analyzestr(pythonApiPath: str) -> int:
        pass

    def analyzeLine(line: str) -> int:
        pass

class MyRoot(PikaStdLib.SysObj):
    compiler = Compiler()
