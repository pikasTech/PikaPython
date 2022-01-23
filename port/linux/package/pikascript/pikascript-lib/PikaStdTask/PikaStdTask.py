from PikaObj import *
import PikaStdData


class Task(TinyObj):
    calls_always = PikaStdData.List()
    calls_when = PikaStdData.List()
    assert_when = PikaStdData.List()
    calls_period = PikaStdData.List()
    assert_period = PikaStdData.List()
    time_period = PikaStdData.List()

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

    def platformGetTick():
        pass
