import PikaStdLib
import PikaStdData
import GTestTask
import PikaMath
import PikaStdDevice
import PikaDebug
import PikaStdData

pkdb = PikaDebug.Debuger()
list = PikaStdData.List()
io =PikaStdDevice.GPIO()
mem = PikaStdLib.MemChecker()

class Test():
    x = 1
    def hi(self):
        print('hi' + str(self.x))
test = Test()
test.hi()

def todo1():
    test.hi()
    mem.max()

print('hello')

task = GTestTask.Task()
task.call_period_ms(todo1, 500)
