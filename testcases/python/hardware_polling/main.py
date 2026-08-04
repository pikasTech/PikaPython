from machine import ADC, IDLE, PWM, WDT, Pin
from time import sleep_ms, ticks_diff, ticks_ms

print(IDLE)

pin = Pin(2, mode=Pin.OUT)
print(pin.value())
print(pin.value(level=1))
print(pin.value())

adc = ADC(identifier=3)
print(adc.read_u16())

pwm = PWM(2, freq=500)
print(pwm.freq())
print(pwm.freq(value=800))
print(pwm.duty_u16())
print(pwm.duty_u16(value=32768))

watchdog = WDT()
print(watchdog.feed())

start = ticks_ms()
sleep_ms(duration=25)
stop = ticks_ms()
print(ticks_diff(new=stop, old=start))
