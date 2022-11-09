import PikaStdLib as std
import pika_lvgl as lv
mem = std.MemChecker()
print('hello SWM320')
print('mem used max:')
mem.max()
print('mem.used now:')
mem.now()