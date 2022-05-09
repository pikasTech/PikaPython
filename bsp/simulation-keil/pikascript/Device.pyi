from PikaObj import *

class LED(TinyObj):
    def on(self):
        pass
    def off(self):
        pass

class Uart(TinyObj):
    def send(self, data:str):
        pass
    def setName(self, name:str):
        pass
    def printName(self):
        pass