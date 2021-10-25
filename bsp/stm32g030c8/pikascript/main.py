import PikaStdLib
import STM32
import PikaPiZero

right = STM32.GPIO()
right.init()
right.setPin('PA0')
right.setMode('in')
right.setPull('down')
right.enable()

left= STM32.GPIO()
left.init()
left.setPin('PC13')
left.setMode('in')
left.setPull('up')
left.enable()

down= STM32.GPIO()
down.init()
down.setPin('PB6')
down.setMode('in')
down.setPull('up')
down.enable()

up= STM32.GPIO()
up.init()
up.setPin('PA15')
up.setMode('in')
up.setPull('up')
up.enable()

print('Hello PikaScript!')
mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()

while True:
    if right.read() :
        print('up')

