import PikaUI as ui
from PikaStdLib import MemChecker as mem

class Page1(ui.Page):
    def build(self):
        return ui.Text('Hello Page1')

class Page2(ui.Page):
    def build(self):
        return ui.Text('Hello Page2')


app = ui.App()
app.pageManager.enter(Page1())
app.timer.cb(0)
app.pageManager.enter(Page2())
app.timer.cb(0)
app.pageManager.back()
app.timer.cb(0)

