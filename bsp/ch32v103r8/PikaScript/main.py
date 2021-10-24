import PikaStdLib
import CH32V103

mem = PikaStdLib.MemChecker()
print('hello PikaScript !')

io1 = CH32V103.GPIO()
io1.init()
io1.setPin('PA1')
io1.setMode('out')
io1.enable()
io1.high()

print(1.4)
print('mem used max:')
mem.max()