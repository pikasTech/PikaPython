import PikaStdLib
from PikaObj import *

print('hello world')
mem = PikaStdLib.MemChecker()
print('mem.max :')
mem.max()
print('mem.now :')
mem.now()