from machine import Pin, Timer, UART

pin_hits = 0
timer_hits = 0
uart_hits = 0


def on_pin(source):
    global pin_hits
    pin_hits += 1


def on_timer(source):
    global timer_hits
    timer_hits += 2


def on_uart(source):
    global uart_hits
    uart_hits += 3


pin = Pin(4, mode=Pin.IN)
pin.irq(handler=on_pin)
pin.trigger()
pin.trigger()

timer = Timer(0)
timer.init(callback=on_timer)
timer.trigger()

uart = UART(2, baudrate=115200)
uart.irq(handler=on_uart)
uart.trigger()

print(Pin.IN)
print(Timer.PERIODIC)
print(pin_hits)
print(timer_hits)
print(uart_hits)
