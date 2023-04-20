import time
import eventloop

run_time = 0

eventloop.set_debug(True)


def test_func(arg1, arg2):
    global run_time
    run_time += 1
    print("Running test function with arguments:", arg1, arg2)
    return arg1 + arg2


def test_func2():
    print("test function 2")


def test_func3(arg):
    print("test function 3 with argument:", arg)


def test_callback(res):
    print("Running test callback function", res)
    assert res == "Hello World"


eventloop.start_new_task(test_func, ("Hello", " World"))

eventloop.start_new_task_periodic(
    test_func2, (),
    period_ms=200
)

eventloop.start_new_task_once(test_func3, ("Hello"))

# Sleep for enough time to allow the periodic task to run multiple times
while run_time < 3:
    time.sleep(0.1)

# Test case 3: Test removing a task
eventloop.start_new_task_periodic(
    test_func, ("Hello", " World"), callback=test_callback, task_name="test_task_remove")
eventloop.remove_task("test_task_remove")
