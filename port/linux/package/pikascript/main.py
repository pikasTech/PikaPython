import PikaStdLib, PikaStdDevice, PikaMath, PikaDebug, PikaCV, PikaNN
import random, re, modbus, socket, unittest, binascii, ctypes, requests, mqtt
import pika_lua, pika_cjson, cjson_test, json
import GTestTask, TempDevTest
import cb_test
import configparser, network
import test_module1, test_cmodule, test_module4, import_test
import hashlib, hmac, aes, base64, time, os
import _thread, weakref, eventloop
import fsm

mem = PikaStdLib.MemChecker()
print('hello pikascript!')
print('mem used max:')
mem.max()
