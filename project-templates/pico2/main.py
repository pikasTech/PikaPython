from machine import Pin

led = Pin(25, Pin.OUT, value=0)
led.on()
print("PikaPython on Raspberry Pi Pico 2")
