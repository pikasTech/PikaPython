import PikaStdLib
import STM32

uart = STM32.UART()
uart.init()
uart.setBaudRate(115200)
uart.setId(1)
uart.enable()

time = STM32.Time()

while True:
    time.sleep_ms(500)
    readBuff = uart.read(2)
    print('read 2 char:')
    print(readBuff)
    
