import PikaStdLib
import STM32

mem = PikaStdLib.MemChecker()
io1 = STM32.GPIO()
time = STM32.Time()
adc1 = STM32.ADC()

io1.init()
io1.setPin('PB4')
io1.setMode('out')
io1.enable()
io1.low()

adc1.init()
adc1.setPin('PA1')
adc1.enable()

print('hello pikascript')
print('mem.max :')
mem.max()
print('mem.now :')
mem.now()

while True:
    io1.low()
    time.sleep_ms(500)
    io1.high()
    time.sleep_ms(500)
    val = adc1.read()
    print('adc1 value:')
    print(val)
