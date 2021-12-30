from PikaObj import *
import Device
import PikaStdLib

mem = PikaStdLib.MemChecker()


for i in range(1, 100):
    print(i)

print('mem used max:')
mem.max()
print('mem used now:')
mem.now()