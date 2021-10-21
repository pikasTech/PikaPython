import PikaStdLib
import PikaMath
import STM32
import PikaPiZero

time = STM32.Time()
uart = STM32.UART()
adc = STM32.ADC()
pin = STM32.GPIO()
pwm = STM32.PWM()
uart = STM32.UART()
rgb = PikaPiZero.RGB()
mem = PikaStdLib.MemChecker()
op = PikaMath.Operator()

uart.init()
uart.setId(1)
uart.setBaudRate(115200)
uart.enable()

rgb.init()
rgb.enable()

print('hello 2')
print('mem used max:')
mem.max() 

i = 0
while i < 100:
    i = i + 1
    rgb.flow()
    print(i)

print('Hello PikaScript!')
