# SPEC: PJ2026-0501; builtin iterable directory regression.
total = 0
for value in [1, 2]:
    total = total + value
for value in (3, 4):
    total = total + value
for value in b"AB":
    total = total + value
for value in bytearray(b"CD"):
    total = total + value

for character in "xy":
    print(character)

mapping = dict()
mapping["a"] = 5
mapping["b"] = 6
for key in mapping:
    print(key)

print(total)
