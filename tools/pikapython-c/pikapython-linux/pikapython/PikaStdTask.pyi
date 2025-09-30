import PikaStdData
import PikaStdLib


class Task(PikaStdLib.SysObj):
    calls = PikaStdData.List()

    def __init__(self):
        pass

    # regist a function to be called always
    def call_always(self, fun_todo: any):
        pass
    
    # regist a function to be called when fun_when() return 'True'
    def call_when(self, fun_todo: any, fun_when: any):
        pass

    # regist a function to be called periodically 
    def call_period_ms(self, fun_todo: any, period_ms: int):
        pass

    # run all registed function once
    def run_once(self):
        pass

    # run all registed function forever
    def run_forever(self):
        pass

    # run all registed function until time is up
    def run_until_ms(self, until_ms: int):
        pass

    # need be overried to supply the system tick
    def platformGetTick(self):
        pass
