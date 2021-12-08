import PikaStdLib
import pikaRTThread

thread = pikaRTThread.Thread()

while True:
    print('hello PikaScript!')
    thread.mdelay(500)
