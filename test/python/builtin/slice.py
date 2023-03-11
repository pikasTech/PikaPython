my_string = "Hello, World!"
out_of_range = False

# 测试索引超出字符串长度的情况
try:
    my_string[len(my_string)] == ""
except:
    print("1.IndexError: 索引超出字符串长度")
    out_of_range = True

assert out_of_range

# 测试切片超出字符串长度的情况
b = my_string[0:100]
print(b)

# 测试使用负数索引访问字符串的情况
assert my_string[-1] == "!"
assert my_string[-5:-1] == "orld"
print("PASS")
