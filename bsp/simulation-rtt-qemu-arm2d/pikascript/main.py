import PikaStdLib
import Arm2D

mem = PikaStdLib.MemChecker()

win = Arm2D.Window()
win.init()
win.background.setColor('white')

win.elems.b1 = Arm2D.Box()
win.elems.b1.init()
win.elems.b1.setColor('blue')
win.elems.b1.move(100, 100)
i = 0
x0 = 100
y0 = 100
sizeX0 = 50
sizeY0 = 50
alpha0 = 180
isIncrace = 1
loopTimes = 0

print('hello pikaScript')
print('mem used max:')
mem.max()
print('mem used now:')
mem.now()
while True:
    win.elems.b1.move(x0 + i * 2, y0 + i * 1)
    win.elems.b1.setAlpha(alpha0 - i * 1)
    win.elems.b1.setSize(sizeX0 + i * 2, sizeY0 + i * 1)
    win.update()
    if isIncrace > 0:
        i = i + 1
        if i > 160:
            isIncrace = 0
    if isIncrace < 1:
        i = i - 1
        if i < 0:
            isIncrace = 1
            loopTimes = loopTimes + 1
