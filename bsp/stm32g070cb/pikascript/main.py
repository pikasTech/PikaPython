import STM32
import PikaStdLib
import PikaMath

time = STM32.Time()
uart = STM32.UART()
adc = STM32.ADC()
pin = STM32.GPIO()
pwm = STM32.PWM()
uart = STM32.UART()
mem = PikaStdLib.MemChecker()
op = PikaMath.Operator()

uart.init()
uart.setId(1)
uart.setBaudRate(115200)
uart.enable()


print('hello 2')
print('mem used max:')
mem.max()

while True:
    time.sleep_ms(10)
    print('flowing...')

