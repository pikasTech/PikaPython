import time
import _thread

finished = False

def test_thread(arg):
    global finished
    for i in range(3):
        print(i)
        time.sleep(0.1)
    finished = True
    print('test_thread arg:', arg)
    assert arg == 'test'

# 开启线程 获取数据
_thread.start_new_thread(test_thread, ('test'))
while not finished:
    time.sleep(0.1)
time.sleep(0.1)
