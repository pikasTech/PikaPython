import PikaStdLib
import STM32

uart = STM32.UART()
uart.init()
uart.setId(1)
uart.setBaudRate(115200)
uart.enable()

time = STM32.Time()
pwm = STM32.PWM()
pwm.setPin('PA8')
pwm.setFrequency(2000)
pwm.setDuty(0.5)
pwm.enable()

while True:
    time.sleep_ms(500)
    pwm.setDuty(0.5)
    time.sleep_ms(500)
    pwm.setDuty(0.001)
    
