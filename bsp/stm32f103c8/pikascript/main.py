import PikaStdLib
import STM32

mem = PikaStdLib.MemChecker()
time = STM32.Time()
uart = STM32.UART()
uart.init()
uart.setBaudRate(115200)
uart.setId(1)
uart.enable()

adc = STM32.ADC()
adc.init()
adc.setPin('PA0')
adc.enable()

pwm = STM32.PWM()
pwm.init()
pwm.setPin('PA8')
pwm.setFrequency(2000)
pwm.setDuty(0.3)
pwm.enable()

while True:
    time.sleep_ms(500)
    pwm.setDuty(0.8)
    readBuff = uart.read(2)
    print('mem used max:')
    mem.max()
    print('read 2 char:')
    print(readBuff)
    print(' ')

    time.sleep_ms(500)
    pwm.setDuty(0.05)
    adcVal = adc.read()
    print('adc value:')
    print(adcVal)

