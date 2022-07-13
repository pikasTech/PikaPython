from PikaObj import *
import PikaStdLib
import PikaStdDevice
import ctypes
import GTestTask
import PikaMath
import test_module1
import test_cmodule
import TempDevTest
import cjson_test
import test_module4
import pika_lua
import import_test
import pika_configparser
import PikaDebug
import PikaCV

mem = PikaStdLib.MemChecker()
print('hello pikascript!')
print('mem used max:')
mem.max()
