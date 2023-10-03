import PikaDebug as pdb
import PikaStdLib, PikaMath, PikaCV, PikaNN
import random, re, modbus, socket, unittest, binascii, ctypes, requests, mqtt
import pika_lua, pika_cjson, cjson_test
import GTestTask, TempDevTest
import cb_test
import configparser, network
import test_module1, test_cmodule, test_module4, import_test, testcase
import test_module5
import hashlib, hmac, aes, base64, time, os, zlib
import _thread, weakref, eventloop
import this
import fsm

mem = PikaStdLib.MemChecker()
print('hello pikascript!')
print('mem used max:')
mem.max()
