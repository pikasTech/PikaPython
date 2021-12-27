import PikaStdLib
import pikaRTThread

thread = pikaRTThread.Thread()
mem = PikaStdLib.MemChecker()

for i in range(0, 3):
    print('hello PikaScript!')
    thread.mdelay(500)

print('mem use max:')
mem.max()
