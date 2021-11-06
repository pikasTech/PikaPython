import PikaStdLib
import STM32

uart = STM32.UART()
uart.init()
uart.setId(1)
uart.setBaudRate(115200)
uart.enable()

time = STM32.Time()
adc1 = STM32.ADC()

adc1.init()
adc1.setPin('PA1')
adc1.enable()

while True:
    val = adc1.read()
    print('adc1 value:')
    print(val)
    time.sleep_ms(500)
    
