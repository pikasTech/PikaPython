import fsm


def state1():
    print("state1")
    return "state2"


def state2():
    print("state2")
    return state3


def state3():
    print("state3")
    fsm.stop()
    return "state1"


def test_fsm():
    fsm.addState(state1, "state1")
    fsm.addState(state2, "state2")
    fsm.addState(state3, "state3")
    fsm.start("state1")
    fsm.wait()

test_fsm()
