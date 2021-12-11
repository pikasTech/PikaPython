import PikaStdLib
import STM32F1

print('hello PikaScript!')
led = STM32F1.GPIO()
led.setPin('PA8')
led.setMode('out')
led.enable()
time = STM32F1.Time()

while True:
    led.high()
    time.sleep_ms(500)
    led.low()
    time.sleep_ms(500)
