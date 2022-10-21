#!pika
import pika_lvgl as lv
import PikaStdLib
mem = PikaStdLib.MemChecker()

obj = lv.obj(lv.scr_act())
obj.set_size(150, 100)

label = lv.label(obj)
label.set_text("test")
label.center()

print('mem used max: %0.2f kB' % (mem.getMax()))
print('mem used now: %0.2f kB' % (mem.getNow()))

#!pika