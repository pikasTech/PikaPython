import PikaStdLib
import PikaDebug
import this
import os
import time
import struct
import flashdb

print('hello pikapython!')
mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()
print('mem used now:')
mem.now()
