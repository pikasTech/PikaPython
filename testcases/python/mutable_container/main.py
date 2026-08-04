# SPEC: PJ2026-0501; mutable container CPython regression.
items = [1, 2]
alias = items
alias[0] = 7
alias.append(3)
print(items)
print(items.pop())
print(items)

mapping = dict()
mapping["first"] = 4
mapping["second"] = 5
print(mapping["first"])
print(mapping.pop("second"))
print(mapping.pop("missing", 9))
print(len(mapping))

data = bytearray(b"ab")
data_alias = data
data_alias[1] = 99
data_alias.append(100)
print(data[1])
print(data.pop())
print(data)
