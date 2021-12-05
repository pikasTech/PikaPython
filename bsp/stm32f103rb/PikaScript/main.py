from PikaObj import  *
import PikaStdLib
import STM32F1

time = STM32F1.Time()
pc0 = STM32F1.GPIO()
pc0.init()
pc0.setPin('PC0')
pc0.setMode('out')
pc0.setPull('up')
pc0.enable()

print('hello PikaScript!')
mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()
while True:
    pc0.high()
    print('h')
    time.sleep_ms(500)
    pc0.low()
    print('l')
    time.sleep_ms(500)
