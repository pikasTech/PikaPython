#!pika
import pika_lvgl as lv
import PikaStdLib
mem = PikaStdLib.MemChecker()

# Create a slider in the center of the display
slider = lv.slider(lv.scr_act())
slider.center()

# Create a label below the slider
slider_label = lv.label(lv.scr_act())
slider_label.set_text("0%")

slider_label.align_to(slider, lv.ALIGN.OUT_BOTTOM_MID, 0, 10)

print('mem used max: %0.2f kB' % (mem.getMax()))
print('mem used now: %0.2f kB' % (mem.getNow()))

#!pika