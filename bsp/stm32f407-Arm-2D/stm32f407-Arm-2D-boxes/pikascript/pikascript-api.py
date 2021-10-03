class Box(TinyObj):
    def init():
        pass
    def moveTo(x:int, y:int):
        pass
    def moveLeft(x:int):
        pass
    def moveRight(x:int):
        pass
    def setColor(color:str):
        pass
    def setSize(sizeX:int, sizeY:int):
        pass
    def on():
        pass
    def off():
        pass

class MemoryChecker(TinyObj):
    # check memory used now
    def now():
        pass
    # check memory used max
    def max():
        pass
    # check the size of memory pool
    def size():
        pass

class PikaMain(BaseObj):
    mem = MemoryChecker()
    box = Box()