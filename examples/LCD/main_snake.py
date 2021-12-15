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
w = 10
h = 10
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

# fruit init
f = PikaPiZero.Point()
f.x = 30
f.y = 20

# memory check
mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()

# main loop
d = 0
isUpdate = 1
while True:
    if isUpdate:
        print('is update')
        isUpdate = 0
        # check eat fruit
        if f.x == s.x:
            if f.y == s.y:
                b = s
                i = 0
                while i < len:
                    b = b.next
                    i = i + 1
                b.next = PikaPiZero.Point()
                b.next.prev = b
                len = len + 1
                f.x = f.x + 30
                if f.x > x_max:
                    f.x = f.x - x_max
                f.y = f.y + 30
                if f.y > y_max:
                    f.y = f.y - y_max
        # update snake body
        print('update snake body')
        b = s
        i = 0
        while i < len:
            b = b.next
            i = i + 1
        i = 0
        while i < len:
            b = b.prev
            b.next.x = b.x
            b.next.y = b.y
            i = i + 1
        if d == 0:
            s.x = s.x + 10
            if s.x > x_max:
                s.x = 0
        if d == 1:
            s.y = s.y - 10
            if s.y < 0:
                s.y = y_max
        if d == 2:
            s.y = s.y + 10
            if s.y > y_max:
                s.y = 0
        if d == 3:
            s.x = s.x - 10
            if s.x < 0:
                s.x = x_max
        b = s
        i = 0
        lcd.clear('white')
        print('fruit.x')
        print(f.x)
        print('fruit.y')
        print(f.y)
        lcd.fill(f.x, f.y, w, h, 'green')
        print('snake lengh')
        print(len)
        while i < len:
            lcd.fill(b.x, b.y, w, h, 'blue')
            b = b.next
            i = i + 1
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
