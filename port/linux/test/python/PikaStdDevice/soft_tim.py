from PikaStdDevice import Timer
import time

tim = Timer()
cb_times = 0
def cb_test(signal):
    global cb_times
    if cb_times > 0:
        return
    cb_times += 1
    print("cb_test: signal = %d, cb_times = %d" % (signal, cb_times))

tim.setCallback(cb_test, Timer.SIGNAL_ANY)
tim.setId(-1) # -1 means soft timer
tim.setPeriod(100) # 100ms
tim.enable()

while True:
    time.sleep(1)
    print("cb_times = %d" % cb_times)
    if cb_times >= 1:
        tim.close()
        break
