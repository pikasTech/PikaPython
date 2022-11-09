#!pika
import pika_lvgl as lv
import PikaStdLib

mem = PikaStdLib.MemChecker()

bar1 = lv.bar(lv.scr_act())
bar1.set_size(200, 20)
bar1.align(lv.ALIGN.CENTER,0 ,0)
bar1.set_value(70, lv.ANIM.OFF)

print('mem used max: %0.2f kB' % (mem.getMax()))
print('mem used now: %0.2f kB' % (mem.getNow()))
exit()
#!pika
