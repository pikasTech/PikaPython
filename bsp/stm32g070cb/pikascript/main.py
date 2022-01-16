import PikaStdLib
import STM32G0
import PikaPiZero

mem = PikaStdLib.MemChecker()
print('hello pikascript')
print('mem used max:')
mem.max()

rgb = PikaPiZero.RGB()
rgb.enable()
while True:
    rgb.flow()
    print('flowing...')
  