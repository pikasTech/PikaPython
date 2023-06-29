import PikaStdLib
import PikaStdData as std
import re

s2 = std.String('你好，Hello, Bonjour.')

i = 0
for c in s2:
    print(i, c)
    i += 1
print(s2[2])
s2[2] = ','
print(s2)
s2[2] = '，'
print(s2)
print(s2.replace('你好，', 'Hi, '))


