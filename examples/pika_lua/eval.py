import pika_lua

# 测试数字
assert pika_lua.evals('return 1 + 1') == 2

# 测试字符串
assert pika_lua.evals('return "hello"') == "hello"

# 测试布尔值
assert pika_lua.evals('return true') == True

# 测试nil
assert pika_lua.evals('return nil') == None

# 测试变量赋值
pika_lua.evals('x = 10')
assert pika_lua.evals('return x') == 10

# 测试函数调用
pika_lua.evals('function add(a, b) return a + b end')
assert pika_lua.evals('return add(1, 2)') == 3

# table to list
l = pika_lua.evals('return {1, 2, 3}')
assert l[0] == 1
assert l[1] == 2
assert l[2] == 3

# table to dict
d = pika_lua.evals('return {a = 1, b = 2, c = 3}')
assert d['a'] == 1
assert d['b'] == 2
assert d['c'] == 3

print('PASS')
