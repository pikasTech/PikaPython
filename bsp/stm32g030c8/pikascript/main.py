import PikaStdLib
import PikaPiZero
import STM32

right = STM32.GPIO()
right.init()
right.setPin('PA0')
right.setMode('in')
right.setPull('down')
right.enable()
up = STM32.GPIO()
up.init()
up.setPin('PA15')
up.setMode('in')
up.setPull('up')
up.enable()
oled = PikaPiZero.OLED()
oled.init()

p0 = PikaPiZero.Point()
p0.x = 7
p0.y = 4
p_next_num = 0

k = 0
while k < 6:
    p = p0
    i = 0
    while i < p_next_num:
        p = p.next
        i = i + 1
    p.next = PikaPiZero.Point()
    p.next.prev = p
    p_next_num = p_next_num + 1
    k = k + 1


mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()

pos = 0
isUpdate = 1
while True:
    if isUpdate:
        isUpdate = 0
        p = p0
        i = 0
        while i < p_next_num:
            p = p.next
            i = i + 1
        i = 0
        while i < p_next_num:
            p = p.prev
            p.next.x = p.x
            p.next.y = p.y
            i = i + 1
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
        p = p0
        i = 0
        oled.clear()
        while i < p_next_num:
            oled.drawPoint(p.x, p.y)
            p = p.next
            i = i + 1
        oled.refresh()
    if right.read() == 1:
        pos = 0
        isUpdate = 1
    if up.read() == 0:
        pos = 2
        isUpdate = 1
