import PikaStdLib
import STM32F4

print('hello pikascript!')
mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()

