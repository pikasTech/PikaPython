import PikaStdLib
import PikaStdDevice
import PikaMath
from PikaObj import *

operator = PikaMath.Operator()

print('hello world')
mem = PikaStdLib.MemChecker()
print('mem.max :')
mem.max()
print('mem.now :')
mem.now()
