import PikaStdLib
import PikaStdData
import GTestTask
import PikaMath
import PikaStdDevice
import PikaDebug
import PikaStdData
from PikaObj import *

pkdb = PikaDebug.Debuger()
list = PikaStdData.List()
io =PikaStdDevice.GPIO()

class Test():
    x = 1
    def hi(self):
        print('hi' + str(self.x))
test = Test()
test.hi()

print('hello')
