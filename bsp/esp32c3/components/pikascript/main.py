import PikaStdLib
import machine

print('hello PikaScript!')
mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()

# io1 = machine.GPIO()

# io1.init()
# io1.setPin('P5')
# io1.setMode('out')
# io1.enable()
# io1.high()

# io2 = machine.GPIO()

# io2.init()
# io2.setPin('P4')
# io2.setMode('in')
# io2.enable()
# a = io2.read()

# print("OK", a)