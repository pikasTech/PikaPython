import PikaStdLib
import PikaStdDevice
import PikaStdData
import GTestTask
import PikaMath
import PikaDebug
from PikaObj import *

pkdb = PikaDebug.Debuger()

i = 0
while i < 10:
    i = i + 1
    print('i :' + str(i))
#    pkdb.set_trace()

