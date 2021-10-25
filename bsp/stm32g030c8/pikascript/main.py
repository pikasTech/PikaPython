import PikaStdLib

mem = PikaStdLib.MemChecker()
print('hello 2')
print('mem used max:')
mem.max() 

i = 0
while i < 100:
    i = i + 1
    print(i)

print('Hello PikaScript!')
