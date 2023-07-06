
class Test:
    val = 1

    def cb(self):
        print(self.val)
        self.val += 1


t = Test()
fn = getattr(t, 'cb')
fn()
print(t.val)
