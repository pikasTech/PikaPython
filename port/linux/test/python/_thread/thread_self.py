import _thread
import time


class Test:
    _val = 1

    def __init__(self):
        self._val = 2
        _thread.start_new_thread(self.init, ())

    def init(self):
        print('self._val:', self._val)
        self._val = 3

test = Test()
while test._val != 3:
    time.sleep(0.1)

time.sleep(0.5)
