# 测试用例
assert ''.join([]) == ''
assert ','.join(['a']) == 'a'
assert ','.join(['a', 'b']) == 'a,b'
assert ''.join(['a', 'b', 'c']) == 'abc'
assert '-'.join(['a', 'b', 'c']) == 'a-b-c'
assert ' '.join(['hello', 'world']) == 'hello world'
print("PASS")