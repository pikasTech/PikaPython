# 测试 max() 函数
assert max(1, 2, 3) == 3, "max() 函数错误: max(1, 2, 3) 应返回 3"
assert max(-1, -2, -3) == -1, "max() 函数错误: max(-1, -2, -3) 应返回 -1"
assert max(5, 5) == 5, "max() 函数错误: max(5, 5) 应返回 5"
assert max([1, 2, 3, 4]) == 4, "max() 函数错误: max([1, 2, 3, 4]) 应返回 4"

# 测试 min() 函数
assert min(1, 2, 3) == 1, "min() 函数错误: min(1, 2, 3) 应返回 1"
assert min(-1, -2, -3) == -3, "min() 函数错误: min(-1, -2, -3) 应返回 -3"
assert min(5, 5) == 5, "min() 函数错误: min(5, 5) 应返回 5"
assert min([1, 2, 3, 4]) == 1, "min() 函数错误: min([1, 2, 3, 4]) 应返回 1"

print('PASS')
