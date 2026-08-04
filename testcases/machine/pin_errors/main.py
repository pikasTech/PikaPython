from machine import Pin


try:
    Pin(-1)
except ValueError:
    print("identifier")

try:
    Pin(1, mode=99)
except ValueError:
    print("mode")

try:
    Pin(1, mode=Pin.OUT, pull=Pin.PULL_UP)
except ValueError:
    print("pull")

try:
    Pin(1, mode=Pin.IN, value=1)
except ValueError:
    print("initial")

pin = Pin(1, mode=Pin.OUT)
try:
    pin.value(2)
except ValueError:
    print("level")

pin.value(1)
print(pin.value())
