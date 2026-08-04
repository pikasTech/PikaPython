data = b"abcdef"
print(len(b""), repr(b""))
print(data[-99:99])
print(data[99:])
print(data[::-1])
print(data[4:1:-2])
print(b"" in data, b"bc" in data)
print(97 in data, 255 in data)
print(b"ab" * -1, b"ab" * 0, b"ab" * 3)
