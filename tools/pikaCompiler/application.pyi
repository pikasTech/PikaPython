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

    def analysestr(pythonApiPath: str) -> int:
        pass

    def analyseLine(line: str) -> int:
        pass

class MyRoot(PikaStdLib.SysObj):
    compiler = Compiler()
