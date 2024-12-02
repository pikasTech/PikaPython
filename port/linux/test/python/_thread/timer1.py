import time
from timer import Timer


tim = Timer()


def task1(a, b):
    print('task1:', a, b)


def task2(a, b):
    print('task2:', a, b)


tim.call_after_ms(1000, task1, (1, 2))
tim.call_after_ms(2000, task2, (3, 4))

while len(tim.tasks) > 0:
    time.sleep(1)

tim.stop()
time.sleep(1)
