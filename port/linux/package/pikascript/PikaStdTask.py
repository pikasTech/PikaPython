from PikaObj import *
import PikaStdData


class Task(TinyObj):
    calls = PikaStdData.List()

    def __init__():
        pass

    # regist a function to be called always
    def call_always(fun_todo: any):
        pass
    
    # regist a function to be called when fun_when() return 'True'
    def call_when(fun_todo: any, fun_when: any):
        pass

    # regist a function to be called periodically 
    def call_period_ms(fun_todo: any, period_ms: int):
        pass

    # run all registed function once
    def run_once():
        pass

    # run all registed function forever
    def run_forever():
        pass

    # run all registed function until time is up
    def run_until_ms(until_ms: int):
        pass

    # need be overried to supply the system tick
    def platformGetTick():
        pass
