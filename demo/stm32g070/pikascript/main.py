import PikaStdLib
import PikaStdDeivce
from PikaObj import *

print('hello world')
mem = PikaStdLib.MemChecker()
print('mem.max :')
mem.max()
print('mem.now :')
mem.now()

io1 = PikaStdDeivce.GPIO()
io1.init()
io1.setPin('PB4')
io1.setMode('out')
io1.enable()
io1.on()
