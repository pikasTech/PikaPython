import PikaStdLib
import STM32

mem = PikaStdLib.MemChecker()

time = STM32.Time()

uart = STM32.UART()
uart.init()
uart.setBaudRate(115200)
uart.setId(1)
uart.enable()

io1 = STM32.GPIO()
io1.init()
io1.setPin('PC13')
io1.low()
io1.enable()

adc = STM32.ADC()
adc.init()
adc.setPin('PA0')
adc.enable()

while True:
    time.sleep_ms(500)
    io1.low()
    readBuff = uart.read(2)
    print('mem used max:')
    mem.max()
    print('read 2 char:')
    print(readBuff)
    print(' ')
    time.sleep_ms(500)
    io1.high()
    adcVal = adc.read()
    print('adc value:')
    print(adcVal)
