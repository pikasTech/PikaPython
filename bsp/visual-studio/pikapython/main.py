import PikaStdLib

print('hello pikapython')
for i in range(100):
    print(i)

def test():
    print('test2')

test()

mem = PikaStdLib.MemChecker()

for i in range(10):
    mem.max()
    mem.now()

test()
