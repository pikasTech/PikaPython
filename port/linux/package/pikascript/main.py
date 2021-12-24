import PikaStdLib
import PikaStdDevice
import PikaStdData
import PikaMath
from PikaObj import *

list = PikaStdData.List()
i = 0
while i < 10:
    list.append(i)
    i = i + 1
for item in list:
    print(item)
