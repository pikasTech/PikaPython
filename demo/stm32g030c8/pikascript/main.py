import STM32
import PikaPiZero
import PikaStdLib
time = STM32.Time()
uart = STM32.UART()

<<<<<<< HEAD
uart.init()
uart.setId()
uart.enable()
=======
pwm = STM32.PWM()
pwm.init()
pwm.setPin('PA8')
pwm.setFrequency(2000)
pwm.setDuty(0.5)
pwm.enable()
>>>>>>> 75642075742a22be2a2f4f20354f9ee028543b7c

print('initing rgb...')
rgb = PikaPiZero.RGB()
rgb.init()
rgb.enable()
print('init rgb ok!')

mem = PikaStdLib.MemChecker()
print('mem max:')
mem.max()
while True:
    time.sleep_ms(500)
    rgb.red()
    time.sleep_ms(500)
    rgb.red()
