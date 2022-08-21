import pika_lvgl as lv
import PikaStdLib
mem = PikaStdLib.MemChecker()

sw = lv.switch(lv.scr_act())
sw.align(lv.ALIGN.TOP_MID, 0, 20)

sw = lv.switch(lv.scr_act())
sw.add_state(lv.STATE.CHECKED)
sw.align(lv.ALIGN.TOP_MID, 0, 50)

sw = lv.switch(lv.scr_act())
sw.add_state(lv.STATE.DISABLED)
sw.align(lv.ALIGN.TOP_MID, 0, 80)

sw = lv.switch(lv.scr_act())
sw.add_state(lv.STATE.CHECKED | lv.STATE.DISABLED)
sw.align(lv.ALIGN.TOP_MID, 0, 110)

print('mem used max: %0.2f kB' % (mem.getMax()))
print('mem used now: %0.2f kB' % (mem.getNow()))
