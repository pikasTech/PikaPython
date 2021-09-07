import PikaStdLib
import STM32
from PikaObj import *

print('hello pikascript')
mem = PikaStdLib.MemChecker()

io1 = STM32.GPIO()
io1.init()
io1.setPin('PB4')
io1.setMode('out')
io1.enable()
io1.low()

print('mem.max :')
mem.max()
print('mem.now :')
mem.now()

