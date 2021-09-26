import STM32
import PikaPiZero
import PikaStdLib
time = STM32.Time()
uart = STM32.UART()
rgb = PikaPiZero.RGB()
mem = PikaStdLib.MemChecker()

uart.init()
uart.setId(1)
uart.setBaudRate(115200)
uart.enable()

print('initing rgb...')
rgb.init()
rgb.enable()
print('init rgb ok!')

print('mem max:')
mem.max()
while True:
    time.sleep_ms(50)
    rgb.flow()
