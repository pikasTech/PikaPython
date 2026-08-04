items = []
round_index = 0
checksum = 0
while round_index < 160:
    index = 0
    while index < 8:
        items.append(index)
        index += 1
    while len(items) > 0:
        checksum += items.pop()
    round_index += 1
print(checksum, len(items))

data = bytearray(b"")
round_index = 0
while round_index < 160:
    index = 0
    while index < 8:
        data.append(index)
        index += 1
    while len(data) > 0:
        checksum += data.pop()
    round_index += 1
print(checksum, len(data))
