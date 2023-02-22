import PikaUI_core as core
import textgui

core.set_backend(textgui)

class Widget(core.Widget):
    pass

def App():
    return core.App()

class Page(core.Page):
    pass

class Button(core.Button):
    pass

class Text(core.Text):
    pass

ALIGN = core.ALIGN
