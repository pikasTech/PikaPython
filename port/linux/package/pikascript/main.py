import PikaStdLib, PikaStdDevice, PikaMath, PikaDebug, PikaCV, PikaNN
import random, re, modbus, socket, unittest, binascii, ctypes, requests, mqtt
import pika_lua, pika_cjson, cjson_test
import GTestTask, TempDevTest
import cb_test
import configparser
import test_module1, test_cmodule, test_module4, import_test
import hashlib, hmac, aes, base64, time

mem = PikaStdLib.MemChecker()
print('hello pikascript!')
print('mem used max:')
mem.max()
