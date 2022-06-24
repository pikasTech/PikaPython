import PikaStdData
d = PikaStdData.Dict()
d['a'] = 1
d['b'] = 2
d['c'] = 'test'
print(d)
for item in d:
    print(item)
keys = d.keys()
for k in keys:
    print(k)
print(keys)
