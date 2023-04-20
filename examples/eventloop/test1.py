import time
from eventloop import EventLoop

finished = False

eventloop.set_debug(True)


def test_func(arg1, arg2):
    print("Running test function with arguments:", arg1, arg2)
    return arg1 + arg2


def test_callback(res):
    global finished
    print("Running test callback function", res)
    assert res == "Hello World"
    finished = True


# Test case 1: Add and run a one-time task
event_loop = EventLoop(period_ms=100)

event_loop.start_new_task_once(
    test_func, ("Hello", " World"), callback=test_callback)
event_loop.start()

# Sleep for enough time to allow the one-time task to run
while not finished:
    time.sleep(0.1)

event_loop.stop()
