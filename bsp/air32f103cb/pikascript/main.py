import PikaStdLib
import machine

print('[Info] Air32F103 system init OK.')
print('hello PikaScript!')

def gpio_init(pin, mode):
    io = machine.GPIO()
    io.setPin(pin)
    io.setMode('out')
    io.enable()
    return io

led1 = gpio_init('PB2', 'out')
led2 = gpio_init('PB10', 'out')
led3 = gpio_init('PB11', 'out')
time = machine.Time()

led_loop_num = 10
for i in range(0, led_loop_num):
    print('led loop: [%d/%d]' % (i + 1, led_loop_num))
    led1.high()
	led2.low()
	led3.low()
	time.sleep_ms(200)
	led1.low()
	led2.high()
	led3.low()
	time.sleep_ms(200)
	led1.low()
	led2.low()
	led3.high()
	time.sleep_ms(200)


mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()
print('mem used now:')
mem.now()
