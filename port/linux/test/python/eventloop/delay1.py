import time
from eventloop import EventLoop

run_time = 0

eventloop.set_debug(True)


def test_func(arg1, arg2):
    global run_time
    run_time += 1
    print("Running test function with arguments:", arg1, arg2)
    return arg1 + arg2


def test_func2(arg1, arg2):
    print("test function 2 with arguments:", arg1, arg2)
    return arg1 + arg2


def test_callback(res):
    print("Running test callback function", res)
    assert res == "Hello World"


# Test case 2: Add and run a periodic task

eventloop.start_new_task_once(
    test_func, ("Hello", " World"),
    callback=test_callback,
    delay_ms=200
)

eventloop.start_new_task(
    test_func2, ("Hello", " World"),
    is_periodic=True,
    period_ms=20,
    callback=test_callback
)

# Sleep for enough time to allow the periodic task to run multiple times
while run_time < 1:
    time.sleep(0.1)
