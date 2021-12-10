import PikaStdLib
import pikaRTThread

thread = pikaRTThread.Thread()
mem = PikaStdLib.MemChecker()
i = 0
while i < 3:
    i = i + 1
    print('hello PikaScript!')
    thread.mdelay(500)

print('mem use max:')
mem.max()
