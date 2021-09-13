import PikaStdLib
import STM32

mem = PikaStdLib.MemChecker()
time = STM32.Time()
uart = STM32.UART()
uart.init()
uart.setBaudRate(115200)
uart.setId(1)
uart.enable()

while True:
    time.sleep_s(1)
    readBuff = uart.read(2);
    print('mem used max:')
    mem.max()
    print('read 2 char:')
    print(readBuff)
    print(' ')
