from PikaObj import *


class BackGround(TinyObj):
    def init():
        pass

    def setColor(color: str):
        pass

    def getColor() -> str:
        pass

    def update():
        pass


class ElementList(BaseObj):
    def update():
        pass


class Element(TinyObj):
    def init():
        pass

    def move(x: int, y: int):
        pass

    def right(x: int):
        pass

    def lift(x: int):
        pass

    def up(y: int):
        pass

    def down(y: int):
        pass

    def update():
        pass

    def setAlpha(alpha: int):
        pass


class Box(Element):
    # override
    def update():
        pass

    def init():
        pass

    def setColor(color: str):
        pass

    def setSize(x: int, y: int):
        pass


class Star(Element):
    # override
    def update():
        pass

    def init():
        pass

    def centra(x: int, y: int):
        pass

    def speed(speed: float):
        pass


class Window(BaseObj):
    background = BackGround()
    elems = ElementList()

    def init():
        pass

    def update():
        pass
