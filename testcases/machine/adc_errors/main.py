from machine import ADC


try:
    ADC(-1)
except ValueError:
    print("negative")

try:
    ADC(16)
except ValueError:
    print("range")

try:
    ADC("0")
except TypeError:
    print("type")

adc = ADC(1)
try:
    adc.read_uv()
except RuntimeError:
    print("read_uv")

print(adc.read_u16())
