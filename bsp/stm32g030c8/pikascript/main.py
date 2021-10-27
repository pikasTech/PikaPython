import PikaStdLib
import PikaPiZero
import STM32

right = STM32.GPIO()
right.init()
right.setPin('PA0')
right.setMode('in')
right.setPull('down')
right.enable()
left = STM32.GPIO()
left.init()
left.setPin('PC13')
left.setMode('in')
left.setPull('up')
left.enable()
down = STM32.GPIO()
down.init()
down.setPin('PB6')
down.setMode('in')
down.setPull('up')
down.enable()
up = STM32.GPIO()
up.init()
up.setPin('PA15')
up.setMode('in')
up.setPull('up')
up.enable()
oled = PikaPiZero.OLED()
oled.init()

p0 = PikaPiZero.Point()
p0.x = 0
p0.y = 0
p1 = PikaPiZero.Point()
p1.x = 0
p1.y = 0
p2 = PikaPiZero.Point()

pos = 0
isUpdate = 1
while True:
    if isUpdate:
        isUpdate = 0
        p2.x = p1.x
        p2.y = p1.y
        p1.x = p0.x
        p1.y = p0.y
        if pos == 0:
            p0.x = p0.x + 1
            if p0.x > 15:
                p0.x = 0
        if pos == 1:
            p0.x = p0.x - 1
            if p0.x < 0:
                p0.x = 15
        if pos == 2:
            p0.y = p0.y - 1
            if p0.y < 0:
                p0.y = 7
        if pos == 3:
            p0.y = p0.y + 1
            if p0.y > 7:
                p0.y = 0
        oled.clear()
        oled.drawPoint(p0.x, p0.y)
        oled.drawPoint(p1.x, p1.y)
        oled.drawPoint(p2.x, p2.y)
    if right.read() == 1:
        pos = 0
        isUpdate = 1
    if up.read() == 0:
        pos = 2
        isUpdate = 1

