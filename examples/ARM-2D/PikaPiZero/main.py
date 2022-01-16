from PikaObj import *
import PikaStdLib
import Arm2D

mem = PikaStdLib.MemChecker()
mem.max()
win = Arm2D.Window()
win.init()
win.background.setColor('white')
win.elems.box = Arm2D.Box()
win.elems.box.init()
while True:
    win.elems.box.move(20, 20)
    win.elems.box.setColor('green')
    win.update()
    win.elems.box.move(30, 50)
    win.elems.box.setColor('blue')
    win.update()
    win.elems.box.move(50,30)
    win.elems.box.setColor('red')
    win.update()
    mem.max()
    