import PikaStdLib
import STM32G0

time = STM32G0.Time()
uart = STM32G0.UART()
uart.init()
uart.setId(1)
uart.setBaudRate(115200)
uart.enable()

while True:
    time.sleep_ms(500)
    readBuff = uart.read(2)
    print('read 2 char:')
    print(readBuff)
    
