from machine import Pin


print(Pin.IN)
print(Pin.OUT)
print(Pin.OPEN_DRAIN)
print(Pin.PULL_UP)
print(Pin.PULL_DOWN)

pin = Pin(2, mode=Pin.OUT, value=1)
print(pin.value())
print(pin.off())
print(pin.value())
print(pin.value(True))
print(pin.value())

print(pin.init(mode=Pin.OPEN_DRAIN, pull=Pin.PULL_UP, value=1))
print(pin.value())
print(pin.off())
print(pin.value())

print(pin.init(mode=Pin.IN, pull=Pin.PULL_DOWN))
print(pin.value())
