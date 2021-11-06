import PikaStdLib
import STM32

uart = STM32.UART()
uart.init()
uart.setId(1)
uart.setBaudRate(115200)
uart.enable()

time = STM32.Time()

while True:
    time.sleep_ms(500)
    print('0.5s')
    time.sleep_s(1)
    print('1s')
    
