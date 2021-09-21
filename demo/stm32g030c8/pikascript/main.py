import STM32
import PikaPiZero
import PikaStdLib
time = STM32.Time()
uart = STM32.UART()

uart.init()
uart.setId(1)
uart.enable()

print('initing rgb...')
rgb = PikaPiZero.RGB()
rgb.init()
rgb.enable()
print('init rgb ok!')

mem = PikaStdLib.MemChecker()
print('mem max:')
mem.max()
while True:
    time.sleep_ms(250)
    rgb.red()
    time.sleep_ms(250)
    rgb.green()
    time.sleep_ms(250)
    rgb.white()
    time.sleep_ms(250)
    rgb.blue()
