import PikaStdLib
import STM32

time = STM32.Time()

while True:
    time.sleep_ms(500)
    print('0.5s')
    time.sleep_s(1)
    print('1s')