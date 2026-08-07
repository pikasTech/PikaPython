"""Cooperative event loop for PikaPython.

The loop is intentionally single-threaded.  Call ``run_once`` from the
application main loop; no implicit worker thread is created.
"""
import time


class EventTask:
    def __init__(self, func, args=(), period_ms=None, callback=None, task_name=None, delay_ms=0):
        self.func = func
        self.args = args
        self.period_ms = period_ms
        self.callback = callback
        self.task_name = task_name
        self.next_due = time.ticks_ms() + delay_ms
        self.cancelled = False


class EventLoop:
    def __init__(self, period_ms=100):
        self.period_ms = period_ms
        self.tasks = {}
        self._next_id = 0

    def _name(self, task_name):
        if task_name is not None:
            return task_name
        self._next_id += 1
        return str(self._next_id)

    def start_new_task(self, func, args=(), is_periodic=True, period_ms=1000,
                       callback=None, task_name=None, delay_ms=0):
        name = self._name(task_name)
        if not is_periodic:
            period_ms = None
        self.tasks[name] = EventTask(func, args, period_ms, callback, name, delay_ms)
        return name

    def start_new_task_once(self, func, args=(), callback=None, task_name=None, delay_ms=0):
        return self.start_new_task(func, args, False, None, callback, task_name, delay_ms)

    def start_new_task_periodic(self, func, args=(), period_ms=1000,
                                callback=None, task_name=None, delay_ms=0):
        return self.start_new_task(func, args, True, period_ms, callback, task_name, delay_ms)

    def remove_task(self, task_name):
        if task_name in self.tasks:
            del self.tasks[task_name]

    def run_once(self, now_ms=None):
        """Run due tasks once and return the number of callbacks executed."""
        if now_ms is None:
            now_ms = time.ticks_ms()
        ran = 0
        names = list(self.tasks.keys())
        for name in names:
            task = self.tasks.get(name)
            if task is None or task.cancelled:
                continue
            if time.ticks_diff(now_ms, task.next_due) < 0:
                continue
            result = task.func(*task.args)
            if task.callback is not None:
                task.callback(result)
            ran += 1
            if task.period_ms is None:
                self.remove_task(name)
            else:
                task.next_due = now_ms + task.period_ms
        return ran

    def run(self, duration_ms=None):
        """Run cooperatively for ``duration_ms``; ``None`` runs until empty."""
        started = time.ticks_ms()
        while self.tasks:
            now = time.ticks_ms()
            self.run_once(now)
            if duration_ms is not None and time.ticks_diff(now, started) >= duration_ms:
                break
            time.sleep_ms(self.period_ms)


_default = EventLoop()


def start_new_task(func, args=(), is_periodic=True, period_ms=1000,
                   callback=None, task_name=None, delay_ms=0):
    return _default.start_new_task(func, args, is_periodic, period_ms, callback, task_name, delay_ms)


def start_new_task_once(func, args=(), callback=None, task_name=None, delay_ms=0):
    return _default.start_new_task_once(func, args, callback, task_name, delay_ms)


def start_new_task_periodic(func, args=(), period_ms=1000, callback=None,
                            task_name=None, delay_ms=0):
    return _default.start_new_task_periodic(func, args, period_ms, callback, task_name, delay_ms)


def run_once(now_ms=None):
    return _default.run_once(now_ms)


def remove_task(task_name):
    _default.remove_task(task_name)
