from PikaStdLib import MemChecker as MC
import PikaStdLib
from PikaObj import *
import PikaStdData
import GTestTask
import PikaMath
import PikaStdDevice
import PikaDebug
import ctypes
import PikaStdData

mem = PikaStdLib.MemChecker()

def dcrf32_test():
    rlen = ctypes.c_uint(0)
    rcvbuf = ctypes.c_wchar_p('')
    hdl = ctypes.Test()

    i = 0
    sendbuf = b'\x00\x84\x00\x00\x08'
    while i < 50 :
        hdl.dc_cpuapdu_hex(5, sendbuf, rlen, rcvbuf)
        # hdl.print_rcv(rcvbuf)
        # rlen.value
        i += 1
        mem.max()


dcrf32_test()

print('mem use max:')
mem.max()
