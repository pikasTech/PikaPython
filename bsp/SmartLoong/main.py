import PikaStdLib
import pikaRTThread
import SmartLoong
thread = pikaRTThread.Thread()
mem = PikaStdLib.MemChecker()
print('mem use max(byte): ')
print(int(mem.getMax() * 1000))

led1 = SmartLoong.LED()
led1.setId(1)
led1.enable()
led1.on()

key1 = SmartLoong.KEY()
key1.setId(1)
key1.enable()

i = 0
while i < 3:
    i = i + 1
    led1.on()
    thread.mdelay(500)
    led1.off()
    thread.mdelay(500)

num_press = 0
led_is_on = 0
while num_press < 3:
    if key1.read() == 0:
        num_press = num_press + 1
        print('press num:')
        print(num_press)
        if led_is_on:
            led1.off()
            led_is_on = 0
            thread.mdelay(500)
        else:
            led1.on()
            led_is_on = 1
            thread.mdelay(500)
    thread.mdelay(50)
