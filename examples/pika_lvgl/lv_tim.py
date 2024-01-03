import pika_lvgl as lv
import time
from PikaStdLib import MemChecker

mem = MemChecker()


def date_time_update_cb(timer):
    asctime = time.asctime()
    label.set_text(asctime)
    mem.now()
    print(asctime)


def label_event_cb(e):
    code = e.get_code()
    print('label_event_cb:', code)


label = lv.label(lv.scr_act())
label.add_event_cb(label_event_cb, lv.EVENT.ALL, None)

timer = lv.timer_create_basic()
timer.set_period(1)
timer.set_cb(date_time_update_cb)


for i in range(10):
    time.sleep(0.1)
    lv.task_handler()

lv.deinit()
