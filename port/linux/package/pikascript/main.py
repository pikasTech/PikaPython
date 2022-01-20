import PikaStdLib
import PikaStdDevice
import PikaStdData
import PikaStdTask
import PikaMath
from PikaObj import *
def todo1():
    print('task 1 running...')

def todo2():
    print('task 2 running...')

task = PikaStdTask.Task()
task.do_always(todo1)
task.do_always(todo2)

task.run_once()
