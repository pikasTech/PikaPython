import PikaStdLib
import CH582

io1 = CH582.GPIO()
time = CH582.Time()

io1.init()
io1.setPin('PB8')
io1.setMode('out')
io1.enable()
io1.low()

print('hello PikaScript!')
mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()

while True:
    io1.low()
    print("low")
    time.sleep_ms(1000)
    io1.high()
    print("high")
    time.sleep_ms(1000)
