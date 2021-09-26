import STM32
import PikaPiZero
import PikaStdLib

uart.init()
uart.setId(1)
uart.setBaudRate(9600)
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
