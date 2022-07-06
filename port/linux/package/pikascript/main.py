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
import pika_cjson as cjson
import cjson_test
import test_module4
import pika_lua
import import_test
import pika_configparser as configparser
from PikaStdData import String as S
import PikaDebug

mem = PikaStdLib.MemChecker()
print('hello pikascript!')
print('mem used max:')
mem.max()
