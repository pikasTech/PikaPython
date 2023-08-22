import _thread
import time

_defaultStateMachine: "StateMachine" = None

CONFIG_DEBUG = False


def debug(*info):
    print("debug: ", end='')
    print(*info)


def error(*info):
    print("error: ", end='')
    print(*info)


def info(*info):
    print("info: ", end='')
    print(*info)


class State:
    _name: str = None
    _function: callable = None

    def __init__(self, name: str, function: callable):
        self._name = name
        self._function = function

    def getName(self):
        return self._name

    def getFunction(self):
        return self._function

    def __str__(self):
        return self._name


class StateMachine:
    _statelist: list[State] = None
    _currentState: State = None
    _needStop: bool = False
    _stoped: bool = False

    def __init__(self):
        self._statelist = []

    def addState(self, state: State):
        self._statelist.append(state)

    def getStateByName(self, stateName: str):
        for state in self._statelist:
            if state.getName() == stateName:
                return state
        return None

    def getStateByFunction(self, stateFunction: callable):
        for state in self._statelist:
            if state.getFunction() == stateFunction:
                return state
        return None

    def getState(self, stateNameOrFn: str):
        if type(stateNameOrFn) == str:
            return self.getStateByName(stateNameOrFn)
        else:
            return self.getStateByFunction(stateNameOrFn)

    def setCurrentState(self, stateNameOrFn: str) -> int:
        debug("stateNameOrFn: " + str(stateNameOrFn))
        state = self.getState(stateNameOrFn)
        debug("nextState: " + str(state))
        if state is None:
            error("Error: state not found: " + stateNameOrFn)
            return -1
        self._currentState = state
        return 0

    def runCurrentState(self) -> str:
        if None == self._currentState:
            error("Error: current state is None")
            return None
        fn = self._currentState.getFunction()
        info("current state: " + str(self._currentState))
        debug("current state fn: " + str(fn))
        return fn()

    def mainLoop(self, initStateName: str):
        debug("mainloop: initStateName: " + initStateName)
        res = self.setCurrentState(initStateName)
        debug("mainloop: setCurrentState: " + str(res))
        while not self._needStop:
            nextStateNameOrFn = self.runCurrentState()
            if None == nextStateNameOrFn:
                error("Error: runCurrentState return None")
                break
            res = self.setCurrentState(nextStateNameOrFn)
            if 0 != res:
                error("State Machine Error: " + str(res))
                break
            time.sleep(0.1)
        self._stoped = True
        return

    def start(self, initStateName: str):
        debug("start state machine, initStateName: " + initStateName)
        _thread.start_new_thread(self.mainLoop, (initStateName,))

    def stop(self):
        self._needStop = True
        info("stop state machine")

    def wait(self):
        while not self._stoped:
            time.sleep(0.1)


def _initDefaultStateMachine():
    global _defaultStateMachine
    if _defaultStateMachine is None:
        _defaultStateMachine = StateMachine()


def addState(stateFunction: callable, stateName: str = 'default'):
    _initDefaultStateMachine()
    _defaultStateMachine.addState(State(stateName, stateFunction))


def start(initStateName: str):
    _initDefaultStateMachine()
    _defaultStateMachine.start(initStateName)


def stop():
    _initDefaultStateMachine()
    _defaultStateMachine.stop()


def wait():
    _initDefaultStateMachine()
    _defaultStateMachine.wait()
