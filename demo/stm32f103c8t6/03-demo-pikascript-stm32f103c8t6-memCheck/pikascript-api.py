class LED(TinyObj):
    def on():
        pass
    def off():
        pass

class Uart(TinyObj):
    def send(data:str):
        pass
    def setName(name:str):
        pass
    def printName():
        pass

class MemoryChecker(TinyObj):
    # check memory used now
    def now():
        pass
    # check memory used max
    def max():
        pass

class MyRoot(BaseObj):
    led = LED()
    uart = Uart()
    mem = MemoryChecker()