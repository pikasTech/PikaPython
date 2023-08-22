import _thread
import time

_is_debug = False


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
    _delay_ms = None

    def __init__(self, func, callback, args, period_ms, delay_ms):
        """
        :param func: function to be called
        :param callback: callback function
        :param args: arguments of func
        :param period_ms: period of periodic task
        """
        if period_ms != None:
            self._is_periodic = True

        self._func = func
        self._callback = callback
        self._args = args
        self._period_ms = period_ms
        self._delay_ms = delay_ms
        if not delay_ms is None:
            if period_ms is None:
                period_ms = 0
            self._last_call_time = time.tick_ms() - period_ms + delay_ms
            _debug('last_call_time for delay:', self._last_call_time)
        _debug('func:', self._func)
        _debug('callback:', self._callback)
        _debug('args:', self._args)
        _debug('period_ms:', self._period_ms)
        _debug('delay_ms:', self._delay_ms)
        _debug('is_periodic:', self._is_periodic)


class EventLoop:
    """
    Event Loop
    """
    _tasks: dict[str, EventTask] = {}
    _period_ms = 100
    _thread_stack = 0
    _need_stop = False
    _started = False
    _uuid = 0

    def __init__(self, period_ms=100, thread_stack=0):
        """
        :param period_ms: period of loop
        :param thread_stack: stack size of thread
        """
        self._period_ms = period_ms
        self._thread_stack = thread_stack

    def _add_task(self, task_name, func, callback, args, period_ms, delay_ms):
        if task_name == None:
            self._uuid += 1
            task_name = str(self._uuid)
        _debug('create task:', task_name)
        new_task = EventTask(func, callback, args, period_ms, delay_ms)
        self._tasks[task_name] = new_task

    def start_new_task(self,
                       func, args,
                       is_periodic=True,
                       period_ms=1000,
                       callback=None,
                       task_name=None,
                       delay_ms=None
                       ):
        """
        Add a task to EventLoop
        :param task_name: name of task
        :param func: function to be called
        :param period_ms: period of task
        :param callback: callback function
        :param args: arguments of func
        """
        if is_periodic:
            self._add_task(task_name, func, callback,
                           args, period_ms, delay_ms)
        else:
            self._add_task(task_name, func, callback, args, None, delay_ms)

    def start_new_task_once(self,
                            func, args,
                            callback=None,
                            task_name=None,
                            delay_ms=None):
        """
        Add a task to EventLoop, run once
        :param task_name: name of task
        :param func: function to be called
        :param callback: callback function
        :param args: arguments of func
        """
        self.start_new_task(func, args, False, None,
                            callback, task_name, delay_ms)

    def start_new_task_periodic(self, func, args, period_ms=1000, callback=None, task_name=None, delay_ms=None):
        """
        Add a task to EventLoop, run periodically
        :param task_name: name of task
        :param func: function to be called
        :param period_ms: period of task
        :param callback: callback function
        :param args: arguments of func
        """
        self.start_new_task(func, args, True, period_ms,
                            callback, task_name, delay_ms)

    def remove_task(self, task_name):
        """
        Remove a task from EventLoop
        :param task_name: name of task
        """
        _debug('remove_task', task_name)
        self._tasks.remove(task_name)

    def _run_task(self, task: EventTask):
        _res = task._func(*task._args)
        if task._callback != None:
            task._callback(_res)

    def _run_thread(self):
        task_last = None
        while not self._need_stop:
            tick = time.tick_ms()
            for task_name, task in self._tasks.items():
                if task_last == task:
                    # already run this task last time, run other task first
                    task_last = None
                    continue

                if tick - task._last_call_time > task._period_ms:
                    _debug('run_task', task_name)
                    _debug('tick', tick)
                    _debug('last_call_time', task._last_call_time)
                    task_last = task
                    self._run_task(task)
                    task._last_call_time = tick
                    _debug('is_periodic', task._is_periodic)
                    if not task._is_periodic:
                        self.remove_task(task_name)
                    # only run one task per loop
                    # if the task are removed, the for loop will be broken
                    break 
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


def set_debug(enable: bool):
    global _is_debug
    _is_debug = enable


def _debug(*args):
    if _is_debug:
        print('\x1b[33m[eventloop]', *args, "\x1b[0m")


g_default_event_loop: EventLoop = None


def _get_default_event_loop():
    global g_default_event_loop
    if g_default_event_loop == None:
        g_default_event_loop = EventLoop()
        g_default_event_loop.start()
    return g_default_event_loop


def start_new_task(func, args,
                   is_periodic=True,
                   period_ms=1000,
                   callback=None,
                   task_name=None,
                   delay_ms=None
                   ):
    """
    Add a task to EventLoop
    :param task_name: name of task
    :param func: function to be called
    :param period_ms: period of task
    :param callback: callback function
    :param args: arguments of func
    """
    eventloop = _get_default_event_loop()
    eventloop.start_new_task(func, args, is_periodic,
                             period_ms, callback, task_name, delay_ms)


def start_new_task_once(func, args,
                        callback=None,
                        task_name=None,
                        delay_ms=None
                        ):
    """
    Add a task to EventLoop, run once
    :param task_name: name of task
    :param func: function to be called
    :param callback: callback function
    :param args: arguments of func
    """
    eventloop = _get_default_event_loop()
    eventloop.start_new_task_once(func, args, callback, task_name, delay_ms)


def start_new_task_periodic(func, args,
                            period_ms=1000,
                            callback=None,
                            task_name=None,
                            delay_ms=None
                            ):
    """
    Add a task to EventLoop, run periodically
    :param task_name: name of task
    :param func: function to be called
    :param period_ms: period of task
    :param callback: callback function
    :param args: arguments of func
    """
    eventloop = _get_default_event_loop()
    eventloop.start_new_task_periodic(
        func, args, period_ms, callback, task_name, delay_ms)


def remove_task(task_name):
    """
    Remove a task from EventLoop
    :param task_name: name of task
    """
    eventloop = _get_default_event_loop()
    eventloop.remove_task(task_name)


def stop():
    """
    Stop EventLoop
    """
    if g_default_event_loop == None:
        return
    _debug('stop default eventloop')
    eventloop = _get_default_event_loop()
    eventloop.stop()


def start():
    """
    Start EventLoop
    """
    eventloop = _get_default_event_loop()
    eventloop.start()


def __del__():
    stop()
