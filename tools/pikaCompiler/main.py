import Arm2D
import PyInfo

line = Arm2D.Line()
res = line.on()
line.off()
line.moveTo(20)

while True:
    if right.read() == 1:
        print('right')
    if left.read() == 0:
        print('left')
    if up.read() == 0:
        print('up')
    if down.read() == 0:
        print('down')