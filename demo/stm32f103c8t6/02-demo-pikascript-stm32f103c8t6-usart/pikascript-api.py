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

class MyRoot(BaseObj):
    led = LED()
    uart = Uart()