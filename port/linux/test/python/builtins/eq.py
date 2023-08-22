assert [1, 2, 3] == [1, 2, 3]
assert [1, 2, 3] != [1, 2, 4]

assert (1, 2, 3) == (1, 2, 3)
assert (1, 2, 3) != (1, 2, 4)

assert (1, 2, 3) != [1, 2, 3]
assert [1, 2, 3] != (1, 2, 3)
assert [1, 2, 3] != 1

assert 1 != (1, 2, 3)

assert ('a', 'b', 'c') == ('a', 'b', 'c')
assert ('a', 'b', 'c') != ('a', 'b', 'd')

dict1 = {'a': 1, 'b': 2, 'c': 3}
dict2 = {'a': 1, 'b': 2, 'c': 3}
dict3 = {'a': 1, 'b': 2, 'c': 4}

assert dict1 == dict2
assert dict1 != dict3

print("PASS")
