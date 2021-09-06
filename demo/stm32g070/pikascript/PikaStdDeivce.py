from PikaObj import *


class GPIO(TinyObj):
    def init():
        pass

    def setPin(pinName: str):
        pass

    def getPin() -> str:
        pass

    def setMode(mode: str):
        pass

    def getMode() -> str:
        pass

    def enable():
        pass

    def disable():
        pass

    def on():
        pass

    def off():
        pass

    # need be overrid
    def platformOn():
        pass

    # need override
    def platformOff():
        pass

    # need override
    def platformEnable():
        pass

    # need override
    def platformDisable():
        pass

    # need override
    def platformSetMode(mode: str):
        pass
