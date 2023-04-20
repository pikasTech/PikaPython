import time
from eventloop import EventLoop

run_time = 0

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
event_loop = EventLoop(period_ms=100)

event_loop.add_task_periodic("test_task_periodic", test_func, 1000, test_callback, args=("Hello", " World"))
event_loop.add_task_periodic("test_task_periodic2", test_func2, 200, test_callback, args=("Hello", " World"))
event_loop.start()

# Sleep for enough time to allow the periodic task to run multiple times
while run_time < 3:
    time.sleep(0.1)

event_loop.stop()

# Test case 3: Test removing a task
event_loop.add_task_periodic("test_task_remove", test_func, 1000, test_callback, args=("Hello", " World"))
event_loop.remove_task("test_task_remove")

print(event_loop._tasks)
assert "test_task_remove" not in event_loop._tasks, "Failed to remove the task"
