import pika_lvgl as lv
import PikaStdLib
mem = PikaStdLib.MemChecker()

LV_ROLLER_MODE_INFINITE = 1
roller1 = lv.roller(lv.scr_act())
roller1.set_options("January\nFebruary\nMarch\nApril\
\nMay\nJune\nJuly\nAugust\nSeptember\
\nOctober\nNovember\nDecember", LV_ROLLER_MODE_INFINITE)

roller1.set_visible_row_count(4)
roller1.center()

print('mem used max: %0.2f kB' % (mem.getMax()))
print('mem used now: %0.2f kB' % (mem.getNow()))
