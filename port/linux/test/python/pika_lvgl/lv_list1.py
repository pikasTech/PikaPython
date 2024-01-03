#!pika
import pika_lvgl as lv
import PikaStdLib
mem = PikaStdLib.MemChecker()

# Create a normal drop down list
dd = lv.dropdown(lv.scr_act())
dd.set_options("Apple\nBanana\nOrange\nCherry\nGrape\
Raspberry\nMelon\nOrange\nLemon\nNuts")

dd.align(lv.ALIGN.TOP_MID, 0, 20)

print('mem used max: %0.2f kB' % (mem.getMax()))
print('mem used now: %0.2f kB' % (mem.getNow()))

#!pika