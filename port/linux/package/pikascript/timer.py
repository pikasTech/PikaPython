import _thread
import time


class Timer:
    def __init__(self, tick_ms=10):
        self.tick_ms = tick_ms
        self.is_running = False
        self.stoped = False
        self.tasks = []
        self.debug = False
        self.start()

    def start(self):
        self.is_running = True
        _thread.start_new_thread(self.run, ())

    def run(self):
        while self.is_running:
            tick_now = time.tick_ms()
            for task in self.tasks:
                time_end, func, args = task
                if tick_now >= time_end:
                    func(*args)
                    self.tasks.remove(task)
                    if self.debug:
                        print('tasks after run:', len(self.tasks))
            time.sleep_ms(self.tick_ms)
        self.stoped = True

    def stop(self):
        self.is_running = False
        while not self.stoped:
            time.sleep_ms(self.tick_ms)

    def call_after_ms(self, ms, func, args=()):
        time_end = time.tick_ms() + ms
        self.tasks.append((time_end, func, args))
