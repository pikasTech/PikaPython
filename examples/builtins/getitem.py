
class Test:
    def __getitem__(self, key):
        return key


t = Test()
assert t[1] == 1
assert t['a'] == 'a'
print("PASS")
