import PikaStdLib, PikaStdDevice, PikaMath, PikaDebug, PikaCV, PikaNN
import random, re, modbus, socket, unittest, binascii, ctypes
import pika_lua, pika_cjson, cjson_test
import GTestTask, TempDevTest
import configparser
import test_module1, test_cmodule, test_module4, import_test

mem = PikaStdLib.MemChecker()
print('hello pikascript!')
print('mem used max:')
mem.max()
