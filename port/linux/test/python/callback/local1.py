
class CBTest:
    def set_cb(self, cb):
        self.cb = cb

    def call_cb(self):
        self.cb()

class ObjTest:
    def __init__(self) -> None:
        self.x = 1

    def do_event(self):
        print(self.x)

def test():
    a = ObjTest()
    cb = CBTest()
    cb.set_cb(a.do_event)
    return cb

cb = test()
cb.call_cb()
