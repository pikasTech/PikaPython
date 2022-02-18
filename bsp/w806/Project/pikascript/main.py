import PikaStdLib
import W801Device


pb5=W801Device.GPIO()
pb25=W801Device.GPIO()
time=W801Device.Time()
pwm = W801Device.PWM()

pb5.init()
pb5.setPin('PB5')
pb5.setMode('out')
pb5.setPull('up')
pb5.enable()
pb25.init()
pb25.setPin('PB25')
pb25.setMode('out')
pb25.setPull('up')
pb25.enable()

##pwm.init()
pwm.setPin('PB16')
pwm.enable()
pwm.setFrequency(5)
pwm.setDuty(50.0)

print('hello world')

while True:
    pb5.low()
    time.sleep_s(1)
    pb5.high()
    time.sleep_s(1)
