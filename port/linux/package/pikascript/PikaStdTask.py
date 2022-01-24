from PikaObj import *
import PikaStdData


class Task(TinyObj):
    calls = PikaStdData.List()

    def __init__():
        pass

    def call_always(fun_todo: any):
        pass

    def call_when(fun_todo: any, fun_when: any):
        pass

    def call_period_ms(fun_todo: any, period_ms: int):
        pass

    def run_once():
        pass

    def run_always():
        pass

    def run_until_ms(until_ms: int):
        pass

    def platformGetTick():
        pass
