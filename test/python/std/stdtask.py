from GTestTask import Task
task = Task()
def test1():
    print('test1')

def test2():
    print('test2')

task.call_always(test1)
task.call_always(test2)
task.run_until_ms(1000)
exit()
