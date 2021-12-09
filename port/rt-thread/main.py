import PikaStdLib
import pikaRTThread

thread = pikaRTThread.Thread()
i = 0
while i < 3:
    i = i + 1
    print('hello PikaScript!')
    thread.mdelay(500)
