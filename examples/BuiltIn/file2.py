f = open('test/python/main.py', 'r')

s = f.readline()
print(s)

lines = f.readlines()
print(lines)

f.close()
