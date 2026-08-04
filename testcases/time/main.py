from time import sleep, sleep_ms, ticks_diff, ticks_ms


start = ticks_ms()
sleep(0.0011)
middle = ticks_ms()
print(ticks_diff(middle, start))

sleep_ms(duration=25)
stop = ticks_ms()
print(ticks_diff(new=stop, old=middle))

print(ticks_diff(3, 4294967294))
print(ticks_diff(4294967294, 3))

boolean_start = ticks_ms()
sleep(True)
boolean_stop = ticks_ms()
print(ticks_diff(boolean_stop, boolean_start))
print("time-case: ok")
