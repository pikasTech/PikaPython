from PikaObj import *
import PikaStdLib
import PikaPiZero
import STM32G0

lcd = PikaPiZero.LCD()
lcd.init()
lcd.clear('white')
mem = PikaStdLib.MemChecker()
key = PikaPiZero.KEY()
key.init()
time = STM32G0.Time()
h = 10
w = 10
x = 10
y = 10
x_last = x
y_last = y
is_update = 0
print('mem used max:')
mem.max()
lcd.fill(x, y, w, h, 'blue')
while True:
    if key.get() == 0:
        is_update = 1
        x_last = x
        y_last = y
        x = x + 5
    if key.get() == 1:
        is_update = 1
        x_last = x
        y_last = y
        y = y - 5
    if key.get() == 2:
        is_update = 1
        x_last = x
        y_last = y
        y = y + 5
    if key.get() == 3:
        is_update = 1
        x_last = x
        y_last = y
        x = x - 5
    if is_update:
        is_update = 0
        lcd.fill(x_last, y_last, w, h, 'white')
        lcd.fill(x, y, w, h, 'blue')
