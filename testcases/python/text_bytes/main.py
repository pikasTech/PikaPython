text = "ab"
print(text + "cd")
print(text * 3)
print(text * -2)
print(2 * "xy")
print(text[0])
print(text[-1])
print(text[0:2])
print("b" in text)
print(text < "ac")

data = b"\x00AZ"
print(data + b"\xff")
print(data * 2)
print(data * -1)
print(2 * b"Q")
print(data[0])
print(data[-1])
print(data[1:3])
print(b"AZ" in data)
print(65 in data)
print(data < b"\x01")

total = 0
for value in data:
    total += value
print(total)
