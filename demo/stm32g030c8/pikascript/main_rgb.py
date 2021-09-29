import STM32
import PikaPiZero
import PikaStdLib

rgb.init()
rgb.enable()

print('hello 2')
print('mem used max:')
mem.max()

while True:
    time.sleep_ms(10)
    rgb.flow()
    print('flowing...')
