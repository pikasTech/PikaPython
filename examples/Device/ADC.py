import PikaStdLib
import PikaStdDevice

time = PikaStdDevice.Time()
adc1 = PikaStdDevice.ADC()

adc1.setPin('PA1')
adc1.enable()

while True:
    val = adc1.read()
    print('adc1 value:')
    print(val)
    time.sleep_ms(500)
    
