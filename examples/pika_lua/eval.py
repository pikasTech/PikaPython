import pika_lua

# 测试数字
assert pika_lua.eval('return 1 + 1') == 2

# 测试字符串
assert pika_lua.eval('return "hello"') == "hello"

# 测试布尔值
assert pika_lua.eval('return true') == True

# 测试nil
assert pika_lua.eval('return nil') == None

# 测试变量赋值
pika_lua.eval('x = 10')
assert pika_lua.eval('return x') == 10

# 测试函数调用
pika_lua.eval('function add(a, b) return a + b end')
assert pika_lua.eval('return add(1, 2)') == 3

print('PASS')
