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

while True:
    time.sleep_ms(10)
    rgb.flow()
    print('flowing')


