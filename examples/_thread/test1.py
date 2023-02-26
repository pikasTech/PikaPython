import _thread
import time

task1_finished = False
task2_finished = False


def task1():
    global task1_finished
    print("task1")
    for i in range(10):
        time.sleep(0.05)
        print("task1")
    task1_finished = True


def task2(sleep_time, loop_count):
    global task2_finished
    print("task2:", sleep_time, loop_count)
    for i in range(loop_count):
        time.sleep(sleep_time)
        print("task2")
    task2_finished = True


_thread.start_new_thread(task1, ())
_thread.start_new_thread(task2, (0.05, 10))

while not task1_finished or not task2_finished:
    pass

time.sleep(0.5)  # wait for threads to exit
