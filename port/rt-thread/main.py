import PikaStdLib

print('hello PikaScript!')

mem = PikaStdLib.MemChecker()

print('mem used max:')
mem.max()

i = 0
while i < 100:
    i = i + 1
    print(i)