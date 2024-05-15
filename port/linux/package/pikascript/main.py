import PikaStdLib
import PikaDebug
import this
from subsrc import mod1

print('hello pikapython!')
mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()
print('mem used now:')
mem.now()
