import PikaStdLib
import STM32
from PikaObj import *

print('hello pikascript')
mem = PikaStdLib.MemChecker()

io1 = STM32.GPIO()
time = STM32.Time()

io1.init()
io1.setPin('PB4')
io1.setMode('out')
io1.enable()
io1.low()

print('mem.max :')
mem.max()
print('mem.now :')
mem.now()

while True:
    io1.low()
    time.sleep_ms(500)
    io1.high()
    time.sleep_ms(500)
