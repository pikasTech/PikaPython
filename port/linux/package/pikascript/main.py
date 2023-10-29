import PikaStdLib
import PikaDebug
import this

print('hello pikapython!')
mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()
print('mem used now:')
mem.now()
