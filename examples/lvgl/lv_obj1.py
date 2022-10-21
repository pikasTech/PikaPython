#!pika
import pika_lvgl as lv
import PikaStdLib
mem = PikaStdLib.MemChecker()

obj1 = lv.obj(lv.scr_act())
obj1.set_size(100, 50)
obj1.align(lv.ALIGN.CENTER, -60, -30)

obj2 = lv.obj(lv.scr_act())
obj2.align(lv.ALIGN.CENTER, 60, 30)

print('mem used max: %0.2f kB' % (mem.getMax()))
print('mem used now: %0.2f kB' % (mem.getNow()))

#!pika