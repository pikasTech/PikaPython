from PikaObj import *
import PikaStdLib
import PikaStdData
import ctypes
import GTestTask
import PikaMath
import PikaStdDevice
import test_module1
import test_cmodule
import TempDevTest as dev
import TemplateDevice
from PikaStdData import String

mem = PikaStdLib.MemChecker()
print('hello pikascript!')
print('mem used max:')
mem.max()

dev.test()
