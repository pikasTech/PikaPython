from PikaObj import *


class BackGround(TinyObj):
    def init():
        pass

    def setColor(color: str):
        pass

    def getColor() -> str:
        pass

    def update(ptTile: pointer, bIsNewFrame: int):
        pass


class ElementList(BaseObj):
    def update(ptTile: pointer, bIsNewFrame: int):
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

    def update(ptTile: pointer, bIsNewFrame: int):
        pass


class Box(Element):
    # override
    def update(ptTile: pointer, bIsNewFrame: int):
        pass

    def init():
        pass

    def setColor(color: str):
        pass

    def setSize(x: int, y: int):
        pass


class Star(Element):
    # override
    def update(ptTile: pointer, bIsNewFrame: int):
        pass

    def init():
        pass

    def centra(x: int, y: int):
        pass

    def speed(speed: float):
        pass


class Screen(BaseObj):
    background = BackGround()
    elems = ElementList()

    def newBox(name: str):
        pass

    def newStar(name: str):
        pass

    def init():
        pass

    def update(ptTile: pointer, bIsNewFrame: int):
        pass
