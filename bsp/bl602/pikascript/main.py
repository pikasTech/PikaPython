import PikaStdLib
import PikaStdDevice as std
print('hello pikascript')
mem = PikaStdLib.MemChecker()

dac = std.DAC()
dac.setPin('P14')
dac.enable()
dac.write(1.3)
