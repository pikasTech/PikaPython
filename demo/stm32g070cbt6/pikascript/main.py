import PikaStdLib
import STM32

mem = PikaStdLib.MemChecker()
time = STM32.Time()

io1 = STM32.GPIO()
io1.init()
io1.setPin('PB4')
io1.setMode('out')
io1.enable()
io1.low()

adc1 = STM32.ADC()
adc1.init()
adc1.setPin('PA1')
adc1.enable()

uart1 = STM32.UART()
uart1.init()
uart1.setBaudRate(115200)
uart1.setId(2)
uart1.enable()

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
    print('mem.max :')
    mem.max()
