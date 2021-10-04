import PikaStdLib
import STM32

time = STM32.Time()
uart = STM32.UART()
uart.init()
uart.setBaudRate(115200)
uart.setId(1)
uart.enable()

while True:
    time.sleep_ms(500)
    readBuff = uart.read(2)
    print('read 2 char:')
    print(readBuff)
    
