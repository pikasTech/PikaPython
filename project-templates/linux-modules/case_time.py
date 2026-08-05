from time import sleep, sleep_ms, ticks_diff, ticks_ms


print("sleep-zero:", sleep(0) is None)
print("sleep-bool:", sleep(False) is None)
try:
    sleep(-1)
    print("sleep-negative: accepted")
except ValueError:
    print("sleep-negative: rejected")

start = ticks_ms()
sleep(0.0011)
middle = ticks_ms()
print("extension-sleep:", ticks_diff(middle, start))
sleep_ms(duration=25)
stop = ticks_ms()
print("extension-sleep-ms:", ticks_diff(stop, middle))
print("extension-wrap-positive:", ticks_diff(3, 4294967294))
print("extension-wrap-negative:", ticks_diff(4294967294, 3))
