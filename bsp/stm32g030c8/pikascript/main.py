import PikaStdLib
import STM32
import PikaPiZero

x = 0
y = 0
mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()
oled.drawPoint(x, y)
while True:
    if right.read() == 1:
        if x < 15:
            x = x + 1
        oled.drawPoint(x, y)
    if left.read() == 0:
        if x > 0:
            x = x - 1
        oled.drawPoint(x, y)
    if up.read() == 0:
        if y > 0:
            y = y - 1
        oled.drawPoint(x, y)
    if down.read() == 0:
        if y < 7:
            y = y + 1
        oled.drawPoint(x, y)

