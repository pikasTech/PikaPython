import PikaStdLib
import STM32

uart = STM32.UART()
uart.init()
uart.setId(1)
uart.setBaudRate(115200)
uart.enable()

mem = PikaStdLib.MemChecker()
io1 = STM32.GPIO()
time = STM32.Time()

io1.init()
io1.setPin('PA8')
io1.setMode('out')
io1.enable()
io1.low()

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
    
