import PikaStdLib
import Arm2D

mem = PikaStdLib.MemChecker()

win = Arm2D.Window()
win.background.setColor(Arm2D.COLOR_WHITE)

win.elems.b1 = Arm2D.Box()
win.elems.b1.setColor(Arm2D.COLOR_BLUE)
win.elems.b1.move(100, 100)
i = 0
x0 = 100
y0 = 100
sizeX0 = 50
sizeY0 = 50
alpha0 = 180
isIncrace = 1
loopTimes = 0

print('mem used max:', int(mem.getMax() * 1024), 'b')
print('mem used now:', int(mem.getNow() * 1024), 'b')

star = Arm2D.Star()

fill_x = 0
fill_y = 0


def callback(frameBuffer, isNewFrame):
    global fill_x
    global fill_y
    res = Arm2D.fill_colour(
        frameBuffer,
        Arm2D.create_region(fill_x, fill_y, 100, 100),
        Arm2D.COLOR_RED
    )
    res = Arm2D.tile_copy(
        star,
        frameBuffer,
        Arm2D.create_region(100 - fill_x, 200, 100, 100),
        Arm2D.CP_MODE_COPY
    )
    res = Arm2D.tile_rotation(
        star,
        frameBuffer,
        Arm2D.create_region(150, 150, 100, 100),
        Arm2D.create_location(28, 28),
        fill_x * 0.1,
        Arm2D.COLOR_WHITE
    )
    if isNewFrame:
        fill_x += 1
        fill_y += 2
        if fill_x > 50:
            fill_x = 0
        if fill_y > 50:
            fill_y = 0


win.addCallBack(callback)

while True:
    win.elems.b1.move(x0 + i * 2, y0 + i * 1)
    win.elems.b1.setAlpha(alpha0 - i * 1)
    win.elems.b1.setSize(sizeX0 + i * 2, sizeY0 + i * 1)
    if isIncrace > 0:
        i = i + 1
        if i > 160:
            isIncrace = 0
    if isIncrace < 1:
        i = i - 1
        if i < 0:
            isIncrace = 1
            loopTimes = loopTimes + 1
    Arm2D.update()
