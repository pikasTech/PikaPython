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
win.update()

print('hello pikaScript')
print('mem used max:')
mem.max()
print('mem used now:')
mem.now()

i = 0
x0 = 100
y0 = 100
while i < 100:
    x = x0 + i
    y = y0 + i
    win.elems.b1.move(x, y)
    win.update()
    i = i + 1
    print(i)
