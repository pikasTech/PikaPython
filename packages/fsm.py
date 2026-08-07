"""Single-threaded state machine compatible with the V1 fsm API."""


class State:
    def __init__(self, name, function):
        self._name = name
        self._function = function

    def getName(self):
        return self._name

    def getFunction(self):
        return self._function

    def __str__(self):
        return self._name


class StateMachine:
    def __init__(self):
        self._statelist = []
        self._currentState = None
        self._needStop = False
        self._stoped = False

    def addState(self, state):
        self._statelist.append(state)

    def getStateByName(self, stateName):
        for state in self._statelist:
            if state.getName() == stateName:
                return state
        return None

    def getStateByFunction(self, stateFunction):
        for state in self._statelist:
            if state.getFunction() == stateFunction:
                return state
        return None

    def getState(self, stateNameOrFn):
        if type(stateNameOrFn) == str:
            return self.getStateByName(stateNameOrFn)
        return self.getStateByFunction(stateNameOrFn)

    def setCurrentState(self, stateNameOrFn):
        state = self.getState(stateNameOrFn)
        if state is None:
            return -1
        self._currentState = state
        return 0

    def runCurrentState(self):
        if self._currentState is None:
            return None
        return self._currentState.getFunction()()

    def run_once(self, initStateName=None):
        if initStateName is not None:
            self.setCurrentState(initStateName)
        if self._needStop or self._currentState is None:
            return None
        nextState = self.runCurrentState()
        if nextState is None:
            self._needStop = True
            self._stoped = True
            return None
        if self.setCurrentState(nextState) != 0:
            self._needStop = True
            self._stoped = True
            return None
        return nextState

    def mainLoop(self, initStateName):
        self._needStop = False
        self._stoped = False
        self.setCurrentState(initStateName)
        while not self._needStop:
            self.run_once()
        self._stoped = True

    def start(self, initStateName):
        # V1 started a worker thread; V2 deliberately exposes cooperative start.
        self._needStop = False
        self._stoped = False
        return self.setCurrentState(initStateName)

    def stop(self):
        self._needStop = True
        self._stoped = True

    def wait(self):
        return self._stoped


_defaultStateMachine = None


def _initDefaultStateMachine():
    global _defaultStateMachine
    if _defaultStateMachine is None:
        _defaultStateMachine = StateMachine()


def addState(stateFunction, stateName='default'):
    _initDefaultStateMachine()
    _defaultStateMachine.addState(State(stateName, stateFunction))


def start(initStateName):
    _initDefaultStateMachine()
    return _defaultStateMachine.start(initStateName)


def run_once():
    _initDefaultStateMachine()
    return _defaultStateMachine.run_once()


def stop():
    _initDefaultStateMachine()
    _defaultStateMachine.stop()


def wait():
    _initDefaultStateMachine()
    return _defaultStateMachine.wait()
