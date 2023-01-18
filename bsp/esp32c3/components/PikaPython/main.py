import PikaStdLib
import machine 
print('hello PikaPython')
mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()
print('mem used now:')
mem.now()
