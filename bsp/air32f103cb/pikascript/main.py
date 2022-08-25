import PikaStdLib
import AIR32F1

print('[Info] Air32F103 system init OK.')
print('hello PikaScript!')

mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()
