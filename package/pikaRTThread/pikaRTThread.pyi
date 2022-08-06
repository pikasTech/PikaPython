import PikaStdTask

class Thread:
    def mdelay(ms: int):
        pass

class Task(PikaStdTask.Task):
    # overried
    def platformGetTick(self):
        pass