from PikaObj import *
import Device
import PikaStdLib
import PikaStdData

mem = PikaStdLib.MemChecker()

print('mem used max:')
mem.max()

def EXPECT_EQ(test_name, input, expected):
    print('-----TEST-----')
    print(test_name)
    if input != expected:
        print('[Error]')
        print(test_name)
        print('[info] Expected:')
        print(expected)
        print('[info] Input:')
        print(input)
        # error
        while True:
            is_error = 1
    else:
        print('[ OK  ]')
        print('')

a = 0
for i in range(0, 10):
    if i == 5:
        continue
    a = a + i
EXPECT_EQ('for_if_continue', a, 40)
         
a = 0
for i in range(0, 10):
    if i == 5:
        break
    a = a + i
EXPECT_EQ('for_if_break', a, 10)

a = 0
for i in range(0, 10):
    for k in range(0, 3):
        # print(k)
        a = a + k
EXPECT_EQ('for_for_in_range', a, 30)

a = 0
for i in range(0, 10):
    # print(i)
    a = a + i
EXPECT_EQ('for_in_range', a, 45)

list = PikaStdData.List()
list.append(7)
list.append('eee')
len = list.len()
for item in list:
    # print(item)
    a = item
EXPECT_EQ('list_for_loop', a, 'eee')


list = PikaStdData.List()
list.append(7)
list.append('eee')
len = list.len()
a1 = list.get(0)
a2 = list.get(1)
res = iter(list)
EXPECT_EQ('iter', res, list)

dict = PikaStdData.Dict()
dict.set('a', 1)
dict.set('b', 2)
dict.remove('b')
a = dict.get('a')
b = dict.get('b')
EXPECT_EQ('PikaStdData', a, 1)
EXPECT_EQ('PikaStdData_2', b, 0)