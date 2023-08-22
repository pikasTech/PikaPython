import eventloop
from PikaStdLib import MemChecker
import time

eventloop._is_debug = True
expect_finished = 10
finished = 0

def test_func(arg1, arg2):
    global finished
    finished += 1
    print("finished:", finished)
    print("Running test function with arguments:", arg1, arg2)
    MemChecker().now()


MemChecker().now()
for i in range(expect_finished):
    eventloop.start_new_task_once(
        test_func, ("Hello", " World"), delay_ms=10)
MemChecker().now()

while finished < expect_finished:
    time.sleep(0.1)
eventloop.stop()
