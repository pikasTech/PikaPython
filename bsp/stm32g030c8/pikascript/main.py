import PikaStdLib
import STM32
import PikaPiZero

p0 = PikaPiZero.Point()
p0.x = 0
p0.y = 0
print(p0.x)
mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()
oled.drawPoint(p0.x, p0.y)
while True:
    if right.read() == 1:
        print('right')
        if p0.x < 15:
            p0.x = p0.x + 1
            print(p0.x)
        oled.drawPoint(p0.x, p0.y)
    if left.read() == 0:
        if p0.x > 0:
            p0.x = p0.x - 1
        oled.drawPoint(p0.x, p0.y)
    if up.read() == 0:
        if p0.y > 0:
            p0.y = p0.y - 1
        oled.drawPoint(p0.x, p0.y)
    if down.read() == 0:
        if p0.y < 7:
            p0.y = p0.y + 1
        oled.drawPoint(p0.x, p0.y)

