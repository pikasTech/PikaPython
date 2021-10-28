from PikaObj import *
import PikaStdLib
import PikaPiZero
import STM32
pin = STM32.GPIO()
pin.init()
pin.setPin('PA0')
pin.setMode('in')
pin.setPull('down')
pin.enable()
pin.setPin('PA15')
pin.setMode('in')
pin.setPull('up')
pin.enable()
pin.setPin('PC13')
pin.enable()
pin.setPin('PB6')
pin.enable()
remove('pin')
ll = STM32.lowLevel()
oled = PikaPiZero.OLED()
oled.init()
snake = PikaPiZero.Point()
snake.x = 7
snake.y = 4
snake_lengh = 0
while snake_lengh < 3:
    body = snake
    i = 0
    while i < snake_lengh:
        body = body.next
        i = i + 1
    body.next = PikaPiZero.Point()
    body.next.x = body.x - 1
    body.next.y = body.y
    body.next.prev = body
    snake_lengh = snake_lengh + 1
fruit = PikaPiZero.Point()
fruit.x = 13
fruit.y = 2
mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()
direction = 0
isUpdate = 1
while True:
    if isUpdate:
        isUpdate = 0
        if fruit.x == snake.x:
            if fruit.y == snake.y:
                body = snake
                i = 0
                while i < snake_lengh:
                    body = body.next
                    i = i + 1
                body.next = PikaPiZero.Point()
                body.next.prev = body
                snake_lengh = snake_lengh + 1
                fruit.x = fruit.x + 3
                if fruit.x > 15:
                    fruit.x = fruit.x - 15
                fruit.y = fruit.y + 3
                if fruit.y > 7:
                    fruit.y = fruit.y - 7
        body = snake
        i = 0
        while i < snake_lengh:
            body = body.next
            i = i + 1
        i = 0
        while i < snake_lengh:
            body = body.prev
            body.next.x = body.x
            body.next.y = body.y
            i = i + 1
        if direction == 0:
            snake.x = snake.x + 1
            if snake.x > 15:
                snake.x = 0
        if direction == 1:
            snake.x = snake.x - 1
            if snake.x < 0:
                snake.x = 15
        if direction == 2:
            snake.y = snake.y - 1
            if snake.y < 0:
                snake.y = 7
        if direction == 3:
            snake.y = snake.y + 1
            if snake.y > 7:
                snake.y = 0
        body = snake
        i = 0
        oled.clear()
        oled.drawPoint(fruit.x, fruit.y)
        while i < snake_lengh:
            oled.drawPoint(body.x, body.y)
            body = body.next
            i = i + 1
        oled.refresh()
    if ll.readPin('PA0') == 1:
        direction = 0
        isUpdate = 1
    if ll.readPin('PC13') == 0:
        direction = 1
        isUpdate = 1
    if ll.readPin('PA15') == 0:
        direction = 2
        isUpdate = 1
    if ll.readPin('PB6') == 0:
        direction = 3
        isUpdate = 1
