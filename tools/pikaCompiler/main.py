import Arm2D
import PyInfo

line = Arm2D.Line()
res = line.on()
line.off()
line.moveTo(20)

while True:
    line.off()
    line.on()
