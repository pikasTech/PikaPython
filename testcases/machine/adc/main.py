from machine import ADC, Pin


first = ADC(0)
second = ADC(1)
from_pin = ADC(Pin(0))


def is_u16(value):
    return value >= 0 and value <= 65535


print(is_u16(first.read_u16()))
print(is_u16(second.read_u16()))
print(is_u16(from_pin.read_u16()))

try:
    first.read_uv()
except RuntimeError:
    print("read_uv")
