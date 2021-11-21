import STM32G0
import PikaPiZero
import PikaStdLib

time = STM32G0.Time()
adc = STM32G0.ADC()
pin = STM32G0.GPIO()
pwm = STM32G0.PWM()
uart = STM32G0.UART()
rgb = PikaPiZero.RGB()
mem = PikaStdLib.MemChecker()

rgb.init()
rgb.enable()

print('hello 2')
print('mem used max:')
mem.max()

while True:
    print('flowing')
    rgb.flow()

