import STM32
import PikaPiZero
import PikaStdLib

uart.init()
uart.setId(1)
uart.setBaudRate(115200)
uart.enable()

rgb.init()
rgb.enable()

print('hello 2')

while True:
    time.sleep_ms(50)
    rgb.flow()
