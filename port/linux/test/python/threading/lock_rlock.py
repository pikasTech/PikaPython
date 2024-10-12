import _thread
import time
import threading
import PikaStdLib
# 共享资源
shared_resource = 0

# 互斥锁
mutex = threading.Lock()

# 线程函数

finished = 0


def thread_function(name, delay):
    global shared_resource
    global mutex, finished
    print("delay : %s" % str(delay))
    k = 0
    i = 0
    mem = PikaStdLib.MemChecker()
    for i in range(5):
        # while 1:

        try:
            # 获取互斥锁
            print("%s try to acquire lock. #1" % name)
            res = mutex.acquire(True, None)
            print("res: %s" % str(res))
            if 1:  # 测试RLock或者Lock的超时加上
                print("%s try to acquire lock. #2" % name)
                res = mutex.acquire(True, 0.5)
                print("res: %s" % str(res))
            if res:
                print("%s acquire lock SUCC." % name)
            else:
                print("%s acquire lock FAIL." % name)
            # 打印当前线程名称和共享资源的值
            print("Thread %s: Iteration %d, Shared Resource: %d" %
                  (name, i, shared_resource))

            # 更新共享资源
            shared_resource += 1

            # 模拟工作时间

            time.sleep(delay)
            print("wake")

            # 释放互斥锁
            mutex.release()
            mutex.release()
            k += 1

            print("%s i = %d." % (name, i))
            # print('mem used now:')
            # mem.now()

        except:
            print("------------- error ---------------")

    print("%s exit , at last, i = %d." % (name, k))
    finished += 1

# 主函数


def main():
    # 创建第一个线程
    _thread.start_new_thread(thread_function, ("Thread-1", 0.1))
    time.sleep(0.5)
    # 创建第二个线程
    _thread.start_new_thread(thread_function, ("Thread-2", 0.2))

    # 主线程等待子线程结束
    # 由于 _thread 没有 join 方法，我们通过 sleep 来模拟等待
    # time.sleep(60)
    while finished < 2:
        time.sleep(1)


main()
