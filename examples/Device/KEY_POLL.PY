from machine import GPIO as STD_GPIO

print('PikaScript LED KEY')


LED1 = STD_GPIO()
LED1.setPin('PE5')
LED1.setMode('out')
LED1.enable()

LED2 = STD_GPIO()
LED2.setPin('PE6')
LED2.setMode('out')
LED2.enable()

KEY1 = STD_GPIO()
KEY1.setPin('PE4')
# setPull要在setMode前面才行，PikaStdDevice_GPIO.c函数PikaStdDevice_GPIO_setPull不赋值pull
# PikaStdDevice_GPIO_platformSetMode中才赋值cfg->pull
KEY1.setPull('up')
KEY1.setMode('in')

KEY1.enable()

LED1.high()
LED2.high()
print(KEY1.read())

while True:
    if KEY1.read() == 1:
        LED1.high()
    else:
        LED1.low()
