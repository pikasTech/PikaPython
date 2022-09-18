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
import configparser
import PikaDebug
import PikaCV
import binascii
import unittest
import socket
import random
import re
import PikaNN
import modbus

mem = PikaStdLib.MemChecker()
print('hello pikascript!')
print('mem used max:')
mem.max()
