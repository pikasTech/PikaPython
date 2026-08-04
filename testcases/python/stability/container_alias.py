items = [1, 2]
alias = items
alias[-1] = 7
alias.append(9)
print(items)
print(items.pop(), alias)

mapping = dict()
mapping[False] = "false"
mapping[0] = "zero"
mapping[True] = "true"
mapping[1] = "one"
print(len(mapping), mapping[False], mapping[True])

data = bytearray(b"ab")
data_alias = data
data_alias[0] = 0
data_alias[1] = 255
data_alias.append(128)
print(data[0], data[1], data[-1])
print(data.pop(), len(data_alias))
