from PikaObj import *
import PikaStdTask

class Thread(TinyObj):
    def mdelay(ms: int):
        pass

class Task(PikaStdTask.Task):
    # overried
    def platformGetTick(self):
        pass