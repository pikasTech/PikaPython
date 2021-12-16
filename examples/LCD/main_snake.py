from PikaObj import *
import PikaStdLib
import PikaPiZero
import STM32G0

# hardware init
lcd = PikaPiZero.LCD()
lcd.init()
lcd.clear('white')
key = PikaPiZero.KEY()
key.init()
time = STM32G0.Time()
x_max = 120
y_max = 150

# snake init
s = PikaPiZero.Point()
w = 9
h = 9
s.x = 50
s.y = 10
len = 0
while len < 3:
    b = s
    i = 0
    while i < len:
        b = b.next
        i = i + 1
    b.next = PikaPiZero.Point()
    b.next.x = b.x - 10
    b.next.y = b.y
    b.next.prev = b
    len = len + 1
# ring link
b.next = s
s.prev = b

i = 0
b = s
while i < len:
    lcd.fill(b.x, b.y, w, h, 'blue')
    b = b.next
    i = i + 1

print('snake lengh')
print(len)

# fruit init
f = PikaPiZero.Point()
f.x = 30
f.y = 20
lcd.fill(f.x, f.y, w, h, 'green')

# memory check
mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()

# main loop
d = 0
isUpdate = 1
while True:
    if isUpdate:
        isUpdate = 0
        # check eat fruit
        if f.x == s.x:
            if f.y == s.y:
                # have eat fruit
                b = s
                i = 0
                while i < len:
                    b = b.next
                    i = i + 1
                b.next = PikaPiZero.Point()
                b.next.prev = b
                len = len + 1
                print('snake lengh')
                print(len)
                print('mem used max:')
                mem.max()
                f.x = f.x + 30
                if f.x > x_max:
                    f.x = f.x - x_max
                f.y = f.y + 30
                if f.y > y_max:
                    f.y = f.y - y_max
                lcd.fill(f.x, f.y, w, h, 'green')
        # move snake by the direction
        if d == 0:
            x_new = s.x + 10
            y_new = s.y
            if x_new > x_max:
                x_new = 0
        if d == 1:
            x_new = s.x
            y_new = s.y - 10
            if y_new < 0:
                y_new = y_max
        if d == 2:
            x_new = s.x
            y_new = s.y + 10
            if y_new > y_max:
                y_new = 0
        if d == 3:
            x_new = s.x - 10
            y_new = s.y
            if x_new < 0:
                x_new = x_max
        # drow the snake and fruit
        # clear last body
        lcd.fill(s.prev.x, s.prev.y, w, h, 'white')
        # new body
        s.prev.x = x_new
        s.prev.y = y_new
        # head is last body
        s = s.prev
        lcd.fill(s.x, s.y, w, h, 'blue')
        b = s
        i = 0
    key_val = key.get()
    if key_val == 0:
        d = 0
        isUpdate = 1
    if key_val == 1:
        d = 1
        isUpdate = 1
    if key_val == 2:
        d = 2
        isUpdate = 1
    if key_val == 3:
        d = 3
        isUpdate = 1
