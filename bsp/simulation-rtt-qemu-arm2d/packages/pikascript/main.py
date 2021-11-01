import PikaStdLib
import Arm2D

mem = PikaStdLib.MemChecker()

screen = Arm2D.Screen()
screen.init()
screen.background.setColor('white')

screen.elems.b1 = Arm2D.Box()
screen.elems.b1.init()
screen.elems.b1.setColor('blue')
screen.elems.b1.move(100, 100)

print('hello world')
print('mem used max:')
mem.max()
print('mem used now:')
mem.now()
