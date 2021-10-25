import PikaStdLib
import STM32
import PikaPiZero

uart = STM32.UART()
uart = STM32.UART()
rgb = PikaPiZero.RGB()
mem = PikaStdLib.MemChecker()

uart.init()
uart.setId(1)
uart.setBaudRate(115200)
uart.enable()

iic = STM32.IIC()
iic.init()
iic.setPinSDA('PB0')
iic.setPinSCL('PB1')
iic.setDeviceAddr(88)
iic.enable()
print(iic.read(0, 4))

print('hello 2')
print('mem used max:')
mem.max()

i = 0
while i < 100:
    i = i + 1
    print(i)

print('Hello PikaScript!')
