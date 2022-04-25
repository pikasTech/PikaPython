import PikaStdLib
import PikaStdData
import ctypes
import GTestTask
import PikaMath
import PikaStdDevice

def dcrf32_test():
    rlen = ctypes.c_uint(0)
    rcvbuf = ctypes.c_wchar_p('')
    t = ctypes.Test()
    sendbuf = b'\x00\x84\x00\x00\x08'
    i = 0
    while i < 3 :
        
        t.dc_cpuapdu_hex(5, sendbuf, rlen, rcvbuf)
        t.print_rcv(rcvbuf)
        i += 1



mem = PikaStdLib.MemChecker()

num = 10
c = str(num)
s = "num = " + c
print(s)

cnt = 0
while cnt < 3:
    dcrf32_test()
    print('----------------------')
    log = 'loop '+ str(cnt) + 'mem use max:' 
    print(log)
    mem.max()
    print('----------------------')
    cnt += 1

print('mem use max:')
mem.max()