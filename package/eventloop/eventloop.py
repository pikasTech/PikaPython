import _thread
import time


class EventTask:
    """
    Task Item of EventLoop
    """
    _func = None
    _callback = None
    _args = None
    _period_ms = None
    _is_periodic = False
    _last_call_time = 0

    def __init__(self, func, callback, args, period_ms):
        """
        :param func: function to be called
        :param callback: callback function
        :param args: arguments of func
        :param period_ms: period of periodic task
        """
        self._func = func
        self._callback = callback
        self._args = args
        self._period_ms = period_ms
        if period_ms != None:
            # print('period_ms', period_ms)
            self._is_periodic = True


class EventLoop:
    """
    Event Loop
    """
    _tasks: dict[str, EventTask] = {}
    _period_ms = 100
    _thread_stack = 0
    _need_stop = False
    _started = False

    def __init__(self, period_ms=100, thread_stack=0):
        """
        :param period_ms: period of loop
        :param thread_stack: stack size of thread
        """
        self._period_ms = period_ms
        self._thread_stack = thread_stack

    def _add_task(self, task_name, func, callback=None, args=None, period_ms=None):
        new_task = EventTask(func, callback, args, period_ms)
        self._tasks[task_name] = new_task

    def add_task_once(self, task_name, func, callback=None, args=None):
        """
        Add a task to EventLoop, run once
        :param task_name: name of task
        :param func: function to be called
        :param callback: callback function
        :param args: arguments of func
        """
        self._add_task(task_name, func, callback, args, None)

    def add_task_periodic(self, task_name, func, period_ms=1000, callback=None, args=None):
        """
        Add a task to EventLoop, run periodically
        :param task_name: name of task
        :param func: function to be called
        :param period_ms: period of task
        :param callback: callback function
        :param args: arguments of func
        """
        self._add_task(task_name, func, callback, args, period_ms)

    def remove_task(self, task_name):
        """
        Remove a task from EventLoop
        :param task_name: name of task
        """
        self._tasks.remove(task_name)


    def _run_thread(self):
        while not self._need_stop:
            tick = time.tick_ms()
            for task_name, task in self._tasks.items():
                if task._is_periodic:
                    # print('periodic', task_name, tick, task._last_call_time, task._period_ms)
                    if tick - task._last_call_time > task._period_ms:
                        _res = task._func(*task._args)
                        if task._callback != None:
                            task._callback(_res)
                        task._last_call_time = tick
                else:
                    _res = task._func(*task._args)
                    if task._callback != None:
                        task._callback(_res)
                    self.remove_task(task_name)
            if self._need_stop:
                break
            time.sleep_ms(self._period_ms)
        self._started = False

    def start(self):
        """
        Start EventLoop
        """
        if self._started:
            print('EventLoop already started')
            return
        self._need_stop = False
        last_stack_size = _thread.stack_size(self._thread_stack)
        _thread.start_new_thread(self._run_thread, ())
        last_stack_size = _thread.stack_size(last_stack_size)
        self._started = True

    def stop(self):
        """
        Stop EventLoop
        """
        if not self._started:
            print('EventLoop not started')
            return
        self._need_stop = True
        while self._started:
            time.sleep(0.1)
        time.sleep(1)

