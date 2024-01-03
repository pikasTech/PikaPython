#!pika
import pika_lvgl as lv
import PikaStdLib
mem = PikaStdLib.MemChecker()

# create a simple button
btn1 = lv.btn(lv.scr_act())

btn1.align(lv.ALIGN.CENTER,0,-40)
label=lv.label(btn1)
label.set_text("Button")

# create a toggle button
btn2 = lv.btn(lv.scr_act())

btn2.align(lv.ALIGN.CENTER,0,40)
btn2.add_flag(lv.obj.FLAG.CHECKABLE)
btn2.set_height(lv.SIZE.CONTENT)

label=lv.label(btn2)
label.set_text("Toggle")
label.center()

print('mem used max: %0.2f kB' % (mem.getMax()))
print('mem used now: %0.2f kB' % (mem.getNow()))

#!pika