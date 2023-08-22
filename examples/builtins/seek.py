f = open('test/assets/test.jpg', 'rb')
len = f.seek(0, 2)
print(len)
f.close()